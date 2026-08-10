/****************************************************************************
 * apps/testing/fs/fs_testsuites/mtd/util/testsuites_mtd_util.h
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

#ifndef TESTSUITES_MTD_UTIL_H
#define TESTSUITES_MTD_UTIL_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/mtd/mtd.h>

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct testsuites_mtd_driver
{
  FAR struct mtd_dev_s *mtd;
  struct mtd_geometry_s geometry;
  FAR uint8_t *buffer;
  char pathname[PATH_MAX];
};

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TESTSUITES_MTD \
  cmocka_unit_test_setup_teardown(testsuites_mtd_ftl_01, \
          testsuites_mtd_setup, testsuites_mtd_teardown),

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* TEST UTIL FUNCTIONS */

int testsuites_mtd_setup(FAR void **state);
int testsuites_mtd_teardown(FAR void **state);

#endif /* TESTSUITES_MTD_UTIL_H */
