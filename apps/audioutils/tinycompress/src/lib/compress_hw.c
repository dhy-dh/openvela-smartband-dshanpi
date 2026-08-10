/****************************************************************************
 * apps/audioutils/tinycompress/src/lib/compress_hw.c
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

#include <debug.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <mqueue.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/time.h>
#include <unistd.h>

#include <nuttx/audio/audio.h>
#include <nuttx/config.h>

#include <sound/compress_offload.h>
#include <tinycompress/compress_ops.h>

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define DEFAULT_MAX_POLL_WAIT_MS 20000

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct compress_hw_data
{
  char name[32];
  int fd;
  mqd_t mq;
  int flags;
  int max_poll_wait_ms;
  int nonblocking;
  FAR void *session;
  dq_queue_t bufferq;
  struct compr_config config;

  compress_event_t on_event;
  FAR void *cookie;
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int compress_hw_get_capabilities(int fd, int ac_type, int ac_subtype,
                                        FAR struct audio_caps_s *caps)
{
  int ret;

  caps->ac_len = sizeof(struct audio_caps_s);
  caps->ac_type = ac_type;
  caps->ac_subtype = ac_subtype;

  ret = ioctl(fd, AUDIOIOC_GETCAPS, caps);
  if (ret < 0)
    {
      return -errno;
    }

  return ret;
}

static int compress_hw_query_supported_codecs(int fd,
                                              int codecs[MAX_NUM_CODECS])
{
  struct audio_caps_s caps;
  int ncodecs = 0;
  int ac_subtype;
  int format;
  int ret;
  int k;

  ret = compress_hw_get_capabilities(fd, AUDIO_TYPE_QUERY, AUDIO_TYPE_QUERY,
                                     &caps);
  if (ret < 0)
    {
      return ret;
    }

  format = caps.ac_format.hw;

  for (; format; )
    {
      ac_subtype = AUDIO_FMT_UNDEF;
      if (format & (1 << (AUDIO_FMT_PCM - 1)))
        {
          ac_subtype = AUDIO_FMT_PCM;
          format &= ~(1 << (AUDIO_FMT_PCM - 1));
        }
      else if (format & (1 << (AUDIO_FMT_MP3 - 1)))
        {
          ac_subtype = AUDIO_FMT_MP3;
          format &= ~(1 << (AUDIO_FMT_MP3 - 1));
        }
      else if (format & (1 << (AUDIO_FMT_AC3 - 1)))
        {
          ac_subtype = AUDIO_FMT_AC3;
          format &= ~(1 << (AUDIO_FMT_AC3 - 1));
        }
      else if (format & (1 << (AUDIO_FMT_WMA - 1)))
        {
          ac_subtype = AUDIO_FMT_WMA;
          format &= ~(1 << (AUDIO_FMT_WMA - 1));
        }
      else if (format & (1 << (AUDIO_FMT_DTS - 1)))
        {
          ac_subtype = AUDIO_FMT_DTS;
          format &= ~(1 << (AUDIO_FMT_DTS - 1));
        }
      else if (format & (1 << (AUDIO_FMT_OGG_VORBIS - 1)))
        {
          ac_subtype = AUDIO_FMT_OGG_VORBIS;
          format &= ~(1 << (AUDIO_FMT_OGG_VORBIS - 1));
        }
      else if (format & (1 << (AUDIO_FMT_FLAC - 1)))
        {
          ac_subtype = AUDIO_FMT_FLAC;
          format &= ~(1 << (AUDIO_FMT_FLAC - 1));
        }
      else if (format & (1 << (AUDIO_FMT_AMR - 1)))
        {
          ac_subtype = AUDIO_FMT_AMR;
          format &= ~(1 << (AUDIO_FMT_AMR - 1));
        }
      else if (format & (1 << (AUDIO_FMT_AAC - 1)))
        {
          ac_subtype = AUDIO_FMT_AAC;
          format &= ~(1 << (AUDIO_FMT_AAC - 1));
        }
      else if (format & (1 << (AUDIO_FMT_OTHER - 1)))
        {
          ac_subtype = AUDIO_FMT_OTHER;
          format &= ~(1 << (AUDIO_FMT_OTHER - 1));
        }

      ret = compress_hw_get_capabilities(fd, AUDIO_TYPE_QUERY, ac_subtype,
                                         &caps);
      if (ret < 0)
        {
          continue;
        }

      if (ac_subtype == AUDIO_FMT_OTHER)
        {
          for (; caps.ac_controls.w; )
            {
              ac_subtype = AUDIO_FMT_UNDEF;

              if (caps.ac_controls.w & (1 << (AUDIO_FMT_OPUS - 1)))
                {
                  ac_subtype = AUDIO_FMT_OPUS;
                  caps.ac_controls.w &= ~(1 << (AUDIO_FMT_OPUS - 1));
                }

              ret = compress_hw_get_capabilities(fd, AUDIO_TYPE_QUERY,
                                                 ac_subtype, &caps);
              if (ret < 0)
                {
                  continue;
                }

              for (k = 0; k < sizeof(caps.ac_controls.b) &&
                          ncodecs < MAX_NUM_CODECS;
                   k++)
                {
                  if (caps.ac_controls.b[k] == AUDIO_SUBFMT_END)
                    {
                      if (k == 0)
                        {
                          codecs[ncodecs++] = ac_subtype;
                        }

                      break;
                    }

                  codecs[ncodecs++] = caps.ac_controls.b[k];
                }
            }
        }
      else
        {
          for (k = 0;
               k < sizeof(caps.ac_controls.b) && ncodecs < MAX_NUM_CODECS;
               k++)
            {
              if (caps.ac_controls.b[k] == AUDIO_SUBFMT_END)
                {
                  if (k == 0)
                    {
                      codecs[ncodecs++] = ac_subtype;
                    }

                  break;
                }

              codecs[ncodecs++] = caps.ac_controls.b[k];
            }
        }
    }

  return ncodecs;
}

static bool compress_hw_is_codec_type_supported(int fd,
                                                FAR struct snd_codec *codec)
{
  int codecs[MAX_NUM_CODECS];
  int ncodecs;
  int i;

  ncodecs = compress_hw_query_supported_codecs(fd, codecs);

  for (i = 0; i < ncodecs; i++)
    {
      if (codecs[i] == codec->id)
        {
          return true;
        }
    }

  return false;
}

static void compress_hw_deinit(FAR struct compress_hw_data *compress)
{
  char mqname[64];

  if (compress->mq >= 0)
    {
      ioctl(compress->fd, AUDIOIOC_UNREGISTERMQ, compress->session);
      mq_close(compress->mq);
      compress->mq = -1;
      snprintf(mqname, sizeof(mqname), "/tmp/%s", compress->name);
      mq_unlink(mqname);
    }

  ioctl(compress->fd, AUDIOIOC_RELEASE, compress->session);
  close(compress->fd);
  compress->fd = -1;
}

static int compress_hw_init(FAR struct compress_hw_data *compress,
                            FAR const char *name)
{
  char mqname[64];
  int ret;

  struct mq_attr attr =
    {
      .mq_maxmsg = 32,
      .mq_msgsize = sizeof(struct audio_msg_s),
    };

  /* Open device */

  compress->fd = open(name, O_RDWR | O_CLOEXEC);
  if (compress->fd < 0)
    {
      return -errno;
    }

  /* Configure */

  ret = ioctl(compress->fd, AUDIOIOC_RESERVE, &compress->session);
  if (ret < 0)
    {
      ret = -errno;
      goto out;
    }

  /* Create message queue */

  snprintf(mqname, sizeof(mqname), "/tmp/%s", compress->name);
  compress->mq =
      mq_open(mqname, O_RDWR | O_CREAT | O_CLOEXEC, 0644, &attr);
  if (compress->mq < 0)
    {
      ret = -errno;
      goto out;
    }

  ret = ioctl(compress->fd, AUDIOIOC_REGISTERMQ, compress->mq);
  if (ret < 0)
    {
      ret = -errno;
      goto out;
    }

  return 0;

