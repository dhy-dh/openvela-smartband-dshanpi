/****************************************************************************
 * apps/benchmarks/osperf/osperf.c
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

#include <assert.h>
#include <limits.h>
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/poll.h>
#include <unistd.h>

#include <nuttx/atomic.h>
#include <nuttx/mutex.h>
#include <nuttx/sched.h>
#include <nuttx/wqueue.h>
#include <nuttx/mutex.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct performance_time_s
{
  clock_t start;
  clock_t end;
};

struct performance_thread_s
{
  sem_t sem;
  pthread_mutex_t lock;
  pthread_cond_t cond;
  mutex_t mutex;
  struct performance_time_s time;
};

struct performance_entry_s
{
  const char name[NAME_MAX];
  CODE size_t (*entry)(void);
};

/****************************************************************************
 * Private Functions Prototypes
 ****************************************************************************/

static size_t pthread_create_performance(void);
static size_t pthread_switch_performance(void);
static size_t pthread_cond_signal_performance(void);
static size_t pthread_cond_signal_switch_performance(void);
static size_t nxmutex_performance(void);
static size_t context_switch_performance(void);
#ifdef CONFIG_BUILD_FLAT
static size_t hpwork_performance(void);
#endif
static size_t poll_performance(void);
#ifndef CONFIG_DISABLE_MQUEUE
static size_t mqueue_performance(void);
#endif
static size_t semwait_performance(void);
static size_t sempost_performance(void);
static size_t mutex_lock_performance(void);
static size_t mutex_unlock_performance(void);
static size_t nullop_performance(void);
static size_t atomic_performance(void);
static size_t function_call_performance(void);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct performance_entry_s g_entry_list[] =
{
  {"pthread-create", pthread_create_performance},
  {"pthread-switch", pthread_switch_performance},
  {"pthread-cond-signal", pthread_cond_signal_performance},
  {"pthread-cond-signal-switch", pthread_cond_signal_switch_performance},
  {"context-switch", context_switch_performance},
  {"nxmutex-lock", nxmutex_performance},
#ifdef CONFIG_BUILD_FLAT
  {"hpwork", hpwork_performance},
#endif
  {"poll-write", poll_performance},
#ifndef CONFIG_DISABLE_MQUEUE
  {"mqueue-send", mqueue_performance},
#endif
  {"semwait", semwait_performance},
  {"sempost", sempost_performance},
  {"mutex-lock", mutex_lock_performance},
  {"mutex-unlock", mutex_unlock_performance},
  {"null-op", nullop_performance},
  {"atomic-calculate", atomic_performance},
  {"function-call", function_call_performance}
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int performance_thread_create(FAR void *(*entry)(FAR void *),
                                     FAR void *arg, int priority)
{
  struct sched_param param;
  pthread_attr_t attr;
  pthread_t tid;

  param.sched_priority = priority;
  pthread_attr_init(&attr);
  pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  pthread_attr_setschedparam(&attr, &param);
  pthread_create(&tid, &attr, entry, arg);
  DEBUGASSERT(tid > 0);
  return tid;
}

static void performance_start(FAR struct performance_time_s *result)
{
  result->start = perf_gettime();
}

static void performance_end(FAR struct performance_time_s *result)
{
  result->end = perf_gettime();
}

static size_t performance_gettime(FAR struct performance_time_s *result)
{
  struct timespec ts;
  perf_convert(result->end - result->start, &ts);
  return ts.tv_sec * NSEC_PER_SEC + ts.tv_nsec;
}

/****************************************************************************
 * Pthread switch performance
 ****************************************************************************/

static FAR void *pthread_switch_task(FAR void *arg)
{
  FAR struct performance_thread_s *perf = arg;
  sem_wait(&perf->sem);
  performance_end(&perf->time);
  return NULL;
}

static FAR void *pthread_cond_task(FAR void *arg)
{
  FAR struct performance_thread_s *perf = arg;

  pthread_mutex_lock(&perf->lock);
  sem_post(&perf->sem);
  pthread_cond_wait(&perf->cond, &perf->lock);
  pthread_mutex_unlock(&perf->lock);
  performance_end(&perf->time);
  return NULL;
}

static size_t pthread_switch_performance(void)
{
  struct performance_thread_s perf;
  pthread_t tid;

  sem_init(&perf.sem, 0, 0);
  tid = performance_thread_create(pthread_switch_task, &perf,
                                  CONFIG_BENCHMARK_OSPERF_PRIORITY + 1);

  performance_start(&perf.time);
  sem_post(&perf.sem);
  pthread_join(tid, NULL);

  return performance_gettime(&perf.time);
}

static size_t nxmutex_performance(void)
{
  struct performance_thread_s perf;

  nxmutex_init(&perf.mutex);

  performance_start(&perf.time);
  nxmutex_lock(&perf.mutex);
  nxmutex_unlock(&perf.mutex);
  performance_end(&perf.time);

  return performance_gettime(&perf.time);
}

static size_t pthread_cond_signal_performance(void)
{
  struct performance_thread_s perf;

  pthread_mutex_init(&perf.lock, NULL);
  pthread_cond_init(&perf.cond, NULL);

  performance_start(&perf.time);
  pthread_mutex_lock(&perf.lock);
  pthread_cond_signal(&perf.cond);
  pthread_mutex_unlock(&perf.lock);
  performance_end(&perf.time);

  return performance_gettime(&perf.time);
}

static size_t pthread_cond_signal_switch_performance(void)
{
  struct performance_thread_s perf;
  pthread_t tid;

  sem_init(&perf.sem, 0, 0);
  pthread_mutex_init(&perf.lock, NULL);
  pthread_cond_init(&perf.cond, NULL);

  tid = performance_thread_create(pthread_cond_task, &perf,
                                  CONFIG_BENCHMARK_OSPERF_PRIORITY + 1);

  sem_wait(&perf.sem);
  performance_start(&perf.time);
  pthread_mutex_lock(&perf.lock);
  pthread_cond_signal(&perf.cond);
  pthread_mutex_unlock(&perf.lock);
  pthread_join(tid, NULL);
  sem_destroy(&perf.sem);

  return performance_gettime(&perf.time);
}

/****************************************************************************
 * Pthread create performance
 ****************************************************************************/

static FAR void *pthread_create_task(FAR void *arg)
{
  FAR struct performance_time_s *time = arg;
  performance_end(time);
  return NULL;
}

static size_t pthread_create_performance(void)
{
  struct performance_time_s result;
  struct sched_param param;
  pthread_attr_t attr;
  pthread_t tid;

  sched_getparam(gettid(), &param);
  param.sched_priority++;
  pthread_attr_init(&attr);
  pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  pthread_attr_setschedparam(&attr, &param);

  performance_start(&result);
  pthread_create(&tid, &attr, pthread_create_task, &result);
  pthread_join(tid, NULL);

  return performance_gettime(&result);
}

/****************************************************************************
 * Contxt create performance
 ****************************************************************************/

static FAR void *context_switch_task(FAR void *arg)
{
  FAR struct performance_time_s *time = arg;
  sched_yield();
  performance_end(time);
  return 0;
}

static size_t context_switch_performance(void)
{
  struct performance_time_s time;
  int tid;

  tid = performance_thread_create(context_switch_task, &time,
                                  CONFIG_BENCHMARK_OSPERF_PRIORITY);
  sched_yield();
  performance_start(&time);
  sched_yield();
  pthread_join(tid, NULL);
  return performance_gettime(&time);
}

#ifdef CONFIG_BUILD_FLAT
/****************************************************************************
 * wdog performance
 ****************************************************************************/

static void work_handle(void *arg)
{
  FAR struct performance_time_s *time = ((FAR void **)arg)[2];
  FAR sem_t *sem = ((void **)arg)[1];
  performance_end(time);
  sem_post(sem);
}

static size_t hpwork_performance(void)
{
  struct performance_time_s result;
  struct work_s work;
  sem_t sem = SEM_INITIALIZER(0);
  int ret;

  FAR void *args = (void *[])
  {
    (FAR void *)&work,
    (FAR void *)&sem,
    (FAR void *)&result
  };

  memset(&result, 0, sizeof(result));
  memset(&work, 0, sizeof(work));
  performance_start(&result);
  ret = work_queue(HPWORK, &work, work_handle, args, 0);
  DEBUGASSERT(ret == 0);

  sem_wait(&sem);
  return performance_gettime(&result);
}
#endif

/****************************************************************************
 * poll-write performance
 ****************************************************************************/

static FAR void *poll_task(FAR void *arg)
{
  FAR void **argv = arg;
  FAR struct performance_time_s *time = argv[0];
  int pipefd = (int)(uintptr_t)argv[1];

  performance_start(time);
  write(pipefd, "a", 1);
  return 0;
}

static size_t poll_performance(void)
{
  struct performance_time_s result;
  struct pollfd fds;
  FAR void *argv[2];
  int pipefd[2];
  int ret;

  ret = pipe(pipefd);
  DEBUGASSERT(ret == 0);
  fds.fd = pipefd[0];
  fds.events = POLLIN;
  argv[0] = (FAR char *)&result;
  argv[1] = (FAR char *)(uintptr_t)pipefd[1];

  ret = performance_thread_create(poll_task, argv,
                                  CONFIG_BENCHMARK_OSPERF_PRIORITY - 1);

  poll(&fds, 1, -1);
  performance_end(&result);

  pthread_join(ret, NULL);
  close(pipefd[0]);
  close(pipefd[1]);
  return performance_gettime(&result);
}

/****************************************************************************
 * mqueue-send performance
 ****************************************************************************/

static FAR void *mqueue_task(FAR void *arg)
{
  FAR void **argv = arg;
  FAR struct performance_time_s *time = argv[0];
  mqd_t mqdes = (mqd_t)(uintptr_t)argv[1];
  char buffer[32];

  mq_receive(mqdes, buffer, sizeof(buffer), NULL);
  performance_end(time);
  return NULL;
}

#ifndef CONFIG_DISABLE_MQUEUE
static size_t mqueue_performance(void)
{
  struct performance_time_s result;
  const char mqname[] = "/osperf_mq";
  const char msg[] = "test";
  struct mq_attr attr;
  FAR void *argv[2];
  mqd_t mqdes;
  int ret;

  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 32;
  attr.mq_flags = 0;
  attr.mq_curmsgs = 0;

  mqdes = mq_open(mqname, O_CREAT | O_RDWR, 0666, &attr);
  DEBUGASSERT(mqdes != (mqd_t)-1);

  argv[0] = (FAR char *)&result;
  argv[1] = (FAR char *)(uintptr_t)mqdes;

  ret = performance_thread_create(mqueue_task, argv,
                                  CONFIG_BENCHMARK_OSPERF_PRIORITY + 1);

  performance_start(&result);
  mq_send(mqdes, msg, sizeof(msg), 0);

  pthread_join(ret, NULL);
  mq_close(mqdes);
  mq_unlink(mqname);

  return performance_gettime(&result);
}
#endif

/****************************************************************************
 * semwait_performance
 ****************************************************************************/

static size_t semwait_performance(void)
{
  struct performance_time_s result;
  sem_t sem;

  sem_init(&sem, 0, 2);

  performance_start(&result);
  sem_wait(&sem);
  performance_end(&result);

  sem_destroy(&sem);
  return performance_gettime(&result);
}

/****************************************************************************
 * sempost_performance
 ****************************************************************************/

static size_t sempost_performance(void)
{
  struct performance_time_s result;
  sem_t sem;

  sem_init(&sem, 0, 0);

  performance_start(&result);
  sem_post(&sem);
  performance_end(&result);

  sem_destroy(&sem);
  return performance_gettime(&result);
}

/****************************************************************************
 * mutex_lock_performance
 ****************************************************************************/

static size_t mutex_lock_performance(void)
{
  struct performance_time_s result;
  mutex_t mutex;

  nxmutex_init(&mutex);

  performance_start(&result);
  nxmutex_lock(&mutex);
  performance_end(&result);

  nxmutex_destroy(&mutex);
  return performance_gettime(&result);
}

/****************************************************************************
 * mutex_unlock_performance
 ****************************************************************************/

static size_t mutex_unlock_performance(void)
{
  struct performance_time_s result;
  mutex_t mutex;

  nxmutex_init(&mutex);

  performance_start(&result);
  nxmutex_unlock(&mutex);
  performance_end(&result);

  nxmutex_destroy(&mutex);
  return performance_gettime(&result);
}

/****************************************************************************
 * null_performance
 ****************************************************************************/

static size_t nullop_performance(void)
{
  struct performance_time_s result;

  performance_start(&result);
  performance_end(&result);

  return performance_gettime(&result);
}

/****************************************************************************
 * function_call_performance
 ****************************************************************************/

static void call_test_handler(void)
{
}

/* Use global function pointers to prevent compiler optimizations. */

static void (*volatile call_test_func)(void) = call_test_handler;

static size_t function_call_performance(void)
{
  struct performance_time_s result;

  performance_start(&result);
  call_test_func();
  performance_end(&result);

  return performance_gettime(&result);
}

/****************************************************************************
 * atomic_performance
 ****************************************************************************/

static size_t atomic_performance(void)
{
  struct performance_time_s result;
  atomic_t val;

  performance_start(&result);
  atomic_set(&val, 1234);
  atomic_read(&val);
  atomic_add_relaxed(&val, 123);
  atomic_sub_relaxed(&val, 123);
  atomic_and_relaxed(&val, 123);
  atomic_or_relaxed(&val, 123);
  atomic_xor_relaxed(&val, 123);
  atomic_cmpxchg(&val, &val, 123);
  performance_end(&result);

  return performance_gettime(&result);
}

/****************************************************************************
 * performance_help
 ****************************************************************************/

static void performance_help(void)
{
  printf("Usage: performance [OPTIONS] [name]\n\n");
  printf("OPTIONS:\n");
  printf("\t-c, \tNumber of times to run each test\n");
  printf("\t-d, \tShow detail of each test\n");
  printf("\t-h, \tShow this help message\n");
  printf("\t-l, \tList all tests\n");
}

/****************************************************************************
 * performance_run
 ****************************************************************************/

static void performance_run(const FAR struct performance_entry_s *item,
                            size_t count, bool detail)
{
  size_t total = 0;
  size_t max = 0;
  size_t min = 0;
  size_t i;

  for (i = 0; i < count; i++)
    {
#ifdef CONFIG_BUILD_FLAT
      irqstate_t flags = up_irq_save();
#endif
      size_t time = item->entry();
#ifdef CONFIG_BUILD_FLAT
      up_irq_restore(flags);
#endif

      total += time;
      if (time > max)
        {
          max = time;
        }

      if (time < min || min == 0)
        {
          min = time;
        }

      if (detail)
        {
          printf("\t%zu: %zu\n", i, time);
        }
    }

  printf("%-*s %10zu %10zu %10zu\n", NAME_MAX, item->name, max, min,
         total / count);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

static const FAR
struct performance_entry_s *find_entry(FAR const char *name)
{
  size_t i;

  for (i = 0; i < nitems(g_entry_list); i++)
    {
      if (strcmp(name, g_entry_list[i].name) == 0)
        {
          return &g_entry_list[i];
        }
    }

  return NULL;
}

void performance_list(void)
{
  size_t i;

  for (i = 0; i < nitems(g_entry_list); i++)
    {
      printf("%s\n", g_entry_list[i].name);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const FAR struct performance_entry_s *item = NULL;
  bool detail = false;
  size_t count = 100;
  size_t i;
  int opt;

  while ((opt = getopt(argc, argv, "dc:hl")) != -1)
    {
      switch (opt)
        {
          case 'd':
            detail = true;
            break;
          case 'c':
            count = strtoul(optarg, NULL, 0);
            break;
          case 'h':
            performance_help();
            return EXIT_SUCCESS;
          case 'l':
            performance_list();
            return EXIT_SUCCESS;
          default:
            performance_help();
            return EXIT_FAILURE;
        }
    }

  if (optind < argc)
    {
      item = find_entry(argv[optind]);
      if (item == NULL)
        {
          printf("Can't find %s\n", argv[optind]);
          return EXIT_FAILURE;
        }
    }

  printf("OS performance args: count:%zu, detail:%s\n", count,
         detail ? "true" : "false");

  printf("==============================================================\n");
  printf("%-*s %10s %10s %10s\n", NAME_MAX, "Describe", "Max", "Min", "Avg");

  if (item != NULL)
    {
      performance_run(item, count, detail);
      return EXIT_SUCCESS;
    }

  for (i = 0; i < nitems(g_entry_list); i++)
    {
      item = &g_entry_list[i];
      performance_run(item, count, detail);
    }

  return EXIT_SUCCESS;
}
