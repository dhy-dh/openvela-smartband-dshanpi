/****************************************************************************
 * apps/audioutils/alsa-lib/pcm_dmix.c
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

#include <fcntl.h>
#include <math.h>
#include <nuttx/audio/audio.h>
#include <speex_resampler.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/param.h>

#include "bits_convert.h"
#include "channels_map.h"
#include "pcm_local.h"

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define RESAMPLE_IO_FORMART SND_PCM_FORMAT_S16_LE

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef void(mix_engine_t)(FAR volatile void *dst,
                           FAR const void *src, size_t size);

typedef struct
{
  snd_pcm_uframes_t nmaxframes; /* The maximum number of bytes */
  snd_pcm_uframes_t nframes;    /* The number of bytes used */
  FAR uint8_t *data;            /* Offset of the first sample */
} pcm_buffer_t;

typedef struct snd_pcm_dmix
{
  FAR sem_t *semid;

  unsigned long *appl_ptr;
  unsigned long appl_reset;

  FAR struct bitsconv_data *bc;
  FAR struct bitsconv_data *bc_s16;
  FAR struct chmap_data *cm;
  FAR SpeexResamplerState *resampler;
  FAR uint8_t *resmp_out;
  snd_pcm_t *spcm;
  snd_pcm_state_t state;
  pcm_buffer_t last_buffer;
  bool running;

  struct
  {
    FAR uint8_t **samp;
    FAR mix_engine_t *mix_engine;
  } mixer;
} snd_pcm_dmix_t;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void default_mixer_8(FAR volatile int8_t *dst,
                            FAR const int8_t *src, size_t size)
{
  int16_t sample;

  while (size-- > 0)
    {
      sample = *src + *dst;
      if (sample > INT8_MAX)
        {
          sample = INT8_MAX;
        }
      else if (sample < INT8_MIN)
        {
          sample = INT8_MIN;
        }

      *dst = (int8_t)sample;

      src++;
      dst++;
    }
}

static void default_mixer_16(FAR volatile int16_t *dst,
                             FAR const int16_t *src, size_t size)
{
  int32_t sample;

  while (size-- > 0)
    {
      sample = *src + *dst;
      if (sample > INT16_MAX)
        {
          sample = INT16_MAX;
        }
      else if (sample < INT16_MIN)
        {
          sample = INT16_MIN;
        }

      *dst = (int16_t)sample;

      src++;
      dst++;
    }
}

static void default_mixer_24(FAR volatile int8_t *dst,
                             FAR const int8_t *src, size_t size)
{
  int32_t sample;

  while (size-- > 0)
    {
      sample = src[0] + (src[1] << 8) + (src[2] << 16);
      if (sample == 0)
        {
          sample += dst[0] + (dst[1] << 8) + (dst[2] << 16);
          if (sample > 0x7fffff)
            {
              sample = 0x7fffff;
            }
          else if (sample < -0x800000)
            {
              sample = -0x800000;
            }
        }

      dst[0] = (int8_t)(sample & 0xff);
      dst[1] = (int8_t)((sample >> 8) & 0xff);
      dst[2] = (int8_t)((sample >> 16) & 0xff);

      dst += 3;
      src += 3;
    }
}

static void default_mixer_32(FAR volatile int32_t *dst,
                             FAR const int32_t *src, size_t size)
{
  int64_t sample;

  while (size-- > 0)
    {
      sample = *src + *dst;
      if (sample > INT32_MAX)
        {
          sample = INT32_MAX;
        }
      else if (sample < INT32_MIN)
        {
          sample = INT32_MIN;
        }

      *dst = (int32_t)sample;

      src++;
      dst++;
    }
}

static void default_mixer_init(FAR snd_pcm_dmix_t *dmix)
{
  switch (dmix->spcm->format)
    {
    case SND_PCM_FORMAT_S8:
    case SND_PCM_FORMAT_U8:
      dmix->mixer.mix_engine = (FAR mix_engine_t *)default_mixer_8;
      break;
    case SND_PCM_FORMAT_S16_LE:
    case SND_PCM_FORMAT_S16_BE:
      dmix->mixer.mix_engine = (FAR mix_engine_t *)default_mixer_16;
      break;
    case SND_PCM_FORMAT_S24_LE:
    case SND_PCM_FORMAT_S24_BE:
      dmix->mixer.mix_engine = (FAR mix_engine_t *)default_mixer_24;
      break;
    case SND_PCM_FORMAT_S32_LE:
    case SND_PCM_FORMAT_S32_BE:
      dmix->mixer.mix_engine = (FAR mix_engine_t *)default_mixer_32;
      break;
    default:
      return;
    }
}

static snd_pcm_state_t snd_pcm_dmix_state(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;

  return dmix->state;
}

