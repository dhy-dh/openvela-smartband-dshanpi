/****************************************************************************
 * apps/testing/drivers/can_chrdev/src/test_char_can_echo.c
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

#include <assert.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "charcantest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CM_CLASS_CAN_DLC 8

/****************************************************************************
 * Public Data
 ****************************************************************************/

extern uint8_t g_can_chrdev0_txconfirm_cnt;
extern uint8_t g_can_chrdev1_txconfirm_cnt;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void can_echo(int sender, int receiver, struct can_msg_s *txmsg,
                     int msgsize)
{
  struct can_msg_s tmp;
  ssize_t nbytes;
  int cnt = 0;
  int ret;

  nbytes = write(sender, txmsg, msgsize);

  assert_int_equal(msgsize, nbytes);
  usleep(100000);

#ifdef CONFIG_CAN_TXCONFIRM
  /* Check whether the txconfirm msg is sent */

  ret = ioctl(sender, FIONREAD, &cnt);
  assert(ret == 0);
  assert_int_equal(cnt, 1);

  /* Read the txconfirm msg */

  memset(&tmp, 0, sizeof(struct can_msg_s));
  nbytes = read(sender, &tmp, sizeof(struct can_msg_s));
  assert_int_equal(nbytes, sizeof(struct can_msg_s));
  assert_int_equal(tmp.cm_hdr.ch_tcf, 1);
#endif

  ret = ioctl(receiver, FIONREAD, &cnt);
  assert(ret == 0);
  assert_int_equal(cnt, 1);

  /* Read the msg which is echoed by sender */

  memset(&tmp, 0, sizeof(struct can_msg_s));
  nbytes = read(receiver, &tmp, sizeof(struct can_msg_s));
  assert_int_equal(nbytes, sizeof(struct can_msg_s));
  assert_int_equal(memcmp(&tmp, txmsg, msgsize), 0);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: test_charcan_milti_echo
 * Description:
 *   This function send can message and check whether the sending result is
 *   right or not.
 ****************************************************************************/

void test_charcan_multi_echo(FAR void **state)
{
  FAR struct test_charcan_s *confs = (FAR struct test_charcan_s *)*state;
  uint8_t canmsg_databyte;
  size_t msgsize;
  int i;
  int j;

  assert_true(confs && confs->txmsg != NULL);

  for (i = 0; i < CAN_MSG_NUMBER; i++)
    {
      /* initilize confs->txmsg */

      memset(&(confs->txmsg[i].cm_hdr), 0, sizeof(struct can_hdr_s));
      confs->txmsg[i].cm_hdr.ch_id  = confs->can_id;
      confs->txmsg[i].cm_hdr.ch_dlc = CAN_MSG_NUMBER + i;
      confs->txmsg[i].cm_hdr.ch_edl = confs->txmsg[i].cm_hdr.ch_dlc >
                                      CM_CLASS_CAN_DLC ? 1 : 0;

      canmsg_databyte = cm_charcan_dlc2bytes(confs->txmsg[i].cm_hdr.ch_dlc);

      for (j = 0; j < canmsg_databyte; j++)
        {
          confs->txmsg[i].cm_data[j] = i + j;
        }

      msgsize = CAN_MSGLEN(canmsg_databyte);

      /* check whether the sending result is right or not */

      can_echo(confs->fd[0], confs->fd[1], &confs->txmsg[i],
               msgsize);
      can_echo(confs->fd[1], confs->fd[0], &confs->txmsg[i],
               msgsize);
    }
}
