/****************************************************************************
 * apps/audioutils/tinycompress/src/utils/crecord.c
 *
 * This g_file is provided under a dual BSD/LGPLv2.1 license.  When using or
 * redistributing this g_file, you may do so under either license.
 *
 * BSD LICENSE
 *
 * crecord command line recorder for compress audio record in alsa
 * Copyright (c) 2011-2012, Intel Corporation
 * Copyright (c) 2013-2014, Wolfson Microelectronic Ltd.
 * All rights reserved.
 *
 * Author: Vinod Koul <vkoul@kernel.org>
 * Author: Charles Keepax <ckeepax@opensource.wolfsonmicro.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS
 * "AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * LGPL LICENSE
 *
 * crecord command line recorder for compress audio record in alsa
 * Copyright (c) 2011-2012, Intel Corporation
 * Copyright (c) 2013-2014, Wolfson Microelectronic Ltd.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to
 * the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <sound/compress_params.h>
#include <tinycompress/tinycompress.h>
#include <tinycompress/tinywave.h>

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define DEFAULT_FORMAT SND_AUDIOMODE_PCM_S16_LE
#define DEFAULT_CODEC_ID SND_AUDIOCODEC_PCM
#define DEFAULT_CHANNELS 1
#define DEFAULT_RATE 8000

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct codec_id
{
  FAR const char *name;
  unsigned int id;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static int g_file;
static FILE *g_finfo;
static int g_verbose;
static bool g_streamed = false;

static const struct codec_id g_codec_ids[] =
{
  {"PCM", SND_AUDIOCODEC_PCM},
  {"MP3", SND_AUDIOCODEC_MP3},
  {"AMR", SND_AUDIOCODEC_AMR},
  {"AMRWB", SND_AUDIOCODEC_AMRWB },
  {"AMRWBPLUS", SND_AUDIOCODEC_AMRWBPLUS},
  {"AAC", SND_AUDIOCODEC_AAC},
  {"WMA", SND_AUDIOCODEC_WMA},
  { "REAL", SND_AUDIOCODEC_REAL},
  {"VORBIS", SND_AUDIOCODEC_VORBIS},
  {"OPUS", SND_AUDIOCODEC_OPUS},
  {"FLAC", SND_AUDIOCODEC_FLAC},
  {"IEC61937", SND_AUDIOCODEC_IEC61937},
  {"G723_1", SND_AUDIOCODEC_G723_1},
  {"G729", SND_AUDIOCODEC_G729},
  {"BESPOKE", SND_AUDIOCODEC_BESPOKE},
};

#define CREC_NUM_CODEC_IDS (sizeof(g_codec_ids) / sizeof(g_codec_ids[0]))

/****************************************************************************
 * Private Functions
 ****************************************************************************/

FAR static const char *codec_name_from_id(unsigned int id)
{
  static char hexname[12];
  int i;

  for (i = 0; i < CREC_NUM_CODEC_IDS; ++i)
    {
      if (g_codec_ids[i].id == id)
        {
          return g_codec_ids[i].name;
        }
    }

  snprintf(hexname, sizeof(hexname), "0x%x", id);

  return hexname;
}

static void usage(void)
{
  int i;

  fprintf(stderr, "usage: crecord [OPTIONS] [g_filename.wav]\n"
         "-c\tcard number\n"
         "-d\tdevice name\n"
         "-b\tbuffer size\n"
         "-f\tfragments\n"
         "-v\tg_verbose mode\n"
         "-l\tlength of record in seconds\n"
         "-h\tPrints this help list\n\n"
         "-C\tSpecify the number of channels (default %u)\n"
         "-R\tSpecify the sample rate (default %u)\n"
         "-F\tSpecify the format: S16_LE, S32_LE (default S16_LE)\n"
         "-I\tSpecify codec ID (default %s)\n\n"
         "Example:\n"
         "\tcrecord -d 0 test.wav\n"
         "\tcrecord -d 0 -f 5 test.wav\n"
         "\tcrecord -d 0 -I MP3 raw.mp3\n\n"
         "Valid codec IDs:\n",
         DEFAULT_CHANNELS, DEFAULT_RATE,
         codec_name_from_id(DEFAULT_CODEC_ID));

  for (i = 0; i < CREC_NUM_CODEC_IDS; ++i)
    {
      fprintf(stderr, "%s%c", g_codec_ids[i].name,
              ((i + 1) % 8) ? ' ' : '\n');
    }

  fprintf(stderr, "\nor the value in decimal or hex\n");

  exit(EXIT_FAILURE);
}

static int print_time(FAR struct compress *compress)
{
  struct timespec tstamp;
  unsigned int avail;

  if (compress_get_hpointer(compress, &avail, &tstamp) != 0)
    {
      fprintf(stderr, "Error querying timestamp\n");
      fprintf(stderr, "ERR: %s\n", compress_get_error(compress));
      return -1;
    }
  else
    {
      fprintf(g_finfo, "DSP recorded %jd.%jd\n", (intmax_t)tstamp.tv_sec,
             (intmax_t)tstamp.tv_nsec * 1000);
    }

  return 0;
}