static int snd_pcm_dmix_negotiate(FAR snd_pcm_t *pcm,
                                  FAR struct audio_info_s *info)
{
  FAR snd_pcm_hw_params_t *hw_params;
  FAR snd_pcm_format_mask_t *fmask;
  snd_pcm_format_t format;
  unsigned int min;
  unsigned int max;
  int ret;

  snd_pcm_hw_params_alloca(&hw_params);
  ret = snd_pcm_hw_params_any(pcm, hw_params);
  if (ret < 0)
    {
      return ret;
    }

  info->format = AUDIO_FMT_PCM;

  if (CONFIG_AUDIOUTILS_ALSA_LIB_OUTPUT_FORMAT)
    {
      info->subformat = CONFIG_AUDIOUTILS_ALSA_LIB_OUTPUT_FORMAT;
    }
  else
    {
      snd_pcm_format_mask_alloca(&fmask);
      snd_pcm_hw_params_get_format_mask(hw_params, fmask);
      if (snd_pcm_format_mask_test(fmask, pcm->format))
        {
          info->subformat = snd_pcm_format_width(pcm->format);
        }
      else if (snd_pcm_format_mask_test(fmask, SND_PCM_FORMAT_S16_LE))
        {
          info->subformat = snd_pcm_format_width(SND_PCM_FORMAT_S16_LE);
        }
      else if (snd_pcm_format_mask_test(fmask, SND_PCM_FORMAT_S32_LE))
        {
          info->subformat = snd_pcm_format_width(SND_PCM_FORMAT_S32_LE);
        }
      else
        {
          for (format = 0; format <= SND_PCM_FORMAT_LAST; format++)
            {
              if (snd_pcm_format_mask_test(fmask, format))
                {
                  info->subformat = snd_pcm_format_width(format);
                  break;
                }
            }
        }
    }

  if (CONFIG_AUDIOUTILS_ALSA_LIB_OUTPUT_CHANNELS)
    {
      info->channels = CONFIG_AUDIOUTILS_ALSA_LIB_OUTPUT_CHANNELS;
    }
  else
    {
      ret = snd_pcm_hw_params_get_channels_min(hw_params, &min);
      if (ret < 0)
        {
          return ret;
        }

      ret = snd_pcm_hw_params_get_channels_max(hw_params, &max);
      if (ret < 0)
        {
          return ret;
        }

      if (pcm->channels >= min && pcm->channels <= max)
        {
          info->channels = pcm->channels;
        }
      else
        {
          info->channels = min;
        }
    }

  if (CONFIG_AUDIOUTILS_ALSA_LIB_OUTPUT_RATE)
    {
      info->samplerate = CONFIG_AUDIOUTILS_ALSA_LIB_OUTPUT_RATE;
    }
  else
    {
      ret = snd_pcm_hw_params_get_rate_min(hw_params, &min, NULL);
      if (ret < 0)
        {
          return ret;
        }

      ret = snd_pcm_hw_params_get_rate_max(hw_params, &max, NULL);
      if (ret < 0)
        {
          return ret;
        }

      if (pcm->rate >= min && pcm->rate <= max)
        {
          info->samplerate = pcm->rate;
        }
      else
        {
          info->samplerate = min;
        }
    }

  return 0;
}

int snd_pcm_dmix_sem_open(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  dmix->semid = sem_open(pcm->name, O_CREAT, 0666, 1);
  if (dmix->semid == SEM_FAILED)
    {
      return -errno;
    }

  return 0;
}

static void snd_pcm_dmix_sem_close(FAR snd_pcm_dmix_t *dmix)
{
  if (dmix->semid != SEM_FAILED)
    {
      sem_close(dmix->semid);
      dmix->semid = SEM_FAILED;
    }

  return;
}

static inline int dmix_down_sem(FAR snd_pcm_dmix_t *dmix)
{
  int ret;
  while ((ret = sem_wait(dmix->semid) != 0))
    {
      if (errno != EINTR)
        {
          return ret;
        }
    }

  return ret;
}

static inline int dmix_up_sem(FAR snd_pcm_dmix_t *dmix)
{
  return sem_post(dmix->semid);
}

static snd_pcm_uframes_t
snd_pcm_dmix_playback_avail(FAR snd_pcm_dmix_t *dmix)
{
  snd_pcm_sframes_t used;

  if (dmix->state == SND_PCM_STATE_PAUSED)
    {
      return 0;
    }

  used = dmix->spcm->ops->avail_update(dmix->spcm->ops_arg);
  if (used == -EPIPE)
    {
      if (dmix->state == SND_PCM_STATE_DRAINING ||
          dmix->state == SND_PCM_STATE_SETUP)
        {
          dmix->state = SND_PCM_STATE_SETUP;
          return 0;
        }

      dmix->state = SND_PCM_STATE_XRUN;
      return 0;
    }

  if (dmix->state != SND_PCM_STATE_PREPARED)
    {
      dmix->appl_reset = dmix->spcm->appl;
    }

  return used;
}

static inline snd_pcm_uframes_t snd_pcm_dmix_written(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;

  return (*dmix->appl_ptr - dmix->appl_reset) * pcm->period_size;
}

static int snd_pcm_dmix_delay(FAR snd_pcm_t *pcm,
                              FAR snd_pcm_sframes_t *delayp)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  int ret;

  switch (dmix->state)
    {
    case SND_PCM_STATE_DRAINING:
    case SND_PCM_STATE_RUNNING:
    case SND_PCM_STATE_PREPARED:
    case SND_PCM_STATE_SUSPENDED:
      ret = dmix->spcm->ops->delay(dmix->spcm->ops_arg, delayp);
      *delayp = *delayp * pcm->rate / dmix->spcm->rate;
      *delayp += dmix->last_buffer.nframes;
      SNDDEBUG("get delay:%ld", *delayp);
      return ret;
    case SND_PCM_STATE_XRUN:
      return -EPIPE;
    case SND_PCM_STATE_DISCONNECTED:
      return -ENODEV;
    default:
      return -EBADFD;
    }
}

