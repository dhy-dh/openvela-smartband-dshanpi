/****************************************************************************
 * apps/audioutils/tinycompress/src/utils/cplay.c
 *
 * This file is provided under a dual BSD/LGPLv2.1 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * BSD LICENSE
 *
 * tinyplay command line player for compress audio offload in alsa
 * Copyright (c) 2011-2012, Intel Corporation
 * All rights reserved.
 *
 * Author: Vinod Koul <vkoul@kernel.org>
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
 * tinyplay command line player for compress audio offload in alsa
 * Copyright (c) 2011-2012, Intel Corporation.
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

#include <fcntl.h>
#include <inttypes.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <sound/compress_params.h>
#include <tinycompress/tinycompress.h>
#include <tinycompress/tinymp3.h>
#include <tinycompress/tinywave.h>

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define ID3V2_HEADER_SIZE 10
#define ID3V2_FILE_IDENTIFIER_SIZE 3
#define ADIF_HEADER_SIZE 20

/****************************************************************************
 * Private Types
 ****************************************************************************/

enum
{
  DO_NOTHING = -1,
  DO_PAUSE_PUSH,
  DO_PAUSE_RELEASE
};

struct mp3_header
{
  uint16_t sync;
  uint8_t format1;
  uint8_t format2;
};

struct codec_id
{
  FAR const char *name;
  unsigned int id;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static int g_verbose;
static int g_interactive;
static bool g_is_paused = false;
static long g_term_c_lflag = -1;
static long g_stdin_flags = -1;

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

#define CPLAY_NUM_CODEC_IDS (sizeof(g_codec_ids) / sizeof(g_codec_ids[0]))

static const int g_aac_sample_rates[] =
{
  96000, 88200, 64000, 48000, 44100,
  32000, 24000, 22050, 16000, 12000,
  11025, 8000,  7350
};

#define MAX_SR_NUM sizeof(g_aac_sample_rates) / sizeof(g_aac_sample_rates[0])

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void play_samples(FAR char *name, unsigned int card,
                         unsigned int device, unsigned long buffer_size,
                         unsigned int frag, unsigned long codec_id);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int get_sample_rate_from_index(int sr_index)
{
  if (sr_index >= 0 && sr_index < MAX_SR_NUM)
    {
      return g_aac_sample_rates[sr_index];
    }

  return 0;
}

static void init_stdin(void)
{
  struct termios term;
  int ret;

  if (!g_interactive)
    {
      return;
    }

  ret = tcgetattr(fileno(stdin), &term);
  if (ret < 0)
    {
      printf("Unable to get terminal attributes.\n");
      exit(EXIT_FAILURE);
    }

  /* Save previous terminal flags */

  g_term_c_lflag = term.c_lflag;

  /* Save previous stdin flags and add O_NONBLOCK */

  g_stdin_flags = fcntl(fileno(stdin), F_GETFL);
  if (g_stdin_flags < 0 ||
      fcntl(fileno(stdin), F_SETFL, g_stdin_flags | O_NONBLOCK) < 0)
    {
      printf("stdin O_NONBLOCK flag setup failed\n");
    }

  /* Prepare to enter noncanonical mode */

  term.c_lflag &= ~ICANON;

  ret = tcsetattr(fileno(stdin), TCSANOW, &term);
  if (ret < 0)
    {
      printf("Unable to set terminal attributes.\n");
      exit(EXIT_FAILURE);
    }
}

static void done_stdin(void)
{
  struct termios term;
  int ret;

  if (!g_interactive)
    {
      return;
    }

  if (g_term_c_lflag == -1 || g_stdin_flags == -1)
    {
      return;
    }

  ret = tcgetattr(fileno(stdin), &term);
  if (ret < 0)
    {
      printf("Unable to get terminal attributes.\n");
      exit(EXIT_FAILURE);
    }

  /* Restore previous terminal attributes */

  term.c_lflag = g_term_c_lflag;

  ret = tcsetattr(fileno(stdin), TCSANOW, &term);
  if (ret < 0)
    {
      printf("Unable to set terminal attributes.\n");
      exit(EXIT_FAILURE);
    }

  /* Restore previous stdin attributes */

  ret = fcntl(fileno(stdin), F_SETFL, g_stdin_flags);
  if (ret < 0)
    {
      printf("Unable to set stdin attributes.\n");
      exit(EXIT_FAILURE);
    }
}

static int do_pause(void)
{
  unsigned char chr;

  if (!g_interactive)
    {
      return DO_NOTHING;
    }

  while (read(fileno(stdin), &chr, 1) == 1)
    {
      switch (chr)
        {
        case '\r':
        case ' ':
          if (g_is_paused)
            {
              printf("\r=== Resume ===\n");
              return DO_PAUSE_RELEASE;
            }
          else
            {
              printf("\r=== Pause ===\n");
              return DO_PAUSE_PUSH;
            }
          break;
        default:
          break;
        }
    }

  return DO_NOTHING;
}

static void usage(void)
{
  int i;

  printf("usage: cplay [OPTIONS] filename\n"
         "-c\tcard number\n"
         "-d\tdevice name\n"
         "-I\tspecify codec ID (default is mp3)\n"
         "-b\tbuffer size\n"
         "-f\tfragments\n\n"
         "-v\tverbose mode\n"
         "-i\tinteractive mode (press SPACE or ENTER for play/pause)\n"
         "-h\tPrints this help list\n\n"
         "Example:\n"
         "\tcplay -d 0 test.mp3\n"
         "\tcplay -d 0 -f 4 test.mp3\n\n"
         "Valid codec IDs:\n");

  for (i = 0; i < CPLAY_NUM_CODEC_IDS; ++i)
    {
      printf("%s%c", g_codec_ids[i].name, ((i + 1) % 8) ? ' ' : '\n');
    }

  printf("\nor the value in decimal or hex\n");

  exit(EXIT_FAILURE);
}

static int find_adts_header(FAR FILE *file, FAR unsigned int *num_channels,
                            FAR unsigned int *sample_rate,
                            FAR unsigned int *format)
{
  unsigned char buf[5];
  int ret;

  ret = fread(buf, sizeof(buf), 1, file);
  if (ret < 0)
    {
      printf("open file error: %d\n", ret);
      return 0;
    }

  fseek(file, 0, SEEK_SET);

  if ((buf[0] != 0xff) || ((buf[1] & 0xf0) != 0xf0))
    {
      return 0;
    }

  switch (buf[1] >> 3 & 0x1)
    {
    case 0x0:
      *format = SND_AUDIOSTREAMFORMAT_MP4ADTS;
      break;
    case 0x1:
      *format = SND_AUDIOSTREAMFORMAT_MP2ADTS;
      break;
    }

  switch (buf[2] >> 2 & 0xf)
    {
    case 0x0:
      *sample_rate = 96000;
      break;
    case 0x1:
      *sample_rate = 88200;
      break;
    case 0x2:
      *sample_rate = 64000;
      break;
    case 0x3:
      *sample_rate = 48000;
      break;
    case 0x4:
      *sample_rate = 44100;
      break;
    case 0x5:
      *sample_rate = 32000;
      break;
    case 0x6:
      *sample_rate = 24000;
      break;
    case 0x7:
      *sample_rate = 22050;
      break;
    case 0x8:
      *sample_rate = 16000;
      break;
    case 0x9:
      *sample_rate = 12000;
      break;
    case 0xa:
      *sample_rate = 11025;
      break;
    case 0xb:
      *sample_rate = 8000;
      break;
    case 0xc:
      *sample_rate = 7350;
      break;
    default:
      break;
    }

  switch (((buf[2] & 0x1) << 2) | (buf[3] >> 6))
    {
    case 1:
      *num_channels = 1;
      break;
    case 2:
      *num_channels = 2;
      break;
    case 3:
      *num_channels = 3;
      break;
    case 4:
      *num_channels = 4;
      break;
    case 5:
      *num_channels = 5;
      break;
    case 6:
      *num_channels = 6;
      break;
    case 7:
      *num_channels = 7;
      break;
    default:
      break;
    }

  return 1;
}

static int find_adif_header(FAR FILE *file, FAR unsigned int *num_channels,
                            FAR unsigned int *sample_rate,
                            FAR unsigned int *format)
{
  unsigned char adif_header[ADIF_HEADER_SIZE];
  unsigned char adif_id[4];
  int skip_size = 0;
  int bitstream_type;
  int sr_index;
  int ret;

  ret = fread(adif_id, sizeof(unsigned char), 4, file);
  if (ret < 0)
    {
      printf("read data from file err: %d\n", ret);
      return 0;
    }

  /* Adif id */

  if ((adif_id[0] != 0x41) || (adif_id[1] != 0x44) || (adif_id[2] != 0x49) ||
      (adif_id[3] != 0x46))
    {
      return 0;
    }

  ret = fread(adif_header, sizeof(unsigned char), ADIF_HEADER_SIZE, file);
  if (ret != ADIF_HEADER_SIZE)
    {
      return 0;
    }

  /* Copyright string */

  if (adif_header[0] & 0x80)
    {
      skip_size = 9;
    }

  bitstream_type = adif_header[0 + skip_size] & 0x10;

  if (bitstream_type == 0)
    {
      sr_index = (adif_header[7 + skip_size] & 0x78) >> 3;
    }
  else
    {
      sr_index = ((adif_header[4 + skip_size] & 0x07) << 1) |
                 ((adif_header[5 + skip_size] & 0x80) >> 7);
    }

  /* Sample rate */

  *sample_rate = get_sample_rate_from_index(sr_index);

  /* FIXME: assume channels is 2 */

  *num_channels = 2;

  *format = SND_AUDIOSTREAMFORMAT_ADIF;
  fseek(file, 0, SEEK_SET);
  return 1;
}

static int parse_aac_header(FAR FILE *file, FAR unsigned int *num_channels,
                            FAR unsigned int *sample_rate,
                            FAR unsigned int *format)
{
  if (find_adts_header(file, num_channels, sample_rate, format))
    {
      return 1;
    }
  else if (find_adif_header(file, num_channels, sample_rate, format))
    {
      return 1;
    }
  else
    {
      printf("can't find streams format\n");
      return 0;
    }

  return 1;
}

static int parse_mp3_header(FAR struct mp3_header *header,
                            FAR unsigned int *num_channels,
                            FAR unsigned int *sample_rate,
                            FAR unsigned int *bit_rate)
{
  int sample_rate_idx;
  int bit_rate_idx;
  int mp3_version;
  int channel_idx;
  int ver_idx;
  int layer;

  /* check sync bits */

  if ((header->sync & MP3_SYNC) != MP3_SYNC)
    {
      printf("Error: Can't find sync word\n");
      return -1;
    }

  ver_idx = (header->sync >> 11) & 0x03;
  mp3_version = ver_idx == 0 ? MPEG25 : ((ver_idx & 0x1) ? MPEG1 : MPEG2);
  layer = 4 - ((header->sync >> 9) & 0x03);
  bit_rate_idx = ((header->format1 >> 4) & 0x0f);
  sample_rate_idx = ((header->format1 >> 2) & 0x03);
  channel_idx = ((header->format2 >> 6) & 0x03);

  if (sample_rate_idx == 3 || layer == 4 || bit_rate_idx == 15)
    {
      printf("Error: Can't find valid header\n");
      return -1;
    }

  *num_channels = (channel_idx == MONO ? 1 : 2);
  *sample_rate = mp3_sample_rates[mp3_version][sample_rate_idx];
  *bit_rate = (mp3_bit_rates[mp3_version][layer - 1][bit_rate_idx]) * 1000;
  if (g_verbose)
    {
      printf("%s: exit\n", __func__);
    }

  return 0;
}

static int print_time(FAR struct compress *compress)
{
  unsigned int avail;
  struct timespec tstamp;

  if (compress_get_hpointer(compress, &avail, &tstamp) != 0)
    {
      printf("Error querying timestamp\n");
      printf("ERR: %s\n", compress_get_error(compress));
      return -1;
    }
  else
    {
      printf("DSP played %jd.%jd\n", (intmax_t)tstamp.tv_sec,
             (intmax_t)tstamp.tv_nsec * 1000);
    }

  return 0;
}

int main(int argc, FAR char **argv)
{
  unsigned int codec_id = SND_AUDIOCODEC_MP3;
  unsigned long buffer_size = 0;
  unsigned int card = 0;
  unsigned int device = 0;
  unsigned int frag = 0;
  FAR char *file;
  int c;
  int i;

  if (argc < 2)
    {
      usage();
    }

  g_verbose = 0;
  while ((c = getopt(argc, argv, "hvb:f:c:d:I:i")) != -1)
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
        case 'I':
          if (optarg[0] == '0')
            {
              codec_id = strtol(optarg, NULL, 0);
            }
          else
            {
              for (i = 0; i < CPLAY_NUM_CODEC_IDS; ++i)
                {
                  if (strcmp(optarg, g_codec_ids[i].name) == 0)
                    {
                      codec_id = g_codec_ids[i].id;
                      break;
                    }
                }

              if (i == CPLAY_NUM_CODEC_IDS)
                {
                  printf("Unrecognised ID: %s\n", optarg);
                  usage();
                }
            }
          break;
        case 'v':
          g_verbose = 1;
          break;
        case 'i':
          printf("g_interactive mode: ON\n");
          g_interactive = 1;
          break;
        default:
          exit(EXIT_FAILURE);
        }
    }

  if (optind >= argc)
    {
      usage();
    }

  file = argv[optind];

  play_samples(file, card, device, buffer_size, frag, codec_id);

  printf("Finish Playing.... Close Normally\n");
  exit(EXIT_SUCCESS);
}

