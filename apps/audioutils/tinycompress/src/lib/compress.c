/****************************************************************************
 * apps/audioutils/tinycompress/src/lib/compress.c
 *
 * This file is provided under a dual BSD/LGPLv2.1 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * BSD LICENSE
 *
 * tinycompress library for compress audio offload in alsa
 * Copyright (c) 2011-2012, Intel Corporation
 * All rights reserved.
 *
 * Author: Vinod Koul <vkoul@kernel.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * LGPL LICENSE
 *
 * tinycompress library for compress audio offload in alsa
 * Copyright (c) 2011-2012, Intel Corporation.
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to
 * the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <string.h>

#include <tinycompress/compress_ops.h>

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct compress
{
  FAR const struct compress_ops *ops;
  FAR void *data;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

extern const struct compress_ops g_compress_hw_ops;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

FAR struct compress *compress_open(unsigned int card, unsigned int device,
                                   unsigned int flags,
                                   FAR struct compr_config *config)
{
  FAR struct compress *compress;
  char name[128];

  if (card == 0)
    {
      snprintf(name, sizeof(name), "/dev/audio/compr%u%c", device,
               (flags & COMPRESS_IN) ? 'p' : 'c');
    }
  else
    {
      snprintf(name, sizeof(name), "/dev/audio/comprC%uD%u%c", card, device,
               (flags & COMPRESS_IN) ? 'p' : 'c');
    }

  compress = calloc(1, sizeof(struct compress));
  if (!compress)
    {
      return NULL;
    }

  compress->ops = &g_compress_hw_ops;
  compress->data = compress->ops->open_by_name(name, flags, config);
  if (compress->data == NULL)
    {
      free(compress);
      return NULL;
    }

  return compress;
}

FAR struct compress *compress_open_by_name(FAR const char *name,
                                           unsigned int flags,
                                           FAR struct compr_config *config)
{
  FAR struct compress *compress;

  compress = calloc(1, sizeof(struct compress));
  if (!compress)
    {
      return NULL;
    }

  compress->ops = &g_compress_hw_ops;

  compress->data = compress->ops->open_by_name(name, flags, config);
  if (compress->data == NULL)
    {
      free(compress);
      return NULL;
    }

  return compress;
}

void compress_close(FAR struct compress *compress)
{
  compress->ops->close(compress->data);

  free(compress);
}

int compress_get_hpointer(FAR struct compress *compress,
                          FAR unsigned int *avail,
                          FAR struct timespec *tstamp)
{
  return compress->ops->get_hpointer(compress->data, avail, tstamp);
}

int compress_get_tstamp(FAR struct compress *compress,
                        FAR unsigned int *samples,
                        FAR unsigned int *sampling_rate)
{
  return compress->ops->get_tstamp(compress->data, samples, sampling_rate);
}

int compress_write(FAR struct compress *compress, FAR const void *buf,
                   unsigned int size)
{
  return compress->ops->write(compress->data, buf, size);
}

int compress_read(FAR struct compress *compress, FAR void *buf,
                  unsigned int size)
{
  return compress->ops->read(compress->data, buf, size);
}

int compress_start(FAR struct compress *compress)
{
  return compress->ops->start(compress->data);
}

int compress_stop(FAR struct compress *compress)
{
  return compress->ops->stop(compress->data);
}

int compress_pause(FAR struct compress *compress)
{
  return compress->ops->pause(compress->data);
}

int compress_resume(FAR struct compress *compress)
{
  return compress->ops->resume(compress->data);
}

int compress_drain(FAR struct compress *compress)
{
  return compress->ops->drain(compress->data);
}

int compress_reset(FAR struct compress *compress)
{
  return compress->ops->reset(compress->data);
}

int compress_next_track(FAR struct compress *compress)
{
  return compress->ops->next_track(compress->data);
}

int compress_partial_drain(FAR struct compress *compress)
{
  return compress->ops->partial_drain(compress->data);
}

int compress_set_gapless_metadata(FAR struct compress *compress,
                                  FAR struct compr_gapless_mdata *mdata)
{
  return compress->ops->set_gapless_metadata(compress->data, mdata);
}

bool is_codec_supported(unsigned int card, unsigned int device,
                        unsigned int flags, FAR struct snd_codec *codec)
{
  FAR const struct compress_ops *ops = &g_compress_hw_ops;
  char name[128];

  if (card == 0)
    {
      snprintf(name, sizeof(name), "/dev/audio/compr%u%c", device,
               (flags & COMPRESS_IN) ? 'p' : 'c');
    }
  else
    {
      snprintf(name, sizeof(name), "/dev/audio/comprC%uD%u%c", card, device,
               (flags & COMPRESS_IN) ? 'p' : 'c');
    }

  return ops->is_codec_supported_by_name(name, flags, codec);
}

bool is_codec_supported_by_name(FAR const char *name, unsigned int flags,
                                FAR struct snd_codec *codec)
{
  FAR const struct compress_ops *ops = &g_compress_hw_ops;

  return ops->is_codec_supported_by_name(name, flags, codec);
}

void compress_set_max_poll_wait(FAR struct compress *compress,
                                int milliseconds)
{
  compress->ops->set_max_poll_wait(compress->data, milliseconds);
}

void compress_nonblock(FAR struct compress *compress, int nonblock)
{
  compress->ops->set_nonblock(compress->data, nonblock);
}

int compress_wait(FAR struct compress *compress, int timeout_ms)
{
  return compress->ops->wait(compress->data, timeout_ms);
}

int is_compress_running(FAR struct compress *compress)
{
  return compress->ops->is_compress_running(compress->data);
}

int is_compress_ready(FAR struct compress *compress)
{
  return compress->ops->is_compress_ready(compress->data);
}

FAR const char *compress_get_error(FAR struct compress *compress)
{
  return compress->ops->get_error(compress->data);
}

int compress_set_codec_params(FAR struct compress *compress,
                              FAR struct snd_codec *codec)
{
  return compress->ops->set_codec_params(compress->data, codec);
}

int compress_get_codec_params(FAR struct compress *compress,
                              FAR struct snd_codec *codec)
{
  return compress->ops->get_codec_params(compress->data, codec);
}

int compress_set_volume(FAR struct compress *compress, float volume)
{
  return compress->ops->set_volume(compress->data, volume);
}

int compress_set_params(FAR struct compress *compress,
                        FAR const char *params)
{
  return compress->ops->set_params(compress->data, params);
}

int compress_set_event_callback(FAR struct compress *compress,
                                compress_event_t on_event,
                                FAR void *cookie)
{
  return compress->ops->set_event_callback(compress->data, on_event, cookie);
}

int compress_set_current_config(FAR struct compress *compress,
                                FAR struct compr_config *config)
{
  return compress->ops->set_current_config(compress->data, config);
}

int compress_get_current_config(FAR struct compress *compress,
                                FAR struct compr_config *config)
{
  return compress->ops->get_current_config(compress->data, config);
}

int compress_get_file_descriptor(FAR struct compress *compress)
{
  return compress->ops->get_file_descriptor(compress->data);
}

int compress_poll_available(FAR struct compress *compress)
{
  return compress->ops->poll_available(compress->data);
}

int compress_flush(FAR struct compress *compress)
{
  return compress->ops->flush(compress->data);
}