static int snd_pcm_dmix_hw_refine(FAR snd_pcm_t *pcm,
                                  FAR snd_pcm_hw_params_t *params)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;

  return dmix->spcm->ops->hw_refine(dmix->spcm->ops_arg, params);
}

static int snd_pcm_dmix_hw_params(FAR snd_pcm_t *pcm,
                                  FAR snd_pcm_hw_params_t *params)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  snd_pcm_uframes_t period_size;
  snd_pcm_hw_params_t *sparams;
  struct audio_info_s info;
  snd_pcm_format_t format;
  snd_pcm_access_t access;
  unsigned int channels;
  unsigned int periods;
  unsigned int rate;
  bool first_instance;
  int ret;

  ret = dmix_down_sem(dmix);
  if (ret < 0)
    {
      SNDERR("down sem fail, ret:%d, errno:%d", ret, errno);
      return ret;
    }

  snd_pcm_hw_params_get_format(params, &format);
  snd_pcm_hw_params_get_channels(params, &channels);
  snd_pcm_hw_params_get_rate(params, &rate, NULL);
  snd_pcm_hw_params_get_periods(params, &periods, NULL);
  snd_pcm_hw_params_get_period_size(params, &period_size, NULL);
  snd_pcm_hw_params_get_access(params, &access);

  pcm->rate = rate;
  pcm->channels = channels;
  pcm->format = format;

  ret = snd_pcm_dmix_negotiate(pcm, &info);
  if (ret < 0)
    {
      goto err;
    }

  SNDINFO("negotiate format:%" PRIu8 ", channels:%" PRIu8 ", rate:%" PRIu32,
          info.subformat, info.channels, info.samplerate);

  snd_pcm_hw_params_alloca(&sparams);

  snd_pcm_hw_params_set_format(
      dmix->spcm, sparams,
      snd_pcm_build_linear_format(info.subformat, info.subformat, 0, 0));
  snd_pcm_hw_params_set_channels(dmix->spcm, sparams, info.channels);
  snd_pcm_hw_params_set_rate(dmix->spcm, sparams, info.samplerate, 0);

  ret = snd_pcm_hw_params_any(dmix->spcm, sparams);
  if (ret < 0)
    {
      SNDERR("Invalid snd_pcm_hw_params_any return: %d", ret);
      goto err;
    }

  snd_pcm_hw_params_get_rate(sparams, &rate, 0);

  snd_pcm_hw_params_set_periods(dmix->spcm, sparams, periods, 0);
  snd_pcm_hw_params_set_period_size(dmix->spcm, sparams,
                                    period_size * rate / pcm->rate, 0);
  snd_pcm_hw_params_set_access(dmix->spcm, sparams, access);

  first_instance = snd_pcm_state(dmix->spcm) == AUDIO_STATE_OPEN;

  ret = snd_pcm_hw_params(dmix->spcm, sparams);
  if (ret < 0)
    {
      goto err;
    }

  dmix->appl_ptr = &dmix->spcm->appl;

  pcm->periods = dmix->spcm->periods;
  pcm->period_size = ceilf((float)dmix->spcm->period_size * pcm->rate /
                           dmix->spcm->rate);
  pcm->period_time = pcm->period_size * 1000 * 1000 / pcm->rate;
  pcm->start_threshold =
       first_instance ? pcm->period_size * pcm->periods : pcm->period_size;

  snd_pcm_hw_params_set_periods(pcm, params, pcm->periods, 0);
  snd_pcm_hw_params_set_period_size(pcm, params, pcm->period_size, 0);
  snd_pcm_hw_params_set_period_time(pcm, params, pcm->period_time, 0);

  dmix->state = SND_PCM_STATE_SETUP;
  dmix_up_sem(dmix);

  return 0;

err:
  dmix_up_sem(dmix);
  return ret;
}

