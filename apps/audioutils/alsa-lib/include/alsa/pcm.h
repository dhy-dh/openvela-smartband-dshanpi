/****************************************************************************
 * apps/audioutils/alsa-lib/include/alsa/pcm.h
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

#ifndef __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ALSA_PCM_H
#define __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ALSA_PCM_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <alloca.h>
#include <nuttx/compiler.h>
#include <sound/asound.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define SND_PCM_NONBLOCK O_NONBLOCK

/****************************************************************************
 * Public Types
 ****************************************************************************/

typedef unsigned int snd_pcm_format_mask_t;
typedef FILE snd_output_t;

/** PCM type */

typedef enum
{
  /** Kernel level PCM */

  SND_PCM_TYPE_HW,

  SND_PCM_TYPE_DMIX,

  SND_PCM_TYPE_LAST = SND_PCM_TYPE_DMIX
} snd_pcm_type_t;

typedef struct
{
  /** base address of channel samples */

  FAR void *addr;

  /** offset to first sample in bits */

  unsigned int first;

  /** samples distance in bits */

  unsigned int step;
} snd_pcm_channel_area_t;

/* PCM handle */

typedef struct snd_pcm_s snd_pcm_t;

/* PCM stream (direction) */

typedef enum
{
  /* Playback stream */

  SND_PCM_STREAM_PLAYBACK,

  /* Capture stream */

  SND_PCM_STREAM_CAPTURE,
  SND_PCM_STREAM_LAST = SND_PCM_STREAM_CAPTURE
} snd_pcm_stream_t;

/* PCM access type */

typedef enum
{
  /* mmap access with simple interleaved channels */

  SND_PCM_ACCESS_MMAP_INTERLEAVED,

  /* mmap access with simple non interleaved channels */

  SND_PCM_ACCESS_MMAP_NONINTERLEAVED,

  /* mmap access with complex placement */

  SND_PCM_ACCESS_MMAP_COMPLEX,

  /* snd_pcm_readi/snd_pcm_writei access */

  SND_PCM_ACCESS_RW_INTERLEAVED,

  /* snd_pcm_readn/snd_pcm_writen access */

  SND_PCM_ACCESS_RW_NONINTERLEAVED,
  SND_PCM_ACCESS_LAST = SND_PCM_ACCESS_RW_NONINTERLEAVED
} snd_pcm_access_t;

/* PCM sample format */

typedef enum
{
  /* Unknown */

  SND_PCM_FORMAT_UNKNOWN = -1,

  /* Signed 8 bit */

  SND_PCM_FORMAT_S8 = SNDRV_PCM_FORMAT_S8,

  /* Unsigned 8 bit */

  SND_PCM_FORMAT_U8 = SNDRV_PCM_FORMAT_U8,

  /* Signed 16 bit Little Endian */

  SND_PCM_FORMAT_S16_LE = SNDRV_PCM_FORMAT_S16_LE,

  /* Signed 16 bit Big Endian */

  SND_PCM_FORMAT_S16_BE = SNDRV_PCM_FORMAT_S16_BE,

  /* Unsigned 16 bit Little Endian */

  SND_PCM_FORMAT_U16_LE = SNDRV_PCM_FORMAT_U16_LE,

  /* Unsigned 16 bit Big Endian */

  SND_PCM_FORMAT_U16_BE = SNDRV_PCM_FORMAT_U16_BE,

  /* Signed 24 bit Little Endian using low three bytes in 32-bit word */

  SND_PCM_FORMAT_S24_LE = SNDRV_PCM_FORMAT_S24_LE,

  /* Signed 24 bit Big Endian using low three bytes in 32-bit word */

  SND_PCM_FORMAT_S24_BE = SNDRV_PCM_FORMAT_S24_BE,

  /* Unsigned 24 bit Little Endian using low three bytes in 32-bit word */

  SND_PCM_FORMAT_U24_LE = SNDRV_PCM_FORMAT_U24_LE,

  /* Unsigned 24 bit Big Endian using low three bytes in 32-bit word */

  SND_PCM_FORMAT_U24_BE = SNDRV_PCM_FORMAT_U24_BE,

  /* Signed 32 bit Little Endian */

  SND_PCM_FORMAT_S32_LE = SNDRV_PCM_FORMAT_S32_LE,

  /* Signed 32 bit Big Endian */

  SND_PCM_FORMAT_S32_BE = SNDRV_PCM_FORMAT_S32_BE,

  /* Unsigned 32 bit Little Endian */

  SND_PCM_FORMAT_U32_LE = SNDRV_PCM_FORMAT_U32_LE,

  /* Unsigned 32 bit Big Endian */

  SND_PCM_FORMAT_U32_BE = SNDRV_PCM_FORMAT_U32_BE,

  /* Float 32 bit Little Endian */

  SND_PCM_FORMAT_FLOAT_LE = SNDRV_PCM_FORMAT_FLOAT_LE,

  /* Float 32 bit Big Endian */

  SND_PCM_FORMAT_FLOAT_BE = SNDRV_PCM_FORMAT_FLOAT_BE,

  /* Signed 16 bit CPU endian */

  SND_PCM_FORMAT_S16 = SNDRV_PCM_FORMAT_S16,

  /* Unsigned 16 bit CPU endian */

  SND_PCM_FORMAT_U16 = SNDRV_PCM_FORMAT_U16,

  /* Signed 24 bit CPU endian */

  SND_PCM_FORMAT_S24 = SNDRV_PCM_FORMAT_S24,

  /* Unsigned 24 bit CPU endian */

  SND_PCM_FORMAT_U24 = SNDRV_PCM_FORMAT_U24,

  /* Signed 32 bit CPU endian */

  SND_PCM_FORMAT_S32 = SNDRV_PCM_FORMAT_S32,

  /* Unsigned 32 bit CPU endian */

  SND_PCM_FORMAT_U32 = SNDRV_PCM_FORMAT_U32,

  /* Float 32 bit CPU endian */

  SND_PCM_FORMAT_FLOAT = SNDRV_PCM_FORMAT_FLOAT,

  SND_PCM_FORMAT_LAST = SNDRV_PCM_FORMAT_U32_BE,
} snd_pcm_format_t;