out:
  compress_hw_deinit(compress);
  return ret;
}

static int compress_hw_set_codec_params(FAR void *compress_data,
                                        FAR struct snd_codec *codec)
{
  FAR struct compress_hw_data *compress = compress_data;
  struct audio_caps_desc_s caps_desc;
  int ret;

  if (!codec)
    {
      return -EINVAL;
    }

  memset(&caps_desc, 0, sizeof(caps_desc));
  caps_desc.caps.ac_len = sizeof(struct audio_caps_s);
  caps_desc.caps.ac_type =
      compress->flags & COMPRESS_IN ? AUDIO_TYPE_OUTPUT : AUDIO_TYPE_INPUT;
  caps_desc.caps.ac_channels = codec->ch_in;
  caps_desc.caps.ac_controls.hw[0] = codec->sample_rate;
  caps_desc.caps.ac_controls.b[3] = codec->sample_rate >> 16;
  caps_desc.caps.ac_format.b[0] = codec->pcm_format;
  caps_desc.caps.ac_subtype = codec->id;

  caps_desc.caps.ac_codec.ch_mode = codec->ch_mode;
  caps_desc.caps.ac_codec.level = codec->level;
  caps_desc.caps.ac_codec.profile = codec->profile;
  caps_desc.caps.ac_codec.rate_control = codec->rate_control;
  caps_desc.caps.ac_codec.format = codec->format;
  caps_desc.caps.ac_codec.align = codec->align;
  caps_desc.caps.ac_codec.bit_rate = codec->bit_rate;

  memcpy(&caps_desc.caps.ac_codec.options, &codec->options,
    sizeof(codec->options));

  ret = ioctl(compress->fd, AUDIOIOC_CONFIGURE, &caps_desc);
  audinfo("configure, codec:%" PRIu32 ", bit_rate:%" PRIu32 ", ch:%" PRIu32
          ", rate:%" PRIu32 ", ret:%d\n",
          codec->id, codec->bit_rate, codec->ch_in,
          codec->sample_rate, ret);
  if (ret < 0)
    {
      return -errno;
    }

  return 0;
}

