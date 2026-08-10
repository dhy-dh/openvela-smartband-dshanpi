/****************************************************************************
 * apps/audioutils/tinycompress/src/utils/wave.c
 *
 * SPDX-License-Identifier: (LGPL-2.1-only OR BSD-3-Clause)
 *
 * WAVE helper functions
 *
 * Copyright 2021 NXP
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <string.h>

#include <sound/compress_params.h>
#include <tinycompress/tinywave.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct wave_header g_blank_wave_header =
{
  .riff =
    {
      .chunk =
        {
          .desc = "RIFF",
        },
      .format = "WAVE",
    },
    .fmt =
      {
        .chunk =
          {
            .desc = "fmt ", /* Note the space is important here */
            .size = sizeof(g_blank_wave_header.fmt) -
                      sizeof(g_blank_wave_header.fmt.chunk),
          },
        .type = 0x01, /* PCM */
      },
    .data =
      {
        .chunk =
          {
            .desc = "data",
          },
      },
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void init_wave_header(FAR struct wave_header *header, uint16_t channels,
                      uint32_t rate, uint16_t samplebits)
{
  memcpy(header, &g_blank_wave_header, sizeof(g_blank_wave_header));

  header->fmt.channels = channels;
  header->fmt.rate = rate;
  header->fmt.byterate = channels * rate * (samplebits / 8);
  header->fmt.blockalign = channels * (samplebits / 8);
  header->fmt.samplebits = samplebits;
}

void size_wave_header(FAR struct wave_header *header, uint32_t size)
{
  header->riff.chunk.size =
      sizeof(*header) - sizeof(header->riff.chunk) + size;
  header->data.chunk.size = size;
}

int parse_wave_header(FAR struct wave_header *header,
                      FAR unsigned int *channels, FAR unsigned int *rate,
                      FAR unsigned int *format)
{
  if (strncmp(header->riff.chunk.desc, "RIFF", 4) != 0)
    {
      fprintf(stderr, "RIFF magic not found\n");
      return -1;
    }

  if (strncmp(header->riff.format, "WAVE", 4) != 0)
    {
      fprintf(stderr, "WAVE magic not found\n");
      return -1;
    }

  if (strncmp(header->fmt.chunk.desc, "fmt", 3) != 0)
    {
      fprintf(stderr, "FMT section not found");
      return -1;
    }

  *channels = header->fmt.channels;
  *rate = header->fmt.rate;

  switch (header->fmt.samplebits)
    {
    case 8:
      *format = SND_AUDIOMODE_PCM_U8;
      break;
    case 16:
      *format = SND_AUDIOMODE_PCM_S16_LE;
      break;
    case 32:
      *format = SND_AUDIOMODE_PCM_S32_LE;
      break;
    default:
      fprintf(stderr, "Unsupported sample bits %d\n",
              header->fmt.samplebits);
      return -1;
    }

  return 0;
}
