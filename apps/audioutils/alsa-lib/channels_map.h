/****************************************************************************
 * apps/audioutils/alsa-lib/channels_map.h
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

#ifndef __APPS_AUDIOUTILS_ALSA_LIB_CHANNELS_MAP_H
#define __APPS_AUDIOUTILS_ALSA_LIB_CHANNELS_MAP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <alsa/pcm.h>

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct chmap_data
{
  FAR void *chmap_buf;
  size_t buf_size;

  snd_pcm_format_t format;
  unsigned int src_channels;
  unsigned int dst_channels;
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

int chmap_init(FAR struct chmap_data **cm, snd_pcm_format_t format,
               unsigned int src_channels, unsigned int dst_channels,
               size_t buf_size);

int chmap_process(FAR struct chmap_data *cm, FAR const void *in_data,
                  snd_pcm_uframes_t in_frames, FAR void **out_data,
                  FAR snd_pcm_uframes_t *out_frames);

void chmap_release(FAR struct chmap_data *cm);

#endif /* __APPS_AUDIOUTILS_ALSA_LIB_CHANNELS_MAP_H */