static int compress_hw_get_codec_params(FAR void *compress_data,
                                        FAR struct snd_codec *codec)
{
  FAR struct compress_hw_data *compress = compress_data;
  struct audio_info_s info;
  int ret;

  if (!codec)
    {
      return -EINVAL;
    }

  memset(codec, 0, sizeof(*codec));
  ret = ioctl(compress->fd, AUDIOIOC_GETAUDIOINFO, &info);
  if (ret < 0)
    {
      return -errno;
    }

  codec->id = info.format;
  codec->ch_in = info.channels;
  codec->ch_out = info.channels;
  codec->sample_rate = info.samplerate;
  codec->pcm_format = info.subformat;
  codec->ch_mode = info.codec.ch_mode;
  codec->level = info.codec.level;
  codec->profile = info.codec.profile;
  codec->rate_control = info.codec.rate_control;
  codec->format = info.codec.format;
  codec->align = info.codec.align;
  codec->bit_rate = info.codec.bit_rate;

  memcpy(&codec->options, &info.codec.options, sizeof(info.codec.options));
  return 0;
}

static int compress_hw_set_current_config(FAR void *compress_data,
                                          FAR struct compr_config *config)
{
  FAR struct compress_hw_data *compress = compress_data;
  struct audio_buf_desc_s buf_desc;
  struct ap_buffer_info_s buf_info;
  int ret;
  int i;

  ret = compress_hw_set_codec_params(compress, config->codec);
  if (ret < 0)
    {
      return ret;
    }

  if (config->fragments)
    {
      /* Try to set BUFINFO and don't care the returns */

      buf_info.nbuffers = config->fragments;
      buf_info.buffer_size = config->fragment_size;

      audinfo("set buffer info, n:%d size:%d\n", buf_info.nbuffers,
              buf_info.buffer_size);

      ioctl(compress->fd, AUDIOIOC_SETBUFFERINFO, &buf_info);
    }

  ret = ioctl(compress->fd, AUDIOIOC_GETBUFFERINFO, &buf_info);
  audinfo("get buffer info, n:%d size:%d ret:%d\n", buf_info.nbuffers,
          buf_info.buffer_size, ret);

  if (ret >= 0)
    {
      config->fragments = buf_info.nbuffers;
      config->fragment_size = buf_info.buffer_size;
    }
  else
    {
      config->fragments = CONFIG_AUDIO_NUM_BUFFERS;
      config->fragment_size = CONFIG_AUDIO_BUFFER_NUMBYTES;
    }

  dq_init(&compress->bufferq);

  for (i = 0; i < config->fragments; i++)
    {
      FAR struct ap_buffer_s *buffer;

      buf_desc.numbytes = config->fragment_size;
      buf_desc.u.pbuffer = &buffer;
      ret = ioctl(compress->fd, AUDIOIOC_ALLOCBUFFER, &buf_desc);
      if (ret < 0)
        {
          return -errno;
        }

      dq_addlast(&buffer->dq_entry, &compress->bufferq);
    }

  compress->config.fragment_size = config->fragment_size;
  compress->config.fragments = config->fragments;
  return 0;
}

