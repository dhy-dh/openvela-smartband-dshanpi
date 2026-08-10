/****************************************************************************
 * apps/system/perf-tools/builtin-write.c
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

#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "builtin.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void show_usage(void)
{
  printf("perf write [arguments...]\n");
  printf("\t-a addr -s size\n");
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int cmd_write(int argc, FAR const char **argv)
{
  int opt;
  uintptr_t addr = 0;
  uint32_t size = 0;
  struct stat st;
  int fd;

  while ((opt = getopt(argc, (char **)argv, "a:s:")) != ERROR)
    {
      switch (opt)
        {
          case 'a':
            addr = strtoul(optarg, NULL, 16);
            break;
          case 's':
            size = strtoul(optarg, NULL, 16);
            break;
          default:
            show_usage();
            return -EINVAL;
        }
    }

  if (stat(PERF_SAVE_FILE_NAME, &st) < 0)
    {
      fprintf(stderr, "stat failed\n");
      return -EINVAL;
    }

  if (st.st_size > size)
    {
      fprintf(stderr, "file size %ld large\n", st.st_size);
      return -EINVAL;
    }

  fd = open(PERF_SAVE_FILE_NAME, O_RDONLY);

  if (fd < 0)
    {
      fprintf(stderr, "cannot open file\n");
      return -EINVAL;
    }

  if (read(fd, (uint8_t *)addr, st.st_size) < 0)
    {
      fprintf(stderr, "cannot read file\n");
      return -EINVAL;
    }

  close(fd);

  return 0;
}