static int snd_pcm_dmix_prepare(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  int buf_len;
  int ret;

  SNDDEBUG("enter, dmix->state:%d", dmix->state);

  if (dmix->state == SND_PCM_STATE_XRUN)
    {
      snd_pcm_reset(pcm);
      return 0;
    }

  if (dmix->state != SND_PCM_STATE_SETUP)
    {
      SNDERR("pcm not setup");
      return -EPERM;
    }

  ret = dmix_down_sem(dmix);
  if (ret < 0)
    {
      SNDERR("down sem fail, ret:%d, errno:%d", ret, errno);
      return ret;
    }

  dmix->last_buffer.data = malloc(snd_pcm_frames_to_bytes(pcm,
                                  pcm->period_size));
  if (!dmix->last_buffer.data)
    {
      SNDERR("malloc last_buffer.data fail");
      ret = -ENOMEM;
      goto err;
    }

  dmix->last_buffer.nmaxframes = pcm->period_size;
  dmix->last_buffer.nframes = 0;

  /* Pcm->format -> s16 */

  if (pcm->format != RESAMPLE_IO_FORMART ||
      pcm->access == SND_PCM_ACCESS_RW_NONINTERLEAVED)
    {
      buf_len = pcm->period_size * pcm->channels * 16 / 2;
      ret = bitsconv_init(&dmix->bc_s16, pcm->channels, pcm->format,
                          RESAMPLE_IO_FORMART, buf_len);
      if (ret < 0)
        {
          SNDERR("Failed to initialize bitconverter: %d\n", ret);
          goto err;
        }
    }

  /* Pcm->channels -> dmix->channels */

  if (dmix->spcm->channels != pcm->channels)
    {
      buf_len = pcm->period_size * dmix->spcm->channels * 16 / 2;
      ret = chmap_init(&dmix->cm, RESAMPLE_IO_FORMART, pcm->channels,
                       dmix->spcm->channels, buf_len);
      if (ret < 0)
        {
          SNDERR("Failed to initialize channel map: %d\n", ret);
          goto err;
        }
    }

  /* Pcm->rate -> dmix->rate */

  if (dmix->spcm->rate != pcm->rate)
    {
      dmix->resampler = speex_resampler_init(
            dmix->spcm->channels, pcm->rate, dmix->spcm->rate, 0, &ret);
      if (ret != RESAMPLER_ERR_SUCCESS)
        {
          SNDERR("Failed to initialize resampler: %s\n",
                  speex_resampler_strerror(ret));
          ret = -ret;
          goto err;
        }

      dmix->resmp_out = malloc(snd_pcm_frames_to_bytes(dmix->spcm,
                               dmix->spcm->period_size));
      if (!dmix->resmp_out)
        {
          SNDERR("Failed to mallo resmp_out\n");
          ret = -ENOMEM;
          goto err;
        }
    }

  /* S16 -> dmix->format */

  if (dmix->spcm->format != RESAMPLE_IO_FORMART)
    {
      ret = bitsconv_init(&dmix->bc, dmix->spcm->channels,
                          RESAMPLE_IO_FORMART, dmix->spcm->format,
                          snd_pcm_frames_to_bytes(dmix->spcm,
                                                  dmix->spcm->period_size));
      if (ret < 0)
        {
          SNDERR("Failed to initialize bitconverter: %d\n", ret);
          goto err;
        }
    }

  default_mixer_init(dmix);
  dmix->state = SND_PCM_STATE_PREPARED;
  dmix_up_sem(dmix);
  return 0;

err:
  if (dmix->bc_s16)
    {
      bitsconv_release(dmix->bc_s16);
      dmix->bc_s16 = NULL;
    }

  if (dmix->bc)
    {
      bitsconv_release(dmix->bc);
      dmix->bc = NULL;
    }

  if (dmix->cm)
    {
      chmap_release(dmix->cm);
      dmix->cm = NULL;
    }

  if (dmix->resampler)
    {
      speex_resampler_destroy(dmix->resampler);
      dmix->resampler = NULL;
    }

  if (dmix->resmp_out)
    {
      free(dmix->resmp_out);
    }

  if (dmix->last_buffer.data)
    {
      free(dmix->last_buffer.data);
    }

  dmix_up_sem(dmix);
  SNDERR("fail ret:%d", ret);
  return ret;
}

static int snd_pcm_dmix_reset(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  int ret;

  ret = dmix->spcm->ops->prepare(dmix->spcm->ops_arg);
  if (ret < 0)
    {
      SNDERR("ioctl failed: %d, errno:%d", ret, -errno);
      return ret;
    }

  if (dmix->state == SND_PCM_STATE_XRUN)
    {
      dmix->state = SND_PCM_STATE_PREPARED;
    }

  return 0;
}

static int snd_pcm_dmix_start(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  int ret;

  if (dmix->state != SND_PCM_STATE_PREPARED)
    {
      return 0;
    }

  ret = dmix->spcm->ops->start(dmix->spcm->ops_arg);
  if (ret < 0)
    {
      SNDERR("start error:%d\n", ret);
      return ret;
    }

  dmix->state = SND_PCM_STATE_RUNNING;
  dmix->running = true;

  return ret;
}

static void snd_pcm_dmix_dump(FAR snd_pcm_t *pcm, snd_output_t *out)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;

  SNDINFO("format       : %s\n",  snd_pcm_format_name(dmix->spcm->format));
  SNDINFO("channels     : %u\n",  dmix->spcm->channels);
  SNDINFO("rate         : %u\n",  dmix->spcm->rate);
  SNDINFO("period       : %u\n",  dmix->spcm->periods);
  SNDINFO("period_time  : %u\n",  dmix->spcm->period_time);
  SNDINFO("period_frames: %ld\n", dmix->spcm->period_size);
  SNDINFO("period_bytes : %zd\n",
          snd_pcm_frames_to_bytes(dmix->spcm, dmix->spcm->period_size));
  SNDINFO("buffer_frames: %lu\n",
          dmix->spcm->period_size * dmix->spcm->periods);
  SNDINFO("buffer_bytes : %zd\n",
          snd_pcm_frames_to_bytes(dmix->spcm, dmix->spcm->period_size) *
          dmix->spcm->periods);
}

static int snd_pcm_dmix_drop(FAR snd_pcm_t *pcm)
{
  return 0;
}

