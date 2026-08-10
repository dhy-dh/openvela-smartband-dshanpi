/****************************************************************************
 * apps/system/coredump/coredump.c
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
#include <sys/types.h>
#include <sys/endian.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <syslog.h>
#include <dirent.h>
#include <elf.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>

#include <nuttx/binfmt/binfmt.h>
#include <nuttx/streams.h>
#include <nuttx/sched.h>
#include <nuttx/coredump.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifdef CONFIG_BOARD_COREDUMP_COMPRESSION
#  define COREDUMP_FILE_SUFFIX ".lzf"
#else
#  define COREDUMP_FILE_SUFFIX ".core"
#endif

#define COREDUMP_FILE_SUFFIX_LEN (sizeof(COREDUMP_FILE_SUFFIX) - 1)

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef CODE void (*dumpfile_cb_t)(FAR const char *path,
                                   FAR const char *filename,
                                   FAR void *arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

#ifdef CONFIG_BOARD_MEMORY_RANGE
static struct memory_region_s g_memory_region[] =
  {
    CONFIG_BOARD_MEMORY_RANGE
  };
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * dumpfile_is_valid
 ****************************************************************************/

static bool dumpfile_is_valid(FAR const char *name)
{
  FAR const char *suffix;
  size_t name_len;

  name_len = strlen(name);
  if (name_len < COREDUMP_FILE_SUFFIX_LEN)
    {
      return false;
    }

  suffix = name + name_len - COREDUMP_FILE_SUFFIX_LEN;
  return !memcmp(suffix, COREDUMP_FILE_SUFFIX, COREDUMP_FILE_SUFFIX_LEN);
}

/****************************************************************************
 * dumpfile_iterate
 ****************************************************************************/

static int dumpfile_iterate(FAR const char *path,
                            dumpfile_cb_t cb, FAR void *arg)
{
  FAR struct dirent *entry;
  FAR DIR *dir;
  int ret;

  dir = opendir(path);
  if (dir == NULL)
    {
      ret = mkdir(path, 0777);
      if (ret < 0)
        {
          printf("Coredump mkdir %s fail\n", path);
          return -errno;
        }
    }

  while ((entry = readdir(dir)) != NULL)
    {
      if (entry->d_type == DT_REG && dumpfile_is_valid(entry->d_name))
        {
          cb(path, entry->d_name, arg);
        }
    }

  closedir(dir);
  return 0;
}

/****************************************************************************
 * dumpfile_count
 ****************************************************************************/

static void dumpfile_count(FAR const char *path, FAR const char *filename,
                           FAR void *arg)
{
  FAR size_t *max = (FAR size_t *)arg;

  *max += 1;
}

/****************************************************************************
 * dumpfile_delete
 ****************************************************************************/

static void dumpfile_delete(FAR const char *path, FAR const char *filename,
                            FAR void *arg)
{
  FAR char *dumppath = arg;
  int ret;

  sprintf(dumppath, "%s/%s", path, filename);
  printf("Remove %s\n", dumppath);
  ret = remove(dumppath);
  if (ret < 0)
    {
      printf("Remove %s fail\n", dumppath);
    }
}

/****************************************************************************
 * coredump_get_info
 ****************************************************************************/

