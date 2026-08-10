/****************************************************************************
 * apps/netutils/rpmsg_tun/rpmsg_tun_client.c
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

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "rpmsg_tun_util.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: main
 ****************************************************************************/

int main(int argc, char *argv[])
{
  int nlfd = -1;
  int rpmsgfd;
  int tunfd;
  int ret;

  if (argc != 5 && argc != 6)
    {
      fprintf(stderr, "Usage: %s <tunname> <ip> <mask> <cpu> <ifname> or\n"
                      "Usage: %s <tunname> <ip> <mask> <cpu>\n",
                      argv[0], argv[0]);
      return -EINVAL;
    }

  tunfd = rpmsg_tun_setup(argv[1], argv[2], argv[3]);
  if (tunfd < 0)
    {
      return tunfd;
    }

  if (argc == 6)
    {
      nlfd = rpmsg_tun_connect_netlink();
      if (nlfd < 0)
        {
          close(tunfd);
          return nlfd;
        }
    }

  for (; ; )
    {
      if (nlfd >= 0)
        {
          ret = rpmsg_tun_wait_running(nlfd, argv[5]);
          if (ret < 0)
            {
              break;
            }
        }

      rpmsgfd = rpmsg_tun_connect(argv[4], argv[1]);
      if (rpmsgfd < 0)
        {
          sleep(3);
          continue;
        }

      ret = rpmsg_tun_loop(tunfd, rpmsgfd, nlfd, argc == 6 ? argv[5] : NULL);
      close(rpmsgfd);
      if (ret < 0)
        {
          break;
        }
    }

  if (nlfd >= 0)
    {
      close(nlfd);
    }

  close(tunfd);
  return ret;
}