static int snd_pcm_dmix_drain_lastbuffer(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  snd_pcm_sframes_t transfer;
  void *data[pcm->channels];
  ssize_t offset;
  ssize_t step;
  ssize_t len;

  transfer = dmix->last_buffer.nmaxframes - dmix->last_buffer.nframes;

  if (pcm->access == SND_PCM_ACCESS_RW_INTERLEAVED)
    {
      offset = snd_pcm_frames_to_bytes(pcm, dmix->last_buffer.nframes);
      len = snd_pcm_frames_to_bytes(pcm, transfer);

      memset(dmix->last_buffer.data + offset, 0, len);
      return snd_pcm_writei(pcm, dmix->last_buffer.data + offset, transfer);
    }
  else if (pcm->access == SND_PCM_ACCESS_RW_NONINTERLEAVED)
    {
      offset = snd_pcm_samples_to_bytes(pcm, dmix->last_buffer.nframes);
      step = snd_pcm_samples_to_bytes(pcm, dmix->last_buffer.nmaxframes);
      len = snd_pcm_samples_to_bytes(pcm, transfer);

      for (int i = 0; i < pcm->channels; i++)
        {
          data[i] = dmix->last_buffer.data + step * i + offset;
          memset(data[i], 0, len);
        }

      return snd_pcm_writen(pcm, data, transfer);
    }

  return -EINVAL;
}

static int snd_pcm_dmix_drain(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;

  SNDINFO("device:%s", pcm->name);

  switch (dmix->state)
    {
    case SND_PCM_STATE_PREPARED:
      if (!dmix->running)
        {
          while (snd_pcm_dmix_written(pcm) < pcm->start_threshold)
            {
              snd_pcm_dmix_drain_lastbuffer(pcm);
            }
        }
      break;
    case SND_PCM_STATE_PAUSED:
    case SND_PCM_STATE_XRUN:
      dmix->state = SND_PCM_STATE_SETUP;
      return 0;
    case SND_PCM_STATE_SETUP:
      return 0;
    case SND_PCM_STATE_OPEN:
      return -EBADFD;
    case SND_PCM_STATE_RUNNING:
        if (dmix->last_buffer.nframes > 0)
          {
            snd_pcm_dmix_drain_lastbuffer(pcm);
          }
      break;
    default:
      break;
    }

  dmix->state = SND_PCM_STATE_DRAINING;

  do
    {
      snd_pcm_dmix_playback_avail(dmix);
      if (pcm->mode & SND_PCM_NONBLOCK)
        {
          return dmix->state == SND_PCM_STATE_SETUP ? 0 : -EAGAIN;
        }

      snd_pcm_wait(pcm, -1);
    }
  while (dmix->state == SND_PCM_STATE_DRAINING);

  return 0;
}

static int snd_pcm_dmix_close(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  int ret;

  SNDINFO("device:%s", pcm->name);

  pcm->mode &= ~SND_PCM_NONBLOCK;
  snd_pcm_drain(pcm);

  ret = dmix_down_sem(dmix);
  if (ret < 0)
    {
      SNDERR("down sem fail, ret:%d, errno:%d", ret, errno);
      return ret;
    }

  if (dmix->running)
    {
      snd_pcm_close(dmix->spcm);
      dmix->running = false;
    }

  if (dmix->last_buffer.data)
    {
      free(dmix->last_buffer.data);
    }

  if (dmix->bc_s16)
    {
      bitsconv_release(dmix->bc_s16);
    }

  if (dmix->bc)
    {
      bitsconv_release(dmix->bc);
    }

  if (dmix->cm)
    {
      chmap_release(dmix->cm);
    }

  if (dmix->resampler)
    {
      speex_resampler_destroy(dmix->resampler);
    }

  if (dmix->resmp_out)
    {
      free(dmix->resmp_out);
    }

  dmix_up_sem(dmix);
  snd_pcm_dmix_sem_close(dmix);
  pcm->private_data = NULL;
  free(dmix);
  return 0;
}

static int snd_pcm_dmix_pause(FAR snd_pcm_t *pcm, int enable)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  int ret;

  SNDINFO("enable:%d device:%s", enable, pcm->name);

  ret = dmix->spcm->ops->pause(dmix->spcm->ops_arg, enable);
  if (ret < 0)
    {
      SNDERR("pause:%d, ret:%d", enable, ret);
      return -errno;
    }

  if (enable)
    {
      dmix->state = SND_PCM_STATE_PAUSED;
    }
  else
    {
      snd_pcm_prepare(dmix->spcm);
      dmix->state = SND_PCM_STATE_RUNNING;
    }

  return ret;
}

int snd_pcm_dmix_resume(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;

  if (dmix->state == SND_PCM_STATE_XRUN)
    {
      snd_pcm_reset(pcm);
    }

  return 0;
}

static snd_pcm_sframes_t snd_pcm_dmix_avail_update(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  snd_pcm_sframes_t avail;

  if (dmix->state == SND_PCM_STATE_XRUN)
    {
      return -EPIPE;
    }

  avail = snd_pcm_dmix_playback_avail(dmix);

  if (avail < 0 && (!dmix->running || dmix->state == SND_PCM_STATE_PAUSED))
    {
      snd_pcm_reset(pcm);
      avail = snd_pcm_dmix_playback_avail(dmix);
    }

  return avail;
}

static int snd_pcm_dmix_mmap(FAR snd_pcm_t *pcm)
{
  return 0;
}

static int snd_pcm_dmix_munmap(FAR snd_pcm_t *pcm)
{
  return 0;
}

static int snd_pcm_dmix_mmap_begin(FAR snd_pcm_t *pcm,
                                   FAR const snd_pcm_channel_area_t **areas,
                                   FAR snd_pcm_uframes_t *offset,
                                   FAR snd_pcm_uframes_t *frames)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;

  snd_pcm_dmix_avail_update(pcm);
  return dmix->spcm->ops->mmap_begin(dmix->spcm->ops_arg, areas, offset,
                                     frames);
}

