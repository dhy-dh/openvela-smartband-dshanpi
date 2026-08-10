/****************************************************************************
 * apps/audioutils/alsa-lib/pcm_params.c
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

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/param.h>

#include "pcm_local.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static inline int snd_pcm_hw_is_mask(unsigned int var)
{
  return var >= SNDRV_PCM_HW_PARAM_FIRST_MASK &&
         var <= SNDRV_PCM_HW_PARAM_LAST_MASK;
}

static inline int snd_pcm_hw_is_range(unsigned int var)
{
  return var >= SNDRV_PCM_HW_PARAM_FIRST_INTERVAL &&
         var <= SNDRV_PCM_HW_PARAM_LAST_INTERVAL;
}

static inline bool snd_pcm_hw_mask_is_single(unsigned int mask)
{
  return mask && ffs(mask) == fls(mask);
}

static inline int snd_pcm_hw_mask_min(unsigned int mask)
{
  return MAX(ffs(mask) - 1, 0);
}

static inline int snd_pcm_hw_mask_max(unsigned int mask)
{
  return MAX(fls(mask) - 1, 0);
}

static inline void snd_pcm_hw_mask_reset(FAR snd_mask_t *mask,
                                         unsigned int from, unsigned int to)
{
  mask->mask &= ~(((1u << (to - from + 1)) - 1) << from);
}

static int snd_pcm_hw_params_set(FAR snd_pcm_hw_params_t *params,
                                 snd_pcm_hw_param_t var, unsigned int val)
{
  FAR snd_interval_t *interval;
  FAR snd_mask_t *mask;

  assert(params);
  if (snd_pcm_hw_is_mask(var))
    {
      mask = &params->masks[var - SNDRV_PCM_HW_PARAM_FIRST_MASK];
      mask->mask = (1u << val);
    }
  else if (snd_pcm_hw_is_range(var))
    {
      interval = &params->intervals[var - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
      interval->min = val;
      interval->max = val;
    }
  else
    {
      assert(0);
      return -EINVAL;
    }

  return 0;
}

static int snd_pcm_hw_params_get(FAR const snd_pcm_hw_params_t *params,
                                 snd_pcm_hw_param_t var,
                                 FAR unsigned int *val)
{
  FAR const snd_interval_t *interval;
  FAR const snd_mask_t *mask;

  assert(params);
  if (snd_pcm_hw_is_mask(var))
    {
      mask = &params->masks[var - SNDRV_PCM_HW_PARAM_FIRST_MASK];
      if (!snd_pcm_hw_mask_is_single(mask->mask))
        {
          return -EINVAL;
        }

      *val = snd_pcm_hw_mask_min(mask->mask);
    }
  else
    {
      interval = &params->intervals[var - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
      if (interval->min != interval->max)
        {
          return -EINVAL;
        }

      *val = interval->min;
    }

  return 0;
}

static int snd_pcm_hw_params_get_min(FAR const snd_pcm_hw_params_t *params,
                                     snd_pcm_hw_param_t var,
                                     FAR unsigned int *val)
{
  FAR const snd_interval_t *interval;
  FAR const snd_mask_t *mask;

  assert(params);
  if (snd_pcm_hw_is_mask(var))
    {
      mask = &params->masks[var - SNDRV_PCM_HW_PARAM_FIRST_MASK];
      *val = snd_pcm_hw_mask_min(mask->mask);
    }
  else
    {
      interval = &params->intervals[var - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
      *val = interval->min;
    }

  return 0;
}

static int snd_pcm_hw_params_set_min(FAR snd_pcm_hw_params_t *params,
                                     snd_pcm_hw_param_t var,
                                     unsigned int val)
{
  FAR snd_interval_t *interval;
  FAR snd_mask_t *mask;

  assert(params);
  if (snd_pcm_hw_is_mask(var))
    {
      mask = &params->masks[var - SNDRV_PCM_HW_PARAM_FIRST_MASK];
      snd_pcm_hw_mask_reset(mask, 0, val);
      mask->mask |= (1u << val);
    }
  else
    {
      interval = &params->intervals[var - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
      interval->min = val;
    }

  return 0;
}

static int snd_pcm_hw_params_get_max(FAR const snd_pcm_hw_params_t *params,
                                     snd_pcm_hw_param_t var,
                                     FAR unsigned int *val)
{
  FAR const snd_interval_t *interval;
  FAR const snd_mask_t *mask;

  assert(params);
  if (snd_pcm_hw_is_mask(var))
    {
      mask = &params->masks[var - SNDRV_PCM_HW_PARAM_FIRST_MASK];
      *val = snd_pcm_hw_mask_max(mask->mask);
    }
  else
    {
      interval = &params->intervals[var - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
      *val = interval->max;
    }

  return 0;
}

static int snd_pcm_hw_params_set_max(FAR snd_pcm_hw_params_t *params,
                                     snd_pcm_hw_param_t var,
                                     unsigned int val)
{
  FAR snd_interval_t *interval;
  FAR snd_mask_t *mask;

  assert(params);
  if (snd_pcm_hw_is_mask(var))
    {
      mask = &params->masks[var - SNDRV_PCM_HW_PARAM_FIRST_MASK];
      snd_pcm_hw_mask_reset(mask, val, sizeof(mask->mask) * 8);
      mask->mask |= (1u << val);
    }
  else
    {
      interval = &params->intervals[var - SNDRV_PCM_HW_PARAM_FIRST_INTERVAL];
      interval->max = val;
    }

  return 0;
}

static int snd_pcm_hw_params_set_mask(FAR snd_pcm_hw_params_t *params,
                                      snd_pcm_hw_param_t var,
                                      snd_pcm_format_mask_t val)
{
  FAR snd_mask_t *mask;

  assert(params);
  if (snd_pcm_hw_is_mask(var))
    {
      mask = &params->masks[var - SNDRV_PCM_HW_PARAM_FIRST_MASK];
      mask->mask = val;
    }
  else
    {
      return -EINVAL;
    }

  return 0;
}

static int snd_pcm_hw_params_get_mask(FAR snd_pcm_hw_params_t *params,
                                      snd_pcm_hw_param_t var,
                                      FAR snd_pcm_format_mask_t *val)
{
  FAR snd_mask_t *mask;

  assert(params);
  if (snd_pcm_hw_is_mask(var))
    {
      mask = &params->masks[var - SNDRV_PCM_HW_PARAM_FIRST_MASK];
      *val = mask->mask;
    }
  else
    {
      return -EINVAL;
    }

  return 0;
}

static int snd_pcm_hw_params_choose(snd_pcm_hw_params_t *params)
{
  unsigned int buffer_time = 0;
  unsigned int buffer_size = 0;
  unsigned int period_time = 0;
  unsigned int period_size = 0;
  unsigned int channels = 0;
  unsigned int periods = 0;
  unsigned int format = 0;
  unsigned int access = 0;
  unsigned int rate = 0;

  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_ACCESS, &access);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_ACCESS, access);
  snd_pcm_hw_params_get_min(params, SNDRV_PCM_HW_PARAM_FORMAT, &format);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_FORMAT, format);
  snd_pcm_hw_params_get_min(params, SNDRV_PCM_HW_PARAM_CHANNELS, &channels);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_CHANNELS, channels);
  snd_pcm_hw_params_get_min(params, SNDRV_PCM_HW_PARAM_RATE, &rate);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_RATE, rate);
  if (!access || !format || !channels || !rate)
    {
      return -EINVAL;
    }

  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME,
                        &period_time);
  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
                        &period_size);
  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_PERIODS, &periods);
  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_BUFFER_TIME,
                        &buffer_time);
  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_BUFFER_SIZE,
                        &buffer_size);
  if (period_time && buffer_time)
    {
      periods = buffer_time / period_time;
      period_size = period_time * rate / 1000000;
      buffer_size = period_size * periods;
    }
  else if (period_size && buffer_size)
    {
      periods = buffer_size / period_size;
      period_time = period_size * 1000000 / rate;
      buffer_time = period_time * periods;
    }
  else if (periods && period_time)
    {
      period_size = period_time * rate / 1000000;
      buffer_size = period_size * periods;
      buffer_time = period_time * periods;
    }
  else if (periods && period_size)
    {
      period_time = period_size * 1000000 / rate;
      buffer_size = period_size * periods;
      buffer_time = period_time * periods;
    }
  else if (periods && buffer_time)
    {
      period_time = buffer_time / periods;
      period_size = period_time * rate / 1000000;
      buffer_size = period_size * periods;
    }
  else if (periods && buffer_size)
    {
      period_size = buffer_size / periods;
      period_time = period_size * 1000000 / rate;
      buffer_time = period_time * periods;
    }
  else
    {
      return -EINVAL;
    }

  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME, period_time);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE, period_size);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIODS, periods);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_BUFFER_TIME, buffer_time);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_BUFFER_SIZE, buffer_size);

  return 0;
}

static int snd_pcm_hw_params_internal(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params)
{
  unsigned int val = 0;
  int ret;

  assert(pcm && params);

  ret = snd_pcm_hw_params_choose(params);
  if (ret < 0)
    {
      return ret;
    }

  ret = pcm->ops->hw_params(pcm->ops_arg, params);
  if (ret < 0)
    {
      return ret;
    }

  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_ACCESS, &val);
  pcm->access = val;
  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_FORMAT, &val);
  pcm->format = val;
  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_CHANNELS, &pcm->channels);
  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_RATE, &pcm->rate);
  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE, &val);
  pcm->period_size = val;
  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME,
                       &pcm->period_time);
  snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_PERIODS, &pcm->periods);
  pcm->sample_bits = snd_pcm_format_physical_width(pcm->format);
  pcm->frame_bits = pcm->sample_bits * pcm->channels;

  SNDINFO("format:%u, ch:%u, rate:%u, periods:%u, period_time:%u",
          pcm->format, pcm->channels, pcm->rate, pcm->periods,
          pcm->period_time);

  ret = snd_pcm_mmap(pcm);
  if (ret < 0)
    {
      return ret;
    }

  return 0;
}

/****************************************************************************
 * Public HW Functions
 ****************************************************************************/

