/****************************************************************************
 * apps/audioutils/alsa-lib/aplay.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define DEFAULT_RATE 44100
#define DEFAULT_CHANNELS 2
#define DEFAULT_FORMAT SND_PCM_FORMAT_S16
#define DEFAULT_DEVICE "default"
#define DEFAULT_PERIOD_TIME 20000
#define DEFAULT_BUFFER_TIME 80000

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void usage(FAR const char *command)
{
  printf("Usage: %s [OPTION]... [FILE]...\n", command);
  printf("\n");
  printf("  -h, --help              Display this help message and exit\n");
  printf("  -D, --device=NAME       Specify the PCM device (default: %s)\n",
         DEFAULT_DEVICE);
  printf("  -r, --rate=#            Sampling rate (Hz), default %d\n",
         DEFAULT_RATE);
  printf("  -c, --channels=#        Number of channels, default %d\n",
         DEFAULT_CHANNELS);
  printf("  -d, --duration=#        interrupt after # seconds\n");
  printf("  -F, --period-time=#     distance between interrupts is # "
         "microseconds, default %d\n",
         DEFAULT_PERIOD_TIME);
  printf("  -B, --buffer-time=#     buffer duration is # microseconds, "
         "default %d\n",
         DEFAULT_BUFFER_TIME);
  printf("  -f, --format=FORMAT     Sample format, default S16_LE\n");
  printf("The available format shortcuts are:\n");
  printf("-f cd (16 bit little endian, 44100, stereo)\n");
  printf("-f dat (16 bit little endian, 48000, stereo)\n");
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  snd_pcm_format_t format = DEFAULT_FORMAT;
  unsigned int buffer_time = DEFAULT_BUFFER_TIME;
  unsigned int period_time = DEFAULT_PERIOD_TIME;
  FAR const char *device = DEFAULT_DEVICE;
  unsigned int rate = DEFAULT_RATE;
  unsigned int channels = DEFAULT_CHANNELS;
  snd_pcm_uframes_t frames_limit = ULONG_MAX;
  snd_pcm_uframes_t frames_written = 0;
  snd_pcm_uframes_t frames;
  FAR snd_pcm_hw_params_t *hwparams;
  FAR const char *file_name = NULL;
  FAR snd_pcm_t *handle;
  unsigned int duration = 0;
  size_t frame_bytes;
  size_t buffer_size;
  ssize_t read_bytes;
  FAR char *endptr;
  FAR char *buffer;
  int option_index;
  int fd = -1;
  int opt;
  int ret = EXIT_FAILURE;

  static const struct option long_options[] =
    {
      {"help", no_argument, 0, 'h'},
      {"device", required_argument, 0, 'D'},
      {"rate", required_argument, 0, 'r'},
      {"channels", required_argument, 0, 'c'},
      {"duration", required_argument, 0, 'd'},
      {"period-time", required_argument, 0, 'F'},
      {"buffer-time", required_argument, 0, 'B'},
      {"format", required_argument, 0, 'f'},
      {0, 0, 0, 0}
    };

  if (argc < 2)
    {
      usage(argv[0]);
      return ret;
    }

  while ((opt = getopt_long(argc, argv, "hD:r:c:d:F:B:f:", long_options,
                            &option_index)) != -1)
    {
      switch (opt)
        {
        case 'h':
          usage(argv[0]);
          return EXIT_SUCCESS;
        case 'D':
          device = optarg;
          break;
        case 'r':
          rate = strtoul(optarg, &endptr, 10);
          if (*endptr != '\0')
            {
              printf("Invalid sampling rate: %s\n", optarg);
              return ret;
            }
          break;
        case 'd':
          duration = strtoul(optarg, &endptr, 10);
          if (*endptr != '\0')
            {
              printf("Invalid duration: %s\n", optarg);
              return ret;
            }
          break;
        case 'c':
          channels = strtoul(optarg, &endptr, 10);
          if (*endptr != '\0')
            {
              printf("Invalid number of channels: %s\n", optarg);
              return ret;
            }
          break;
        case 'F':
          period_time = strtoul(optarg, &endptr, 10);
          if (*endptr != '\0')
            {
              printf("Invalid period time: %s\n", optarg);
              return ret;
            }
          break;
        case 'B':
          buffer_time = strtoul(optarg, &endptr, 10);
          if (*endptr != '\0')
            {
              printf("Invalid buffer time: %s\n", optarg);
              return ret;
            }
          break;
        case 'f':
          if (strcasecmp(optarg, "cd") == 0)
            {
              format = SND_PCM_FORMAT_S16_LE;
              rate = 44100;
              channels = 2;
            }
          else if (strcasecmp(optarg, "dat") == 0)
            {
              format = SND_PCM_FORMAT_S16_LE;
              rate = 48000;
              channels = 2;
            }
          else
            {
              format = snd_pcm_format_value(optarg);
              if (format == SND_PCM_FORMAT_UNKNOWN)
                {
                  printf("Unsupported sample format: %s\n", optarg);
                  return ret;
                }
            }
          break;
        default:
          usage(argv[0]);
          return ret;
        }
    }

  if (optind < argc)
    {
      file_name = argv[optind];
    }

  if (file_name)
    {
      fd = open(file_name, O_RDONLY | O_BINARY);
      if (fd == -1)
        {
          printf("unable to open %s err:%s\n", file_name, strerror(errno));
          return ret;
        }
    }

  if (snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0) < 0)
    {
      printf("Unable to open audio device %s\n", device);
      goto file_exit;
    }

  snd_pcm_hw_params_alloca(&hwparams);
  if (snd_pcm_hw_params_any(handle, hwparams) < 0)
    {
      printf("Unable to initialize hardware parameter structure\n");
      goto snd_exit;
    }

  if (snd_pcm_hw_params_set_access(handle, hwparams,
                                   SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
    {
      printf("Failed to set interleaved mode\n");
      goto snd_exit;
    }

  if (snd_pcm_hw_params_set_format(handle, hwparams, format) < 0)
    {
      printf("Failed to set sample format\n");
      goto snd_exit;
    }

  if (snd_pcm_hw_params_set_channels(handle, hwparams, channels) < 0)
    {
      printf("Failed to set number of channels\n");
      goto snd_exit;
    }

  if (snd_pcm_hw_params_set_rate(handle, hwparams, rate, 0) < 0)
    {
      printf("Failed to set sampling rate\n");
      goto snd_exit;
    }

  if (snd_pcm_hw_params_set_period_time(handle, hwparams, period_time, 0) <
      0)
    {
      printf("Failed to set period time\n");
      goto snd_exit;
    }

  if (snd_pcm_hw_params_set_buffer_time(handle, hwparams, buffer_time, 0) <
      0)
    {
      printf("Failed to set buffer time\n");
      goto snd_exit;
    }

  if (snd_pcm_hw_params(handle, hwparams) < 0)
    {
      printf("Unable to set hardware parameters\n");
      goto snd_exit;
    }

  frame_bytes = snd_pcm_format_physical_width(format) / 8 * channels;
  if (snd_pcm_hw_params_get_buffer_size(hwparams, &frames) < 0)
  {
      printf("Unable to get buffer size\n");
      goto snd_exit;
  }

  buffer_size = frames * frame_bytes;
  buffer = malloc(buffer_size);
  if (!buffer)
    {
      printf("Failed to allocate buffer, size:%zu\n", buffer_size);
      goto snd_exit;
    }

  if (duration > 0)
  {
    frames_limit = duration * rate;
  }

  while ((read_bytes = read(fd, buffer, buffer_size)) > 0)
    {
      frames = read_bytes / frame_bytes;
      frames_written += frames;
      while (frames > 0)
        {
          ret = snd_pcm_writei(handle, buffer, frames);
          if (ret < 0)
            {
              ret = snd_pcm_recover(handle, ret, 0);
              if (ret < 0)
                {
                  printf("snd_pcm_writei error: %s\n", snd_strerror(ret));
                  ret = EXIT_FAILURE;
                  goto buf_exit;
                }
            }

          frames -= ret;
        }

      if (frames_written >= frames_limit)
        {
          break;
        }
    }

  snd_pcm_drain(handle);
  ret = EXIT_SUCCESS;

buf_exit:
  free(buffer);
snd_exit:
  snd_pcm_close(handle);
file_exit:
  close(fd);
  return ret;
}
