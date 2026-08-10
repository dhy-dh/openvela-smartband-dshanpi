/****************************************************************************
 * apps/system/lsan/lsan_main.c
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

#include <nuttx/config.h>
#include <stdio.h>
#include <unistd.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: show_usage
 ****************************************************************************/

static void show_usage(void)
{
  fprintf(stderr, "Usage: lsan [-i interval] [-c count]\n");
  fprintf(stderr, "  -i interval: Check for leaks every interval seconds\n");
  fprintf(stderr, "  -c count:    Check for leaks count times\n");
  exit(EXIT_FAILURE);
}

/****************************************************************************
 * Name: lsan_main
 *
 * Description:
 *   Main entry point for the lsan command.
 *
 * Input Parameters:
 *   interval - The interval between leak checks. Default is 1 seconds.
 *   count - The number of times to check for leaks. A value of -1 means
 *           to run forever.
 ****************************************************************************/

extern void __lsan_do_recoverable_leak_check(void);

int main(int argc, FAR char *argv[])
{
  int ch;
  int interval = 5;
  int count = -1;

  if (argc == 1 || argv[1][0] != '-')
    show_usage();

  while ((ch = getopt(argc, argv, "i:c:h:")) != EOF)
    {
      switch (ch)
        {
          case 'i':
            interval = atoi(optarg);
            break;
          case 'c':
            count = atoi(optarg);
            break;
          case 'h':
          default:
            show_usage();
        }
    }

  do
    {
      __lsan_do_recoverable_leak_check();
      sleep(interval);
    }
  while (--count != 0);

  return 0;
}