int snd_pcm_format_mask_test(FAR const snd_pcm_format_mask_t *mask,
                             snd_pcm_format_t val)
{
  return *mask & (1u << val);
}

void snd_pcm_format_mask_set(FAR snd_pcm_format_mask_t *mask,
                             FAR snd_pcm_format_t val)
{
  *mask |= (1u << val);
}

int snd_pcm_hw_params_any(FAR snd_pcm_t *pcm,
                          FAR snd_pcm_hw_params_t *params)
{
  return pcm->ops->hw_refine(pcm->ops_arg, params);
}

int snd_pcm_hw_params_get_format(FAR const snd_pcm_hw_params_t *params,
                                 FAR snd_pcm_format_t *val)
{
  unsigned int format;
  int ret;

  ret = snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_FORMAT, &format);
  if (ret >= 0)
    {
      *val = format;
    }

  return ret;
}

void snd_pcm_hw_params_get_format_mask(FAR snd_pcm_hw_params_t *params,
                                       FAR snd_pcm_format_mask_t *mask)
{
  snd_pcm_hw_params_get_mask(params, SNDRV_PCM_HW_PARAM_FORMAT, mask);
}

int snd_pcm_hw_params_set_format(FAR snd_pcm_t *pcm,
                                 FAR snd_pcm_hw_params_t *params,
                                 snd_pcm_format_t format)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_FORMAT, format);
}