static snd_pcm_sframes_t snd_pcm_dmix_mmap_commit(FAR snd_pcm_t *pcm,
                                                  snd_pcm_uframes_t offset,
                                                  snd_pcm_uframes_t size)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;

  return dmix->spcm->ops->mmap_commit(dmix->spcm->ops_arg, offset, size);
}

static snd_pcm_sframes_t snd_pcm_dmix_write_period(FAR snd_pcm_t *pcm,
                                                   FAR void **bufs,
                                                   FAR void *out_addr)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  snd_pcm_uframes_t xfer = 0;
  snd_pcm_sframes_t ret = 0;
  snd_pcm_uframes_t in_frames;
  snd_pcm_uframes_t out_frames;
  snd_pcm_uframes_t out_size;
  snd_pcm_uframes_t written;
  FAR void *in_data = bufs[0];
  FAR const void *in_datas[1];
  FAR void *out_data;

  in_frames = pcm->period_size;
  out_frames = dmix->spcm->period_size;

  if (dmix->bc_s16)
    {
      ret = bitsconv_process(
            dmix->bc_s16, (FAR const void **)bufs, in_frames, &out_data,
            &out_size, pcm->access == SND_PCM_ACCESS_RW_NONINTERLEAVED);
      if (ret < 0)
        {
          SNDERR("bitsconv err, ret:%ld", ret);
          return ret;
        }

      in_data = out_data;
    }

  if (dmix->cm)
    {
      ret =
          chmap_process(dmix->cm, in_data, in_frames, &out_data, &out_size);
      if (ret < 0)
        {
          SNDERR("chmap err, ret:%ld", ret);
          return ret;
        }

      in_data = out_data;
    }

  if (dmix->resampler)
    {
      if (in_frames > UINT32_MAX || out_frames > UINT32_MAX)
        {
          SNDERR("frame count too large for resampler");
          return -EINVAL;
        }

      spx_uint32_t in_len = (spx_uint32_t)in_frames;
      spx_uint32_t out_len = (spx_uint32_t)out_frames;

      ret = speex_resampler_process_interleaved_int(
            dmix->resampler, in_data, &in_len,
            (FAR int16_t *)dmix->resmp_out, &out_len);
      if (ret != RESAMPLER_ERR_SUCCESS)
        {
          SNDERR("resample err, ret:%ld", ret);
          return -EIO;
        }

      in_frames = in_len;
      out_frames = out_len;
      in_data = dmix->resmp_out;
    }

  if (dmix->bc)
    {
      in_datas[0] = in_data;
      ret = bitsconv_process(dmix->bc, in_datas, out_frames,
                             &out_data, &out_size, false);
      if (ret < 0)
        {
          SNDERR("bitsconv err, ret:%ld", ret);
          return ret;
        }

      in_data = out_data;
    }

  if (pcm->volume != 1.0f)
    {
      snd_pcm_softvol_scale(dmix->spcm->format, pcm->volume, in_data,
                            out_frames * dmix->spcm->channels);
    }

  ret = dmix_down_sem(dmix);
  if (ret < 0)
    {
      SNDERR("down sem fail, ret:%ld, errno:%d", ret, errno);
      return ret;
    }

  out_data = out_addr;

  dmix->mixer.mix_engine(out_data, in_data,
                         out_frames * dmix->spcm->channels);
  dmix_up_sem(dmix);
  ret = snd_pcm_mmap_commit(pcm, 0, dmix->spcm->period_size);
  if (ret < 0)
    {
      SNDERR("forward app ptr fail, ret:%ld", ret);
      return ret;
    }

  xfer = in_frames;
  if (dmix->state == SND_PCM_STATE_PREPARED)
    {
      written = snd_pcm_dmix_written(pcm);
      if (dmix->running)
        {
          dmix->state = SND_PCM_STATE_RUNNING;
        }
      else if (written >= pcm->start_threshold)
        {
          ret = snd_pcm_start(pcm);
        }
    }

  SNDDEBUG("leave, xfer:%ld, ret:%ld", xfer, ret);
  return xfer > 0 ? (snd_pcm_sframes_t)xfer : ret;
}

