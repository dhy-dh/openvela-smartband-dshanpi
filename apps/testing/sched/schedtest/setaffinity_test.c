/****************************************************************************
 * apps/testing/sched/schedtest/setaffinity_test.c
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
#include <pthread.h>
#include <sched.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <cmocka.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define SUM 1000000000

/****************************************************************************
 * Private Data
 ****************************************************************************/

static volatile int g_count = 0;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/**
 * Due to FIFO scheduling, the thread will only be executed if it is
 * switched to a new CPU.
 */

static FAR void *task_assert_cpu(void *arg)
{
  int new_cpu = *(int *)arg;
  int cpu = sched_getcpu();

  assert_int_equal(cpu, new_cpu);

  return NULL;
}

static FAR void *task_add_sum(void *arg)
{
  int i;

  for (i = 0; i < SUM; i++)
    {
      g_count++;
    }

  return NULL;
}

/**
 * Test changing the affinity of current thread.
 */

static void test_set_current(void **state)
{
  int ret;
  cpu_set_t cpuset;
  int old_cpu = sched_getcpu();
  int new_cpu = (old_cpu == 0) ? 1 : 0;

  ret = sched_getaffinity(0, sizeof(cpu_set_t), &cpuset);
  assert_int_equal(ret, 0);
  assert_true(CPU_ISSET(old_cpu, &cpuset));

  CPU_ZERO(&cpuset);
  CPU_SET(new_cpu, &cpuset);
  ret = sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
  assert_int_equal(ret, 0);

  ret = sched_getaffinity(0, sizeof(cpu_set_t), &cpuset);
  assert_int_equal(ret, 0);
  assert_false(CPU_ISSET(old_cpu, &cpuset));
  assert_true(CPU_ISSET(new_cpu, &cpuset));
}

/**
 * Test changing the affinity of other thread.
 */

static void test_set_others(void **state)
{
  int ret;
  cpu_set_t cpuset;
  pthread_attr_t attr;
  pthread_t thread;

  int old_cpu = sched_getcpu();
  int new_cpu = (old_cpu == 0) ? 1 : 0;

  pthread_attr_init(&attr);
  CPU_ZERO(&cpuset);
  CPU_SET(old_cpu, &cpuset);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
  ret = pthread_create(&thread, &attr, task_assert_cpu, &new_cpu);
  assert_int_equal(ret, 0);

  ret = sched_getaffinity(thread, sizeof(cpu_set_t), &cpuset);
  assert_int_equal(ret, 0);
  assert_true(CPU_ISSET(old_cpu, &cpuset));

  CPU_ZERO(&cpuset);
  CPU_SET(new_cpu, &cpuset);
  ret = sched_setaffinity(thread, sizeof(cpu_set_t), &cpuset);
  assert_int_equal(ret, 0);

  ret = sched_getaffinity(thread, sizeof(cpu_set_t), &cpuset);
  assert_int_equal(ret, 0);
  assert_true(CPU_ISSET(new_cpu, &cpuset));

  pthread_join(thread, NULL);
}

/**
 * Verify that two threads on different CPUs are executing in parallel.
 */

static void test_parallel(void **state)
{
  int i;
  int ret;
  cpu_set_t cpuset;
  pthread_attr_t attr;
  pthread_t thread[2];
  g_count = 0;

  int old_cpu = sched_getcpu();
  int new_cpu = (old_cpu == 0) ? 1 : 0;

  pthread_attr_init(&attr);
  CPU_ZERO(&cpuset);
  CPU_SET(old_cpu, &cpuset);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);

  for (i = 0; i < 2; i++)
    {
      ret = pthread_create(&thread[i], &attr, task_add_sum, NULL);
      assert_int_equal(ret, 0);
    }

  /* Let thread[0] start executing in new cpu */

  CPU_ZERO(&cpuset);
  CPU_SET(new_cpu, &cpuset);
  ret = sched_setaffinity(thread[0], sizeof(cpu_set_t), &cpuset);
  assert_int_equal(ret, 0);

  /* Give up the CPU and let thread[1] start executing in old cpu */

  sched_yield();

  pthread_join(thread[0], NULL);
  pthread_join(thread[1], NULL);

  assert_int_not_equal(g_count, SUM * 2);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_set_current),
      cmocka_unit_test(test_set_others),
      cmocka_unit_test(test_parallel),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