int snd_pcm_hw_params_set_format_mask(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params,
                                      FAR snd_pcm_format_mask_t *mask)
{
  return snd_pcm_hw_params_set_mask(params, SNDRV_PCM_HW_PARAM_FORMAT,
                                    *mask);
}

int snd_pcm_hw_params_get_channels(FAR const snd_pcm_hw_params_t *params,
                                   FAR unsigned int *val)
{
  return snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_CHANNELS, val);
}

int snd_pcm_hw_params_get_channels_min(FAR const snd_pcm_hw_params_t *params,
                                       FAR unsigned int *val)
{
  return snd_pcm_hw_params_get_min(params, SNDRV_PCM_HW_PARAM_CHANNELS, val);
}

int snd_pcm_hw_params_get_channels_max(FAR const snd_pcm_hw_params_t *params,
                                       FAR unsigned int *val)
{
  return snd_pcm_hw_params_get_max(params, SNDRV_PCM_HW_PARAM_CHANNELS, val);
}

int snd_pcm_hw_params_set_channels(FAR snd_pcm_t *pcm,
                                   FAR snd_pcm_hw_params_t *params,
                                   unsigned int val)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_CHANNELS, val);
}

int snd_pcm_hw_params_set_channels_min(FAR snd_pcm_t *pcm,
                                       FAR snd_pcm_hw_params_t *params,
                                       FAR unsigned int *val)
{
  return snd_pcm_hw_params_set_min(params, SNDRV_PCM_HW_PARAM_CHANNELS,
                                   *val);
}

