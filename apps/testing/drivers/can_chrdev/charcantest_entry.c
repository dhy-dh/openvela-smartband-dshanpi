/****************************************************************************
 * apps/testing/drivers/can_chrdev/charcantest_entry.c
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

#include "charcantest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: cmocka_test_main
 ****************************************************************************/

int main(int argc, FAR char * argv[])
{
  struct test_charcan_s confs =
    {
      .dev_path[0] = CONFIG_TESTING_CHARCAN_SENDER_DEVICE,
      .dev_path[1] = CONFIG_TESTING_CHARCAN_RECEIVER_DEVICE,
      .can_id      = CONFIG_TESTING_CHARCAN_CANMSG_ID,
    };

  test_charcan_parse_args(argc, argv, &confs);

  /* Add Test Cases */

  const struct CMUnitTest charcantestsuite[] =
    {
      cmocka_unit_test_prestate_setup_teardown(
      test_char_can_controller, test_charcan_setup,
      test_charcan_teardown, &confs),
      cmocka_unit_test_prestate_setup_teardown(
      test_char_can_transceiver, test_charcan_setup,
      test_charcan_teardown, &confs),
      cmocka_unit_test_prestate_setup_teardown(
      test_charcan_multi_echo, test_charcan_setup,
      test_charcan_teardown, &confs),
      cmocka_unit_test_prestate_setup_teardown(
      test_charcan_nonblock_sent_overflow, test_charcan_setup,
      test_charcan_teardown, &confs),
      cmocka_unit_test_prestate_setup_teardown(
      test_charcan_block_sent_overflow, test_charcan_setup,
      test_charcan_teardown, &confs),
      cmocka_unit_test_prestate_setup_teardown(
      test_char_can_poll, test_charcan_setup,
      test_charcan_teardown, &confs),
    };

  /* Run Test cases */

  return cmocka_run_group_tests(charcantestsuite, NULL, NULL);
}
