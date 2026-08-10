/****************************************************************************
 * apps/testing/drivers/can_chrdev/src/test_char_can_overflow.c
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
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include "charcantest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CM_CHRDEV_CANMSG_NUM (CONFIG_CAN_TXFIFOSIZE + 1)
#define CM_CHRDEV_CANMSG_LEN 12
#define CM_CHRDEV_CANMSG_TOTAL_SIZE \
       (CM_CHRDEV_CANMSG_LEN * CM_CHRDEV_CANMSG_NUM)

/****************************************************************************
 * Public Data
 ****************************************************************************/

static char g_cm_chrdev_canmsg_buf[CM_CHRDEV_CANMSG_TOTAL_SIZE];

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: test_charcan_nonblock_sent_overflow
 * Description:
 *   Check whether overflow occurs in the send process, the send
 *   process is with "NONBLOCK" property.
 ****************************************************************************/

void test_charcan_nonblock_sent_overflow(FAR void **state)
{
  FAR struct test_charcan_s *confs     = (FAR struct test_charcan_s *)*state;
  uint8_t                    canmsg_databyte;
  size_t                     msgsize;
  int                        ret;
  int                        i;

  assert_true(confs && confs->txmsg != NULL);

  ret = ioctl(confs->fd[0], CANIOC_SET_STATE, CAN_STATE_STOP);
  assert_true(ret >= 0);

  /* initilize confs->txmsg */

  init_can_txmsg(&confs->txmsg[0]);
  canmsg_databyte = cm_charcan_dlc2bytes(confs->txmsg[0].cm_hdr.ch_dlc);
  msgsize         = CAN_MSGLEN(canmsg_databyte);

  for (i = 0; i <= CONFIG_CAN_TXFIFOSIZE; i++)
    {
      write(confs->fd[0], &confs->txmsg[0], msgsize);
    }

  /* check whether overflow in the send process. */

  assert_true(errno == EAGAIN);

  ret = ioctl(confs->fd[0], CANIOC_OFLUSH, NULL);
  assert_true(ret == 0);

  ret = ioctl(confs->fd[0], CANIOC_SET_STATE, CAN_STATE_START);
  assert_true(ret == 0);
}

/****************************************************************************
 * Name: test_charcan_block_sent_overflow
 * Description:
 *   Check whether overflow occurs in the send process, the send
 *   process is with "BLOCK" property.
 ****************************************************************************/

void test_charcan_block_sent_overflow(FAR void **state)
{
  FAR struct test_charcan_s *confs = (FAR struct test_charcan_s *)*state;
  uint8_t                    canmsg_databyte;
  size_t                     msgsize;
  ssize_t                    nbytes;
  int                        flags;
  int                        i;

  assert_true(confs && confs->txmsg != NULL);

  /* set the fd to block property */

  flags = fcntl(confs->fd[0], F_GETFL, 0);
  fcntl(confs->fd[0], F_SETFL, flags & ~O_NONBLOCK);
  flags = fcntl(confs->fd[0], F_GETFL, 0);
  assert_true((flags & O_NONBLOCK) == 0);

  /* initilize confs->txmsg */

  init_can_txmsg(&confs->txmsg[0]);
  canmsg_databyte = cm_charcan_dlc2bytes(confs->txmsg[0].cm_hdr.ch_dlc);
  msgsize         = CAN_MSGLEN(canmsg_databyte);

  for (i = 0; i <= CONFIG_CAN_TXFIFOSIZE; i++)
    {
      memcpy(g_cm_chrdev_canmsg_buf + i * msgsize,
             &confs->txmsg[0], msgsize);
    }

  nbytes = write(confs->fd[0], g_cm_chrdev_canmsg_buf,
                 sizeof(g_cm_chrdev_canmsg_buf));

  /* check whether overflow in the send process. */

  assert_true(nbytes == sizeof(g_cm_chrdev_canmsg_buf));
}
