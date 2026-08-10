/****************************************************************************
 * apps/audioutils/alsa-lib/pcm_softvol.c
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

#include "pcm_local.h"

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define PCM_SND_SCALAR(name, type)                                          \
  static void snd_pcm_scalar_##name(FAR void *data,                         \
                                    unsigned int nb_samples, float volume)  \
  {                                                                         \
    FAR type *d = (FAR type *)data;                                         \
    unsigned int i = 0;                                                     \
                                                                            \
    for (i = 0; i < nb_samples; i++)                                        \
      {                                                                     \
        d[i] *= volume;                                                     \
      }                                                                     \
  }

PCM_SND_SCALAR(s16, int16_t)
PCM_SND_SCALAR(u16, uint16_t)
PCM_SND_SCALAR(s32, int32_t)
PCM_SND_SCALAR(u32, uint32_t)

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void snd_pcm_softvol_scale(snd_pcm_format_t format, float volume,
                           FAR void *data, unsigned int samples)
{
  switch (format)
    {
    case SND_PCM_FORMAT_S16:
      snd_pcm_scalar_s16(data, samples, volume);
      break;
    case SND_PCM_FORMAT_U16:
      snd_pcm_scalar_u16(data, samples, volume);
      break;
    case SND_PCM_FORMAT_S32:
      snd_pcm_scalar_s32(data, samples, volume);
      break;
    case SND_PCM_FORMAT_U32:
      snd_pcm_scalar_u32(data, samples, volume);
      break;
    default:
      SNDERR("Not support yet! format:%d", format);
    }
}
