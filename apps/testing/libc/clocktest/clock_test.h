/****************************************************************************
 * apps/testing/libc/clocktest/clock_test.h
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

#ifndef __INCLUDE_APPS_TESTING_CLOCKTEST_TEST_H
#define __INCLUDE_APPS_TESTING_CLOCKTEST_TEST_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define DECLARE_TEST(name)                 \
    void test_##name(void** state)         \

DECLARE_TEST(timespec_get);

DECLARE_TEST(gethrtime);

DECLARE_TEST(strptime_valid_para);

DECLARE_TEST(strptime_invalid_para);

DECLARE_TEST(timegm_valid_para_01);

DECLARE_TEST(timegm_valid_para_02);

DECLARE_TEST(timegm_invalid_para);

#endif /* __INCLUDE_APPS_TESTING_CLOCKTEST_TEST_H */
