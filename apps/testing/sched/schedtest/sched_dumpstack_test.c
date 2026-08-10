/****************************************************************************
 * apps/testing/sched/schedtest/sched_dumpstack_test.c
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

static void test_error_id(void **state)
{
  sched_dumpstack(-1);
}

static void test_current_cpu_run(void **state)
{
  pthread_t thread = gettid();

  sched_dumpstack(thread);
}

static void test_current_cpu_ready(void **state)
{
  pthread_t thread;

  /* Set the cpu affinity of the new thread */

  pthread_create(&thread, NULL, current_ready, NULL);
  sched_yield();

  sched_dumpstack(thread);

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
  pthread_attr_t tattr;
  cpu_set_t cpuset;
  int current_cpu = sched_getcpu();
  int other_cpu = (current_cpu == 0) ? 1 : 0;

  /* Set the cpu affinity of the new thread */

  pthread_attr_init(&tattr);
  CPU_ZERO(&cpuset);
  CPU_SET(other_cpu, &cpuset);
  pthread_attr_setaffinity_np(&tattr, sizeof(cpu_set_t), &cpuset);
  pthread_create(&thread, &tattr, other_run, NULL);
  up_mdelay(1000);

  sched_dumpstack(thread);

  pthread_join(thread, NULL);
}

static void test_other_cpu_ready(void **state)
{
  pthread_t thread;
  pthread_attr_t tattr;
  cpu_set_t cpuset;
  int current_cpu = sched_getcpu();
  int other_cpu = (current_cpu == 0) ? 1 : 0;

  /* Set the cpu affinity of the new thread */

  pthread_attr_init(&tattr);
  CPU_ZERO(&cpuset);
  CPU_SET(other_cpu, &cpuset);
  pthread_attr_setaffinity_np(&tattr, sizeof(cpu_set_t), &cpuset);
  pthread_create(&thread, &tattr, other_ready, NULL);
  up_mdelay(1000);

  sched_dumpstack(thread);

  pthread_join(thread, NULL);
}

#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_error_id),
      cmocka_unit_test(test_current_cpu_run),
      cmocka_unit_test(test_current_cpu_ready),
#ifdef CONFIG_SMP
      cmocka_unit_test(test_other_cpu_run),
      cmocka_unit_test(test_other_cpu_ready),
#endif
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