int snd_pcm_hw_params_set_channels_max(FAR snd_pcm_t *pcm,
                                       FAR snd_pcm_hw_params_t *params,
                                       FAR unsigned int *val)
{
  return snd_pcm_hw_params_set_max(params, SNDRV_PCM_HW_PARAM_CHANNELS,
                                   *val);
}

int snd_pcm_hw_params_get_rate(FAR const snd_pcm_hw_params_t *params,
                               FAR unsigned int *val, FAR int *dir)
{
  return snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_RATE, val);
}

int snd_pcm_hw_params_get_rate_min(FAR const snd_pcm_hw_params_t *params,
                                   FAR unsigned int *val, FAR int *dir)
{
  return snd_pcm_hw_params_get_min(params, SNDRV_PCM_HW_PARAM_RATE, val);
}

int snd_pcm_hw_params_get_rate_max(FAR const snd_pcm_hw_params_t *params,
                                   FAR unsigned int *val, FAR int *dir)
{
  return snd_pcm_hw_params_get_max(params, SNDRV_PCM_HW_PARAM_RATE, val);
}

int snd_pcm_hw_params_set_rate(FAR snd_pcm_t *pcm,
                               FAR snd_pcm_hw_params_t *params,
                               unsigned int val, int dir)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_RATE, val);
}

int snd_pcm_hw_params_set_rate_near(FAR snd_pcm_t *pcm,
                                    FAR snd_pcm_hw_params_t *params,
                                    FAR unsigned int *val, FAR int *dir)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_RATE, *val);
}

int snd_pcm_hw_params_set_rate_min(FAR snd_pcm_t *pcm,
                                   FAR snd_pcm_hw_params_t *params,
                                   FAR unsigned int *val, FAR int *dir)
{
  return snd_pcm_hw_params_set_min(params, SNDRV_PCM_HW_PARAM_RATE, *val);
}

int snd_pcm_hw_params_set_rate_max(FAR snd_pcm_t *pcm,
                                   FAR snd_pcm_hw_params_t *params,
                                   FAR unsigned int *val, FAR int *dir)
{
  return snd_pcm_hw_params_set_max(params, SNDRV_PCM_HW_PARAM_RATE, *val);
}

int snd_pcm_hw_params_set_period_time(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params,
                                      unsigned int us, int dir)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME, us);
}

int snd_pcm_hw_params_set_period_time_near(FAR snd_pcm_t *pcm,
                                           FAR snd_pcm_hw_params_t *params,
                                           FAR unsigned int *us,
                                           FAR int *dir)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME, *us);
}

int snd_pcm_hw_params_get_period_time(FAR const snd_pcm_hw_params_t *params,
                                      FAR unsigned int *us, FAR int *dir)
{
  return snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME, us);
}

int snd_pcm_hw_params_set_period_size(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params,
                                      snd_pcm_uframes_t val, int dir)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE, val);
}

int snd_pcm_hw_params_set_period_size_near(FAR snd_pcm_t *pcm,
                                           FAR snd_pcm_hw_params_t *params,
                                           FAR snd_pcm_uframes_t *val,
                                           FAR int *dir)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE, *val);
}

int snd_pcm_hw_params_get_period_size(FAR const snd_pcm_hw_params_t *params,
                                      FAR snd_pcm_uframes_t *val,
                                      FAR int *dir)
{
  unsigned int period_size;
  int ret;

  ret = snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
                              &period_size);
  if (ret >= 0)
    {
      *val = period_size;
    }

  return ret;
}

int snd_pcm_hw_params_set_periods(FAR snd_pcm_t *pcm,
                                  FAR snd_pcm_hw_params_t *params,
                                  unsigned int val, int dir)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIODS, val);
}

int snd_pcm_hw_params_set_periods_near(FAR snd_pcm_t *pcm,
                                       FAR snd_pcm_hw_params_t *params,
                                       FAR unsigned int *val, FAR int *dir)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIODS, *val);
}

int snd_pcm_hw_params_get_periods(FAR const snd_pcm_hw_params_t *params,
                                  FAR unsigned int *val, FAR int *dir)
{
  return snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_PERIODS, val);
}

int snd_pcm_hw_params_set_buffer_size(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params,
                                      snd_pcm_uframes_t val)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_BUFFER_SIZE, val);
}

