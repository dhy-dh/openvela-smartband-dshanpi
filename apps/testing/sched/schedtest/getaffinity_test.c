/****************************************************************************
 * apps/testing/sched/schedtest/getaffinity_test.c
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

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <cmocka.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void *task_run(void *arg)
{
  up_mdelay(1000);

  return NULL;
}

/**
 * Test getting the affinity of current thread.
 */

static void test_get_current(void **state)
{
  int ret;
  cpu_set_t cpuset;
  int cpu = sched_getcpu();

  ret = sched_getaffinity(0, sizeof(cpu_set_t), &cpuset);
  assert_int_equal(ret, 0);
  assert_true(CPU_ISSET(cpu, &cpuset));
}

/**
 * Test getting the affinity of other thread.
 */

static void test_get_others(void **state)
{
  int ret;
  cpu_set_t cpuset;
  pthread_attr_t attr;
  pthread_t thread;
  int old_cpu = sched_getcpu();
  int new_cpu = (old_cpu != 1) ? 1 : 0;

  pthread_attr_init(&attr);
  CPU_ZERO(&cpuset);
  CPU_SET(new_cpu, &cpuset);
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);

  ret = pthread_create(&thread, &attr, task_run, NULL);
  assert_int_equal(ret, 0);

  ret = sched_getaffinity(thread, sizeof(cpu_set_t), &cpuset);
  assert_int_equal(ret, 0);
  assert_true(CPU_ISSET(new_cpu, &cpuset));

  pthread_join(thread, NULL);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_get_current),
      cmocka_unit_test(test_get_others),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
