/****************************************************************************
 * apps/testing/drivers/can_chrdev/include/charcantest.h
 *
 * Copyright (C) 2024 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#ifndef _H_CHAR_CAN_TEST_H_
#define _H_CHAR_CAN_TEST_H_

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "charcantest_util.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CM_CANMSG_ID 0x111

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

static inline void init_can_txmsg(FAR struct can_msg_s *txmsg)
{
  /* initilize confs->txmsg */

  memset(&txmsg->cm_hdr, 0, sizeof(struct can_hdr_s));

  txmsg->cm_hdr.ch_id  = CM_CANMSG_ID;
  txmsg->cm_hdr.ch_dlc = CAN_MSG_NUMBER;
  txmsg->cm_hdr.ch_edl = 0;
}

static inline uint8_t cm_charcan_dlc2bytes(uint8_t dlc)
{
  if (dlc > 8)
    {
#ifdef CONFIG_CAN_FD
      switch (dlc)
        {
          case 9:
            return 12;

          case 10:
            return 16;

          case 11:
            return 20;

          case 12:
            return 24;

          case 13:
            return 32;

          case 14:
            return 48;

          default:
          case 15:
            return 64;
        }
#else
      return 8;
#endif
    }

  return dlc;
}

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* TEST CASES FUNCTIONS */

void test_char_can_controller(FAR void **state);
void test_char_can_transceiver(FAR void **state);
void test_charcan_multi_echo(FAR void **state);
void test_charcan_nonblock_sent_overflow(FAR void **state);
void test_charcan_block_sent_overflow(FAR void **state);
void test_char_can_poll(FAR void **state);

#endif /* _H_CHAR_CAN_TEST_H_ */
