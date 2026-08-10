/****************************************************************************
 * apps/audioutils/alsa-lib/pcm_local.h
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

#ifndef __APPS_AUDIOUTILS_ALSA_LIB_PCM_LOCAL_H
#define __APPS_AUDIOUTILS_ALSA_LIB_PCM_LOCAL_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <alsa/error.h>
#include <alsa/pcm.h>
#include <mqueue.h>
#include <sys/ioctl.h>

/****************************************************************************
 * Public Types
 ****************************************************************************/

typedef struct
{
  int (*close)(FAR snd_pcm_t *pcm);
  int (*hw_refine)(FAR snd_pcm_t *pcm, FAR snd_pcm_hw_params_t *params);
  int (*hw_params)(FAR snd_pcm_t *pcm, FAR snd_pcm_hw_params_t *params);
  void (*dump)(FAR snd_pcm_t *pcm, FAR snd_output_t *out);
  int (*prepare)(FAR snd_pcm_t *pcm);
  int (*reset)(FAR snd_pcm_t *pcm);
  int (*start)(FAR snd_pcm_t *pcm);
  int (*drop)(FAR snd_pcm_t *pcm);
  int (*drain)(FAR snd_pcm_t *pcm);
  int (*pause)(FAR snd_pcm_t *pcm, int enable);
  snd_pcm_state_t (*state)(FAR snd_pcm_t *pcm);
  int (*delay)(FAR snd_pcm_t *pcm, FAR snd_pcm_sframes_t *delayp);
  int (*resume)(FAR snd_pcm_t *pcm);
  snd_pcm_sframes_t (*writei)(FAR snd_pcm_t *pcm, FAR const void *buffer,
                              snd_pcm_uframes_t size);
  snd_pcm_sframes_t (*writen)(FAR snd_pcm_t *pcm, FAR void **bufs,
                              snd_pcm_uframes_t size);
  snd_pcm_sframes_t (*readi)(FAR snd_pcm_t *pcm, FAR void *buffer,
                             snd_pcm_uframes_t size);
  snd_pcm_sframes_t (*avail_update)(FAR snd_pcm_t *pcm);
  int (*mmap)(FAR snd_pcm_t *pcm);
  int (*munmap)(FAR snd_pcm_t *pcm);
  int (*mmap_begin)(FAR snd_pcm_t *pcm,
                    FAR const snd_pcm_channel_area_t **areas,
                    FAR snd_pcm_uframes_t *offset,
                    FAR snd_pcm_uframes_t *frames);
  snd_pcm_sframes_t (*mmap_commit)(FAR snd_pcm_t *pcm,
                                   snd_pcm_uframes_t offset,
                                   snd_pcm_uframes_t size);
  int (*poll_descriptors_count)(FAR snd_pcm_t *pcm);
  int (*poll_descriptors)(FAR snd_pcm_t *pcm, FAR struct pollfd *pfds,
                          unsigned int space);
  int (*poll_revents)(FAR snd_pcm_t *pcm, FAR struct pollfd *pfds,
                      unsigned int nfds, FAR unsigned short *revents);
} snd_pcm_ops_t;

struct snd_pcm_s
{
  FAR char *name;
  snd_pcm_type_t type;
  snd_pcm_stream_t stream;
  int mode;
  snd_pcm_access_t access;
  snd_pcm_format_t format;
  unsigned int channels;
  unsigned int rate;
  snd_pcm_uframes_t period_size;
  unsigned int period_time;
  unsigned int periods;
  snd_pcm_uframes_t start_threshold;
  unsigned int sample_bits;
  unsigned int frame_bits;
  unsigned long appl;
  float volume;
  const FAR snd_pcm_ops_t *ops;
  FAR snd_pcm_t *ops_arg;
  FAR void *private_data;
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

int snd_pcm_new(FAR snd_pcm_t **pcmp, snd_pcm_type_t type,
                FAR const char *name, snd_pcm_stream_t stream, int mode);
int snd_pcm_free(FAR snd_pcm_t *pcm);

void snd_pcm_softvol_scale(snd_pcm_format_t format, float volume,
                           FAR void *data, unsigned int samples);

int snd_pcm_hw_open(FAR snd_pcm_t **pcmp, FAR const char *name,
                    snd_pcm_stream_t stream, int mode);

int snd_pcm_dmix_open(FAR snd_pcm_t **pcmp, FAR const char *name,
                      snd_pcm_stream_t stream, int mode);

#endif /* __APPS_AUDIOUTILS_ALSA_LIB_PCM_LOCAL_H */