static int compress_hw_poll_available(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;
  bool nonblocking = compress->nonblocking;
  FAR struct ap_buffer_s *buffer;
  struct audio_msg_s msg;
  bool received = false;
  struct timespec ts0 =
    {
      0,
      0
    };

  for (; ; )
    {
      if (mq_timedreceive(compress->mq, (FAR char *)&msg, sizeof(msg), NULL,
                          nonblocking ? &ts0 : NULL) < 0)
        {
          if (errno != ETIMEDOUT)
            {
              return -errno;
            }

          return received ? 0 : -EAGAIN;
        }

      if (msg.msg_id == AUDIO_MSG_DEQUEUE)
        {
          buffer = msg.u.ptr;
          buffer->curbyte = 0;
          dq_addlast(&buffer->dq_entry, &compress->bufferq);
        }
      else
        {
          if (compress->on_event)
            {
              compress->on_event(compress->cookie, msg.msg_id, msg.u.ptr);
            }
        }

      received = true;
      nonblocking = true;
    }
}

static int compress_hw_peek_buffer(FAR struct compress_hw_data *compress,
                                   FAR struct ap_buffer_s **buffer)
{
  int ret;

  *buffer = (FAR struct ap_buffer_s *)dq_peek(&compress->bufferq);
  if (*buffer)
    {
      return 0;
    }

  ret = compress_hw_poll_available(compress);
  if (ret < 0)
    {
      return ret;
    }

  *buffer = (FAR struct ap_buffer_s *)dq_peek(&compress->bufferq);
  if (!*buffer)
    {
      return -EAGAIN;
    }

  return 0;
}

static int compress_hw_enqueue_buffer(FAR struct compress_hw_data *compress,
                                      FAR struct ap_buffer_s *buffer,
                                      bool final)
{
  struct audio_buf_desc_s desc;
  int ret;

  if (final)
    {
      buffer->flags |= AUDIO_APB_FINAL;
      audinfo("compress %s enqueue apb final.\n", compress->name);
    }
  else
    {
      buffer->flags &= ~AUDIO_APB_FINAL;
    }

  memset(&desc, 0, sizeof(desc));
  buffer->nbytes = buffer->curbyte;
  buffer->curbyte = 0;
  desc.u.buffer = buffer;
  ret = ioctl(compress->fd, AUDIOIOC_ENQUEUEBUFFER, &desc);
  if (ret < 0)
    {
      return -errno;
    }

  return 0;
}

static int compress_hw_get_current_config(FAR void *compress_data,
                                          FAR struct compr_config *config)
{
  FAR struct compress_hw_data *compress = compress_data;
  struct ap_buffer_info_s buf_info;
  int ret;

  if (!config)
    {
      return -EINVAL;
    }

  ret = ioctl(compress->fd, AUDIOIOC_GETBUFFERINFO, &buf_info);
  if (ret < 0)
    {
      return -errno;
    }

  config->fragments = buf_info.nbuffers;
  config->fragment_size = buf_info.buffer_size;
  return compress_hw_get_codec_params(compress, config->codec);
}

static void *compress_hw_open_by_name(FAR const char *name,
                                      unsigned int flags,
                                      FAR struct compr_config *config)
{
  FAR struct compress_hw_data *compress;
  int ret;

  if (!((flags & COMPRESS_OUT) || (flags & COMPRESS_IN)))
    {
      return NULL;
    }

  compress = calloc(1, sizeof(struct compress_hw_data));
  if (!compress)
    {
      return NULL;
    }

  compress->max_poll_wait_ms = DEFAULT_MAX_POLL_WAIT_MS;
  strlcpy(compress->name, strrchr(name, '/'), sizeof(compress->name));
  compress->flags = flags;

  ret = compress_hw_init(compress, name);
  if (ret < 0)
    {
      goto fail;
    }

  if (config)
    {
      ret = compress_hw_set_current_config(compress, config);
      if (ret < 0)
        {
          compress_hw_deinit(compress);
          goto fail;
        }
    }

  return compress;

fail:
  free(compress);
  return NULL;
}

