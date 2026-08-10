/****************************************************************************
 * apps/examples/memory_pressure/pressure_main.c
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

#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

static volatile bool g_exit = false;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void usage(void)
{
  printf("Usage: pressure -e <exit> -d <threshold> -h <help> "
         "-l <leak size> -m <malloc size>\n");
  printf("  -e <exit>        : exit deamon\n");
  printf("  -d <threshold>   : threshold in KB\n");
  printf("  -h <help>        : show this help\n");
  printf("  -l <leak size>   : malloc and leak size in KB\n");
  printf("  -m <malloc size> : malloc size in KB\n");
  printf("  -t <interval>    : notification interval in seconds\n");

  printf("Example:\n");

  /* When the maximum available memory is less than 1M,
   * an alarm will be issued every 5 seconds.
   */

  printf("  pressure -d 1024 -t 5\n");

  /* Allocate 1M memory, free memory after 1s */

  printf("  pressure -m 1024\n");

  /* Allocate and leak 1M memory */

  printf("  pressure -l 1024\n");
  exit(0);
}

static int monitor_memory_pressure(size_t threshold, int interval)
{
  struct pollfd fds[1];
  int ret;

  int fd = open("/proc/pressure/memory", O_RDWR);
  if (fd < 0)
    {
      perror("ERROR: Failed to open /proc/pressure/memory");
      return fd;
    }

  fds[0].fd = fd;
  fds[0].events = POLLPRI;

  ret = dprintf(fd, "%d %d\n", threshold * 1024, interval * 1000 * 1000);
  if (ret < 0)
    {
      perror("ERROR: write failed");
      close(fd);
      return ret;
    }

  printf("Starting memory pressure daemon with threshold %d bytes\n",
         threshold);
  while (!g_exit)
    {
      ret = poll(fds, sizeof(fds) / sizeof(fds[0]), 1000);
      if (ret < 0)
        {
          perror("ERROR: poll failed");
          close(fd);
          return ret;
        }
      else if (ret == 0)
        {
          continue;
        }

      if (fds[0].revents & POLLPRI)
        {
          char buf[128];
          lseek(fd, 0, SEEK_SET);
          ret = read(fd, buf, sizeof(buf));
          if (ret < 0)
            {
              perror("ERROR: read failed");
              close(fd);
              return ret;
            }

          /* The string returned by procfs does not have a '\0' terminator. */

          buf[ret] = '\0';

          syslog(LOG_WARNING, "Memory pressure notification! "
                 "Available memory is too low, %s", buf);
        }
    }

  close(fd);
  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  FAR const char *opts = "ed:hl:m:t:";
  FAR void *ptr = NULL;
  size_t size = 0;
  int exit = 0;
  int leak_size = 0;
  int malloc_size = 0;
  int threshold = 0;
  int interval = 0;
  int opt;

  while ((opt = getopt(argc, argv, opts)) != -1)
    {
      switch (opt)
      {
      case 'd':
        threshold = atoi(optarg);
        break;
      case 'e':
        exit = 1;
        break;
      case 'h':
        usage();
        break;
      case 'l':
        leak_size = atoi(optarg);
        break;
      case 'm':
        malloc_size = atoi(optarg);
        break;
      case 't':
        interval = atoi(optarg);
        break;
      default:
        usage();
        break;
      }
    }

  if (exit)
    {
      g_exit = true;
      return EXIT_SUCCESS;
    }

  /* Handle memory allocation options */

  if (leak_size > 0 || malloc_size > 0)
    {
      size = leak_size > 0 ? leak_size : malloc_size;
      ptr = malloc(size * 1024);
      if (ptr == NULL)
        {
          perror("ERROR: Failed to allocate memory");
          return EXIT_FAILURE;
        }

      /* Fill the memory to ensure it's actually allocated */

      memset(ptr, 0xaa, size);

      if (leak_size > 0)
        {
          /* Intentionally not freeing the memory to simulate leak */

          printf("Successfully allocated and leaked %d KB memory at %p\n",
                 leak_size, ptr);
          return EXIT_SUCCESS;
        }
      else
        {
          printf("Successfully allocated %d KB memory at %p, "
                 "will free after 1 second\n", malloc_size, ptr);
          sleep(1);
          free(ptr);
          printf("Memory freed\n");
          return EXIT_SUCCESS;
        }
    }

  /* Handle daemon option */

  if (threshold > 0 && interval > 0)
    {
      if (monitor_memory_pressure(threshold, interval) < 0)
        {
          perror("ERROR: Failed to start memory pressure daemon");
          return EXIT_FAILURE;
        }

      printf("Daemon exit requested\n");
      return EXIT_SUCCESS;
    }

  usage();
  return EXIT_SUCCESS;
}
