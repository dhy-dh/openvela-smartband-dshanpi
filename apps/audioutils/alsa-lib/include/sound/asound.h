/****************************************************************************
 * apps/audioutils/alsa-lib/include/sound/asound.h
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

#ifndef __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ASOUND_ASOUND_H
#define __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ASOUND_ASOUND_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <endian.h>
#include <nuttx/audio/audio.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define SNDRV_PROTOCOL_VERSION(major, minor, subminor)                      \
  (((major) << 16) | ((minor) << 8) | (subminor))

#define SNDRV_PCM_HW_PARAM_ACCESS         0
#define SNDRV_PCM_HW_PARAM_FORMAT         1
#define SNDRV_PCM_HW_PARAM_FIRST_MASK     SNDRV_PCM_HW_PARAM_ACCESS
#define SNDRV_PCM_HW_PARAM_LAST_MASK      SNDRV_PCM_HW_PARAM_FORMAT
#define SNDRV_PCM_HW_PARAM_CHANNELS       2
#define SNDRV_PCM_HW_PARAM_RATE           3
#define SNDRV_PCM_HW_PARAM_PERIOD_TIME    4
#define SNDRV_PCM_HW_PARAM_PERIOD_SIZE    5
#define SNDRV_PCM_HW_PARAM_PERIODS        6
#define SNDRV_PCM_HW_PARAM_BUFFER_TIME    7
#define SNDRV_PCM_HW_PARAM_BUFFER_SIZE    8
#define SNDRV_PCM_HW_PARAM_FIRST_INTERVAL SNDRV_PCM_HW_PARAM_CHANNELS
#define SNDRV_PCM_HW_PARAM_LAST_INTERVAL  SNDRV_PCM_HW_PARAM_BUFFER_SIZE

/****************************************************************************
 * Public Types
 ****************************************************************************/

enum sndrv_pcm_format
{
  SNDRV_PCM_FORMAT_MU_LAW = AUDIO_SUBFMT_PCM_MU_LAW,
  SNDRV_PCM_FORMAT_A_LAW  = AUDIO_SUBFMT_PCM_A_LAW,
  SNDRV_PCM_FORMAT_U8     = AUDIO_SUBFMT_PCM_U8,
  SNDRV_PCM_FORMAT_S8     = AUDIO_SUBFMT_PCM_S8,
  SNDRV_PCM_FORMAT_U16_LE = AUDIO_SUBFMT_PCM_U16_LE,
  SNDRV_PCM_FORMAT_S16_BE = AUDIO_SUBFMT_PCM_S16_BE,
  SNDRV_PCM_FORMAT_S16_LE = AUDIO_SUBFMT_PCM_S16_LE,
  SNDRV_PCM_FORMAT_U16_BE = AUDIO_SUBFMT_PCM_U16_BE,
  SNDRV_PCM_FORMAT_U24_LE = AUDIO_SUBFMT_PCM_U24_LE,
  SNDRV_PCM_FORMAT_S24_BE = AUDIO_SUBFMT_PCM_S24_BE,
  SNDRV_PCM_FORMAT_S24_LE = AUDIO_SUBFMT_PCM_S24_LE,
  SNDRV_PCM_FORMAT_U24_BE = AUDIO_SUBFMT_PCM_U24_BE,
  SNDRV_PCM_FORMAT_U32_LE = AUDIO_SUBFMT_PCM_U32_LE,
  SNDRV_PCM_FORMAT_U32_BE = AUDIO_SUBFMT_PCM_U32_BE,
  SNDRV_PCM_FORMAT_S32_LE = AUDIO_SUBFMT_PCM_S32_LE,
  SNDRV_PCM_FORMAT_S32_BE = AUDIO_SUBFMT_PCM_S32_BE,
  SNDRV_PCM_FORMAT_FLOAT_LE = AUDIO_SUBFMT_PCM_FLOAT_LE,
  SNDRV_PCM_FORMAT_FLOAT_BE = AUDIO_SUBFMT_PCM_FLOAT_BE,
  SNDRV_PCM_FORMAT_FIRST  = SNDRV_PCM_FORMAT_MU_LAW,
  SNDRV_PCM_FORMAT_LAST   = SNDRV_PCM_FORMAT_U32_BE,
#if BYTE_ORDER == LITTLE_ENDIAN
  SNDRV_PCM_FORMAT_S16    = SNDRV_PCM_FORMAT_S16_LE,
  SNDRV_PCM_FORMAT_U16    = SNDRV_PCM_FORMAT_U16_LE,
  SNDRV_PCM_FORMAT_S24    = SNDRV_PCM_FORMAT_S24_LE,
  SNDRV_PCM_FORMAT_U24    = SNDRV_PCM_FORMAT_U24_LE,
  SNDRV_PCM_FORMAT_S32    = SNDRV_PCM_FORMAT_S32_LE,
  SNDRV_PCM_FORMAT_U32    = SNDRV_PCM_FORMAT_U32_LE,
  SNDRV_PCM_FORMAT_FLOAT  = SNDRV_PCM_FORMAT_FLOAT_LE,
#else
  SNDRV_PCM_FORMAT_S16    = SNDRV_PCM_FORMAT_S16_BE,
  SNDRV_PCM_FORMAT_U16    = SNDRV_PCM_FORMAT_U16_BE,
  SNDRV_PCM_FORMAT_S24    = SNDRV_PCM_FORMAT_S24_BE,
  SNDRV_PCM_FORMAT_U24    = SNDRV_PCM_FORMAT_U24_BE,
  SNDRV_PCM_FORMAT_S32    = SNDRV_PCM_FORMAT_S32_BE,
  SNDRV_PCM_FORMAT_U32    = SNDRV_PCM_FORMAT_U32_BE,
  SNDRV_PCM_FORMAT_FLOAT  = SNDRV_PCM_FORMAT_FLOAT_BE,
#endif
};

typedef unsigned long snd_pcm_uframes_t;
typedef long snd_pcm_sframes_t;

typedef int snd_pcm_hw_param_t;

typedef struct snd_interval
{
  unsigned int min;
  unsigned int max;
} snd_interval_t;

typedef struct snd_mask
{
  unsigned int mask;
} snd_mask_t;

typedef struct snd_pcm_hw_params
{
  snd_mask_t masks[SNDRV_PCM_HW_PARAM_LAST_MASK -
                   SNDRV_PCM_HW_PARAM_FIRST_MASK + 1];
  snd_interval_t intervals[SNDRV_PCM_HW_PARAM_LAST_INTERVAL -
                           SNDRV_PCM_HW_PARAM_FIRST_INTERVAL + 1];
} snd_pcm_hw_params_t;

typedef struct snd_pcm_sw_params
{
  snd_pcm_uframes_t avail_min;       /* Min avail frames for wakeup */
  snd_pcm_uframes_t start_threshold; /* Min hw_avail frames for automatic start */
  snd_pcm_uframes_t stop_threshold;  /* Min avail frames for automatic stop */
  snd_pcm_uframes_t silence_size;    /* Silence block size */
  snd_pcm_uframes_t boundary;        /* Pointers wrap point */
} snd_pcm_sw_params_t;

#ifdef __cplusplus
}
#endif

#endif /* __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ASOUND_ASOUND_H */