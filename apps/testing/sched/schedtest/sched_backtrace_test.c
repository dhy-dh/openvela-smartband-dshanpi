/****************************************************************************
 * apps/testing/sched/schedtest/sched_backtrace_test.c
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

#include <nuttx/config.h>
#include <nuttx/arch.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <cmocka.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void *current_ready(void *arg)
{
  sched_yield();

  return NULL;
}

static void test_current_cpu_run(void **state)
{
  pthread_t thread = gettid();
  void *buffer[10];
  int num_frames;
  int i;

  num_frames = sched_backtrace(thread, buffer, 10, 0);
  assert_int_not_equal(num_frames, 0);
  printf("Backtrace:\n");
  for (i = 0; i < num_frames; i++)
    {
      printf("Frame %d: %p\n", i, buffer[i]);
    }
}

static void test_current_cpu_ready(void **state)
{
  pthread_t thread;
  void *buffer[10];
  int num_frames;
  int i;

  /**
   * If in SMP, newly created threads will be put on other idle CPUs,
   * so need to set CPU affinity.
   */

#ifdef CONFIG_SMP
  pthread_attr_t attr;
  cpu_set_t cpuset;
  int current_cpu = sched_getcpu();

  pthread_attr_init(&attr);
  CPU_ZERO(&cpuset);
  CPU_SET(current_cpu, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  pthread_create(&thread, &attr, current_ready, NULL);
#else
  pthread_create(&thread, NULL, current_ready, NULL);
#endif

  sched_yield();

  num_frames = sched_backtrace(thread, buffer, 10, 0);
  assert_int_not_equal(num_frames, 0);
  printf("Backtrace:\n");
  for (i = 0; i < num_frames; i++)
    {
      printf("Frame %d: %p\n", i, buffer[i]);
    }

  pthread_join(thread, NULL);
}

#ifdef CONFIG_SMP

static void *other_run(void *arg)
{
  up_mdelay(1000 * 10);

  return NULL;
}

static void *other_ready(void *arg)
{
  sleep(10);

  return NULL;
}

static void test_other_cpu_run(void **state)
{
  pthread_t thread;
  void *buffer[10];
  int num_frames;
  int i;
  pthread_attr_t attr;
  cpu_set_t cpuset;
  int current_cpu = sched_getcpu();
  int other_cpu = (current_cpu == 0) ? 1 : 0;

  pthread_attr_init(&attr);
  CPU_ZERO(&cpuset);
  CPU_SET(other_cpu, &cpuset);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  pthread_create(&thread, &attr, other_run, NULL);
  up_mdelay(1000);

  num_frames = sched_backtrace(thread, buffer, 10, 0);
  assert_int_not_equal(num_frames, 0);
  printf("Backtrace:\n");
  for (i = 0; i < num_frames; i++)
    {
      printf("Frame %d: %p\n", i, buffer[i]);
    }

  pthread_join(thread, NULL);
}

static void test_other_cpu_ready(void **state)
{
  pthread_t thread;
  void *buffer[10];
  int num_frames;
  int i;
  pthread_attr_t attr;
  cpu_set_t cpuset;
  int current_cpu = sched_getcpu();
  int other_cpu = (current_cpu == 0) ? 1 : 0;

  pthread_attr_init(&attr);
  CPU_ZERO(&cpuset);
  CPU_SET(other_cpu, &cpuset);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  pthread_create(&thread, &attr, other_ready, NULL);
  up_mdelay(1000);

  num_frames = sched_backtrace(thread, buffer, 10, 0);
  assert_int_not_equal(num_frames, 0);
  printf("Backtrace:\n");
  for (i = 0; i < num_frames; i++)
    {
      printf("Frame %d: %p\n", i, buffer[i]);
    }

  pthread_join(thread, NULL);
}

#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_current_cpu_run),
      cmocka_unit_test(test_current_cpu_ready),
#ifdef CONFIG_SMP
      cmocka_unit_test(test_other_cpu_run),
      cmocka_unit_test(test_other_cpu_ready),
#endif
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