static void get_codec_pcm(FAR FILE *file, FAR struct compr_config *config,
                          FAR struct snd_codec *codec)
{
  struct wave_header header;
  unsigned int channels;
  unsigned int format;
  unsigned int rate;
  size_t read;

  read = fread(&header, 1, sizeof(header), file);
  if (read != sizeof(header))
    {
      printf("Unable to read header \n");
      goto exit;
    }

  if (parse_wave_header(&header, &channels, &rate, &format) == -1)
    {
      goto exit;
    }

  if (!rate)
    {
      printf("invalid sample rate %d\n", rate);
      goto exit;
    }

  codec->id = SND_AUDIOCODEC_PCM;
  codec->ch_in = channels;
  codec->ch_out = channels;
  codec->sample_rate = rate;
  codec->bit_rate = 0;
  codec->rate_control = 0;
  codec->profile = SND_AUDIOCODEC_PCM;
  codec->level = 0;
  codec->ch_mode = 0;
  codec->format = format;

  return;

exit:
  fclose(file);
  exit(EXIT_FAILURE);
}

static void get_codec_aac(FAR FILE *file, FAR struct compr_config *config,
                          FAR struct snd_codec *codec)
{
  unsigned int channels = 0;
  unsigned int format = 0;
  unsigned int rate = 0;

  if (parse_aac_header(file, &channels, &rate, &format) == 0)
    {
      fclose(file);
      exit(EXIT_FAILURE);
    }

  fseek(file, 0, SEEK_SET);

  codec->id = SND_AUDIOCODEC_AAC;
  codec->ch_in = channels;
  codec->ch_out = channels;
  codec->sample_rate = rate;
  codec->bit_rate = 0;
  codec->rate_control = 0;
  codec->profile = SND_AUDIOPROFILE_AAC;
  codec->level = 0;
  codec->ch_mode = 0;
  codec->format = format;
}