static int coredump_get_info(int fd, FAR struct coredump_info_s *info)
{
  struct lib_rawsistream_s rawstream;
  FAR struct lib_sistream_s *stream;
  char name[COREDUMP_INFONAME_SIZE];
  Elf_Ehdr ehdr;
  Elf_Phdr phdr;
  Elf_Nhdr nhdr;
  int ret;

  lib_rawsistream(&rawstream, fd);
  stream = (FAR struct lib_sistream_s *)&rawstream;

#ifdef CONFIG_BOARD_COREDUMP_COMPRESSION
  struct lib_lzfsistream_s lzfstream;
  lib_lzfsistream(&lzfstream, stream);
  stream = (FAR struct lib_sistream_s *)&lzfstream;
#endif

  ret = lib_stream_gets(stream, &ehdr, sizeof(ehdr));
  if (ret != sizeof(ehdr))
    {
      return ret < 0 ? ret : -EIO;
    }

  if (memcmp(ehdr.e_ident, ELFMAG, EI_MAGIC_SIZE) != 0)
    {
      return -EINVAL;
    }

  /* The last program header is for NuttX core info note. */

  ret = lib_stream_seek(stream,
                        ehdr.e_phoff + ehdr.e_phentsize * (ehdr.e_phnum - 1),
                        SEEK_SET);
  if (ret < 0)
    {
      return ret;
    }

  ret = lib_stream_gets(stream, &phdr, sizeof(phdr));
  if (ret != sizeof(phdr))
    {
      return ret < 0 ? ret : -EIO;
    }

  ret = lib_stream_seek(stream, phdr.p_offset, SEEK_SET);
  if (ret < 0)
    {
      return ret;
    }

  /* The note header must be exactly match. */

  ret = lib_stream_gets(stream, &nhdr, sizeof(nhdr));
  if (ret != sizeof(nhdr))
    {
      return ret < 0 ? ret : -EIO;
    }

  if (nhdr.n_type != COREDUMP_MAGIC ||
      nhdr.n_namesz != COREDUMP_INFONAME_SIZE ||
      nhdr.n_descsz != sizeof(struct coredump_info_s))
    {
      return -EINVAL;
    }

  ret = lib_stream_gets(stream, name, nhdr.n_namesz);
  if (ret != nhdr.n_namesz)
    {
      return ret < 0 ? ret : -EIO;
    }

  if (strncmp(name, "NuttX", 6) != 0)
    {
      return -EINVAL;
    }

  ret = lib_stream_gets(stream, info, sizeof(*info));
  if (ret != sizeof(*info))
    {
      return ret < 0 ? ret : -EIO;
    }

  return ret;
}

/****************************************************************************
 * coredump_restore_path
 ****************************************************************************/

static FAR char *coredump_restore_path(FAR char *paths)
{
  FAR char *saveptr;
  FAR char *path = strtok_r(paths, ",", &saveptr);
  FAR char *oldpath = NULL;
  struct timespec old =
    {
      .tv_sec = LONG_MAX,
      .tv_nsec = LONG_MAX
    };

  while (path != NULL)
    {
      struct coredump_info_s info;
      int fd = open(path, O_RDONLY | O_CLOEXEC);
      if (fd < 0)
        {
          if (errno == EISDIR)
            {
              return path;
            }

          continue;
        }

      if (coredump_get_info(fd, &info) < 0)
        {
          close(fd);
          return path;
        }

      if (info.time.tv_sec < old.tv_sec ||
          (info.time.tv_sec == old.tv_sec &&
           info.time.tv_nsec < old.tv_nsec))
        {
          old = info.time;
          oldpath = path;
        }

      path = strtok_r(NULL, ",", &saveptr);
      close(fd);
    }

  return oldpath;
}

/****************************************************************************
 * coredump_clear_info
 ****************************************************************************/

static int coredump_clear_info(int fd,
                               FAR const struct coredump_info_s *info)
{
  off_t off = info->size - sizeof(info);
  int ret;
  Elf_Nhdr nhdr =
    {
      0
    };

  off -= COREDUMP_INFONAME_SIZE;
  off -= sizeof(Elf_Nhdr);
  ret = pwrite(fd, &nhdr, sizeof(nhdr), off);
  if (ret < 0)
    {
      return -errno;
    }

  /* Erase the core file header too */

  ret = pwrite(fd, "\0\0\0\0", EI_MAGIC_SIZE, 0);
  if (ret < 0)
    {
      return -errno;
    }

  return 0;
}

/****************************************************************************
 * coredump_restore_to_file
 ****************************************************************************/