int snd_pcm_hw_params_set_buffer_size_near(FAR snd_pcm_t *pcm,
                                           FAR snd_pcm_hw_params_t *params,
                                           FAR snd_pcm_uframes_t *val)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_BUFFER_SIZE, *val);
}

int snd_pcm_hw_params_get_buffer_size(FAR const snd_pcm_hw_params_t *params,
                                      FAR snd_pcm_uframes_t *val)
{
  unsigned int buffer_size;
  int ret;

  ret = snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_BUFFER_SIZE,
                              &buffer_size);
  if (ret >= 0)
    {
      *val = buffer_size;
    }

  return ret;
}

int snd_pcm_hw_params_set_buffer_time(FAR snd_pcm_t *pcm,
                                      FAR snd_pcm_hw_params_t *params,
                                      unsigned int us, int dir)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_BUFFER_TIME, us);
}

int snd_pcm_hw_params_set_buffer_time_near(FAR snd_pcm_t *pcm,
                                           FAR snd_pcm_hw_params_t *params,
                                           FAR unsigned int *us,
                                           FAR int *dir)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_BUFFER_TIME, *us);
}

int snd_pcm_hw_params_get_buffer_time(FAR const snd_pcm_hw_params_t *params,
                                      FAR unsigned int *us, FAR int *dir)
{
  return snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_BUFFER_TIME, us);
}

int snd_pcm_hw_params_get_access(FAR const snd_pcm_hw_params_t *params,
                                 FAR snd_pcm_access_t *val)
{
  unsigned int access;
  int ret;

  ret = snd_pcm_hw_params_get(params, SNDRV_PCM_HW_PARAM_ACCESS, &access);
  if (ret >= 0)
    {
      *val = (snd_pcm_access_t)access;
    }

  return ret;
}

int snd_pcm_hw_params_set_access(FAR snd_pcm_t *pcm,
                                 FAR snd_pcm_hw_params_t *params,
                                 snd_pcm_access_t access)
{
  return snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_ACCESS, access);
}

int snd_pcm_hw_params_current(FAR snd_pcm_t *pcm,
                              FAR snd_pcm_hw_params_t *params)
{
  assert(pcm && params);

  if (pcm->ops->state(pcm) <= SND_PCM_STATE_OPEN)
    {
      return -EBADFD;
    }

  memset(params, 0, sizeof(*params));
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_ACCESS, pcm->access);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_FORMAT, pcm->format);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_CHANNELS, pcm->channels);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_RATE, pcm->rate);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIOD_SIZE,
                        pcm->period_size);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIOD_TIME,
                        pcm->period_time);
  snd_pcm_hw_params_set(params, SNDRV_PCM_HW_PARAM_PERIODS, pcm->periods);

  return 0;
}

int snd_pcm_hw_params(FAR snd_pcm_t *pcm, FAR snd_pcm_hw_params_t *params)
{
  int ret;
  assert(pcm && params);

  ret = snd_pcm_hw_params_internal(pcm, params);
  if (ret < 0)
    {
      return ret;
    }

  return snd_pcm_prepare(pcm);
}

/****************************************************************************
 * Public SW Functions
 ****************************************************************************/

int snd_pcm_sw_params_current(FAR snd_pcm_t *pcm,
                              FAR snd_pcm_sw_params_t *params)
{
  assert(pcm && params);

  memset(params, 0, sizeof(snd_pcm_sw_params_t));
  params->avail_min = pcm->period_size;
  params->start_threshold = pcm->start_threshold;

  return 0;
}

int snd_pcm_sw_params_set_start_threshold(FAR snd_pcm_t *pcm,
                                          FAR snd_pcm_sw_params_t *params,
                                          snd_pcm_uframes_t val)
{
  assert(pcm && params);

  params->start_threshold = val;
  return 0;
}

int snd_pcm_sw_params_get_start_threshold(FAR snd_pcm_sw_params_t *params,
                                          FAR snd_pcm_uframes_t *val)
{
  assert(params);

  *val = params->start_threshold;
  return 0;
}

int snd_pcm_sw_params_set_stop_threshold(FAR snd_pcm_t *pcm,
                                         FAR snd_pcm_sw_params_t *params,
                                         snd_pcm_uframes_t val)
{
  assert(pcm && params);

  params->stop_threshold = val;
  return 0;
}

