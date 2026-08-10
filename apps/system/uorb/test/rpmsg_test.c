/****************************************************************************
 * apps/system/uorb/test/rpmsg_test.c
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

#ifdef __NuttX__
#include <nuttx/uorb.h>
#else
#include <linux/uorb.h>
#endif

#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <sensor/accel.h>
#include "utility.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct orb_test_rpmsg_s
{
  uint64_t timestamp;
  int32_t val;
  char str[128];
};

ORB_DECLARE(orb_test_rpmsg);

static const char test_rpmsg_format[] =
  "timestamp:%" PRIu64 ",val:%" PRId32 ",str:[%s]";

ORB_DEFINE(orb_test_rpmsg, struct orb_test_rpmsg_s, test_rpmsg_format);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void test1(void)
{
  struct orb_test_rpmsg_s sample;
  int instance = 0;
  int afd;

  test_note("adv and pub, kill -9 ->unadv\n");

  sample.val = 100;
  afd = orb_advertise_multi(ORB_ID(orb_test_rpmsg), &sample, &instance);
  if (afd < 0)
    {
      test_note("advertise failed: %d", errno);
    }

  while (1)
    {
      sleep(1);
      sample.timestamp = orb_absolute_time();
      sample.val++;
      snprintf(sample.str, 128, "%s:%d hello %" PRId32,
               CONFIG_RPMSG_LOCAL_CPUNAME, getpid(), sample.val);
      orb_publish(ORB_ID(orb_test_rpmsg), afd, &sample);
    }

  orb_unadvertise(afd);
}

static void test2(void)
{
  struct orb_test_rpmsg_s sample;
  int instance = 0;
  int afd;

  test_note("adv, kill -9 ->unadv\n");

  sample.val = 0;
  afd = orb_advertise_multi(ORB_ID(orb_test_rpmsg), &sample, &instance);
  if (afd < 0)
    {
      test_note("advertise failed: %d", errno);
    }

  while (1)
    {
      sleep(1);
    }

  orb_unadvertise(afd);
}

static void test3(void)
{
  orb_id_t meta = ORB_ID(orb_test_rpmsg);
  struct orb_test_rpmsg_s sample;
  struct pollfd fds[1];
  int sfd;
  int ret;

  test_note("sub copy, kill -9 ->unsub\n");
  sfd = orb_subscribe_multi(ORB_ID(orb_test_rpmsg), 0);
  if (sfd < 0)
    {
      test_fail("subscribe failed: %d", errno);
    }

  fds[0].fd = sfd;
  fds[0].events = POLLIN;
  while (1)
    {
      ret = poll(fds, 1, -1);
      if (ret < 0)
        {
          test_fail("poll failed, ret:%d\n", ret);
          break;
        }

      orb_copy(ORB_ID(orb_test_rpmsg), sfd, &sample);
      orb_info(meta->o_format, meta->o_name, &sample);
    }

  orb_unsubscribe(sfd);
}

static void test4(void)
{
  int sfd;

  test_note("sub, kill -9->unsub\n");
  sfd = orb_subscribe_multi(ORB_ID(orb_test_rpmsg), 0);
  if (sfd < 0)
    {
      test_fail("subscribe failed: %d", errno);
    }

  while (1)
    {
      sleep(1);
    }

  orb_unsubscribe(sfd);
}

static void test6(void)
{
  int sfd;
  struct sensor_ioctl_s *ioctl;
  char str[] = "hello x";
  char str1[] = "hello ld";
  int ret;

  ioctl = malloc(sizeof(*ioctl) + 20);
  if (!ioctl)
    {
      return;
    }

  memset(ioctl, 0, sizeof(*ioctl) + 20);
  ioctl->len = strlen(str);
  strcpy(ioctl->data, str);

  test_note("sub, ioctl kill -9 > unsub\n");
  sfd = orb_subscribe_multi(ORB_ID(sensor_accel), 0);
  if (sfd < 0)
    {
      test_fail("subscribe failed: %d", errno);
    }

  sleep(1);
  ret = orb_ioctl(sfd, 0x1122, (unsigned long)(uintptr_t)ioctl);
  test_note("1122 orb ioctl ret:%d, %d, len:%"PRIu32"\n", ret, errno,
         ioctl->len);

  ioctl->len = strlen(str1);
  strcpy(ioctl->data, str1);
  ret = orb_ioctl(sfd, 0x1133, (unsigned long)(uintptr_t)ioctl);
  test_note("1133 orb ioctl ret:%d, %d, len:%"PRIu32"\n", ret, errno,
         ioctl->len);
  free(ioctl);

  ret = orb_ioctl(sfd, SNIOC_SELFTEST, 999);
  test_note("999 orb ioctl selftest ret:%d, %d\n", ret, errno);
  ret = orb_ioctl(sfd, SNIOC_SELFTEST, 101);
  test_note("101 orb ioctl selftest ret:%d, %d\n", ret, errno);
  ret = orb_ioctl(sfd, SNIOC_SELFTEST, 100);
  test_note("100 orb ioctl selftest ret:%d, %d\n", ret, errno);

  char arg1[] = "999";
  ret = orb_ioctl(sfd, SNIOC_SET_CALIBVALUE, (unsigned long)(uintptr_t)arg1);
  test_note("arg1 orb ioctl selftest ret:%d, %d\n", ret, errno);
  char arg2[] = "111";
  ret = orb_ioctl(sfd, SNIOC_SET_CALIBVALUE, (unsigned long)(uintptr_t)arg2);
  test_note("arg2 orb ioctl selftest ret:%d, %d\n", ret, errno);
  char arg3[] = "333";
  ret = orb_ioctl(sfd, SNIOC_SET_CALIBVALUE, (unsigned long)(uintptr_t)arg3);
  test_note("arg3 orb ioctl selftest ret:%d, %d\n", ret, errno);

  orb_unsubscribe(sfd);
}

static void test(int num)
{
  switch (num)
    {
    case 1:
      test1();
      break;

    case 2:
      test2();
      break;

    case 3:
      test3();
      break;

    case 4:
      test4();
      break;

    case 6:
      test6();
      break;
    }
}

int main(int argc, FAR char *argv[])
{
  struct sensor_state_s state;
  int ret;

  if (argc < 2)
    {
      test_note("input parameter is invalid\n");
      test_note("uorb_rpmsg 1 & :adv, publish, kill -9 unadv\n");
      test_note("uorb_rpmsg 2 & :adv, kill -9 unadv\n");
      test_note("uorb_rpmsg 3 & :sub, copy, kill -9 unsub\n");
      test_note("uorb_rpmsg 4 & :sub, kill -9 unsub\n");
      test_note("uorb_rpmsg 5 :check node state\n");
      test_note("uorb_rpmsg 6 verify ioctl \n");
      return 0;
    }

  if (atoi(argv[1]) == 5)
    {
      ret = open(argv[2], 0);
      if (ret >= 0)
        {
          if (ioctl(ret, SNIOC_GET_STATE, &state) < 0)
            {
              test_note("get state failed. errno:%d", errno);
            }
          else
            {
              test_note("nadv:%"PRIu32", nsub:%"PRIu32"\n",
                        state.nadvertisers, state.nsubscribers);
            }

          close(ret);
        }
      else if(errno == ENOENT)
        {
          test_note("nadv:0, nsub:0");
        }
      else
        {
          test_note("failed to query path:%s. errno:%d", argv[2], errno);
        }
    }
  else
    {
      test(atoi(argv[1]));
    }

  return 0;
}