static int coredump_restore_to_file(int fd, FAR const char *file,
                                    FAR const struct coredump_info_s *info)
{
  FAR unsigned char *swap;
  size_t offset = 0;
  ssize_t writesize;
  ssize_t readsize;
  int ret = 0;
  int dumpfd;

  dumpfd = open(file, O_CREAT | O_WRONLY | O_TRUNC | O_CLOEXEC, 0777);
  if (dumpfd < 0)
    {
      printf("Open %s fail\n", file);
      return -errno;
    }

  swap = malloc(CONFIG_SYSTEM_COREDUMP_SWAPBUFFER_SIZE);
  if (swap == NULL)
    {
      printf("Malloc fail\n");
      goto fd_err;
    }

  lseek(fd, 0, SEEK_SET);
  while (offset < info->size)
    {
      readsize = info->size - offset;
      if (readsize > CONFIG_SYSTEM_COREDUMP_SWAPBUFFER_SIZE)
        {
          readsize = CONFIG_SYSTEM_COREDUMP_SWAPBUFFER_SIZE;
        }

      readsize = read(fd, swap, readsize);
      if (readsize < 0)
        {
          printf("Read %s fail\n", file);
          break;
        }
      else if (readsize == 0)
        {
          break;
        }

      writesize = write(dumpfd, swap, readsize);
      if (writesize != readsize)
        {
          printf("Write %s fail\n", file);
          break;
        }

      offset += writesize;
    }

  if (offset < info->size)
    {
      printf("Coredump error [%s] need [%zu], but just get %zu\n",
             file, info->size, offset);
      ret = -EINVAL;
      goto swap_err;
    }

  printf("Coredump finish [%s][%zu]\n", file, info->size);

swap_err:
  free(swap);
fd_err:
  close(dumpfd);
  return ret;
}

/****************************************************************************
 * coredump_restore_to_path
 ****************************************************************************/

static int coredump_restore_to_path(int fd, FAR const char *path, size_t max,
                                    FAR const struct coredump_info_s *info)
{
  char dumppath[PATH_MAX];
  size_t nums = 0;
  int ret;

  ret = dumpfile_iterate(path, dumpfile_count, &nums);
  if (ret < 0)
    {
      return ret;
    }

  if (nums >= max)
    {
      ret = dumpfile_iterate(path, dumpfile_delete, dumppath);
      if (ret < 0)
        {
          return ret;
        }
    }

  /* 'date -d @$(printf "%d" 0x6720C67E)' restore utc to date */

  ret = snprintf(dumppath, sizeof(dumppath),
                 "%s/%.16s-%llx"COREDUMP_FILE_SUFFIX,
                 path, info->name.version,
                 (unsigned long long)info->time.tv_sec);

  while (ret--)
    {
      if (dumppath[ret] == ' ' || dumppath[ret] == ':')
        {
          dumppath[ret] = '-';
        }
    }

  return coredump_restore_to_file(fd, dumppath, info);
}

/****************************************************************************
 * coredump_restore
 ****************************************************************************/

static int coredump_restore(FAR const char *file,
                            FAR const char *path, size_t max)
{
  char paths[] = CONFIG_SYSTEM_COREDUMP_RESTORE_PATHS;
  struct coredump_info_s info;
  struct stat st;
  int ret;
  int fd;

  fd = open(file, O_RDWR | O_CLOEXEC);
  if (fd < 0)
    {
      return -errno;
    }

  ret = coredump_get_info(fd, &info);
  if (ret < 0)
    {
      printf("No core data in %s\n", file);
      goto err;
    }

  if (path == NULL)
    {
      path = coredump_restore_path(paths);
    }

  if (stat(path, &st) >= 0 && S_ISDIR(st.st_mode))
    {
      ret = coredump_restore_to_path(fd, path, max, &info);
    }
  else
    {
      ret = coredump_restore_to_file(fd, path, &info);
    }

  /* Clear the original coredump after dumping */

  if (ret >= 0)
    {
      ret = coredump_clear_info(fd, &info);
    }

err:
  close(fd);
  return ret;
}

/****************************************************************************
 * coredump_now
 ****************************************************************************/