/* PCM state */

typedef enum
{
  /* Open */

  SND_PCM_STATE_OPEN = AUDIO_STATE_OPEN,

  /* Ready to start */

  SND_PCM_STATE_PREPARED = AUDIO_STATE_PREPARED,

  /* Paused */

  SND_PCM_STATE_PAUSED = AUDIO_STATE_PAUSED,

  /* Stopped: underrun (playback) or overrun (capture) detected */

  SND_PCM_STATE_XRUN = AUDIO_STATE_XRUN,

  /* Draining: running (playback) or stopped (capture) */

  SND_PCM_STATE_DRAINING = AUDIO_STATE_DRAINING,

  /* Running */

  SND_PCM_STATE_RUNNING = AUDIO_STATE_RUNNING,

  /* Setup installed */

  SND_PCM_STATE_SETUP,

  /* Hardware is suspended */

  SND_PCM_STATE_SUSPENDED,

  /* Hardware is disconnected */

  SND_PCM_STATE_DISCONNECTED,
  SND_PCM_STATE_LAST = SND_PCM_STATE_DISCONNECTED,

  /* Private - used internally in the library - do not use */

  SND_PCM_STATE_PRIVATE1 = 1024
} snd_pcm_state_t;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

int snd_pcm_open(FAR snd_pcm_t **pcm, FAR const char *name,
                 snd_pcm_stream_t stream, int mode);
int snd_pcm_close(FAR snd_pcm_t *pcm);
FAR const char *snd_pcm_name(FAR snd_pcm_t *pcm);
snd_pcm_type_t snd_pcm_type(FAR snd_pcm_t *pcm);
snd_pcm_stream_t snd_pcm_stream(FAR snd_pcm_t *pcm);
int snd_pcm_poll_descriptors_count(FAR snd_pcm_t *pcm);
int snd_pcm_poll_descriptors(FAR snd_pcm_t *pcm, FAR struct pollfd *pfds,
                             unsigned int space);
int snd_pcm_poll_descriptors_revents(FAR snd_pcm_t *pcm,
                                     FAR struct pollfd *pfds,
                                     unsigned int nfds,
                                     FAR unsigned short *revents);
