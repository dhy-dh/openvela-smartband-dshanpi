/****************************************************************************
 * apps/testing/rpmsg/rpmsgmtd.c
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
#include <stdint.h>
#include <setjmp.h>
#include <sys/ioctl.h>
#include <sys/random.h>
#include <syslog.h>
#include <fcntl.h>
#include <cmocka.h>
#include <unistd.h>

#include <nuttx/fs/fs.h>
#include <nuttx/kmalloc.h>
#include <nuttx/mtd/mtd.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define MTD_BLOCKSIZE          512
#define MTD_ERASESIZE          4096
#define MTD_SIZE               (1024 * 128)
#define MTD_ERASESTATE         0xff
#define MTD_NAMESIZE           32

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct pre_build_s
{
  char                    source[MTD_NAMESIZE];
  struct mtd_geometry_s   geo;
  int                     fd;
  FAR char               *cpu;
  bool                    server;
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifdef CONFIG_RPMSGMTD_SERVER
/****************************************************************************
 * Name: mtddriver_register
 *
 * Description:
 *   Non-standard function to register rpmsgmtd device
 *
 * Input Parameters:
 *   minor: Selects suffix of device named /dev/ramN, N={1,2,3...}
 *
 * Returned Value:
 *   Zero on success; a negated errno value on failure.
 *
 ****************************************************************************/

static int mtddriver_register(FAR const char *path)
{
  struct rammtd_config_s config;
  FAR struct mtd_dev_s *mtd;
  FAR uint8_t *start;
  int ret;

  /* Inode private data is a reference to the ramdisk device structure */

  start = (FAR uint8_t *)malloc(MTD_SIZE);
  assert_true(start != NULL);

  config.blocksize    = MTD_BLOCKSIZE;
  config.erasesize    = MTD_ERASESIZE;
  config.erase_state  = MTD_ERASESTATE;
  config.size         = MTD_SIZE;
  config.start        = start;

  mtd = rammtd_initialize_with_config(&config);
  if (mtd == NULL)
    {
      syslog(LOG_ERR, "Fail to initialize MTD device\n");
      free(start);
      return ERROR;
    }

  ret = register_mtddriver(path, mtd, 0, start);
  if (ret < 0)
    {
      syslog(LOG_ERR, "register_mtddriver failed: %d\n", -ret);
      rammtd_uninitialize(mtd);
      free(start);
    }

  return ret;
}
#endif

#ifdef CONFIG_RPMSGMTD_SERVER
/****************************************************************************
 * Name: mtddriver_unregister
 ****************************************************************************/

static int mtddriver_unregister(FAR const char *path)
{
  FAR struct inode *inode;
  FAR void *mem;
  int ret;

  ret = find_mtddriver(path, &inode);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to find mtddriver, ret=%d\n", ret);
      return ret;
    }

  mem = inode->i_private;
  ret = close_mtddriver(inode);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to close mtddriver, ret=%d\n", ret);
      return ret;
    }

  ret = unregister_mtddriver(path);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: unregister driver failed, ret=%d\n", ret);
      return ret;
    }

  /* Release the driver space */

  free(mem);

  return ret;
}
#endif

/****************************************************************************
 * Name: show_usage
 ****************************************************************************/

