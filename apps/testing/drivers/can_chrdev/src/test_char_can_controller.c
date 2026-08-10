/****************************************************************************
 * apps/testing/drivers/can_chrdev/src/test_char_can_controller.c
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

#include "charcantest.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: test_char_can_controller
 * Description:
 *   This function control charcan controller and acquire the result.
 ****************************************************************************/

void test_char_can_controller(FAR void **state)
{
  FAR struct test_charcan_s *confs = (FAR struct test_charcan_s *)*state;
  size_t controller_states_size;
  unsigned long mode;
  uint8_t i;
  uint8_t j;
  int ret;

  /* Set controller mode */

  unsigned long controller_states[] =
    {
      CAN_STATE_STOP,
      CAN_STATE_START
    };

  controller_states_size = nitems(controller_states);

  for (i = 0; i < USER_DEV_NUMBER; i++)
    {
      for (j = 0; j < controller_states_size; j++)
        {
          /* Set controller mode */

          ret = ioctl(confs->fd[i], CANIOC_SET_STATE,
                      controller_states[j]);
          assert_true(ret >= 0);

          /* Get controller mode */

          ret = ioctl(confs->fd[i], CANIOC_GET_STATE,
                      (unsigned long)(&mode));
          assert_true(ret >= 0 && mode == controller_states[j]);
        }
    }
}
