/****************************************************************************
 * apps/testing/fs/fs_testsuites/mtd/util/testsuites_mtd_util.c
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

#include <stdio.h>
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

#include "testsuites_mtd_util.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAMMTD_DRIVER_NAME "/dev/test_rammtd"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * testsuites_mtd_setup
 ****************************************************************************/

int testsuites_mtd_setup(FAR void **state)
{
  FAR struct testsuites_mtd_driver *driver;
  int ret;

  driver = zalloc(sizeof(struct testsuites_mtd_driver));
  assert_false(driver == NULL);

  driver->buffer = zalloc(MTD_BUFFERSIZE);
  assert_false(driver->buffer == NULL);

  snprintf(driver->pathname, PATH_MAX, RAMMTD_DRIVER_NAME);
  driver->mtd = rammtd_initialize(driver->buffer, MTD_BUFFERSIZE);
  assert_false(driver->mtd == NULL);

  ret = MTD_IOCTL(driver->mtd, MTDIOC_GEOMETRY,
                  (unsigned long)((uintptr_t)&driver->geometry));
  assert_false(ret < 0);

  srand(time(NULL));
  *state = driver;
  return ret;
}

/****************************************************************************
 * testsuites_mtd_teardown
 ****************************************************************************/

int testsuites_mtd_teardown(FAR void **state)
{
  FAR struct testsuites_mtd_driver *driver = *state;

  rammtd_uninitialize(driver->mtd);
  free(driver->buffer);
  free(driver);
  return 0;
}
