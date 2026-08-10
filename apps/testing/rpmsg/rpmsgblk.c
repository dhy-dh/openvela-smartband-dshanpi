/****************************************************************************
 * apps/testing/rpmsg/rpmsgblk.c
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
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>

#include <cmocka.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/random.h>

#include <nuttx/fs/fs.h>
#include <nuttx/drivers/ramdisk.h>
#include <nuttx/drivers/rpmsgblk.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define BLK_NSECTORS 8
#define BLK_SECTSIZE 512
#define MTD_NAMESIZE 32

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct pre_build_s
{
  FAR struct inode *driver;
  char              source[MTD_NAMESIZE];
  struct geometry   cfg;
  int               fd;
  FAR char         *cpu;
  bool              server;
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifdef CONFIG_BLK_RPMSG_SERVER

/****************************************************************************
 * Name: rpmsgblk_server_register
 *
 * Description:
 *   Non-standard function to register a ramdisk or a romdisk
 *
 * Input Parameters:
 *   path:          device name
 *   nsectors:      Number of sectors on device
 *   sectize:       The size of one sector
 *   rdflags:       See RDFLAG_* definitions
 *   buffer:        RAM disk backup memory
 *
 * Returned Value:
 *   Zero on success; a negated errno value on failure.
 *
 ****************************************************************************/

static int rpmsgblk_server_register(FAR const char *path)
{
  struct ramdisk_config_s config;
  FAR uint8_t *buffer;
  int ret;

  /* Malloc RAM disk backup memory */

  buffer = (FAR uint8_t *)malloc(BLK_NSECTORS * BLK_SECTSIZE);
  assert_true(buffer != NULL);

  config.nsectors = BLK_NSECTORS;
  config.sectsize = BLK_SECTSIZE;
  config.buffer = buffer;
  config.rdflags = RDFLAG_WRENABLED | RDFLAG_FUNLINK;
  config.name = path;

  ret = ramdisk_register_with_config(&config);
  if (ret < 0)
    {
      syslog(LOG_ERR, "register_blockdriver failed: %d\n", -ret);
      free(buffer);
    }

  return ret;
}

/****************************************************************************
 * Name: rpmsgblk_server_unregister
 ****************************************************************************/

static int rpmsgblk_server_unregister(FAR const char *path)
{
  FAR void *buffer;
  int ret;
  int fd;

  /* Free RAM disk backup memory */

  fd = open(path, O_RDWR | O_DIRECT);
  if (fd < 0)
    {
      syslog(LOG_ERR, "open block device failed: %d\n", fd);
      return fd;
    }

  ret = ioctl(fd, BIOC_XIPBASE, (unsigned long)((uintptr_t)&buffer));
  if (ret < 0)
    {
      syslog(LOG_ERR, "get RAM disk backup memory failed: %d\n", ret);
      close(fd);
      return ret;
    }

  close(fd);

  /* Unregister block driver */

  ret = ramdisk_unregister(path);
  if (ret < 0)
    {
      syslog(LOG_ERR, "unregister blockdriver failed: %d\n", ret);
      return ret;
    }

  free(buffer);

  return ret;
}

#else /* CONFIG_BLK_RPMSG_SERVER */

/****************************************************************************
 * Name: testcase1
 ****************************************************************************/

static void testcase1(FAR void **state)
{
  FAR struct pre_build_s *pre = (FAR struct pre_build_s *)(*state);

  pre->fd = open(pre->source, O_RDWR | O_DIRECT);
  assert_true(pre->fd >= 0);

  assert_true(close(pre->fd) == 0);
}

/****************************************************************************
 * Name: testcase2
 ****************************************************************************/

static void testcase2(FAR void **state)
{
  FAR struct pre_build_s *pre = (FAR struct pre_build_s *)(*state);
  int ret;

  pre->fd = open(pre->source, O_RDWR | O_DIRECT);
  assert_true(pre->fd >= 0);

  ret = ioctl(pre->fd, BIOC_GEOMETRY, (unsigned long)((uintptr_t)&pre->cfg));
  assert_true(ret >= 0);
  assert_true(pre->cfg.geo_nsectors == BLK_NSECTORS);
  assert_true(pre->cfg.geo_sectorsize == BLK_SECTSIZE);
  assert_true(pre->cfg.geo_writeenabled == RDFLAG_WRENABLED);

  assert_true(close(pre->fd) == 0);
}

/****************************************************************************
 * Name: testcase3_setup
 ****************************************************************************/

static int testcase3_setup(FAR void **state)
{
  FAR struct pre_build_s *pre = (FAR struct pre_build_s *)(*state);
  int ret;

  pre->fd = open(pre->source, O_RDWR | O_DIRECT);
  assert_true(pre->fd >= 0);

  ret = ioctl(pre->fd, BIOC_GEOMETRY, (unsigned long)((uintptr_t)&pre->cfg));
  assert_true(ret >= 0);

  return 0;
}

