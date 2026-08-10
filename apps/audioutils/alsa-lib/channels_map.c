/****************************************************************************
 * apps/audioutils/alsa-lib/channels_map.c
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

#include "channels_map.h"
#include "pcm_local.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int chmap_init(FAR struct chmap_data **cm, snd_pcm_format_t format,
               unsigned int src_channels, unsigned int dst_channels,
               size_t buf_size)
{
  if (*cm != NULL)
    {
      return 0;
    }

  if (format != SND_PCM_FORMAT_S16_LE && format != SND_PCM_FORMAT_S32_LE)
    {
      SNDERR("format not supported");
      return -ENOTSUP;
    }

  *cm = malloc(sizeof(struct chmap_data));
  if (*cm == NULL)
    {
      SNDERR("chmap_ap_init failed");
      return -ENOMEM;
    }

  (*cm)->chmap_buf = malloc(buf_size);
  if ((*cm)->chmap_buf == NULL)
    {
      free(*cm);
      *cm = NULL;
      SNDERR("malloc chmap_buf failed");
      return -ENOMEM;
    }

  (*cm)->format = format;
  (*cm)->src_channels = src_channels;
  (*cm)->dst_channels = dst_channels;
  (*cm)->buf_size = buf_size;

  return 0;
}

static int chmap_process_16(FAR struct chmap_data *cm, uint8_t src_ch,
                            uint8_t dst_ch, FAR const void *in_data,
                            snd_pcm_uframes_t in_frames)
{
  FAR const int16_t *in;
  FAR int16_t *out;
  int32_t tmp;
  int out_len;
  int iin;
  int iout;
  int i;
  int j;
  int z;

  out_len = in_frames * dst_ch * sizeof(int16_t);
  if (out_len > cm->buf_size)
    {
      SNDERR("cm out_buf too small");
      return -EPERM;
    }

  in = (FAR const int16_t *)in_data;
  out = (FAR int16_t *)cm->chmap_buf;
  for (i = 0; i < dst_ch; i++)
    {
      for (j = 0; j < in_frames; j++)
        {
          tmp = 0;
          iout = j * dst_ch + i;
          for (z = 0; z < src_ch; z++)
            {
              iin = j * src_ch + z;
              tmp += in[iin];
            }

          tmp /= src_ch;

          if (tmp > 0x7fff)
            {
              out[iout] = 0x7fff;
            }
          else if (tmp < -0x8000)
            {
              out[iout] = -0x8000;
            }
          else
            {
              out[iout] = tmp;
            }
        }
    }

  return 0;
}

static int chmap_process_32(FAR struct chmap_data *cm, uint8_t src_ch,
                            uint8_t dst_ch, FAR const void *in_data,
                            snd_pcm_uframes_t in_frames)
{
  FAR const int32_t *in;
  FAR int32_t *out;
  int64_t tmp;
  int out_len;
  int iin;
  int iout;
  int i;
  int j;
  int z;

  out_len = in_frames * dst_ch * sizeof(int32_t);
  if (out_len > cm->buf_size)
    {
      SNDERR("cm out_buf too small");
      return -EPERM;
    }

  in = (FAR const int32_t *)in_data;
  out = (FAR int32_t *)cm->chmap_buf;
  for (i = 0; i < dst_ch; i++)
    {
      for (j = 0; j < in_frames; j++)
        {
          tmp = 0;
          iout = j * dst_ch + i;
          for (z = 0; z < src_ch; z++)
            {
              iin = j * src_ch + z;
              tmp += in[iin];
            }

          tmp /= src_ch;

          if (tmp > (int)0x7fffffff)
            {
              out[iout] = (int)0x7fffffff;
            }
          else if (tmp < (int)0x80000000)
            {
              out[iout] = (int)0x80000000;
            }
          else
            {
              out[iout] = tmp;
            }
        }
    }

  return 0;
}

int chmap_process(FAR struct chmap_data *cm, FAR const void *in_data,
                  snd_pcm_uframes_t in_frames, FAR void **out_data,
                  FAR snd_pcm_uframes_t *out_frames)
{
  if (!cm)
    {
      SNDERR("chmap_ap_init not run");
      return -EPERM;
    }

  if (cm->format == SND_PCM_FORMAT_S16_LE)
    {
      chmap_process_16(cm, cm->src_channels, cm->dst_channels, in_data,
                       in_frames);
    }
  else if (cm->format == SND_PCM_FORMAT_S32_LE)
    {
      chmap_process_32(cm, cm->src_channels, cm->dst_channels, in_data,
                       in_frames);
    }
  else
    {
      SNDERR("unknown format");
      return -ENOTSUP;
    }

  *out_data = cm->chmap_buf;
  *out_frames = in_frames;

  return 0;
}

void chmap_release(FAR struct chmap_data *cm)
{
  if (!cm)
    {
      return;
    }

  if (cm->chmap_buf != NULL)
    {
      free(cm->chmap_buf);
      cm->chmap_buf = NULL;
    }

  free(cm);

  return;
}