static void show_usage(FAR const char *progname, int exitcode)
{
  printf("Usage: %s [-c <remote cpu>] [-d <dev_name>] [-u <dev_name>] \
         [-m <server>]\n", progname);
  printf("Where:\n");
  printf(" -c: <name> remote cpu which regists test driver\n"
         " -d: <path> register rpmsgmtd device\n"
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
  int ret = 0;

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
            memcpy(pre->source, optarg, sizeof(pre->source));

            if (pre->server)
              {
#ifdef CONFIG_RPMSGMTD_SERVER
                ret = mtddriver_register(pre->source);
#endif
              }
            else
              {
#ifdef CONFIG_RPMSGMTD
                ret = rpmsgmtd_register(pre->cpu, pre->source, NULL);
#endif
              }

            if (ret == 0)
              {
                syslog(LOG_INFO, "register rpmsgmtd device success.\n");
              }
            else
              {
                syslog(LOG_ERR, "register rpmsgmtd device failed.\n");
              }

            break;
          case 'u':
            memcpy(pre->source, optarg, sizeof(pre->source));
#ifdef CONFIG_RPMSGMTD_SERVER
            if (mtddriver_unregister(pre->source) < 0)
              {
                syslog(LOG_INFO, "unregister mtd device fail.\n");
              }
            else
              {
                syslog(LOG_INFO, "unregister mtd device success.\n");
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

#ifdef CONFIG_RPMSGMTD
/****************************************************************************
 * Name: testcase1
 *
 * Description:
 *   Test rpmsgmtd device open, ioctl command and close option.
 *
 * Returned Value:
 *   Zero on success; a negated errno value on failure.
 *
 ****************************************************************************/

static void testcase1(FAR void** state)
{
  FAR const char *source = (FAR const char *)(*state);
  struct partition_info_s partinfo;
  FAR uint8_t *start;
  FAR uint8_t *data;
  uint8_t result;
  size_t index;
  int fd;
  int ret;

  fd = open(source, O_RDWR | O_DIRECT);
  assert_true(fd > 0);

  ret = ioctl(fd, BIOC_XIPBASE, (unsigned long)((uintptr_t)&start));
  assert_true(ret == 0);

  ret = ioctl(fd, MTDIOC_BULKERASE, NULL);
  assert_true(ret == 0);

  data = malloc(MTD_SIZE);
  assert_true(data != NULL);

  ret = read(fd, data, MTD_SIZE);
  assert_true(ret == MTD_SIZE);
  for (index = 0; index < MTD_SIZE; index++)
    {
      assert_true(data[index] == MTD_ERASESTATE);
    }

  free(data);

  ret = ioctl(fd, MTDIOC_ERASESTATE, (unsigned long)((uintptr_t)&result));
  assert_true(ret == 0);
  assert_true(result == MTD_ERASESTATE);

  ret = ioctl(fd, BIOC_PARTINFO, (unsigned long)((uintptr_t)&partinfo));
  assert_true(ret == 0);
  assert_true(partinfo.numsectors == MTD_SIZE / MTD_BLOCKSIZE &&
              partinfo.sectorsize == MTD_BLOCKSIZE &&
              partinfo.startsector == 0);

  syslog(LOG_INFO, "partinfo.numsectors = %d partinfo.sectorsize = %d\
         partinfo.startsector = %lld partinfo.parent = %s\n",
         partinfo.numsectors, partinfo.sectorsize,
         partinfo.startsector, partinfo.parent);

  close(fd);
}

/****************************************************************************
 * Name: testcase2_setup
 ****************************************************************************/

static int testcase2_setup(FAR void **state)
{
  FAR struct pre_build_s *pre = (FAR struct pre_build_s *)(*state);
  time_t t;
  int ret;

  pre->fd = open(pre->source, O_RDWR | O_DIRECT);
  assert_true(pre->fd > 0);

  ret = ioctl(pre->fd, MTDIOC_GEOMETRY,
              (unsigned long)((uintptr_t)&pre->geo));
  assert_true(ret == 0);
  assert_true(pre->geo.blocksize == MTD_BLOCKSIZE &&
              pre->geo.erasesize == MTD_ERASESIZE &&
              pre->geo.neraseblocks == MTD_SIZE / MTD_ERASESIZE);

  srand((unsigned)time(&t));
  return ret;
}

/****************************************************************************
 * Name: testcase2
 *
 * Description:
 *   Test rpmsgmtd device read, write option.
 *
 * Returned Value:
 *   Zero on success; a negated errno value on failure.
 *
 ****************************************************************************/

static void testcase2(FAR void ** state)
{
  FAR struct pre_build_s *pre = (FAR struct pre_build_s *)(*state);
  FAR char *output;
  FAR char *input;
  off_t offset;
  int ret;

  input = malloc(pre->geo.blocksize * 2);
  assert_true(input != NULL);
  output = input + pre->geo.blocksize;

  for (offset = 0; offset < pre->geo.erasesize; offset += pre->geo.blocksize)
    {
      assert_true(lseek(pre->fd, offset, SEEK_SET) >= 0);

      getrandom(input, pre->geo.blocksize, 0);

      ret = write(pre->fd, input, pre->geo.blocksize);
      assert_true(ret == pre->geo.blocksize);

      /* Let's write each time we need to move the pointer back to the
       * beginning
       */

      assert_true(lseek(pre->fd, offset, SEEK_SET) >= 0);

      ret = read(pre->fd, output, pre->geo.blocksize);
      assert_true(ret == pre->geo.blocksize);
      assert_true(memcmp(input, output, pre->geo.blocksize) == 0);
    }

  free(input);
  close(pre->fd);
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char *argv[])
{
  struct pre_build_s pre;

  parse_commandline(argc, argv, &pre);

#ifdef CONFIG_RPMSGMTD
  if (pre.server == false)
    {
      const struct CMUnitTest tests[] =
        {
          cmocka_unit_test_prestate_setup_teardown(testcase1, NULL,
                                                   NULL, &pre),
          cmocka_unit_test_prestate_setup_teardown(testcase2,
                                                   testcase2_setup,
                                                   NULL, &pre),
        };

      cmocka_run_group_tests(tests, NULL, NULL);
      rpmsgmtd_unregister(pre.source);
      return 0;
    }
#endif

  return 0;
}