static int compress_hw_is_compress_running(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;
  struct audio_status_s status;
  int ret;

  ret = ioctl(compress->fd, AUDIOIOC_GETSTATUS, &status);
  if (ret < 0)
    {
      return -errno;
    }

  return status.state == AUDIO_STATE_RUNNING;
}

static int compress_hw_stop(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;
  struct audio_buf_desc_s buf_desc;
  struct audio_status_s status;

  if (ioctl(compress->fd, AUDIOIOC_GETSTATUS, &status) >= 0 &&
      (status.state == AUDIO_STATE_RUNNING ||
      status.state == AUDIO_STATE_PAUSED))
    {
      if (ioctl(compress->fd, AUDIOIOC_STOP, compress->session) < 0)
        {
          auderr("cannot stop the stream");
          return -errno;
        }
    }

  compress->nonblocking = false;
  while (dq_count(&compress->bufferq) < compress->config.fragments)
    {
      compress_hw_poll_available(compress);
    }

  while (!dq_empty(&compress->bufferq))
    {
      buf_desc.u.buffer =
          (FAR struct ap_buffer_s *)dq_remfirst(&compress->bufferq);
      ioctl(compress->fd, AUDIOIOC_FREEBUFFER, &buf_desc);
    }

  return 0;
}

static void compress_hw_close(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;

  compress_hw_stop(compress);

  compress_hw_deinit(compress);
  free(compress);
}

static int compress_hw_is_compress_ready(FAR void *compress_data)
{
  return 1;
}

static int compress_hw_get_hpointer(FAR void *compress_data,
                                    FAR unsigned int *avail,
                                    struct timespec *tstamp)
{
  FAR struct compress_hw_data *compress = compress_data;
  FAR struct ap_buffer_s *buffer;
  FAR struct dq_entry_s *cur;
  long pos;

  *avail = 0;
  for (cur = dq_peek(&compress->bufferq); cur; cur = dq_next(cur))
    {
      buffer = (FAR struct ap_buffer_s *)cur;
      avail += buffer->nbytes - buffer->curbyte;
    }

  if (ioctl(compress->fd, AUDIOIOC_GETPOSITION, &pos))
    {
      auderr("cannot get position");
      return -errno;
    }

  tstamp->tv_sec = pos / 1000;
  tstamp->tv_nsec = (pos % 1000) * 1000000;

  return 0;
}

static int compress_hw_get_tstamp(FAR void *compress_data,
                                  FAR unsigned int *samples,
                                  FAR unsigned int *sampling_rate)
{
  return -ENOTSUP;
}

static int compress_hw_write(FAR void *compress_data, FAR const void *buf,
                             size_t size)
{
  FAR struct compress_hw_data *compress = compress_data;
  FAR struct ap_buffer_s *buffer;
  size_t left = size;
  int ret = 0;

  while (left > 0)
    {
      size_t len;

      ret = compress_hw_peek_buffer(compress, &buffer);
      if (ret < 0)
        {
          break;
        }

      len = MIN(buffer->nmaxbytes - buffer->curbyte, left);
      memcpy(buffer->samp + buffer->curbyte, buf, len);
      buffer->curbyte += len;

      if (buffer->curbyte == buffer->nmaxbytes)
        {
          dq_remfirst(&compress->bufferq);
          ret = compress_hw_enqueue_buffer(compress, buffer, false);
          if (ret < 0)
            {
              break;
            }
        }

      buf = (char *)buf + len;
      left -= len;
    }

  return left != size ? size - left : ret;
}

