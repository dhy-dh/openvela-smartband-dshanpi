/****************************************************************************
 * apps/testing/drivers/can_chrdev/util/charcantest_util.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "charcantest_util.h"

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void show_usage(FAR const char *progname, int exitcode);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: show_usage
 ****************************************************************************/

static void show_usage(FAR const char *progname, int exitcode)
{
  printf("Usage: %s -s <sdev path> "
         "-r <rdev path> \n", progname);
  printf("example:\n");
  printf("cmocka_charcan or cmocka_charcan -s /dev/can0 -r /dev/can1\n");
  printf("default:\n");
  printf("default: <sdev path> is /dev/can0,"
         "<rdev path> is /dev/can1.\n");
  exit(exitcode);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: test_charcan_setup
 ****************************************************************************/

void test_charcan_parse_args(int argc, FAR char **argv,
                       FAR struct test_charcan_s *conf)
{
  int option;

  while ((option = getopt(argc, argv, "i:s:r:")) != ERROR)
    {
      switch (option)
        {
          case 'i':
            conf->can_id = atoi(optarg);
            break;
          case 's':
            conf->dev_path[0] = optarg;
            break;
          case 'r':
            conf->dev_path[1] = optarg;
            break;
          case '?':
            printf("Unknown option: %c\n", optopt);
            show_usage(argv[0], EXIT_FAILURE);
            break;
        }
    }
}

/****************************************************************************
 * Name: test_charcan_setup
 ****************************************************************************/

int test_charcan_setup(FAR void **state)
{
  FAR struct test_charcan_s *confs = (FAR struct test_charcan_s *)*state;
  uint8_t i;

  for (i = 0; i < USER_DEV_NUMBER; i++)
    {
      confs->fd[i] = open(confs->dev_path[i], O_RDWR | O_NONBLOCK);
      assert_true(confs->fd[i] > 0);
    }

  *state = confs;
  return 0;
}

/****************************************************************************
 * Name: test_charcan_teardown
 ****************************************************************************/

int test_charcan_teardown(FAR void **state)
{
  FAR struct test_charcan_s *confs = (FAR struct test_charcan_s *)*state;
  uint8_t i;

  for (i = 0; i < USER_DEV_NUMBER; i++)
    {
      assert_int_equal(close(confs->fd[i]), 0);
    }

  return 0;
}