static int skip_id3v2_header(FAR FILE *file)
{
  unsigned char buffer[ID3V2_HEADER_SIZE + 1];
  uint32_t header_size;
  int bytes_read;
  int ret;

  ret = fseek(file, 0, SEEK_SET);
  if (ret < 0)
    {
      return ret;
    }

  buffer[ID3V2_HEADER_SIZE] = '\0';

  /* Read first ID3V2_HEADER_SIZE chunk */

  bytes_read = fread(buffer, sizeof(unsigned char), ID3V2_HEADER_SIZE, file);

  /* ID3v2 header is 10 bytes long
   *
   * if we can't read the 10 bytes then there's obviously no
   * ID3v2 tag to skip
   */

  if (bytes_read != ID3V2_HEADER_SIZE)
    {
      return 0;
    }

  /* Check if we're dealing with ID3v2 */

  if (strncmp((FAR const char *)buffer, "ID3",
              ID3V2_FILE_IDENTIFIER_SIZE) != 0)
    {
      return 0;
    }

  header_size = buffer[9];
  header_size |= (buffer[8] << 7);
  header_size |= (buffer[7] << 14);
  header_size |= (buffer[6] << 21);

  /* The header size field in ID3v2 header doesn't
   * include the 10 bytes of which the header is made
   * so we need to add them to get the correct position
   */

  return header_size + ID3V2_HEADER_SIZE;
}

