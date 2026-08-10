/****************************************************************************
 * apps/testing/libc/clocktest/timegm_test.c
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

#define UNIX_TSTAMP 1698566400

/****************************************************************************
 * Public Functions
 ****************************************************************************/

DECLARE_TEST(timegm_valid_para_01)
{
  struct tm time;
  time_t result;

  time.tm_year = 2023 - 1900;
  time.tm_mon = 10 - 1;
  time.tm_mday = 29;
  time.tm_hour = 8;
  time.tm_min = 0;
  time.tm_sec = 0;
  time.tm_isdst = -1;

  result = timegm(&time);
  assert_int_equal(result, UNIX_TSTAMP);
}

DECLARE_TEST(timegm_valid_para_02)
{
  struct tm time;
  time_t result;

  time.tm_year = 1970 - 1900;
  time.tm_mon = 0;
  time.tm_mday = 1;
  time.tm_hour = 0;
  time.tm_min = 0;
  time.tm_sec = 0;
  time.tm_isdst = -1;

  result = timegm(&time);
  assert_int_equal(result, 0);
}

DECLARE_TEST(timegm_invalid_para)
{
  struct tm time;
  time_t result;

  time.tm_year = 2023 - 1900;
  time.tm_mon = 2;
  time.tm_mday = 29;
  time.tm_hour = 12;
  time.tm_min = 0;
  time.tm_sec = 0;
  time.tm_isdst = -1;

  result = timegm(&time);
  assert_int_not_equal(result, -1);
}