int snd_pcm_sw_params_get_stop_threshold(FAR snd_pcm_sw_params_t *params,
                                         FAR snd_pcm_uframes_t *val)
{
  assert(params);

  *val = params->stop_threshold;
  return 0;
}

int snd_pcm_sw_params_set_silence_size(FAR snd_pcm_t *pcm,
                                       FAR snd_pcm_sw_params_t *params,
                                       snd_pcm_uframes_t val)
{
  assert(pcm && params);

  params->silence_size = val;
  return 0;
}

int snd_pcm_sw_params_get_silence_size(FAR snd_pcm_sw_params_t *params,
                                       FAR snd_pcm_uframes_t *val)
{
  assert(params);

  *val = params->silence_size;
  return 0;
}

int snd_pcm_sw_params_set_avail_min(FAR snd_pcm_t *pcm,
                                    FAR snd_pcm_sw_params_t *params,
                                    snd_pcm_uframes_t val)
{
  assert(pcm && params);

  params->avail_min = val;
  return 0;
}

int snd_pcm_sw_params_get_avail_min(FAR const snd_pcm_sw_params_t *params,
                                    FAR snd_pcm_uframes_t *val)
{
  assert(params);

  *val = params->avail_min;
  return 0;
}

int snd_pcm_sw_params_get_boundary(FAR const snd_pcm_sw_params_t *params,
                                   FAR snd_pcm_uframes_t *val)
{
  assert(params);

  *val = params->boundary;
  return 0;
}

int snd_pcm_sw_params(FAR snd_pcm_t *pcm, FAR snd_pcm_sw_params_t *params)
{
  assert(pcm && params);

  pcm->start_threshold = params->start_threshold;
  return 0;
}

FAR const char *snd_pcm_access_name(snd_pcm_access_t access)
{
  switch (access)
    {
    case SND_PCM_ACCESS_MMAP_INTERLEAVED:
      return "MMAP_INTERLEAVED";
    case SND_PCM_ACCESS_MMAP_NONINTERLEAVED:
      return "MMAP_NONINTERLEAVED";
    case SND_PCM_ACCESS_MMAP_COMPLEX:
      return "MMAP_COMPLEX";
    case SND_PCM_ACCESS_RW_INTERLEAVED:
      return "RW_INTERLEAVED";
    case SND_PCM_ACCESS_RW_NONINTERLEAVED:
      return "RW_NONINTERLEAVED";
    default:
      return NULL;
    }
}

FAR const char *snd_pcm_format_name(snd_pcm_format_t format)
{
  switch (format)
    {
    case SND_PCM_FORMAT_S8:
      return "S8";
    case SND_PCM_FORMAT_U8:
      return "U8";
    case SND_PCM_FORMAT_S16_LE:
      return "S16_LE";
    case SND_PCM_FORMAT_S16_BE:
      return "S16_BE";
    case SND_PCM_FORMAT_U16_LE:
      return "U16_LE";
    case SND_PCM_FORMAT_U16_BE:
      return "U16_BE";
    case SND_PCM_FORMAT_S24_LE:
      return "S24_LE";
    case SND_PCM_FORMAT_S24_BE:
      return "S24_BE";
    case SND_PCM_FORMAT_U24_LE:
      return "U24_LE";
    case SND_PCM_FORMAT_U24_BE:
      return "U24_BE";
    case SND_PCM_FORMAT_S32_LE:
      return "S32_LE";
    case SND_PCM_FORMAT_S32_BE:
      return "S32_BE";
    case SND_PCM_FORMAT_U32_LE:
      return "U32_LE";
    case SND_PCM_FORMAT_U32_BE:
      return "U32_BE";
    case SND_PCM_FORMAT_FLOAT_LE:
      return "FLOAT_LE";
    case SND_PCM_FORMAT_FLOAT_BE:
      return "FLOAT_BE";
    default:
      return NULL;
    }
}

