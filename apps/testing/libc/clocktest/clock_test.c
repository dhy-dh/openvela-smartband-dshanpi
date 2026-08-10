/****************************************************************************
 * apps/testing/libc/clocktest/clock_test.c
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

#define TEST(name) test_##name \

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  const struct CMUnitTest cmocka_clock_tests[] =
    {
      cmocka_unit_test(TEST(timespec_get)),
      cmocka_unit_test(TEST(gethrtime)),
      cmocka_unit_test(TEST(strptime_valid_para)),
      cmocka_unit_test(TEST(strptime_invalid_para)),
      cmocka_unit_test(TEST(timegm_valid_para_01)),
      cmocka_unit_test(TEST(timegm_valid_para_02)),
      cmocka_unit_test(TEST(timegm_invalid_para)),
    };

  return cmocka_run_group_tests(cmocka_clock_tests, NULL, NULL);
}