static int compress_hw_read(FAR void *compress_data, FAR void *buf,
                            size_t size)
{
  FAR struct compress_hw_data *compress = compress_data;
  FAR struct ap_buffer_s *buffer;
  size_t left = size;
  int ret = 0;

  while (left > 0)
    {
      size_t len;

      ret = compress_hw_peek_buffer(compress, &buffer);
      if (ret < 0)
        {
          break;
        }

      len = MIN(buffer->nbytes - buffer->curbyte, left);
      memcpy(buf, buffer->samp + buffer->curbyte, len);

      buffer->curbyte += len;
      if (buffer->curbyte == buffer->nbytes)
        {
          dq_remfirst(&compress->bufferq);
          buffer->curbyte = buffer->nmaxbytes;
          ret = compress_hw_enqueue_buffer(compress, buffer, false);
          if (ret < 0)
            {
              ret = -errno;
              break;
            }
        }

      buf = (FAR char *)buf + len;
      left -= len;
    }

  return left != size ? size - left : ret;
}

static int compress_hw_start(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;
  FAR struct ap_buffer_s *buffer;
  int ret;

  /* Record device, enqueue all buffer before start */

  if (compress->flags & COMPRESS_OUT)
    {
      while (dq_count(&compress->bufferq))
        {
          buffer = (FAR struct ap_buffer_s *)dq_remfirst(&compress->bufferq);
          buffer->curbyte = buffer->nmaxbytes;

          ret = compress_hw_enqueue_buffer(compress, buffer, false);
          if (ret < 0)
            {
              return ret;
            }
        }
    }

  if (ioctl(compress->fd, AUDIOIOC_START, compress->session))
    {
      auderr("cannot start the stream");
      return -errno;
    }

  return 0;
}

static int compress_hw_pause(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;

  if (ioctl(compress->fd, AUDIOIOC_PAUSE, compress->session) < 0)
    {
      auderr("cannot pause the stream");
      return -errno;
    }

  return 0;
}

static int compress_hw_resume(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;

  if (ioctl(compress->fd, AUDIOIOC_RESUME, compress->session) < 0)
    {
      auderr("cannot resume the stream");
      return -errno;
    }

  return 0;
}

static int compress_hw_flush(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;

  if (ioctl(compress->fd, AUDIOIOC_FLUSH, compress->session) < 0)
    {
      auderr("cannot flush the stream");
      return -errno;
    }

  return 0;
}

static int compress_hw_drain(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;
  FAR struct ap_buffer_s *buffer;
  struct audio_status_s status;
  int ret;

  ret = ioctl(compress->fd, AUDIOIOC_GETSTATUS, &status);
  if (ret < 0)
    {
      return -errno;
    }

  if (status.state != AUDIO_STATE_RUNNING)
    {
      auderr("device not started");
      return -ENODEV;
    }

  if ((compress->flags & COMPRESS_IN))
    {
      buffer = (FAR struct ap_buffer_s *)dq_peek(&compress->bufferq);
      if (buffer && buffer->curbyte)
        {
          dq_remfirst(&compress->bufferq);
          compress_hw_enqueue_buffer(compress, buffer, true);
        }
    }

  while (dq_count(&compress->bufferq) < compress->config.fragments)
    {
      ret = compress_hw_poll_available(compress);
      if (ret < 0)
        {
          return ret;
        }
    }

  return 0;
}

static int compress_hw_reset(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;

  if (ioctl(compress->fd, AUDIOIOC_HWRESET, compress->session) < 0)
    {
      auderr("cannot reset the stream");
      return -errno;
    }

  return 0;
}

static int compress_hw_partial_drain(FAR void *compress_data)
{
  return -ENOTSUP;
}

static int compress_hw_next_track(FAR void *compress_data)
{
  return -ENOTSUP;
}

static int
compress_hw_set_gapless_metadata(FAR void *compress_data,
                                 FAR struct compr_gapless_mdata *mdata)
{
  return -ENOTSUP;
}

static void compress_hw_set_max_poll_wait(FAR void *compress_data,
                                          int milliseconds)
{
  FAR struct compress_hw_data *compress = compress_data;

  compress->max_poll_wait_ms = milliseconds;
}

static void compress_hw_set_nonblock(FAR void *compress_data, int nonblock)
{
  FAR struct compress_hw_data *compress = compress_data;

  compress->nonblocking = !!nonblock;
}

