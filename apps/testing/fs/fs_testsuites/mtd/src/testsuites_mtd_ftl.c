/****************************************************************************
 * apps/testing/fs/fs_testsuites/mtd/src/testsuites_mtd_ftl.c
 *
 * Original Licence:
 *
 *   Copyright (c) 2016-2021 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <cmocka.h>
#include <nuttx/mtd/mtd.h>

#include "testsuites_mtd.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define FTL_TEST_DRIVER_NAME "/dev/ftl_testdriver"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * testsuites_mtd_ftl_01
 ****************************************************************************/

void testsuites_mtd_ftl_01(FAR void **state)
{
  FAR struct testsuites_mtd_driver *driver;
  FAR struct inode *inode;
  FAR unsigned char *output;
  FAR unsigned char *input;
  struct mtd_geometry_s ioctl_geo;
  struct geometry geometry;
  ssize_t nwriten;
  uint16_t blkper;
  int result;
  int i;

  /* This test is used to check ftl basic call */

  driver = *state;
  input = zalloc(MTD_BUFFERSIZE * 2);
  assert_true(input != NULL);
  output = input + MTD_BUFFERSIZE;

  /* Check ftl_initialize_by_path
   * Note: ftl_initialize equivalent to ftl_initialize_by_path
   * The purpose of ftl is to simulator a mtd driver into a block driver.
   */

  result = ftl_initialize_by_path(FTL_TEST_DRIVER_NAME, driver->mtd);
  assert_false(result != 0);

  result = find_blockdriver(FTL_TEST_DRIVER_NAME, 0, &inode);
  assert_false(result != 0);

  /* Check ftl_open */

  result = inode->u.i_bops->open(inode);
  assert_false(result != 0);

  /* Check ftl_write / ftl_read */

  for (i = 0; i < MTD_BUFFERSIZE; i++)
    {
      input[i] = rand() % 256;
    }

  nwriten = MTD_BUFFERSIZE / driver->geometry.blocksize;
  result = inode->u.i_bops->write(inode, input, 0, nwriten);
  assert_false(result != nwriten);

  result = inode->u.i_bops->read(inode, output, 0, nwriten);
  assert_false(result != nwriten);

  result = memcmp(input, output, MTD_BUFFERSIZE);
  assert_true(result == 0);

  /* Check ftl_geometry */

  result = inode->u.i_bops->geometry(inode, &geometry);
  assert_false(result != 0);

  /* Note: ftl->blkper = dev->geo.erasesize / dev->geo.blocksize
   * that we can check whether it is the expected value through the
   * original rammtd info.
   */

  blkper = driver->geometry.erasesize / driver->geometry.blocksize;
  assert_true(geometry.geo_nsectors = driver->geometry.neraseblocks *
              blkper);
  assert_true(geometry.geo_sectorsize = driver->geometry.blocksize);

  /* Check ftl_ioctl */

  result = inode->u.i_bops->ioctl(inode, MTDIOC_GEOMETRY,
                                  (uintptr_t)&ioctl_geo);
  assert_false(result != 0);

  assert_true(ioctl_geo.blocksize = driver->geometry.blocksize);
  assert_true(ioctl_geo.erasesize = driver->geometry.erasesize);
  assert_true(ioctl_geo.neraseblocks = driver->geometry.neraseblocks);

#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
  /* Check ftl_unlink */

  result = inode->u.i_bops->unlink(inode);
  assert_false(result != 0);
#endif

  /* Check ftl_close */

  result = inode->u.i_bops->close(inode);
  assert_false(result != 0);

  free(input);
}
