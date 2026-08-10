/****************************************************************************
 * apps/examples/spi/spi_main.c
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

#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <syslog.h>

#include <nuttx/spi/spi.h>
#include <nuttx/spi/spi_transfer.h>

#define BUFFER_SIZE 8
#define MAX_LOOP 10

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * spi_main
 ****************************************************************************/

/* Certain SPI devices may required different clocking modes */

int main(int argc, FAR char *argv[])
{
  int ret = 0;
  int fd = -1;
  char dev_name[20] = "/dev/spi2";
  int i;
  int loop_count = 0;
  bool running = true;

  struct spi_sequence_s cmd =
    {
      .dev = 2,
      .mode = 0,
      .nbits = 8,
      .ntrans = 1,
      .frequency = 400000,
    };

  /* Declare a local spi_trans_s variable */

  struct spi_trans_s local_trans =
    {
      .deselect = 1,
      .delay = 1000,
      .nwords = BUFFER_SIZE
    };

  /* Allocate memory for txbuffer */

  local_trans.txbuffer = (void *)malloc(local_trans.nwords、
                          * (cmd.nbits / 8));
  if (local_trans.txbuffer == NULL)
    {
      syslog(LOG_ERR, "Memory allocation for txbuffer failed!\n");
      ret = -1;
      goto cleanup;
    }

  /* Allocate memory for rxbuffer */

  local_trans.rxbuffer = (void *)malloc(local_trans.nwords
                          * (cmd.nbits / 8));
  if (local_trans.rxbuffer == NULL)
    {
      syslog(LOG_ERR, "Memory allocation for rxbuffer failed!\n");
      ret = -1;
      goto cleanup_txbuffer;
    }

  /* Fill txbuffer with data (e.g., 'A') and rxbuffer with zero */

  memset(local_trans.txbuffer, 'A', local_trans.nwords);
  memset(local_trans.rxbuffer, 0, local_trans.nwords);

  /* Assign the address of local_trans to cmd.trans */

  cmd.trans = &local_trans;

  /* Open the SPI device for the transfer */

  fd = open(dev_name, O_RDWR);

  if (fd < 0)
    {
      syslog(LOG_INFO, "open failed: %d\n", fd);
      ret = -1;
      goto cleanup_rxbuffer;
    }

  while (running)
    {
      ret = ioctl(fd, SPIIOC_TRANSFER, &cmd);
      if (ret < 0)
        {
          syslog(LOG_INFO, "spi: ioctl(SPIIOC_TRANSFER) failed: %d\n",
                ret);
        }

      syslog(LOG_INFO, "read result:\n");
      for (i = 0 ; i < local_trans.nwords; i++)
        {
          syslog(LOG_INFO, "%02x \n", ((char *)cmd.trans->rxbuffer)[i]);
        }

      syslog(LOG_INFO, "read end\n");

      sleep(1);

      loop_count++;
      if (loop_count >= MAX_LOOP)
        {
          running = false;
        }
    }

  close(fd);

  /* Error handling */

cleanup_rxbuffer:
  free(local_trans.rxbuffer);
cleanup_txbuffer:
  free(local_trans.txbuffer);
cleanup:
  return ret;
}