int snd_pcm_nonblock(FAR snd_pcm_t *pcm, int nonblock);
int snd_pcm_prepare(FAR snd_pcm_t *pcm);
int snd_pcm_reset(FAR snd_pcm_t *pcm);
int snd_pcm_start(FAR snd_pcm_t *pcm);
int snd_pcm_pause(FAR snd_pcm_t *pcm, int enable);
snd_pcm_state_t snd_pcm_state(FAR snd_pcm_t *pcm);
int snd_pcm_delay(FAR snd_pcm_t *pcm, FAR snd_pcm_sframes_t *delayp);
int snd_pcm_drop(FAR snd_pcm_t *pcm);
int snd_pcm_drain(FAR snd_pcm_t *pcm);
int snd_pcm_resume(FAR snd_pcm_t *pcm);
snd_pcm_sframes_t snd_pcm_avail_update(FAR snd_pcm_t *pcm);
int snd_pcm_wait(FAR snd_pcm_t *pcm, int timeout);
snd_pcm_sframes_t snd_pcm_writei(FAR snd_pcm_t *pcm, FAR const void *buffer,
                                 snd_pcm_uframes_t size);
snd_pcm_sframes_t snd_pcm_readi(FAR snd_pcm_t *pcm, FAR void *buffer,
                                snd_pcm_uframes_t size);
snd_pcm_sframes_t snd_pcm_writen(FAR snd_pcm_t *pcm, FAR void **bufs,
                                 snd_pcm_uframes_t size);

int snd_pcm_recover(FAR snd_pcm_t *pcm, int err, int silent);
int snd_pcm_set_params(FAR snd_pcm_t *pcm, snd_pcm_format_t format,
                       snd_pcm_access_t access, unsigned int channels,
                       unsigned int rate, int soft_resample,
                       unsigned int latency);
int snd_pcm_get_params(FAR snd_pcm_t *pcm,
                       FAR snd_pcm_uframes_t *buffer_size,
                       FAR snd_pcm_uframes_t *period_size);

snd_pcm_sframes_t snd_pcm_bytes_to_frames(FAR snd_pcm_t *pcm, ssize_t bytes);
ssize_t snd_pcm_frames_to_bytes(FAR snd_pcm_t *pcm,
                                snd_pcm_sframes_t frames);
long snd_pcm_bytes_to_samples(FAR snd_pcm_t *pcm, ssize_t bytes);
ssize_t snd_pcm_samples_to_bytes(FAR snd_pcm_t *pcm, long samples);

/* Volume control, nuttx private interface */

int snd_pcm_set_volume(FAR snd_pcm_t *pcm, unsigned int volume); /* Volume: [0, 100] */
int snd_pcm_set_volume_db(FAR snd_pcm_t *pcm, int db);           /* Db: dB * 100 */

int snd_pcm_dump(FAR snd_pcm_t *pcm, FAR snd_output_t *out);

/* Hw & sw define */

#define snd_pcm_alloca(ptr, type)                \
    do                                           \
      {                                          \
        *ptr = (FAR type *)alloca(sizeof(type)); \
        memset(*ptr, 0, sizeof(type));           \
      }                                          \
    while (0)

#define snd_pcm_format_mask_alloca(ptr) snd_pcm_alloca(ptr, snd_pcm_format_mask_t)

int snd_pcm_format_mask_test(FAR const snd_pcm_format_mask_t *mask,
                             snd_pcm_format_t val);
void snd_pcm_format_mask_set(FAR snd_pcm_format_mask_t *mask,
                             FAR snd_pcm_format_t val);

#define snd_pcm_hw_params_alloca(ptr) snd_pcm_alloca(ptr, snd_pcm_hw_params_t)

int snd_pcm_hw_params_any(FAR snd_pcm_t *pcm,
                          FAR snd_pcm_hw_params_t *params);

int snd_pcm_hw_params_get_access(FAR const snd_pcm_hw_params_t *params,
                                 FAR snd_pcm_access_t *access);

int snd_pcm_hw_params_set_access(FAR snd_pcm_t *pcm,
                                 FAR snd_pcm_hw_params_t *params,
                                 snd_pcm_access_t access);

int snd_pcm_hw_params_get_format(FAR const snd_pcm_hw_params_t *params,
                                 FAR snd_pcm_format_t *val);
void snd_pcm_hw_params_get_format_mask(FAR snd_pcm_hw_params_t *params,
                                       FAR snd_pcm_format_mask_t *mask);
int snd_pcm_hw_params_set_format(FAR snd_pcm_t *pcm,
                                 FAR snd_pcm_hw_params_t *params,
                                 snd_pcm_format_t format);
int snd_pcm_hw_params_set_format_mask(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params,
                                      FAR snd_pcm_format_mask_t *mask);

int snd_pcm_hw_params_get_channels(FAR const snd_pcm_hw_params_t *params,
                                   FAR unsigned int *val);
int snd_pcm_hw_params_get_channels_min(FAR const snd_pcm_hw_params_t *params,
                                       FAR unsigned int *val);