snd_pcm_format_t snd_pcm_format_value(FAR const char *name)
{
  if (!name)
    {
      return SND_PCM_FORMAT_UNKNOWN;
    }

  if (strcasecmp(name, "S8") == 0)
    {
      return SND_PCM_FORMAT_S8;
    }
  else if (strcasecmp(name, "U8") == 0)
    {
      return SND_PCM_FORMAT_U8;
    }
  else if (strcasecmp(name, "S16_LE") == 0)
    {
      return SND_PCM_FORMAT_S16_LE;
    }
  else if (strcasecmp(name, "S16_BE") == 0)
    {
      return SND_PCM_FORMAT_S16_BE;
    }
  else if (strcasecmp(name, "U16_LE") == 0)
    {
      return SND_PCM_FORMAT_U16_LE;
    }
  else if (strcasecmp(name, "U16_BE") == 0)
    {
      return SND_PCM_FORMAT_U16_BE;
    }
  else if (strcasecmp(name, "S24_LE") == 0)
    {
      return SND_PCM_FORMAT_S24_LE;
    }
  else if (strcasecmp(name, "S24_BE") == 0)
    {
      return SND_PCM_FORMAT_S24_BE;
    }
  else if (strcasecmp(name, "U24_LE") == 0)
    {
      return SND_PCM_FORMAT_U24_LE;
    }
  else if (strcasecmp(name, "U24_BE") == 0)
    {
      return SND_PCM_FORMAT_U24_BE;
    }
  else if (strcasecmp(name, "S32_LE") == 0)
    {
      return SND_PCM_FORMAT_S32_LE;
    }
  else if (strcasecmp(name, "S32_BE") == 0)
    {
      return SND_PCM_FORMAT_S32_BE;
    }
  else if (strcasecmp(name, "U32_LE") == 0)
    {
      return SND_PCM_FORMAT_U32_LE;
    }
  else if (strcasecmp(name, "U32_BE") == 0)
    {
      return SND_PCM_FORMAT_U32_BE;
    }

  return SND_PCM_FORMAT_UNKNOWN;
}

FAR const char *snd_pcm_format_description(snd_pcm_format_t format)
{
  switch (format)
    {
    case SND_PCM_FORMAT_S8:
      return "Signed 8 bit";
    case SND_PCM_FORMAT_U8:
      return "Unsigned 8 bit";
    case SND_PCM_FORMAT_S16_LE:
      return "Signed 16 bit Little Endian";
    case SND_PCM_FORMAT_S16_BE:
      return "Signed 16 bit Big Endian";
    case SND_PCM_FORMAT_U16_LE:
      return "Unsigned 16 bit Little Endian";
    case SND_PCM_FORMAT_U16_BE:
      return "Unsigned 16 bit Big Endian";
    case SND_PCM_FORMAT_S24_LE:
      return "Signed 24 bit Little Endian";
    case SND_PCM_FORMAT_S24_BE:
      return "Signed 24 bit Big Endian";
    case SND_PCM_FORMAT_U24_LE:
      return "Unsigned 24 bit Little Endian";
    case SND_PCM_FORMAT_U24_BE:
      return "Unsigned 24 bit Big Endian";
    case SND_PCM_FORMAT_S32_LE:
      return "Signed 32 bit Little Endian";
    case SND_PCM_FORMAT_S32_BE:
      return "Signed 32 bit Big Endian";
    case SND_PCM_FORMAT_U32_LE:
      return "Unsigned 32 bit Little Endian";
    case SND_PCM_FORMAT_U32_BE:
      return "Unsigned 32 bit Big Endian";
    default:
      return NULL;
    }
}

int snd_pcm_format_little_endian(snd_pcm_format_t format)
{
  switch (format)
    {
    case SND_PCM_FORMAT_S16_LE:
    case SND_PCM_FORMAT_U16_LE:
    case SND_PCM_FORMAT_S24_LE:
    case SND_PCM_FORMAT_U24_LE:
    case SND_PCM_FORMAT_S32_LE:
    case SND_PCM_FORMAT_U32_LE:
      return 1;
    case SND_PCM_FORMAT_S16_BE:
    case SND_PCM_FORMAT_U16_BE:
    case SND_PCM_FORMAT_S24_BE:
    case SND_PCM_FORMAT_U24_BE:
    case SND_PCM_FORMAT_S32_BE:
    case SND_PCM_FORMAT_U32_BE:
      return 0;
    default:
      return -EINVAL;
    }
}

int snd_pcm_format_big_endian(snd_pcm_format_t format)
{
  int val;

  val = snd_pcm_format_little_endian(format);
  if (val < 0)
    {
      return val;
    }

  return !val;
}

int snd_pcm_format_cpu_endian(snd_pcm_format_t format)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  return snd_pcm_format_little_endian(format);
#else
  return snd_pcm_format_big_endian(format);
#endif
}

