/****************************************************************************
 * apps/testing/sched/schedtest/cpucount_test.c
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
 * Private Functions
 ****************************************************************************/

static void test_cpu_count(void **state)
{
  int i;
  int j;
  int ret;
  cpu_set_t cpuset;

  for (i = 0; i < CONFIG_SMP_NCPUS; i++)
    {
      CPU_ZERO(&cpuset);
      for (j = 0; j < i; j++)
        {
          CPU_SET(j, &cpuset);
        }

      ret = sched_cpucount(&cpuset);
      assert_int_equal(ret, i);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_cpu_count),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