/****************************************************************************
 * Name: testcase3
 *
 * Description:
 *   Test rpmsgblk device read, write option.
 *
 * Returned Value:
 *   Zero on success; a negated errno value on failure.
 *
 ****************************************************************************/

static void testcase3(FAR void** state)
{
  FAR struct pre_build_s *pre = (FAR struct pre_build_s *)(*state);
  FAR void *input;
  FAR void *output;
  off_t offset;
  int ret;

  input = malloc(pre->cfg.geo_sectorsize * 2);
  assert_true(input != NULL);
  output = input + pre->cfg.geo_sectorsize;

  /* Test process: convert the device information of the storage device into
   * 'sectors' by bch, and fill each 'sector' with random characters, then
   * read it out and compare whether the writing and The difference between
   * write and store is verified by memcompare.
   * This behavior simulates the behavior of commands such as 'dd' in the
   * system. The general flow is user->bch->ftl->driver
   */

  for (offset = 0; offset < pre->cfg.geo_nsectors; offset++)
    {
      assert_false(lseek(pre->fd, offset * pre->cfg.geo_sectorsize,
                   SEEK_SET) < 0);

      getrandom(input, pre->cfg.geo_sectorsize, 0);

      ret = write(pre->fd, input, pre->cfg.geo_sectorsize);
      assert_false(ret != pre->cfg.geo_sectorsize);

      /* Let's write each time we need to move the pointer back to the
       * beginning
       */

      assert_false(lseek(pre->fd, offset * pre->cfg.geo_sectorsize,
                   SEEK_SET) < 0);

      ret = read(pre->fd, output, pre->cfg.geo_sectorsize);
      assert_false(ret != pre->cfg.geo_sectorsize);

      assert_true(memcmp(input, output, pre->cfg.geo_sectorsize) == 0);
    }

  assert_true(close(pre->fd) == 0);
  free(input);
}

#endif

/****************************************************************************
 * Name: show_usage
 ****************************************************************************/

static void show_usage(FAR const char *progname, int exitcode)
{
  printf("Usage: %s [-m <is server>] [-c <remote cpu>] [-d <dev_name>]\n",
         progname);
  printf("Where:\n");
  printf(" -c: <name> remote cpu which regists test driver\n"
         " -d: <path> register rpmsgblk device\n"
         " -u: <path> unregister rpmsgmtd driver\n"
         " -m: <server>: is server\n");
  exit(exitcode);
}

/****************************************************************************
 * Name: parse_commandline
 ****************************************************************************/

static void parse_commandline(int argc, FAR char **argv,
                              FAR struct pre_build_s *pre)
{
  int option;
  int ret = ERROR;

  pre->server = true;       /* deflaut role */

  while ((option = getopt(argc, argv, "c:d:u:m:")) != ERROR)
    {
      switch (option)
        {
          case 'c':
            pre->cpu = optarg;
            break;
          case 'm':
            pre->server = atoi(optarg);
            break;
          case 'd':
            strlcpy(pre->source, optarg, sizeof(pre->source));

            if (pre->server)
              {
#ifdef CONFIG_BLK_RPMSG_SERVER
                ret = rpmsgblk_server_register(pre->source);
#endif
              }
            else
              {
#ifdef CONFIG_BLK_RPMSG
                ret = rpmsgblk_register(pre->cpu, pre->source, NULL);
#endif
              }

            if (ret == 0)
              {
                syslog(LOG_INFO, "register rpmsgblk device success.\n");
              }
            else
              {
                syslog(LOG_INFO, "register rpmsgblk dev failed: %d\n", ret);
              }
            break;
          case 'u':
            strlcpy(pre->source, optarg, sizeof(pre->source));
#ifdef CONFIG_BLK_RPMSG_SERVER
            if ((ret = rpmsgblk_server_unregister(pre->source)) < 0)
              {
                syslog(LOG_INFO, "unregister block device fail: %d\n", ret);
              }
            else
              {
                syslog(LOG_INFO, "unregister block device success.\n");
              }
#else
            syslog(LOG_ERR, "ERROR: Client unsupport ops: -u.\n");
#endif
            break;
          case '?':
            printf("Unknown option: %c\n", optopt);
            show_usage(argv[0], EXIT_FAILURE);
            break;
        }
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char *argv[])
{
  FAR struct pre_build_s *pre;

  pre = (FAR struct pre_build_s *)malloc(sizeof(struct pre_build_s));
  assert_true(pre != NULL);

  parse_commandline(argc, argv, pre);

#ifdef CONFIG_BLK_RPMSG
  const struct CMUnitTest tests[] =
    {
      cmocka_unit_test_prestate_setup_teardown(testcase1, NULL, NULL,
                                               (FAR void *)pre),
      cmocka_unit_test_prestate_setup_teardown(testcase2, NULL, NULL,
                                               (FAR void *)pre),
      cmocka_unit_test_prestate_setup_teardown(testcase3, testcase3_setup,
                                               NULL, (FAR void *)pre),
    };

  cmocka_run_group_tests(tests, NULL, NULL);
  rpmsgblk_unregister(pre->source);
#endif

  free(pre);
  return 0;
}