int snd_pcm_hw_params_get_channels_max(FAR const snd_pcm_hw_params_t *params,
                                       FAR unsigned int *val);
int snd_pcm_hw_params_set_channels(FAR snd_pcm_t *pcm,
                                   FAR snd_pcm_hw_params_t *params,
                                   unsigned int val);
int snd_pcm_hw_params_set_channels_min(FAR snd_pcm_t *pcm,
                                       FAR snd_pcm_hw_params_t *params,
                                       FAR unsigned int *val);
int snd_pcm_hw_params_set_channels_max(FAR snd_pcm_t *pcm,
                                       FAR snd_pcm_hw_params_t *params,
                                       FAR unsigned int *val);

int snd_pcm_hw_params_get_rate(FAR const snd_pcm_hw_params_t *params,
                               FAR unsigned int *val, FAR int *dir);
int snd_pcm_hw_params_get_rate_min(FAR const snd_pcm_hw_params_t *params,
                                   FAR unsigned int *val, FAR int *dir);
int snd_pcm_hw_params_get_rate_max(FAR const snd_pcm_hw_params_t *params,
                                   FAR unsigned int *val, FAR int *dir);
int snd_pcm_hw_params_set_rate(FAR snd_pcm_t *pcm,
                               FAR snd_pcm_hw_params_t *params,
                               unsigned int val, int dir);
int snd_pcm_hw_params_set_rate_near(FAR snd_pcm_t *pcm,
                                    FAR snd_pcm_hw_params_t *params,
                                    FAR unsigned int *val, FAR int *dir);
int snd_pcm_hw_params_set_rate_min(FAR snd_pcm_t *pcm,
                                   FAR snd_pcm_hw_params_t *params,
                                   FAR unsigned int *val, FAR int *dir);
int snd_pcm_hw_params_set_rate_max(FAR snd_pcm_t *pcm,
                                   FAR snd_pcm_hw_params_t *params,
                                   FAR unsigned int *val, FAR int *dir);

int snd_pcm_hw_params_set_period_time(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params,
                                      unsigned int us, int dir);
int snd_pcm_hw_params_set_period_time_near(FAR snd_pcm_t *pcm,
                                           FAR snd_pcm_hw_params_t *params,
                                           FAR unsigned int *us,
                                           FAR int *dir);
int snd_pcm_hw_params_get_period_time(FAR const snd_pcm_hw_params_t *params,
                                      FAR unsigned int *us, FAR int *dir);

int snd_pcm_hw_params_set_period_size(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params,
                                      snd_pcm_uframes_t val, int dir);
int snd_pcm_hw_params_set_period_size_near(FAR snd_pcm_t *pcm,
                                           FAR snd_pcm_hw_params_t *params,
                                           FAR snd_pcm_uframes_t *val,
                                           FAR int *dir);
int snd_pcm_hw_params_get_period_size(FAR const snd_pcm_hw_params_t *params,
                                      FAR snd_pcm_uframes_t *val,
                                      FAR int *dir);

int snd_pcm_hw_params_set_periods(FAR snd_pcm_t *pcm,
                                  FAR snd_pcm_hw_params_t *params,
                                  unsigned int val, int dir);
int snd_pcm_hw_params_set_periods_near(FAR snd_pcm_t *pcm,
                                       FAR snd_pcm_hw_params_t *params,
                                       FAR unsigned int *val, FAR int *dir);
int snd_pcm_hw_params_get_periods(FAR const snd_pcm_hw_params_t *params,
                                  FAR unsigned int *val, FAR int *dir);

int snd_pcm_hw_params_set_buffer_size(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params,
                                      snd_pcm_uframes_t val);
int snd_pcm_hw_params_set_buffer_size_near(FAR snd_pcm_t *pcm,
                                           FAR snd_pcm_hw_params_t *params,
                                           FAR snd_pcm_uframes_t *val);
int snd_pcm_hw_params_get_buffer_size(FAR const snd_pcm_hw_params_t *params,
                                      FAR snd_pcm_uframes_t *val);

int snd_pcm_hw_params_set_buffer_time(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params,
                                      unsigned int us, int dir);
int snd_pcm_hw_params_set_buffer_time_near(FAR snd_pcm_t *pcm,
                                           FAR snd_pcm_hw_params_t *params,
                                           FAR unsigned int *us,
                                           FAR int *dir);