static int finish_record(void)
{
  struct wave_header header;
  size_t written;
  size_t nread;
  int ret;

  if (!g_file)
    {
      return -ENOENT;
    }

  /* Can't rewind if streaming to stdout */

  if (g_streamed)
    {
      return 0;
    }

  /* Get amount of data written to g_file */

  ret = lseek(g_file, 0, SEEK_END);
  if (ret < 0)
    {
      return -errno;
    }

  written = ret;
  if (written < sizeof(header))
    {
      return -ENOENT;
    }

  written -= sizeof(header);

  /* Sync g_file header from g_file */

  ret = lseek(g_file, 0, SEEK_SET);
  if (ret < 0)
    {
      return -errno;
    }

  nread = read(g_file, &header, sizeof(header));
  if (nread != sizeof(header))
    {
      return -errno;
    }

  /* Update g_file header */

  ret = lseek(g_file, 0, SEEK_SET);
  if (ret < 0)
    {
      return -errno;
    }

  size_wave_header(&header, written);

  written = write(g_file, &header, sizeof(header));
  if (written != sizeof(header))
    {
      return -errno;
    }

  return 0;
}

static void capture_samples(FAR char *name, unsigned int card,
                            unsigned int device, unsigned long buffer_size,
                            unsigned int frag, unsigned int length,
                            unsigned int rate, unsigned int channels,
                            unsigned int format, unsigned int codec_id)
{
  FAR struct compress *compress;
  struct compr_config config;
  struct wave_header header;
  unsigned int total_read = 0;
  unsigned int samplebits;
  struct snd_codec codec;
  unsigned int size;
  FAR char *buffer;
  size_t written;
  int read;
  int ret;

  switch (format)
    {
    case SND_AUDIOMODE_PCM_S32_LE:
      samplebits = 32;
      break;
    default:
      samplebits = 16;
      break;
    }

  /* Convert length from seconds to bytes */

  length = length * rate * (samplebits / 8) * channels;

  if (g_verbose)
    {
      fprintf(g_finfo, "%s: entry, reading %u bytes\n", __func__, length);
    }

  if (!name)
    {
      g_file = STDOUT_FILENO;
    }
  else
    {
      g_file = open(name, O_RDWR | O_CREAT,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
      if (g_file == -1)
        {
          fprintf(stderr, "Unable to open file '%s'\n", name);
          exit(EXIT_FAILURE);
        }
    }

  /* Write a header, will update with size once record is complete */

  if (!g_streamed)
    {
      init_wave_header(&header, channels, rate, samplebits);
      written = write(g_file, &header, sizeof(header));
      if (written != sizeof(header))
        {
          fprintf(stderr, "Error writing output g_file header: %s\n",
                  strerror(errno));
          goto file_exit;
        }
    }

  memset(&codec, 0, sizeof(codec));
  memset(&config, 0, sizeof(config));
  codec.id = codec_id;
  codec.ch_in = channels;
  codec.ch_out = channels;
  codec.sample_rate = rate;

  if (!codec.sample_rate)
    {
      fprintf(stderr, "invalid sample rate %d\n", rate);
      goto file_exit;
    }

  codec.pcm_format = format;
  if ((buffer_size != 0) && (frag != 0))
    {
      config.fragment_size = buffer_size / frag;
      config.fragments = frag;
    }
  else
    {
      config.fragment_size = CONFIG_AUDIO_BUFFER_NUMBYTES;
      config.fragments = CONFIG_AUDIO_NUM_BUFFERS;
    }

  config.codec = &codec;

  compress = compress_open(card, device, COMPRESS_OUT, &config);
  if (!compress || !is_compress_ready(compress))
    {
      fprintf(stderr, "Unable to open Compress Card %u device %u\n", card,
              device);
      if (compress)
        {
          fprintf(stderr, "ERR: %s\n", compress_get_error(compress));
        }

      goto file_exit;
    };

  if (g_verbose)
    {
      fprintf(g_finfo, "%s: Opened compress device\n", __func__);
    }

  size = config.fragments * config.fragment_size;
  buffer = malloc(size);
  if (!buffer)
    {
      fprintf(stderr, "Unable to allocate %d bytes\n", size);
      goto comp_exit;
    }

  fprintf(
      g_finfo,
      "Recording g_file %s On Card %u device %u, with buffer of %u bytes\n",
      name, card, device, size);
  fprintf(g_finfo,
          "Codec %" PRIu32 " Format %" PRIu32 " Channels %" PRIu32
          ", %u Hz\n",
          codec.id, codec.format, codec.ch_out, rate);

  compress_start(compress);

  if (g_verbose)
    {
      fprintf(stderr, "%s: Capturing audio NOW!!!\n", __func__);
    }

  do
    {
      read = compress_read(compress, buffer, size);
      if (read < 0)
        {
          fprintf(stderr, "Error reading sample\n");
          fprintf(stderr, "ERR: %s\n", compress_get_error(compress));
          goto buf_exit;
        }

      if ((unsigned int)read != size)
        {
          fprintf(stderr, "We read %d, DSP sent %d\n", size, read);
        }

      if (read > 0)
        {
          total_read += read;

          written = write(g_file, buffer, read);
          if (written != (size_t)read)
            {
              fprintf(stderr, "Error writing output g_file: %s\n",
                      strerror(errno));
              goto buf_exit;
            }

          if (g_verbose)
            {
              print_time(compress);
              fprintf(g_finfo, "%s: read %d\n", __func__, read);
            }
        }
    }
  while (!length || total_read < length);

  ret = compress_stop(compress);
  if (ret < 0)
    {
      fprintf(stderr, "Error closing stream\n");
      fprintf(stderr, "ERR: %s\n", compress_get_error(compress));
    }

  ret = finish_record();
  if (ret < 0)
    {
      fprintf(stderr, "Failed to finish header: %s\n", strerror(-ret));
      goto buf_exit;
    }

  if (g_verbose)
    {
      fprintf(g_finfo, "%s: exit success\n", __func__);
    }

  free(buffer);
  close(g_file);
  g_file = 0;
  compress_close(compress);

  return;

buf_exit:
  free(buffer);
comp_exit:
  compress_close(compress);
file_exit:
  close(g_file);

  if (g_verbose)
    {
      fprintf(stderr, "%s: exit failure\n", __func__);
    }

  exit(EXIT_FAILURE);
}

static void sig_handler(int signum __attribute__((unused)))
{
  finish_record();

  if (g_file)
    {
      close(g_file);
    }

  _exit(EXIT_FAILURE);
}

int main(int argc, FAR char **argv)
{
  unsigned int channels = DEFAULT_CHANNELS;
  unsigned int codec_id = DEFAULT_CODEC_ID;
  unsigned int format = DEFAULT_FORMAT;
  unsigned int rate = DEFAULT_RATE;
  unsigned long buffer_size = 0;
  unsigned int card = 0;
  unsigned int device = 0;
  unsigned int length = 0;
  unsigned int frag = 0;
  FAR char *file;
  int c;
  int i;

  if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
      fprintf(stderr, "Error registering signal handler\n");
      exit(EXIT_FAILURE);
    }

  if (argc < 2)
    {
      usage();
    }

  g_verbose = 0;
  while ((c = getopt(argc, argv, "hvl:R:C:F:I:b:f:c:d:")) != -1)
    {
      switch (c)
        {
        case 'h':
          usage();
          break;
        case 'b':
          buffer_size = strtol(optarg, NULL, 0);
          break;
        case 'f':
          frag = strtol(optarg, NULL, 10);
          break;
        case 'c':
          card = strtol(optarg, NULL, 10);
          break;
        case 'd':
          device = strtol(optarg, NULL, 10);
          break;
        case 'v':
          g_verbose = 1;
          break;
        case 'l':
          length = strtol(optarg, NULL, 10);
          break;
        case 'R':
          rate = strtol(optarg, NULL, 10);
          break;
        case 'C':
          channels = strtol(optarg, NULL, 10);
          break;
        case 'F':
          if (strcmp(optarg, "S16_LE") == 0)
            {
              format = SND_AUDIOMODE_PCM_S16_LE;
            }
          else if (strcmp(optarg, "S32_LE") == 0)
            {
              format = SND_AUDIOMODE_PCM_S32_LE;
            }
          else
            {
              fprintf(stderr, "Unrecognised format: %s\n", optarg);
              usage();
            }
          break;
        case 'I':
          if (optarg[0] == '0')
            {
              codec_id = strtol(optarg, NULL, 0);
            }
          else
            {
              for (i = 0; i < CREC_NUM_CODEC_IDS; ++i)
                {
                  if (strcmp(optarg, g_codec_ids[i].name) == 0)
                    {
                      codec_id = g_codec_ids[i].id;
                      break;
                    }
                }

              if (i == CREC_NUM_CODEC_IDS)
                {
                  fprintf(stderr, "Unrecognised ID: %s\n", optarg);
                  usage();
                }
            }
          break;
        default:
          exit(EXIT_FAILURE);
        }
    }

  if (optind >= argc)
    {
      g_finfo = fopen("/dev/null", "w");
      g_streamed = true;
    }
  else if (codec_id == SND_AUDIOCODEC_PCM)
    {
      g_finfo = stdout;
      g_streamed = false;
    }
  else
    {
      fprintf(stderr, "ERROR: Only PCM can be written to a WAV file\n");
      exit(EXIT_FAILURE);
    }

  file = argv[optind];

  capture_samples(file, card, device, buffer_size, frag, length, rate,
                  channels, format, codec_id);

  fprintf(g_finfo, "Finish capturing... Close Normally\n");

  exit(EXIT_SUCCESS);
}
