/****************************************************************************
 * apps/testing/drivers/can_chrdev/src/test_char_can_poll.c
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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <poll.h>
#include <unistd.h>

#include "charcantest.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: test_char_can_poll
 * Description:
 *   This function perform poll logic and compare txmsg with rxmsg.
 ****************************************************************************/

void test_char_can_poll(FAR void **state)
{
  FAR struct test_charcan_s *confs = (FAR struct test_charcan_s *)*state;
  struct pollfd              fds;
  uint8_t                    canmsg_databyte;
  size_t                     msgsize;
  ssize_t                    nbytes;
  int                        ret;

  struct can_msg_s rxmsg =
    {
      0
    };

  /* initilize confs->txmsg */

  init_can_txmsg(&confs->txmsg[0]);
  canmsg_databyte = cm_charcan_dlc2bytes(confs->txmsg[0].cm_hdr.ch_dlc);
  msgsize         = CAN_MSGLEN(canmsg_databyte);

  /* initilize struct about poll */

  fds.fd      = confs->fd[1];
  fds.events  = POLLIN;

  nbytes = write(confs->fd[0], &confs->txmsg[0], msgsize);
  assert_int_equal(msgsize, nbytes);

  ret = poll(&fds, 1, 1000);
  assert_true(ret > 0);

  if (fds.revents & POLLIN)
    {
      nbytes = read(confs->fd[1], &rxmsg, sizeof(struct can_msg_s));
      assert_int_equal(memcmp(&confs->txmsg[0], &rxmsg, msgsize), 0);
    }
}