int snd_pcm_hw_params_get_buffer_time(FAR const snd_pcm_hw_params_t *params,
                                      FAR unsigned int *us, FAR int *dir);
int snd_pcm_hw_params_current(FAR snd_pcm_t *pcm,
                              FAR snd_pcm_hw_params_t *params);
int snd_pcm_hw_params(FAR snd_pcm_t *pcm, FAR snd_pcm_hw_params_t *params);

#define snd_pcm_sw_params_alloca(ptr) snd_pcm_alloca(ptr, snd_pcm_sw_params_t)

int snd_pcm_sw_params_current(FAR snd_pcm_t *pcm,
                              FAR snd_pcm_sw_params_t *params);

int snd_pcm_sw_params_set_start_threshold(FAR snd_pcm_t *pcm,
                                          FAR snd_pcm_sw_params_t *params,
                                          snd_pcm_uframes_t val);
int snd_pcm_sw_params_get_start_threshold(FAR snd_pcm_sw_params_t *params,
                                          FAR snd_pcm_uframes_t *val);

int snd_pcm_sw_params_set_stop_threshold(FAR snd_pcm_t *pcm,
                                         FAR snd_pcm_sw_params_t *params,
                                         snd_pcm_uframes_t val);
int snd_pcm_sw_params_get_stop_threshold(FAR snd_pcm_sw_params_t *params,
                                         FAR snd_pcm_uframes_t *val);

int snd_pcm_sw_params_set_silence_size(FAR snd_pcm_t *pcm,
                                       FAR snd_pcm_sw_params_t *params,
                                       snd_pcm_uframes_t val);
int snd_pcm_sw_params_get_silence_size(FAR snd_pcm_sw_params_t *params,
                                       FAR snd_pcm_uframes_t *val);

int snd_pcm_sw_params_set_avail_min(FAR snd_pcm_t *pcm,
                                    FAR snd_pcm_sw_params_t *params,
                                    snd_pcm_uframes_t val);
int snd_pcm_sw_params_get_avail_min(FAR const snd_pcm_sw_params_t *params,
                                    FAR snd_pcm_uframes_t *val);

int snd_pcm_sw_params_get_boundary(FAR const snd_pcm_sw_params_t *params,
                                   FAR snd_pcm_uframes_t *val);

int snd_pcm_sw_params(FAR snd_pcm_t *pcm, FAR snd_pcm_sw_params_t *params);

FAR const char *snd_pcm_stream_name(const snd_pcm_stream_t stream);
FAR const char *snd_pcm_access_name(const snd_pcm_access_t access);
FAR const char *snd_pcm_format_name(const snd_pcm_format_t format);
snd_pcm_format_t snd_pcm_format_value(FAR const char *name);
FAR const char *snd_pcm_format_description(const snd_pcm_format_t format);
FAR const char *snd_pcm_state_name(const snd_pcm_state_t state);

int snd_pcm_mmap(FAR snd_pcm_t *pcm);
int snd_pcm_munmap(FAR snd_pcm_t *pcm);
int snd_pcm_mmap_begin(FAR snd_pcm_t *pcm,
                       FAR const snd_pcm_channel_area_t **areas,
                       FAR snd_pcm_uframes_t *offset,
                       FAR snd_pcm_uframes_t *frames);
snd_pcm_sframes_t snd_pcm_mmap_commit(FAR snd_pcm_t *pcm,
                                      snd_pcm_uframes_t offset,
                                      snd_pcm_uframes_t frames);
snd_pcm_sframes_t snd_pcm_mmap_writei(FAR snd_pcm_t *pcm,
                                      FAR const void *buffer,
                                      snd_pcm_uframes_t size);
snd_pcm_sframes_t snd_pcm_mmap_readi(FAR snd_pcm_t *pcm, FAR void *buffer,
                                     snd_pcm_uframes_t size);

int snd_pcm_format_little_endian(snd_pcm_format_t format);
int snd_pcm_format_big_endian(snd_pcm_format_t format);
int snd_pcm_format_cpu_endian(snd_pcm_format_t format);
int snd_pcm_format_width(snd_pcm_format_t format);
int snd_pcm_format_physical_width(snd_pcm_format_t format);
snd_pcm_format_t snd_pcm_build_linear_format(int width, int pwidth,
                                             int unsignd,
                                             int big_endian);
ssize_t snd_pcm_format_size(snd_pcm_format_t format, size_t samples);

#ifdef __cplusplus
}
#endif

#endif /* __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ALSA_PCM_H */
