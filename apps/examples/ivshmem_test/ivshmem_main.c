/****************************************************************************
 * apps/examples/ivshmem_test/ivshmem_main.c
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
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define U8  8

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: print_usage
 ****************************************************************************/

static void print_usage(FAR const char *progname)
{
  fprintf(stderr, "Usage: %s [-d device] [-m memsize]\n", progname);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -d <device>   Path to UIO device   (default: %s)\n",
         CONFIG_EXAMPLES_IVSHMEM_DEVICE);
  fprintf(stderr, "  -m <memsize>  Memory size in bytes (default: %d)\n",
         CONFIG_EXAMPLES_IVSHMEM_MEMSIZE);
  fprintf(stderr, "  -h            Show this help message\n");
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * ivshmem_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  FAR void    *mmap_addr;
  FAR uint8_t *byte_ptr;
  int i;
  int j;
  int fd;
  int opt;
  int ret                = OK;
  int memsize            = CONFIG_EXAMPLES_IVSHMEM_MEMSIZE;
  FAR const char *device = CONFIG_EXAMPLES_IVSHMEM_DEVICE;

  /* USAGE: nsh> ivshmem_test [-d <devname>] [-m <memsize>] */

  while ((opt = getopt(argc, argv, "d:m:h")) != -1)
    {
      switch (opt)
        {
          case 'd':
            device = optarg;
            break;

          case 'm':
            memsize = atoi(optarg);
            break;

          case 'h':
          default:
            print_usage(argv[0]);
            return 0;
        }
    }

  /* Open pci uio shared memory */

  fd = open(device, O_RDWR | O_SYNC);
  if (fd < 0)
    {
      ret = -errno;
      fprintf(stderr, "ERROR: Failed to open %s\n", device);
      return ret;
    }

  /* Mmap shared memory */

  mmap_addr = mmap(NULL, memsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mmap_addr == MAP_FAILED)
    {
      ret = -errno;
      fprintf(stderr, "ERROR: Failed to mmap shared memory address\n");
      close(fd);
      return ret;
    }

  /* Read from shared memory */

  byte_ptr = (uint8_t *)mmap_addr;

  for (i = 0; i < memsize / U8; i++)
    {
      printf("0x");
      for (j = 0; j < U8; j++)
        {
          printf("%02x", byte_ptr[j]);
        }

      printf("\n");
      byte_ptr += 8;
    }

  /* Unmap shared memory */

  munmap(mmap_addr, memsize);
  close(fd);

  return 0;
}
