/****************************************************************************
 * apps/system/perf-tools/builtin-record.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

#include <nuttx/perf.h>
#include <nuttx/list.h>
#include <nuttx/nuttx.h>

#include "builtin.h"
#include "evlist.h"
#include "evsel.h"
#include "parse-options.h"

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define PERF_MAGIC 0x32454c4946524550ULL
#define PERF_MMAP_SZIE 4096
#define PERF_EVENT_MAX 10

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct perf_file_section_s
{
  uint64_t offset;
  uint64_t size;
};

struct perf_file_header_s
{
  uint64_t magic; /* PERFILE2 */
  uint64_t size;
  uint64_t attr_size;
  struct perf_file_section_s attrs;
  struct perf_file_section_s data;
  struct perf_file_section_s event_types;
  uint64_t flags;
  uint64_t flags1[3];
};

struct perf_file_attr_s
{
  struct perf_event_attr_s  attr;
  struct perf_file_section_s  ids;
};

struct sw_event
{
  uint64_t config;
  FAR char *event_name;
  bool flag;
};

struct cmd_record_s
{
  int out_fd;
  uint32_t attr_offset;
  uint32_t data_offset;
  uint32_t data_size;
  int mmap_fd[CONFIG_SMP_NCPUS];      /* use one ring buffer */
  FAR void *mmap_base[CONFIG_SMP_NCPUS];
  uint32_t nr_fds;
  struct pollfd fds[PERF_EVENT_MAX];
  uint32_t buffer_size;
  bool call_chain;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static volatile int g_record_done = 0;

static struct sw_event events_map[] =
{
  {PERF_COUNT_SW_CPU_CLOCK, "cpu-clock", false},
  {PERF_COUNT_SW_TASK_CLOCK, "task-clock", false},
  {PERF_COUNT_SW_CONTEXT_SWITCHES, "cs", false},
};

static const char record_usage[] =
  "perf record [<options>] [<command>]";

static const struct option_s record_options[] =
{
  OPT_STRING('a', "all-cpus", "system-wide collection from all CPUs"),
  OPT_STRING('c', "count <n>", "event period to sample (us)"),
  OPT_STRING('C', "cpu <cpu>", "list of cpus to monitor in system-wide"),
  OPT_STRING('e', "event <event>",
             "event selector. use 'perf list' to list available events"),
  OPT_STRING('p', "pid <pid>", "stat events on existing process id"),
  OPT_STRING('m', "--mmap-size <size>", "size of mmap buffer"),
  OPT_STRING('g', " ", "enables call-graph recording"),
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void record_cmds_help(int sname)
{
  unsigned int i = 0;
  unsigned int longest = 0;

  printf("\n Usage: %s\n\n", record_usage);

  for (i = 0; i < nitems(record_options); i++)
    {
      if (longest < strlen(record_options[i].long_name))
        {
          longest = strlen(record_options[i].long_name);
        }
    }

  for (i = 0; i < nitems(record_options); i++)
    {
      if (!sname || sname == record_options[i].short_name)
        {
          printf("   -%c, --%-*s   %s\n", record_options[i].short_name,
                                  longest, record_options[i].long_name,
                                  record_options[i].help);
        }
    }

  printf("\n");
}

static int do_write(int fd, void *buf, size_t size)
{
  while (size)
    {
      int ret = write(fd, buf, size);

      if (ret < 0)
        {
          fprintf(stderr, "cannot write\n");
          return ret;
        }

      size -= ret;
      buf += ret;
    }

  return 0;
}

static int mmap_read(FAR struct cmd_record_s *rec)
{
  for (int i = 0; i < CONFIG_SMP_NCPUS; i++)
    {
      FAR struct circbuf_s *cbuf = rec->mmap_base[i];
      ssize_t len;
      uint8_t buffer[512];
      int ret;
      int used_len = circbuf_used(cbuf);

      while (cbuf && used_len > 0)
        {
          len = circbuf_read(cbuf, buffer, 512);
          if (len < 0)
            {
              return len;
            }

          ret = do_write(rec->out_fd, buffer, len);
          if (ret < 0)
            {
              return ret;
            }

          used_len -= len;
          rec->data_size += len;
        }
    }

  return 0;
}

static void perf_data_write_header(FAR struct cmd_record_s *rec,
                                   FAR struct evlist_s *evlist)
{
  struct perf_file_header_s f_header;
  struct perf_file_attr_s   f_attr;
  FAR struct perf_evsel_s *evsel;
  int fd = rec->out_fd;

  lseek(fd, sizeof(f_header), SEEK_SET);

  list_for_every_entry(&evlist->core.entries, evsel,
                       struct perf_evsel_s, node)
    {
      evsel->id_offset = lseek(fd, 0, SEEK_CUR);
      do_write(fd, evsel->id, evsel->ids * sizeof(uint64_t));
    }

  rec->attr_offset = lseek(fd, 0, SEEK_CUR);

  list_for_every_entry(&evlist->core.entries, evsel,
                       struct perf_evsel_s, node)
    {
      f_attr = (struct perf_file_attr_s){
        .attr = evsel->attr,
        .ids = {
          .offset = evsel->id_offset,
          .size   = evsel->ids * sizeof(uint64_t),
        }
      };
      do_write(fd, &f_attr, sizeof(f_attr));
    }

  rec->data_offset = lseek(fd, 0, SEEK_CUR);

  f_header = (struct perf_file_header_s)
    {
      .magic = PERF_MAGIC,
      .size = sizeof(f_header),
      .attr_size = sizeof(f_attr),
      .attrs =
        {
          .offset = rec->attr_offset,
          .size = evlist->core.nr_entries * sizeof(f_attr),
        },

      .data =
        {
          .offset = rec->data_offset,
          .size = rec->data_size,
        },
    };

  lseek(fd, 0, SEEK_SET);
  do_write(fd, &f_header, sizeof(f_header));
  lseek(fd, rec->data_offset + rec->data_size, SEEK_SET);
}

static void record_evsel_mmap(int cpu, int fd,
                              FAR struct cmd_record_s *rec)
{
  ASSERT(cpu <= CONFIG_SMP_NCPUS);

  if (!rec->mmap_base[cpu])
    {
      rec->mmap_base[cpu] = mmap(NULL, rec->buffer_size, PROT_READ,
                                MAP_SHARED, fd, 0);
      ASSERT(rec->mmap_base);
      rec->mmap_fd[cpu] = fd;
      ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

      rec->fds[rec->nr_fds].fd = fd;
      rec->fds[rec->nr_fds].events = POLLIN;
      rec->nr_fds++;
    }
  else
    {
      ioctl(fd, PERF_EVENT_IOC_SET_OUTPUT, rec->mmap_fd[cpu]);
      ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
    }
}

static int record_evsel_open(FAR struct perf_evsel_s *evsel,
                             FAR struct evlist_s *evlist,
                             FAR struct cmd_record_s *rec)
{
  int fd;
  uint64_t id;

  if (evlist->cpu == -1)
    {
      for (int i = 0; i < CONFIG_SMP_NCPUS; i++)
        {
          fd = perf_event_open(&evsel->attr, evlist->pid,
                               i, -1, O_CLOEXEC);
          if (fd < 0)
            {
              return fd;
            }

          record_evsel_mmap(i, fd, rec);
          evsel->evfd = fd;
          ioctl(fd, PERF_EVENT_IOC_ID, &id);
          evsel->id[i] = id;
          evsel->ids++;
        }
    }
  else
    {
      fd = perf_event_open(&evsel->attr, evsel->pid,
                           evsel->cpu, -1, O_CLOEXEC);
      if (fd < 0)
        {
          return fd;
        }

      record_evsel_mmap(evsel->cpu, fd, rec);
      evsel->evfd = fd;
      ioctl(fd, PERF_EVENT_IOC_ID, &id);
      evsel->id[0] = id;
      evsel->ids++;
    }

  return 0;
}

static int evsel_record_start(FAR struct perf_evsel_s *evsel,
                              FAR struct evlist_s *evlist,
                              FAR struct cmd_record_s *rec)
{
  FAR struct perf_event_attr_s *attr = &evsel->attr;
  int ret = 0;

  attr->read_format  = PERF_FORMAT_TOTAL_TIME_ENABLED |
          PERF_FORMAT_TOTAL_TIME_RUNNING |
          PERF_FORMAT_ID;
  attr->sample_type = PERF_SAMPLE_ID;
  attr->sample_type  |= PERF_SAMPLE_IP | PERF_SAMPLE_TID;

  if (rec->call_chain)
    {
      attr->sample_type |= PERF_SAMPLE_CALLCHAIN;
    }

  attr->sample_period = evlist->sample_period;
  attr->disabled = 1;
  attr->inherit = 1;

  evsel->id = zalloc(CONFIG_SMP_NCPUS * sizeof(uint64_t));
  evsel->ids = 0;

  if (!evsel->id)
    {
      fprintf(stderr, "no memory\n");
      return -ENOMEM;
    }

  ret = record_evsel_open(evsel, evlist, rec);
  if (ret < 0)
    {
      fprintf(stderr, "evsel open error\n");
    }

  return ret;
}

int perf_record_handle(FAR struct evlist_s *evlist,
                       FAR struct stat_args_s *stat_args)
{
  FAR struct perf_evsel_s *evsel;
  struct cmd_record_s record;
  pid_t child_pid = -1;
  int status = 0;
  int remain_ms = stat_args->sec * 1000;

  memset(&record, 0, sizeof(struct cmd_record_s));
  g_record_done = 0;

  record.out_fd = open(PERF_SAVE_FILE_NAME,
                       O_RDWR | O_CREAT | O_TRUNC, S_IRUSR);
  if (record.out_fd < 0)
    {
      fprintf(stderr, "cannot open %s\n", PERF_SAVE_FILE_NAME);
      goto err;
    }

  if (stat_args->buffer_size > PERF_MMAP_SZIE)
    {
      record.buffer_size = ALIGN_UP(stat_args->buffer_size, 0x0f);
    }
  else
    {
      record.buffer_size = PERF_MMAP_SZIE;
    }

  record.call_chain = stat_args->call_chain;

  list_for_every_entry(&evlist->core.entries, evsel,
                       struct perf_evsel_s, node)
    {
      status = evsel_record_start(evsel, evlist, &record);

      if (status < 0)
        {
          fprintf(stderr, "parm is invalid\n");
          goto finish;
        }
    }

  perf_data_write_header(&record, evlist);

  if (stat_args->cmd)
    {
      status = posix_spawn(&child_pid, stat_args->cmd,
                            NULL, NULL,
                            stat_args->cmd_args, NULL);
      if (status < 0 || child_pid < 0)
        {
          fprintf(stderr, "cannot create process\n");
          goto finish;
        }
    }

  while (true)
    {
      pid_t pid;

      if (mmap_read(&record) < 0)
        {
          break;
        }

      if ((stat_args->sec > 0 && remain_ms <= 0) ||
          (child_pid > 0 &&
          (pid = waitpid(child_pid, &status, WNOHANG)) > 0))
        {
          break;
        }

      remain_ms -= 100;
      status = poll(record.fds, record.nr_fds, 100);

      if (status < 0)
        {
          if (errno == EINTR || errno == EAGAIN)
            {
              continue;
            }

          mmap_read(&record);
          break;
        }

      if (g_record_done)
        {
          mmap_read(&record);
          break;
        }
    }

finish:
  list_for_every_entry(&evlist->core.entries, evsel,
                       struct perf_evsel_s, node)
    {
      evsel_disable((FAR struct evsel_s *)evsel, evlist);
    }

  mmap_read(&record);
  perf_data_write_header(&record, evlist);

  list_for_every_entry(&evlist->core.entries, evsel,
                       struct perf_evsel_s, node)
    {
      evsel_close((FAR struct evsel_s *)evsel, evlist);
    }

  close(record.out_fd);
err:
  return status;
}

static int parse_events(FAR struct evlist_s *evlist, FAR char *evstr)
{
  int evnum = 0;
  int i;
  int status = -1;

  if (!evstr)
    {
      evlist->attrs = zalloc(sizeof(struct perf_event_attr_s));
      if (evlist->attrs == NULL)
        {
          return -ENOMEM;
        }

      evlist->attrs[0].type = PERF_TYPE_SOFTWARE;
      evlist->attrs[0].config = PERF_COUNT_SW_CPU_CLOCK;
      evlist_add_attrs(evlist, evlist->attrs, 1, evlist->cpu);
      return 0;
    }

  do
    {
      for (i = 0; i < nitems(events_map); i++)
        {
          int len = strlen(events_map[i].event_name);

          if (!strncmp(evstr, events_map[i].event_name, len))
            {
              evnum += 1;
              events_map[i].flag = true;
              evstr += len;
            }
        }
    }
  while (*evstr++ != '\0');

  if (evnum > 0)
    {
      int k = 0;

      evlist->attrs = zalloc(evnum * sizeof(struct perf_event_attr_s));
      if (evlist->attrs == NULL)
        {
          return -ENOMEM;
        }

      while (k < evnum)
        {
          for (i = 0; i < nitems(events_map); i++)
            {
              if (events_map[i].flag)
                {
                  evlist->attrs[k].type = PERF_TYPE_SOFTWARE;
                  evlist->attrs[k].config = events_map[i].config;
                }
            }

          k++;
        }

      status = evlist_add_attrs(evlist, evlist->attrs, evnum, evlist->cpu);
    }

  return status;
}

static void record_sig_handler(int sig)
{
  printf("receive signal %d\n", sig);
  g_record_done = 1;
}

static int run_perf_record(FAR struct evlist_s *evlist,
                           FAR struct stat_args_s *stat_args)
{
  int status = 0;
  int i;

  signal(SIGCHLD, record_sig_handler);
  signal(SIGINT, record_sig_handler);

  evlist->cpu = stat_args->cpu;
  evlist->pid = stat_args->pid;
  for (i = 0; i < nitems(events_map); i++)
    {
      events_map[i].flag = false;
    }

  status = parse_events(evlist, stat_args->events);
  if (status < 0)
    {
      return status;
    }

  status = perf_record_handle(evlist, stat_args);

  return status;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int cmd_record(int argc, FAR const char **argv)
{
  FAR static struct evlist_s *evlist;
  int status = 0;
  struct stat_args_s stat_args;

  printf("perf pid %d\n", getpid());

  memset(&stat_args, 0, sizeof(struct stat_args_s));

  status = parse_stat_options(argc, (char **)argv, &stat_args);
  if (status || stat_args.type == STAT_ARGS_HELP)
    {
      record_cmds_help(0);
      return status;
    }

  evlist = evlist_new();
  if (evlist == NULL)
    {
      return -ENOMEM;
    }

  evlist->sec = stat_args.sec;
  evlist->type = stat_args.type;

  if (stat_args.sample_period)
    {
      evlist->sample_period = stat_args.sample_period * NSEC_PER_TICK;
    }
  else
    {
      evlist->sample_period = 10 * NSEC_PER_TICK;
    }

  if (stat_args.pid == -1 && stat_args.cpu == -1)
    {
      evlist->system_wide = true;
    }
  else
    {
      evlist->system_wide = false;
    }

  status = run_perf_record(evlist, &stat_args);

  evlist_delete(evlist);

  return status;
}