static snd_pcm_sframes_t snd_pcm_dmix_writei(FAR snd_pcm_t *pcm,
                                             FAR const void *buffer,
                                             snd_pcm_uframes_t size)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  FAR uint8_t *in_data = (FAR uint8_t *)buffer;
  FAR const snd_pcm_channel_area_t *areas;
  snd_pcm_state_t state = dmix->state;
  snd_pcm_uframes_t xfer = 0;
  snd_pcm_sframes_t ret = 0;
  snd_pcm_sframes_t transfer;
  snd_pcm_uframes_t offset;
  snd_pcm_uframes_t frames;
  ssize_t offset_src;
  ssize_t offset_dst;
  FAR void *bufs[1];

  SNDDEBUG("enter, buffer:%p, size:%ld", buffer, size);

  if (size == 0)
    {
      return 0;
    }

  switch (state)
    {
    case SND_PCM_STATE_PREPARED:
    case SND_PCM_STATE_RUNNING:
    case SND_PCM_STATE_PAUSED:
      break;
    case SND_PCM_STATE_XRUN:
      return -EPIPE;
    case SND_PCM_STATE_SUSPENDED:
      return -ESTRPIPE;
    case SND_PCM_STATE_DISCONNECTED:
      return -ENODEV;
    default:
      return -EBADFD;
    }

  while (size > 0)
    {
      frames = size;
      ret = snd_pcm_mmap_begin(pcm, &areas, &offset, &frames);
      if (ret < 0)
        {
          SNDERR("mmap begin fail, ret:%ld", ret);
          return ret;
        }

      if (frames == 0)
        {
          if (pcm->mode & SND_PCM_NONBLOCK)
            {
              ret = -EAGAIN;
              break;
            }

          ret = snd_pcm_wait(pcm, -1);
          if (ret < 0)
            {
              break;
            }

          continue;
        }

      if (size < pcm->period_size ||
          (dmix->last_buffer.nframes > 0 &&
           dmix->last_buffer.nframes < dmix->last_buffer.nmaxframes))
        {
          transfer = MIN(size, dmix->last_buffer.nmaxframes -
                               dmix->last_buffer.nframes);
          offset_src = snd_pcm_frames_to_bytes(pcm, xfer);
          if (offset_src < 0)
            {
              ret = offset_src;
              break;
            }

          offset_dst =
              snd_pcm_frames_to_bytes(pcm, dmix->last_buffer.nframes);
          if (offset_dst < 0)
            {
              ret = offset_dst;
              break;
            }

          memcpy(dmix->last_buffer.data + offset_dst, in_data + offset_src,
                 transfer * pcm->frame_bits / 8);
          dmix->last_buffer.nframes += transfer;
          size -= transfer;
          xfer += transfer;

          if (dmix->last_buffer.nframes < dmix->last_buffer.nmaxframes)
            {
              break;
            }

          bufs[0] = dmix->last_buffer.data;
          transfer = snd_pcm_dmix_write_period(pcm, bufs, areas[0].addr);
          if (transfer < 0)
            {
              ret = transfer;
              break;
            }

          dmix->last_buffer.nframes -= transfer;
          continue;
        }

      offset_src = snd_pcm_frames_to_bytes(pcm, xfer);
      if (offset_src < 0)
        {
          ret = offset_src;
          break;
        }

      bufs[0] = in_data + offset_src;
      transfer = snd_pcm_dmix_write_period(pcm, bufs, areas[0].addr);
      if (transfer < 0)
        {
          ret = transfer;
          break;
        }

      size -= transfer;
      xfer += transfer;
    }

  SNDDEBUG("leave, xfer:%ld, ret:%ld", xfer, ret);
  return xfer > 0 ? (snd_pcm_sframes_t)xfer : ret;
}

static snd_pcm_sframes_t snd_pcm_dmix_writen(FAR snd_pcm_t *pcm,
                                             FAR void **bufs,
                                             snd_pcm_uframes_t size)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  FAR const snd_pcm_channel_area_t *areas;
  snd_pcm_state_t state = dmix->state;
  FAR void *in_datas[pcm->channels];
  snd_pcm_uframes_t xfer = 0;
  snd_pcm_sframes_t ret = 0;
  snd_pcm_sframes_t transfer;
  snd_pcm_uframes_t offset;
  snd_pcm_uframes_t frames;
  FAR uint8_t *in_data;
  ssize_t offset_src;
  ssize_t offset_dst;
  ssize_t step;
  ssize_t len;
  int i;

  SNDDEBUG("enter, bufs:%p, size:%ld", bufs, size);

  if (size == 0)
    {
      return 0;
    }

  switch (state)
    {
    case SND_PCM_STATE_PREPARED:
    case SND_PCM_STATE_RUNNING:
    case SND_PCM_STATE_PAUSED:
      break;
    case SND_PCM_STATE_XRUN:
      return -EPIPE;
    case SND_PCM_STATE_SUSPENDED:
      return -ESTRPIPE;
    case SND_PCM_STATE_DISCONNECTED:
      return -ENODEV;
    default:
      return -EBADFD;
    }

  while (size > 0)
    {
      frames = size;
      ret = snd_pcm_mmap_begin(pcm, &areas, &offset, &frames);
      if (ret < 0)
        {
          SNDERR("mmap begin fail, ret:%ld", ret);
          return ret;
        }

      if (frames == 0)
        {
          if (pcm->mode & SND_PCM_NONBLOCK)
            {
              ret = -EAGAIN;
              break;
            }

          ret = snd_pcm_wait(pcm, -1);
          if (ret < 0)
            {
              break;
            }

          continue;
        }

      if (size < pcm->period_size ||
          (dmix->last_buffer.nframes > 0 &&
           dmix->last_buffer.nframes < dmix->last_buffer.nmaxframes))
        {
          transfer = MIN(size, dmix->last_buffer.nmaxframes -
                               dmix->last_buffer.nframes);

          offset_src = snd_pcm_samples_to_bytes(pcm, xfer);
          if (offset_src < 0)
            {
              ret = offset_src;
              break;
            }

          step = snd_pcm_samples_to_bytes(pcm, pcm->period_size);
          if (step < 0)
            {
              ret = step;
              break;
            }

          len = snd_pcm_samples_to_bytes(pcm, transfer);
          if (len < 0)
            {
              ret = len;
              break;
            }

          for (i = 0; i < pcm->channels; i++)
            {
              in_data = bufs[i];
              offset_dst =
                  snd_pcm_samples_to_bytes(pcm, dmix->last_buffer.nframes) +
                  step * i;
              memcpy(dmix->last_buffer.data + offset_dst,
                     in_data + offset_src, len);
              in_datas[i] = dmix->last_buffer.data + step * i;
            }

          dmix->last_buffer.nframes += transfer;
          size -= transfer;
          xfer += transfer;

          if (dmix->last_buffer.nframes < dmix->last_buffer.nmaxframes)
            {
              break;
            }

          transfer = snd_pcm_dmix_write_period(pcm, in_datas, areas[0].addr);
          if (transfer < 0)
            {
              ret = transfer;
              break;
            }

          dmix->last_buffer.nframes -= transfer;
          continue;
        }

      offset_src = snd_pcm_samples_to_bytes(pcm, xfer);
      if (offset_src < 0)
        {
          ret = offset_src;
          break;
        }

      for (i = 0; i < pcm->channels; i++)
      {
        in_data = bufs[i];
        in_datas[i] = in_data + offset_src;
      }

      transfer = snd_pcm_dmix_write_period(pcm, in_datas, areas[0].addr);
      if (transfer < 0)
        {
          ret = transfer;
          break;
        }

      size -= transfer;
      xfer += transfer;
    }

  SNDDEBUG("leave, xfer:%ld, ret:%ld", xfer, ret);
  return xfer > 0 ? (snd_pcm_sframes_t)xfer : ret;
}

