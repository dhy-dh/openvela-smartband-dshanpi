/****************************************************************************
 * apps/testing/sched/events/perf_test.c
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
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include <nuttx/perf.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct perf_read_format_s
{
  uint64_t value;
};

static sem_t g_sem;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void *perf_child_thread(FAR void *parameter)
{
  sem_wait(&g_sem);
  return NULL;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int test_perf_raw(int cpu, int id)
{
  uint64_t start_value;
  uint64_t end_value;
  int event_fd;
  struct perf_read_format_s read_data;
  struct perf_event_attr_s event_attr =
    {
      0
    };

  /* Create cpu event */

  event_attr.type   = PERF_TYPE_RAW;
  event_attr.size   = sizeof(struct perf_event_attr_s);
  event_attr.config = id;
  event_attr.read_format = 0;
  event_attr.disabled = 1;
  event_attr.config1 |= 1;

  event_fd = perf_event_open(&event_attr, -1, cpu, -1, O_CLOEXEC);
  if (event_fd < 0)
    {
      printf("perf event open failed! event_fd = %d\n", event_fd);
    }

  ioctl(event_fd, PERF_EVENT_IOC_RESET, 0);
  ioctl(event_fd, PERF_EVENT_IOC_ENABLE, 0);

  if (read(event_fd, &read_data, sizeof(struct perf_read_format_s)) < 0)
    {
      printf("read perf event data failed!\n");
      return -EINVAL;
    }

  start_value = read_data.value;
  printf("Perf event count number, start: %ld\n", read_data.value);

  if (read(event_fd, &read_data, sizeof(struct perf_read_format_s)) < 0)
    {
      printf("read perf event data failed!\n");
      return -EINVAL;
    }

  end_value = read_data.value;
  printf("Perf event count number, end: %ld\n", read_data.value);
  printf("end - start = %ld\n", end_value - start_value);

  ioctl(event_fd, PERF_EVENT_IOC_DISABLE, 0);
  close(event_fd);

  return EXIT_SUCCESS;
}

int test_perf_cache(int cpu, int id)
{
  uint64_t start_value;
  uint64_t end_value;
  int event_fd;
  struct perf_read_format_s read_data;
  struct perf_event_attr_s event_attr =
    {
      0
    };

  /* Create cpu event */

  event_attr.type   = PERF_TYPE_HW_CACHE;
  event_attr.size   = sizeof(struct perf_event_attr_s);
  event_attr.config = id;
  event_attr.read_format = 0;
  event_attr.disabled = 1;
  event_attr.config1 |= 1;

  event_fd = perf_event_open(&event_attr, -1, cpu, -1, O_CLOEXEC);
  if (event_fd < 0)
    {
      printf("perf event open failed! event_fd = %d\n", event_fd);
    }

  ioctl(event_fd, PERF_EVENT_IOC_RESET, 0);
  ioctl(event_fd, PERF_EVENT_IOC_ENABLE, 0);

  if (read(event_fd, &read_data, sizeof(struct perf_read_format_s)) < 0)
    {
      printf("read perf event data failed!\n");
      return -EINVAL;
    }

  start_value = read_data.value;
  printf("Perf event count number, start: %ld\n", read_data.value);

  if (read(event_fd, &read_data, sizeof(struct perf_read_format_s)) < 0)
    {
      printf("read perf event data failed!\n");
      return -EINVAL;
    }

  end_value = read_data.value;
  printf("Perf event count number, end: %ld\n", read_data.value);
  printf("end - start = %ld\n", end_value - start_value);

  ioctl(event_fd, PERF_EVENT_IOC_DISABLE, 0);
  close(event_fd);

  return EXIT_SUCCESS;
}