static int compress_hw_wait(FAR void *compress_data, int timeout_ms)
{
  FAR struct compress_hw_data *compress = compress_data;
  struct pollfd fds;
  int ret;

  if (!dq_empty(&compress->bufferq))
    {
      return 0;
    }

  memset(&fds, 0, sizeof(fds));
  fds.fd = compress->fd;
  fds.events = POLLOUT | POLLIN;
  ret = poll(&fds, 1, timeout_ms);
  if (ret > 0)
    {
      if (fds.revents & POLLERR)
        {
          auderr("poll returned error!");
          return -EIO;
        }

      if (fds.revents & (POLLOUT | POLLIN))
        {
          return 0;
        }
    }

  return ret < 0 ? -errno : -ETIME;
}

static bool compress_hw_is_codec_supported_by_name(
    FAR const char *name, unsigned int flags, FAR struct snd_codec *codec)
{
  int ret;
  int fd;

  fd = open(name, O_RDWR | O_CLOEXEC);
  if (fd < 0)
    {
      return -errno;
    }

  ret = compress_hw_is_codec_type_supported(fd, codec);

  close(fd);
  return ret;
}

static FAR const char *compress_hw_get_error(FAR void *compress_data)
{
  return strerror(errno);
}

static int compress_hw_set_volume(FAR void *compress_data, float volume)
{
  FAR struct compress_hw_data *compress = compress_data;
  struct audio_caps_desc_s caps_desc;
  int ret;

  memset(&caps_desc, 0, sizeof(caps_desc));
  caps_desc.caps.ac_len = sizeof(struct audio_caps_s);
  caps_desc.caps.ac_type = AUDIO_TYPE_FEATURE;
  caps_desc.caps.ac_format.hw = AUDIO_FU_VOLUME;
  caps_desc.caps.ac_controls.hw[0] = volume * 1000;

  ret = ioctl(compress->fd, AUDIOIOC_CONFIGURE, &caps_desc);
  if (ret < 0)
    {
      return -errno;
    }

  return 0;
}

static int compress_hw_set_params(FAR void *compress_data,
                                  FAR const char *params)
{
  FAR struct compress_hw_data *compress = compress_data;
  int ret;

  ret = ioctl(compress->fd, AUDIOIOC_SETPARAMTER, params);
  if (ret < 0)
    {
      return -errno;
    }

  return 0;
}

static int compress_hw_set_event_callback(FAR void *compress_data,
                                          compress_event_t on_event,
                                          FAR void *cookie)
{
  FAR struct compress_hw_data *compress = compress_data;

  compress->cookie = cookie;
  compress->on_event  = on_event;
  return 0;
}

static int compress_hw_get_file_descriptor(FAR void *compress_data)
{
  FAR struct compress_hw_data *compress = compress_data;

  return compress->mq;
}

const struct compress_ops g_compress_hw_ops =
{
  .open_by_name = compress_hw_open_by_name,
  .close = compress_hw_close,
  .get_hpointer = compress_hw_get_hpointer,
  .get_tstamp = compress_hw_get_tstamp,
  .write = compress_hw_write,
  .read = compress_hw_read,
  .start = compress_hw_start,
  .stop = compress_hw_stop,
  .pause = compress_hw_pause,
  .resume = compress_hw_resume,
  .drain = compress_hw_drain,
  .reset = compress_hw_reset,
  .partial_drain = compress_hw_partial_drain,
  .next_track = compress_hw_next_track,
  .set_gapless_metadata = compress_hw_set_gapless_metadata,
  .set_max_poll_wait = compress_hw_set_max_poll_wait,
  .set_nonblock = compress_hw_set_nonblock,
  .wait = compress_hw_wait,
  .is_codec_supported_by_name = compress_hw_is_codec_supported_by_name,
  .is_compress_running = compress_hw_is_compress_running,
  .is_compress_ready = compress_hw_is_compress_ready,
  .get_error = compress_hw_get_error,
  .set_codec_params = compress_hw_set_codec_params,
  .get_codec_params = compress_hw_get_codec_params,
  .set_volume = compress_hw_set_volume,
  .set_params = compress_hw_set_params,
  .set_event_callback = compress_hw_set_event_callback,
  .set_current_config = compress_hw_set_current_config,
  .get_current_config = compress_hw_get_current_config,
  .get_file_descriptor = compress_hw_get_file_descriptor,
  .poll_available = compress_hw_poll_available,
  .flush = compress_hw_flush,
};
