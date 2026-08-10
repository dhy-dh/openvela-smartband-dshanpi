/****************************************************************************
 * apps/testing/drivers/can_chrdev/util/charcantest_util.h
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

#ifndef _H_CM_CHARCANTEST_UTIL_H
#define _H_CM_CHARCANTEST_UTIL_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <nuttx/config.h>
#include <nuttx/can/can.h>
#include <nuttx/can.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CAN_MSG_NUMBER  8
#define USER_DEV_NUMBER 2

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct test_charcan_s
{
  /* dev_path[0]: sender dev path, dev_path[1]: receiver dev path */

  FAR const char  *dev_path[USER_DEV_NUMBER];
  struct can_msg_s txmsg[CAN_MSG_NUMBER];
  int              fd[USER_DEV_NUMBER];
  int              can_id;
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void test_charcan_parse_args(int argc, FAR char **argv,
                             FAR struct test_charcan_s *conf);
int test_charcan_setup(FAR void **state);
int test_charcan_teardown(FAR void **state);

#endif // _H_CM_CHARCANTEST_UTIL_H