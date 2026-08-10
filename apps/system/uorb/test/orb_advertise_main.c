/****************************************************************************
 * apps/system/uorb/test/orb_advertise_main.c
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

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <uORB/uORB.h>
#include <sensor/rgb.h>
#include "utility.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static bool g_should_exit = false;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void exit_handler(int signo)
{
  g_should_exit = true;
}

/****************************************************************************
 * orb_advertise_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int afd;
  struct sensor_rgb rgb;

  if (signal(SIGINT, exit_handler) == SIG_ERR)
    {
      return -errno;
    }

  g_should_exit = false;
  afd = orb_advertise(ORB_ID(sensor_rgb), NULL);
  if (afd < 0)
    {
      test_fail("failed to orb advertise, afd:%d\n", afd);
      return afd;
    }

  test_note("orb advertise: sensor_rgb\n");

  memset(&rgb, 0, sizeof(rgb));
  while (!g_should_exit)
    {
      rgb.timestamp = orb_absolute_time();
      rgb.r++;
      rgb.g++;
      rgb.b++;
      orb_publish(ORB_ID(sensor_rgb), afd, &rgb);
      sleep(1);
    }

  test_note("orb unadvertise: sensor_rgb\n");
  orb_unadvertise(afd);

  return 0;
}
