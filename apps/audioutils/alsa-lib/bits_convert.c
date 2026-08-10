/****************************************************************************
 * apps/audioutils/alsa-lib/bits_convert.c
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

#include "bits_convert.h"
#include "pcm_local.h"

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define BITSCONV_PACKET(name, in_type, out_type, trans)                     \
  static void bitsconv_##name(FAR const void **in_data, int frames, int ch, \
                              FAR void *out_data)                           \
  {                                                                         \
    FAR out_type *out = (FAR out_type *)out_data;                           \
    FAR const in_type *in = (FAR const in_type *)in_data[0];                \
    int i;                                                                  \
    int j;                                                                  \
                                                                            \
    for (i = 0; i < frames; i++)                                            \
      {                                                                     \
        for (j = 0; j < ch; j++)                                            \
          {                                                                 \
            *(out + j) = (out_type)trans(*(in + j));                        \
          }                                                                 \
                                                                            \
        in += ch;                                                           \
        out += ch;                                                          \
      }                                                                     \
  }

#define BITSCONV_PLANER(name, in_type, out_type, trans)                     \
  static void bitsconv_##name(FAR const void **in_data, int frames, int ch, \
                              FAR void *out_data)                           \
  {                                                                         \
    FAR out_type *out = (FAR out_type *)out_data;                           \
    FAR const in_type *in;                                                  \
    int i;                                                                  \
    int j;                                                                  \
                                                                            \
    for (i = 0; i < frames; i++)                                            \
      {                                                                     \
        for (j = 0; j < ch; j++)                                            \
          {                                                                 \
            in = (FAR const in_type *)in_data[j];                           \
            *(out + j) = (out_type)trans(*(in + i));                        \
          }                                                                 \
                                                                            \
        out += ch;                                                          \
      }                                                                     \
  }

#define trans_default(x) (x)
/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef void *(*trans_func)(void *);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static inline int32_t trans_s16_to_s32(int32_t in)
{
  return in << 16;
}

static inline int16_t trans_s32_to_s16(int32_t in)
{
  return in >> 16;
}

static inline int16_t trans_float_to_s16(float in)
{
  if (in > 1.0f)
    {
      return INT16_MAX;
    }
  else if (in < -1.0f)
    {
      return INT16_MIN;
    }
  else
    {
      return (int16_t)(in * 32767.0f);
    }
}

BITSCONV_PACKET(s16_to_s32, int16_t, int32_t, trans_s16_to_s32);
BITSCONV_PACKET(s32_to_s16, int32_t, int16_t, trans_s32_to_s16);
BITSCONV_PACKET(flt_to_s16, float, int16_t, trans_float_to_s16);
BITSCONV_PLANER(s16p_to_s16, int16_t, int16_t, trans_default);
BITSCONV_PLANER(s16p_to_s32, int16_t, int32_t, trans_s16_to_s32);
BITSCONV_PLANER(s32p_to_s16, int32_t, int16_t, trans_s32_to_s16);
BITSCONV_PLANER(s32p_to_s32, int32_t, int32_t, trans_default);
BITSCONV_PLANER(fltp_to_s16, float, int16_t, trans_float_to_s16);

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int bitsconv_init(FAR struct bitsconv_data **bc, int channels,
                  snd_pcm_format_t src_format, snd_pcm_format_t dst_format,
                  int buf_size)
{
  if (*bc != NULL)
    {
      return 0;
    }

  /* Support s16->s32, s32->s16, float->s16 only */

  if ((src_format != SND_PCM_FORMAT_S16_LE &&
       src_format != SND_PCM_FORMAT_S32_LE &&
       src_format != SND_PCM_FORMAT_FLOAT_LE) ||
      (dst_format != SND_PCM_FORMAT_S16_LE &&
       dst_format != SND_PCM_FORMAT_S32_LE))
    {
      SNDERR("format not supported! src: %d, dst: %d", src_format,
              dst_format);
      return -ENOTSUP;
    }

  *bc = malloc(sizeof(struct bitsconv_data));
  if (*bc == NULL)
    {
      SNDERR("bitsconv_init failed");
      return -ENOMEM;
    }

  (*bc)->bitsconv_buf = malloc(buf_size);
  if ((*bc)->bitsconv_buf == NULL)
    {
      free(*bc);
      *bc = NULL;
      SNDERR("malloc bitsconv_buf failed");
      return -ENOMEM;
    }

  (*bc)->channels = channels;
  (*bc)->src_format = src_format;
  (*bc)->dst_format = dst_format;
  (*bc)->buf_size = buf_size;

  return 0;
}

