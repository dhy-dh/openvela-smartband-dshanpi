/****************************************************************************
 * apps/testing/libc/clocktest/strptime_test.c
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

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmocka.h>
#include <sys/time.h>
#include <time.h>

#include "clock_test.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define VALID_PARA_NUM 5
#define ERR_PARA_NUM 2

/****************************************************************************
 * Public Functions
 ****************************************************************************/

DECLARE_TEST(strptime_valid_para)
{
  int i;
  char *ret;
  char buffer[100];
  struct tm tm;
  const char *test_cases[VALID_PARA_NUM][2] =
    {
      {"2024-10-28", "%Y-%m-%d"},
      {"28/10/2024", "%d/%m/%Y"},
      {"2024-10-28 14:30", "%Y-%m-%d %H:%M"},
      {"2024-02-29", "%Y-%m-%d"},
      {"2023-02-29", "%Y-%m-%d"}
    };

  const int compare_cases[VALID_PARA_NUM][6] =
    {
      {0, 0, 0, 28, 9, 124},
      {0, 0, 0, 28, 9, 124},
      {0, 30, 14, 28, 9, 124},
      {0, 0, 0, 29, 1, 124},
      {0, 0, 0, 29, 1, 123}
    };

  for (i = 0 ; i < VALID_PARA_NUM ; i++)
    {
      memset(&tm, 0, sizeof(struct tm));
      ret = strptime(test_cases[i][0], test_cases[i][1], &tm);
      assert_non_null(ret);
      assert_int_equal(compare_cases[i][0], tm.tm_sec);
      assert_int_equal(compare_cases[i][1], tm.tm_min);
      assert_int_equal(compare_cases[i][2], tm.tm_hour);
      assert_int_equal(compare_cases[i][3], tm.tm_mday);
      assert_int_equal(compare_cases[i][4], tm.tm_mon);
      assert_int_equal(compare_cases[i][5], tm.tm_year);
      strftime(buffer, sizeof(buffer), test_cases[i][1], &tm);
      assert_string_equal(test_cases[i][0], buffer);
    }
}

DECLARE_TEST(strptime_invalid_para)
{
  int i;
  char *ret;
  struct tm tm;

  const char *test_cases[ERR_PARA_NUM][2] =
    {
      {"2024/10/28", "%Y-%m-%d"},
      {"2024-10-32", "%Y-%m-%d"}
    };

  const int compare_cases[ERR_PARA_NUM][6] =
    {
      {0, 0, 0, 28, 9, 124},
      {0, 0, 0, 32, 9, 124},
    };

  for (i = 0; i < ERR_PARA_NUM; i++)
    {
      memset(&tm, 0, sizeof(struct tm));
      ret = strptime(test_cases[i][0], test_cases[i][1], &tm);
      if (ret == NULL)
        {
          assert_int_not_equal(compare_cases[i][3], tm.tm_mday);
          assert_int_not_equal(compare_cases[i][4], tm.tm_mon);
          assert_int_equal(compare_cases[i][5], tm.tm_year);
        }
      else
        {
          assert_string_equal(ret, "");
          assert_int_equal(compare_cases[i][3], tm.tm_mday);
          assert_int_equal(compare_cases[i][4], tm.tm_mon);
          assert_int_equal(compare_cases[i][5], tm.tm_year);
        }
    }
}
