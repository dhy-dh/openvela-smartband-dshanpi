/****************************************************************************
 * apps/audioutils/alsa-lib/alsactl.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with the
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

#include <alsa/asoundlib.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define DEFAULT_DEVICE "pcm0p"
#define DEFAULT_PATH "/data"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static FAR const char *build_dump_path(FAR const char *base,
                                       FAR const char *device,
                                       FAR const struct audio_info_s *info,
                                       FAR char *buffer, size_t buflen)
{
  size_t len = strlen(base);
  int width;
  int ret;

  if (len >= 4 && strcmp(base + len - 4, ".pcm") == 0)
    {
      return base;
    }

  width = snd_pcm_format_physical_width(info->subformat);

  if (len > 0 && base[len - 1] == '/')
    {
      ret = snprintf(buffer, buflen, "%s%s_%lu_%u_%u.pcm", base, device,
                     (unsigned long)info->samplerate, info->channels, width);
    }
  else
    {
      ret = snprintf(buffer, buflen, "%s/%s_%lu_%u_%u.pcm", base, device,
                     (unsigned long)info->samplerate, info->channels, width);
    }

  if (ret < 0 || (size_t)ret >= buflen)
    {
      printf("dump path too long\n");
      return NULL;
    }

  return buffer;
}

static void ioctl_dump(FAR const char *name, FAR const char *path)
{
  struct audio_info_s info;
  char buffer[128];
  char device[64];
  int ret;
  int fd;

  snprintf(device, sizeof(device),
           CONFIG_AUDIOUTILS_ALSA_LIB_DEV_PATH "/%s", name);

  fd = open(device, O_RDWR | O_CLOEXEC);
  if (fd < 0)
    {
      printf("open %s failed: %s\n", device, strerror(errno));
      return;
    }

  ret = ioctl(fd, AUDIOIOC_GETAUDIOINFO, &info);
  if (ret < 0)
    {
      printf("ioctl AUDIOIOC_GETAUDIOINFO failed: %d\n", ret);
      goto out;
    }

  if (path == NULL)
    {
      SNDINFO("device:%s dump end\n", device);
    }
  else
    {
      path = build_dump_path(path, name, &info, buffer, sizeof(buffer));
      if (path == NULL)
        {
          goto out;
        }

      SNDINFO("device:%s path: %s\n", device, path);
    }

  ret = ioctl(fd, AUDIOIOC_DUMP, path);
  if (ret < 0)
    {
      printf("ioctl AUDIOIOC_DUMP failed: %d\n", ret);
    }

out:
  close(fd);
}

static int usage(FAR const char *cmd)
{
  printf("Usage: %s [-d <device>] [--start] [--stop] [-f <path>]\n", cmd);
  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  FAR const char *device = DEFAULT_DEVICE;
  FAR const char *path = DEFAULT_PATH;
  bool start = false;
  bool stop = false;
  int opt;
  int opt_idx = 0;

  static const struct option long_opts[] =
  {
    { "device", required_argument, NULL, 'd' },
    { "start", no_argument, NULL, 's' },
    { "stop", no_argument, NULL, 'e' },
    { "file", required_argument, NULL, 'f' },
    { "help", no_argument, NULL, 'h' },
    { NULL, 0, NULL, 0 }
  };

  while ((opt = getopt_long(argc, argv, "d:sef:h", long_opts,
                            &opt_idx)) != -1)
    {
      switch (opt)
      {
        case 'd':
          device = optarg;
          break;
        case 'e':
          stop = true;
          break;
        case 'f':
          path = optarg;
          break;
        case 's':
          start = true;
          break;
        case 'h':
          return usage(argv[0]);
        default:
          return usage(argv[0]);
      }
    }

  if (!start && !stop)
    {
      return usage(argv[0]);
    }

  if (start)
    {
      ioctl_dump(device, path);
    }

  if (stop)
    {
      ioctl_dump(device, NULL);
    }

  return 0;
}