static snd_pcm_sframes_t snd_pcm_dmix_readi(FAR snd_pcm_t *pcm,
                                          FAR void *buffer,
                                          snd_pcm_uframes_t size)
{
  return -ENODEV;
}

static int snd_pcm_dmix_poll_descriptors_count(FAR snd_pcm_t *pcm)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;

  return dmix->spcm->ops->poll_descriptors_count(dmix->spcm->ops_arg);
}

static int snd_pcm_dmix_poll_descriptors(FAR snd_pcm_t *pcm,
                                         FAR struct pollfd *pfds,
                                         unsigned int space)
{
  FAR snd_pcm_dmix_t *dmix = pcm->private_data;
  int ret;

  ret = dmix->spcm->ops->poll_descriptors(dmix->spcm->ops_arg, pfds, space);
  if (dmix->state == SND_PCM_STATE_DRAINING)
    {
      pfds->events = POLLERR;
    }

  return ret;
}

static const snd_pcm_ops_t snd_pcm_dmix_ops =
{
    .state = snd_pcm_dmix_state,
    .delay = snd_pcm_dmix_delay,
    .hw_refine = snd_pcm_dmix_hw_refine,
    .hw_params = snd_pcm_dmix_hw_params,
    .prepare = snd_pcm_dmix_prepare,
    .reset = snd_pcm_dmix_reset,
    .start = snd_pcm_dmix_start,
    .dump = snd_pcm_dmix_dump,
    .drop = snd_pcm_dmix_drop,
    .drain = snd_pcm_dmix_drain,
    .close = snd_pcm_dmix_close,
    .pause = snd_pcm_dmix_pause,
    .resume = snd_pcm_dmix_resume,
    .avail_update = snd_pcm_dmix_avail_update,
    .mmap = snd_pcm_dmix_mmap,
    .munmap = snd_pcm_dmix_munmap,
    .mmap_begin = snd_pcm_dmix_mmap_begin,
    .mmap_commit = snd_pcm_dmix_mmap_commit,
    .writei = snd_pcm_dmix_writei,
    .writen = snd_pcm_dmix_writen,
    .readi = snd_pcm_dmix_readi,
    .poll_descriptors_count = snd_pcm_dmix_poll_descriptors_count,
    .poll_descriptors = snd_pcm_dmix_poll_descriptors,
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int snd_pcm_dmix_open(FAR snd_pcm_t **pcmp, FAR const char *name,
                      snd_pcm_stream_t stream, int mode)
{
  FAR snd_pcm_t *pcm = NULL;
  FAR snd_pcm_dmix_t *dmix = NULL;
  FAR snd_pcm_t *spcm = NULL;
  int ret;

  if (!pcmp || !name)
    {
      return -EINVAL;
    }

  if (stream != SND_PCM_STREAM_PLAYBACK)
    {
      SNDERR("The dmix plugin supports only playback stream");
      return -EINVAL;
    }

  dmix = calloc(1, sizeof(snd_pcm_dmix_t));
  if (!dmix)
    {
      return -ENOMEM;
    }

  ret = snd_pcm_new(&pcm, SND_PCM_TYPE_DMIX, name, stream, mode);
  if (ret < 0)
    {
      goto open_err;
    }

  pcm->ops = &snd_pcm_dmix_ops;
  pcm->private_data = dmix;
  dmix->state = SND_PCM_STATE_OPEN;

  ret = snd_pcm_dmix_sem_open(pcm);
  if (ret < 0)
    {
      SNDERR("unable to create IPC semaphore");
      goto open_err;
    }

  ret = snd_pcm_hw_open(&spcm, name, stream, mode);
  if (ret < 0)
    {
      goto open_err;
    }

  dmix->spcm = spcm;
  *pcmp = pcm;
  return 0;

open_err:
  if (dmix->semid)
    {
      snd_pcm_dmix_sem_close(dmix);
    }

  free(dmix);

  if (pcm)
    {
      snd_pcm_free(pcm);
    }

  SNDERR("fail ret:%d", ret);
  return ret;
}
