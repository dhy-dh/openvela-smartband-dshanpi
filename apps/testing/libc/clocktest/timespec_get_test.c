/****************************************************************************
 * apps/testing/libc/clocktest/timespec_get_test.c
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
 * Public Functions
 ****************************************************************************/

DECLARE_TEST(timespec_get)
{
  int ret;
  struct timespec ts;

  ret = timespec_get(&ts, TIME_UTC);
  assert_int_equal(ret, TIME_UTC);

  ret = timespec_get(NULL, TIME_UTC);
  assert_int_not_equal(ret, TIME_UTC);

  ret = timespec_get(&ts, -1);
  assert_int_not_equal(ret, TIME_UTC);

  ret = timespec_get(&ts, TIME_UTC + 1);
  assert_int_not_equal(ret, TIME_UTC + 1);
}