static int coredump_now(int pid, FAR char *filename, bool hex)
{
  FAR struct lib_stdoutstream_s *outstream;
  FAR struct lib_hexdumpstream_s *hstream;
#ifdef CONFIG_BOARD_COREDUMP_COMPRESSION
  FAR struct lib_lzfoutstream_s *lstream;
#endif
  FAR void *stream;
  FAR FILE *file;
  int logmask;
  int ret;

  if (filename != NULL)
    {
      file = fopen(filename, "w");
      if (file == NULL)
        {
          return -errno;
        }
    }
  else
    {
      file = stdout;
    }

#ifdef CONFIG_BOARD_COREDUMP_COMPRESSION
  hstream = malloc(sizeof(*hstream) + sizeof(*lstream) +
                   sizeof(*outstream));
#else
  hstream = malloc(sizeof(*hstream) + sizeof(*outstream));
#endif

  if (hstream == NULL)
    {
      ret = -ENOMEM;
      goto err;
    }

#ifdef CONFIG_BOARD_COREDUMP_COMPRESSION
  lstream = (FAR void *)(hstream + 1);
  outstream = (FAR void *)(lstream + 1);
#else
  outstream = (FAR void *)(hstream + 1);
#endif

  printf("Start coredump:\n");
  logmask = setlogmask(LOG_ALERT);

  /* Initialize hex output stream */

  lib_stdoutstream(outstream, file);
  if (file == stdout || hex)
    {
      lib_hexdumpstream(hstream, (FAR void *)outstream);
      stream = hstream;
    }
  else
    {
      stream = outstream;
    }

#ifdef CONFIG_BOARD_COREDUMP_COMPRESSION

  /* Initialize LZF compression stream */

  lib_lzfoutstream(lstream, stream);
  stream = lstream;

#endif

  /* Do core dump */

#ifdef CONFIG_BOARD_MEMORY_RANGE
  ret = coredump(g_memory_region, stream, pid);
#else
  ret = coredump(NULL, stream, pid);
#endif

  setlogmask(logmask);
#  ifdef CONFIG_BOARD_COREDUMP_COMPRESSION
  printf("Finish coredump (Compression Enabled).\n");
#  else
  printf("Finish coredump.\n");
#  endif

  free(hstream);

err:
  if (filename != NULL)
    {
      fclose(file);
    }

  return ret;
}

/****************************************************************************
 * usage
 ****************************************************************************/

static void usage(FAR const char *progname, int exitcode)
{
  fprintf(stderr, "%s [option]:\n", progname);
  fprintf(stderr, "Default usage, will coredump directly\n");
  fprintf(stderr, "\t -p, --pid <pid>, Default, all thread\n");
  fprintf(stderr, "\t -s, --savepath <savepath>\n");
  fprintf(stderr, "\t -d, --devpath <devpath>, Default NULL\n");
  fprintf(stderr, "Second usage, will restore coredump"
                  "from devpath to savepath\n");
  fprintf(stderr, "\t -m, --maxfile <maxfile>,"
                  "Maximum number of coredump files, Default 1\n");
  exit(exitcode);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * coredump_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
#ifdef CONFIG_BOARD_COREDUMP_DEVPATH
  FAR char *devpath = CONFIG_BOARD_COREDUMP_DEVPATH;
#else
  FAR char *devpath = NULL;
#endif

  FAR char *savepath = NULL;
  size_t maxfile = 1;
  int pid = INVALID_PROCESS_ID;
  bool hex = false;
  int ret;

  struct option options[] =
    {
      {"pid", 1, NULL, 'p'},
      {"devpath", 1, NULL, 'd'},
      {"savepath", 1, NULL, 's'},
      {"maxfile", 1, NULL, 'm'},
      {"hex", 1, NULL, 'h'},
      {"help", 0, NULL, 'h'}
    };

  while ((ret = getopt_long(argc, argv, "p:d:f:s:m:h:x", options, NULL))
         != ERROR)
    {
      switch (ret)
        {
          case 'p':
            pid = atoi(optarg);
            break;
          case 'd':
            devpath = optarg;
            break;
          case 's':
            savepath = optarg;
            break;
          case 'm':
            maxfile = atoi(optarg);
            break;
          case 'x':
            hex = true;
            break;
          case 'h':
          default:
            usage(argv[0], EXIT_SUCCESS);
            break;
        }
    }

  if (devpath != NULL)
    {
      coredump_restore(devpath, savepath, maxfile);
    }
  else
    {
      coredump_now(pid, savepath, hex);
    }

  return 0;
}