int bitsconv_process(FAR struct bitsconv_data *bc, FAR const void **in_datas,
                     int in_size, FAR void **out_data,
                     FAR snd_pcm_uframes_t *out_size, bool is_planer)
{
  int out_len;

  if (!bc)
    {
      SNDERR("bitsconv_init not run");
      return -EPERM;
    }

  if (bc->dst_format == SND_PCM_FORMAT_S16_LE)
    {
      out_len = in_size * bc->channels * sizeof(int16_t);
    }
  else if (bc->dst_format == SND_PCM_FORMAT_S32_LE)
    {
      out_len = in_size * bc->channels * sizeof(int32_t);
    }
  else
    {
      goto notsup;
    }

  if (out_len > bc->buf_size)
    {
      SNDERR("bitsconv out_buf too small");
      return -EPERM;
    }

  if (is_planer)
    {
      if (bc->src_format == SND_PCM_FORMAT_S16_LE)
        {
          if (bc->dst_format == SND_PCM_FORMAT_S16_LE)
            {
              bitsconv_s16p_to_s16(in_datas, in_size, bc->channels,
                                   bc->bitsconv_buf);
            }
          else if (bc->dst_format == SND_PCM_FORMAT_S32_LE)
            {
              bitsconv_s16p_to_s32(in_datas, in_size, bc->channels,
                                   bc->bitsconv_buf);
            }
          else
            {
              goto notsup;
            }
        }
      else if (bc->src_format == SND_PCM_FORMAT_S32_LE)
        {
          if (bc->dst_format == SND_PCM_FORMAT_S16_LE)
            {
              bitsconv_s32p_to_s16(in_datas, in_size, bc->channels,
                                   bc->bitsconv_buf);
            }
          else if (bc->dst_format == SND_PCM_FORMAT_S32_LE)
            {
              bitsconv_s32p_to_s32(in_datas, in_size, bc->channels,
                                   bc->bitsconv_buf);
            }
          else
            {
              goto notsup;
            }
        }
      else if (bc->src_format == SND_PCM_FORMAT_FLOAT_LE)
        {
          if (bc->dst_format == SND_PCM_FORMAT_S16_LE)
            {
              bitsconv_fltp_to_s16(in_datas, in_size, bc->channels,
                                   bc->bitsconv_buf);
            }
        }
      else
        {
          goto notsup;
        }
    }
  else
    {
      if (bc->src_format == SND_PCM_FORMAT_S16_LE &&
          bc->dst_format == SND_PCM_FORMAT_S32_LE)
        {
          bitsconv_s16_to_s32(in_datas, in_size, bc->channels,
                              bc->bitsconv_buf);
        }
      else if (bc->src_format == SND_PCM_FORMAT_S32_LE &&
               bc->dst_format == SND_PCM_FORMAT_S16_LE)
        {
          bitsconv_s32_to_s16(in_datas, in_size, bc->channels,
                              bc->bitsconv_buf);
        }
      else if (bc->src_format == SND_PCM_FORMAT_FLOAT_LE &&
               bc->dst_format == SND_PCM_FORMAT_S16_LE)
        {
          bitsconv_flt_to_s16(in_datas, in_size, bc->channels,
                              bc->bitsconv_buf);
        }
      else
        {
          goto notsup;
        }
    }

  *out_data = bc->bitsconv_buf;
  *out_size = (snd_pcm_uframes_t)in_size;

  return 0;

notsup:
  SNDERR("Not support format");
  return -ENOTSUP;
}

int bitsconv_release(FAR struct bitsconv_data *bc)
{
  if (!bc)
    {
      return 0;
    }

  if (bc->bitsconv_buf)
    {
      free(bc->bitsconv_buf);
      bc->bitsconv_buf = NULL;
    }

  free(bc);

  return 0;
}
