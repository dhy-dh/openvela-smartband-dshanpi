/****************************************************************************
 * apps/testing/libc/ratelimit/ratelimit_main.c
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
#include <inttypes.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <cmocka.h>

#include <nuttx/ratelimit.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Each cycle lasts for 1 seconds, and a maximum of 3 logs
 * can be printed in each cycle.
 */

static DEFINE_RATELIMIT_STATE(g_period_ratelimit, 1, 3);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: test_case_period_ratelimit
 ****************************************************************************/

static void test_case_period_ratelimit(FAR void **state)
{
  size_t i;
  const int count_array[10] =
  {
    1, 3, 5, 7, 9, 8, 6, 4, 2, 0,
  };

  for (i = 0; i < nitems(count_array); i++)
    {
      int count = count_array[i];
      int printed_count = 0;
      int expected_count = (count <= g_period_ratelimit.burst) ? \
                       count : g_period_ratelimit.burst;

      while (count--)
        {
          if (!ratelimit_islimited(&g_period_ratelimit))
            {
              printed_count++;
            }
        }

      sleep(1);
      assert_int_equal(printed_count, expected_count);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * ratelimit_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest tests[] =
  {
    cmocka_unit_test(test_case_period_ratelimit),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