static void get_codec_mp3(FAR FILE *file, FAR struct compr_config *config,
                          FAR struct snd_codec *codec)
{
  struct mp3_header header;
  unsigned int channels;
  unsigned int rate;
  unsigned int bits;
  size_t read;
  int offset;

  offset = skip_id3v2_header(file);
  if (offset < 0)
    {
      printf("Failed to get ID3 tag end position.\n");
      goto exit;
    }

  if (fseek(file, offset, SEEK_SET) < 0)
    {
      printf("Unable to seek.\n");
      goto exit;
    }

  read = fread(&header, 1, sizeof(header), file);
  if (read != sizeof(header))
    {
      printf("Unable to read header \n");
      goto exit;
    }

  if (parse_mp3_header(&header, &channels, &rate, &bits) == -1)
    {
      goto exit;
    }

  if (!rate)
    {
      printf("invalid sample rate %d\n", rate);
      goto exit;
    }

  if (fseek(file, offset, SEEK_SET) < 0)
    {
      printf("Failed to set cursor to offset.\n");
      goto exit;
    }

  codec->id = SND_AUDIOCODEC_MP3;
  codec->ch_in = channels;
  codec->ch_out = channels;
  codec->sample_rate = rate;
  codec->bit_rate = bits;
  codec->rate_control = 0;
  codec->profile = 0;
  codec->level = 0;
  codec->ch_mode = 0;
  codec->format = 0;

  return;

exit:
  fclose(file);
  exit(EXIT_FAILURE);
}