int snd_pcm_format_width(snd_pcm_format_t format)
{
  switch (format)
    {
    case SND_PCM_FORMAT_S8:
    case SND_PCM_FORMAT_U8:
      return 8;
    case SND_PCM_FORMAT_S16_LE:
    case SND_PCM_FORMAT_S16_BE:
    case SND_PCM_FORMAT_U16_LE:
    case SND_PCM_FORMAT_U16_BE:
      return 16;
    case SND_PCM_FORMAT_S24_LE:
    case SND_PCM_FORMAT_S24_BE:
    case SND_PCM_FORMAT_U24_LE:
    case SND_PCM_FORMAT_U24_BE:
      return 24;
    case SND_PCM_FORMAT_S32_LE:
    case SND_PCM_FORMAT_S32_BE:
    case SND_PCM_FORMAT_U32_LE:
    case SND_PCM_FORMAT_U32_BE:
    case SND_PCM_FORMAT_FLOAT_LE:
    case SND_PCM_FORMAT_FLOAT_BE:
      return 32;
    default:
      return -EINVAL;
    }
}

int snd_pcm_format_physical_width(snd_pcm_format_t format)
{
  switch (format)
    {
    case SND_PCM_FORMAT_S8:
    case SND_PCM_FORMAT_U8:
      return 8;
    case SND_PCM_FORMAT_S16_LE:
    case SND_PCM_FORMAT_S16_BE:
    case SND_PCM_FORMAT_U16_LE:
    case SND_PCM_FORMAT_U16_BE:
      return 16;
    case SND_PCM_FORMAT_S24_LE:
    case SND_PCM_FORMAT_S24_BE:
    case SND_PCM_FORMAT_U24_LE:
    case SND_PCM_FORMAT_U24_BE:
    case SND_PCM_FORMAT_S32_LE:
    case SND_PCM_FORMAT_S32_BE:
    case SND_PCM_FORMAT_U32_LE:
    case SND_PCM_FORMAT_U32_BE:
    case SND_PCM_FORMAT_FLOAT_LE:
    case SND_PCM_FORMAT_FLOAT_BE:
      return 32;
    default:
      return -EINVAL;
    }
}

snd_pcm_format_t snd_pcm_build_linear_format(int width, int pwidth,
                                             int unsignd,
                                             int big_endian)
{
  if (pwidth == 8 && width == 8)
    {
      return unsignd ? SND_PCM_FORMAT_U8 : SND_PCM_FORMAT_S8;
    }

  if (pwidth == 16 && width == 16)
    {
      return unsignd
        ? (big_endian ? SND_PCM_FORMAT_U16_BE : SND_PCM_FORMAT_U16_LE)
        : (big_endian ? SND_PCM_FORMAT_S16_BE : SND_PCM_FORMAT_S16_LE);
    }

  if (pwidth == 32 && width == 24)
    {
      return unsignd
        ? (big_endian ? SND_PCM_FORMAT_U24_BE : SND_PCM_FORMAT_U24_LE)
        : (big_endian ? SND_PCM_FORMAT_S24_BE : SND_PCM_FORMAT_S24_LE);
    }

  if (pwidth == 32 && width == 32)
    {
      return unsignd
        ? (big_endian ? SND_PCM_FORMAT_U32_BE : SND_PCM_FORMAT_U32_LE)
        : (big_endian ? SND_PCM_FORMAT_S32_BE : SND_PCM_FORMAT_S32_LE);
    }

  return SND_PCM_FORMAT_UNKNOWN;
}

ssize_t snd_pcm_format_size(snd_pcm_format_t format, size_t samples)
{
  switch (format)
    {
    case SND_PCM_FORMAT_S8:
    case SND_PCM_FORMAT_U8:
      return samples;
    case SND_PCM_FORMAT_S16_LE:
    case SND_PCM_FORMAT_S16_BE:
    case SND_PCM_FORMAT_U16_LE:
    case SND_PCM_FORMAT_U16_BE:
      return samples * 2;
    case SND_PCM_FORMAT_S24_LE:
    case SND_PCM_FORMAT_S24_BE:
    case SND_PCM_FORMAT_U24_LE:
    case SND_PCM_FORMAT_U24_BE:
      return samples * 3;
    case SND_PCM_FORMAT_S32_LE:
    case SND_PCM_FORMAT_S32_BE:
    case SND_PCM_FORMAT_U32_LE:
    case SND_PCM_FORMAT_U32_BE:
      return samples * 4;
    default:
      return -EINVAL;
    }
}