int test_perf_cpus(int cpu, int id)
{
  uint64_t start_value;
  uint64_t end_value;
  int event_fd;
  struct perf_read_format_s read_data;
  struct perf_event_attr_s event_attr =
    {
      0
    };

  /* Create cpu event */

  event_attr.type   = PERF_TYPE_HARDWARE;
  event_attr.size   = sizeof(struct perf_event_attr_s);
  event_attr.config = id;
  event_attr.read_format = 0;
  event_attr.disabled = 1;
  event_attr.config1 |= 1;

  event_fd = perf_event_open(&event_attr, -1, cpu, -1, O_CLOEXEC);
  if (event_fd < 0)
    {
      printf("perf event open failed! event_fd = %d\n", event_fd);
    }

  ioctl(event_fd, PERF_EVENT_IOC_RESET, 0);
  ioctl(event_fd, PERF_EVENT_IOC_ENABLE, 0);

  if (read(event_fd, &read_data, sizeof(struct perf_read_format_s)) < 0)
    {
      printf("read perf event data failed!\n");
      return -EINVAL;
    }

  start_value = read_data.value;
  printf("Perf event count number, start: %ld\n", read_data.value);

  if (read(event_fd, &read_data, sizeof(struct perf_read_format_s)) < 0)
    {
      printf("read perf event data failed!\n");
      return -EINVAL;
    }

  end_value = read_data.value;
  printf("Perf event count number, end: %ld\n", read_data.value);
  printf("end - start = %ld\n", end_value - start_value);

  ioctl(event_fd, PERF_EVENT_IOC_DISABLE, 0);
  close(event_fd);

  return EXIT_SUCCESS;
}

int test_perf_task(int cpu, int id)
{
  uint64_t start_value;
  uint64_t end_value;
  struct perf_read_format_s read_data;
  pthread_attr_t attr;
  struct sched_param sparam;
  pthread_t child;
  int parent_fd;
  struct perf_event_attr_s event_attr =
    {
      0
    };

  sem_init(&g_sem, 0, 0);

  /* Create parent task event */

  event_attr.type   = PERF_TYPE_HARDWARE;
  event_attr.size   = sizeof(struct perf_event_attr_s);
  event_attr.config = id;
  event_attr.read_format = 0;
  event_attr.disabled = 1;
  event_attr.inherit = 1;

  parent_fd = perf_event_open(&event_attr, _SCHED_GETTID(),
                              cpu, -1, O_CLOEXEC);
  if (parent_fd < 0)
    {
      printf("perf event open failed! parent_fd = %d\n", parent_fd);
    }

  ioctl(parent_fd, PERF_EVENT_IOC_RESET, 0);
  ioctl(parent_fd, PERF_EVENT_IOC_ENABLE, 0);

  /* Create child task */

  pthread_attr_init(&attr);
  sparam.sched_priority = 240;
  pthread_attr_setschedparam(&attr, &sparam);
  pthread_create(&child, &attr, perf_child_thread, NULL);

  if (read(parent_fd, &read_data, sizeof(struct perf_read_format_s)) < 0)
    {
      printf("read parent_fd data failed!\n");
      return -EINVAL;
    }

  start_value = read_data.value;
  printf("Perf event count number, start: %ld\n", read_data.value);

  if (read(parent_fd, &read_data, sizeof(struct perf_read_format_s)) < 0)
    {
      printf("read parent_fd data failed!\n");
      return -EINVAL;
    }

  end_value = read_data.value;
  printf("Perf event count number, end: %ld\n", read_data.value);
  printf("end - start = %ld\n", end_value - start_value);

  sem_post(&g_sem);

  pthread_join(child, NULL);

  ioctl(parent_fd, PERF_EVENT_IOC_DISABLE, 0);
  close(parent_fd);
  sem_destroy(&g_sem);

  return EXIT_SUCCESS;
}

int main(int argc, FAR char *argv[])
{
  int cpu = 0;
  int id = 0;

  if (argc < 2)
    {
      test_perf_cpus(0, 0);
      test_perf_task(-1, 0);
      test_perf_raw(0, 0x11);
      return EXIT_SUCCESS;
    }

  if (argc >= 3)
    {
      cpu = atoi(argv[2]);
    }

  if (argc >= 4)
    {
      id = atoi(argv[3]);
    }

  if (!strcmp(argv[1], "cpus"))
    {
      test_perf_cpus(cpu, id);
    }
  else if (!strcmp(argv[1], "cache"))
    {
      test_perf_cache(cpu, id);
    }
  else if (!strcmp(argv[1], "task"))
    {
      if (argc < 3)
        {
          cpu = -1;
        }

      test_perf_task(cpu, id);
    }
  else if (!strcmp(argv[1], "raw"))
    {
      if (argc < 4)
        {
          id = 0x11;
        }

      test_perf_raw(cpu, id);
    }

  return EXIT_SUCCESS;
}