static int check_stdin(FAR struct compress *compress)
{
  switch (do_pause())
    {
    case DO_PAUSE_PUSH:
      if (compress_pause(compress) != 0)
        {
          printf("Pause ERROR\n");
          return -1;
        }

      g_is_paused = true;
      break;
    case DO_PAUSE_RELEASE:
      if (compress_resume(compress) != 0)
        {
          printf("Resume ERROR\n");
          return -1;
        }

      g_is_paused = false;
      break;
    case DO_NOTHING:
      break;
    }

  return 0;
}

static void play_samples(FAR char *name, unsigned int card,
                         unsigned int device, unsigned long buffer_size,
                         unsigned int frag, unsigned long codec_id)
{
  FAR struct compress *compress;
  struct compr_config config;
  struct snd_codec codec;
  FAR char *buffer;
  FAR FILE *file;
  int num_read;
  int wrote;
  int size;

  if (g_verbose)
    {
      printf("%s: entry\n", __func__);
    }

  file = fopen(name, "rb");
  if (!file)
    {
      printf("Unable to open file '%s'\n", name);
      exit(EXIT_FAILURE);
    }

  init_stdin();

  switch (codec_id)
    {
    case SND_AUDIOCODEC_PCM:
      get_codec_pcm(file, &config, &codec);
      break;
    case SND_AUDIOCODEC_AAC:
      get_codec_aac(file, &config, &codec);
      break;
    case SND_AUDIOCODEC_MP3:
      get_codec_mp3(file, &config, &codec);
      break;
    default:
      printf("codec ID %ld is not supported\n", codec_id);
      exit(EXIT_FAILURE);
    }

  if ((buffer_size != 0) && (frag != 0))
    {
      config.fragment_size = buffer_size / frag;
      config.fragments = frag;
    }
  else
    {
      /* Use driver defaults */

      config.fragment_size = CONFIG_AUDIO_BUFFER_NUMBYTES;
      config.fragments = CONFIG_AUDIO_NUM_BUFFERS;
    }

  config.codec = &codec;

  compress = compress_open(card, device, COMPRESS_IN, &config);
  if (!compress || !is_compress_ready(compress))
    {
      printf("Unable to open Compress Card %u device %u\n", card, device);
      if (compress)
        {
          printf("ERR: %s\n", compress_get_error(compress));
        }

      goto file_exit;
    }

  if (g_verbose)
    {
      printf("%s: Opened compress device\n", __func__);
    }

  size = config.fragments * config.fragment_size;
  buffer = malloc(size);
  if (!buffer)
    {
      printf("Unable to allocate %d bytes\n", size);
      goto comp_exit;
    }

  /* We will write frag fragment_size and then start */

  num_read = fread(buffer, 1, size, file);
  if (num_read > 0)
    {
      if (g_verbose)
        {
          printf("%s: Doing first buffer write of %d\n", __func__, num_read);
        }

      wrote = compress_write(compress, buffer, num_read);
      if (wrote < 0)
        {
          printf("Error %d playing sample\n", wrote);
          printf("ERR: %s\n", compress_get_error(compress));
          goto buf_exit;
        }

      if (wrote != num_read)
        {
          /* TODO: Buffer pointer needs to be set here */

          printf("We wrote %d, DSP accepted %d\n", num_read, wrote);
        }
    }

  printf("Playing file %s On Card %u device %u, with buffer of %d bytes\n",
         name, card, device, size);
  printf("Format %" PRIu32 " Channels %" PRIu32 ", %" PRIu32 " Hz, "
         "Bit Rate %" PRIu32 "\n",
         codec.id, codec.ch_in, codec.sample_rate, codec.bit_rate);

  compress_start(compress);

  if (g_verbose)
    {
      printf("%s: You should hear audio NOW!!!\n", __func__);
    }

  do
    {
      if (check_stdin(compress) != 0)
        {
          goto buf_exit;
        }

      if (!g_is_paused)
        {
          num_read = fread(buffer, 1, size, file);
        }
      else
        {
          num_read = 0;
        }

      if (num_read > 0)
        {
          wrote = compress_write(compress, buffer, num_read);
          if (wrote < 0)
            {
              printf("Error playing sample\n");
              printf("ERR: %s\n", compress_get_error(compress));
              goto buf_exit;
            }

          if (wrote != num_read)
            {
              /* TODO: Buffer pointer needs to be set here */

              printf("We wrote %d, DSP accepted %d\n", num_read, wrote);
            }

          if (g_verbose)
            {
              print_time(compress);
              printf("%s: wrote %d\n", __func__, wrote);
            }
        }
    }
  while (num_read > 0 || g_is_paused == true);

  if (g_verbose)
    {
      printf("%s: exit success\n", __func__);
    }

  /* Issue drain if it supports */

  compress_drain(compress);
  free(buffer);
  fclose(file);
  compress_close(compress);
  done_stdin();

  return;

buf_exit:
  free(buffer);
comp_exit:
  compress_close(compress);
file_exit:
  done_stdin();
  fclose(file);
  if (g_verbose)
    {
      printf("%s: exit failure\n", __func__);
    }

  exit(EXIT_FAILURE);
}
