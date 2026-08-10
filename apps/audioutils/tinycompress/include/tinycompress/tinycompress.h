/****************************************************************************
 * apps/audioutils/tinycompress/include/tinycompress/tinycompress.h
 *
 * This file is provided under a dual BSD/LGPLv2.1 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * BSD LICENSE
 *
 * Copyright (c) 2011-2012, Intel Corporation
 * All rights reserved.
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

#ifndef __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_TINYCOMPRESS_TINYCOMPRESS_H
#define __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_TINYCOMPRESS_TINYCOMPRESS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define COMPRESS_OUT 0x20000000
#define COMPRESS_IN 0x10000000

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct compr_config
{
  uint32_t fragment_size;
  uint32_t fragments;
  FAR struct snd_codec *codec;
};

struct compr_gapless_mdata
{
  uint32_t encoder_delay;
  uint32_t encoder_padding;
};

struct compress;

typedef void (*compress_event_t)(FAR void *cookie, int msgid,
                                 FAR const void *extra);

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* compress_open: open a new compress stream
 * returns the valid struct compress on success, NULL on failure
 * If config does not specify a requested fragment size, on return
 * it will be updated with the size and number of fragments that
 * were configured
 *
 * @card: sound card number
 * @device: device number
 * @flags: device flags can be COMPRESS_OUT or COMPRESS_IN
 * @config: stream config requested. Returns actual fragment config
 */

FAR struct compress *compress_open(unsigned int card, unsigned int device,
                                   unsigned int flags,
                                   FAR struct compr_config *config);

/* compress_open_by_name: open a new compress stream
 * returns the valid struct compress on success, NULL on failure
 * If config does not specify a requested fragment size, on return
 * it will be updated with the size and number of fragments that
 * were configured.
 * format of name is :
 *    hw:<card>,<device> for real hw compress node
 *    <plugin_libname>:<custom string> for virtual compress node
 *
 * @name: name of the compress node
 * @flags: device flags can be COMPRESS_OUT or COMPRESS_IN
 * @config: stream config requested. Returns actual fragment config
 */

FAR struct compress *compress_open_by_name(FAR const char *name,
                                           unsigned int flags,
                                           FAR struct compr_config *config);

/* compress_close: close the compress stream
 *
 * @compress: compress stream to be closed
 */

void compress_close(FAR struct compress *compress);

/* compress_get_hpointer: get the hw timestamp
 * return 0 on success, negative on error
 *
 * @compress: compress stream on which query is made
 * @avail: buffer availble for write/read, in bytes
 * @tstamp: hw time
 */

int compress_get_hpointer(FAR struct compress *compress,
                          FAR unsigned int *avail,
                          FAR struct timespec *tstamp);

/* compress_get_tstamp: get the raw hw timestamp
 * return 0 on success, negative on error
 *
 * @compress: compress stream on which query is made
 * @samples: number of decoded samples played
 * @sampling_rate: sampling rate of decoded samples
 */

int compress_get_tstamp(FAR struct compress *compress,
                        FAR unsigned int *samples,
                        FAR unsigned int *sampling_rate);

/* compress_write: write data to the compress stream
 * return bytes written on success, negative on error
 * By default this is a blocking call and will not return
 * until all bytes have been written or there was a
 * write error.
 * If non-blocking mode has been enabled with compress_nonblock(),
 * this function will write all bytes that can be written without
 * blocking and will then return the number of bytes successfully
 * written. If the return value is not an error and is < size
 * the caller can use compress_wait() to block until the driver
 * is ready for more data.
 *
 * @compress: compress stream to be written to
 * @buf: pointer to data
 * @size: number of bytes to be written
 */

int compress_write(FAR struct compress *compress, FAR const void *buf,
                   unsigned int size);

/* compress_read: read data from the compress stream
 * return bytes read on success, negative on error
 * By default this is a blocking call and will block until
 * size bytes have been written or there was a read error.
 * If non-blocking mode was enabled using compress_nonblock()
 * the behaviour will change to read only as many bytes as
 * are currently available (if no bytes are available it
 * will return immediately). The caller can then use
 * compress_wait() to block until more bytes are available.
 *
 * @compress: compress stream from where data is to be read
 * @buf: pointer to data buffer
 * @size: size of given buffer
 */

int compress_read(FAR struct compress *compress, FAR void *buf,
                  unsigned int size);

/* compress_start: start the compress stream
 * return 0 on success, negative on error
 *
 * @compress: compress stream to be started
 */

int compress_start(FAR struct compress *compress);

/* compress_stop: stop the compress stream
 * return 0 on success, negative on error
 *
 * @compress: compress stream to be stopped
 */

int compress_stop(FAR struct compress *compress);

/* compress_pause: pause the compress stream
 * return 0 on success, negative on error
 *
 * @compress: compress stream to be paused
 */

int compress_pause(FAR struct compress *compress);

/* compress_resume: resume the compress stream
 * return 0 on success, negative on error
 *
 * @compress: compress stream to be resumed
 */

int compress_resume(FAR struct compress *compress);

/* compress_drain: drain the compress stream
 * return 0 on success, negative on error
 *
 * @compress: compress stream to be drain
 */

int compress_drain(FAR struct compress *compress);

/* compress_reset: reset the compress stream
 * return 0 on success, negative on error
 *
 * @compress: compress stream to be reset
 */

int compress_reset(FAR struct compress *compress);

/* compress_next_track: set the next track for stream
 *
 * return 0 on success, negative on error
 *
 * @compress: compress stream to be transistioned to next track
 */

int compress_next_track(FAR struct compress *compress);

/* compress_partial_drain: drain will return after the last frame is decoded
 * by DSP and will play the , All the data written into compressed
 * ring buffer is decoded
 *
 * return 0 on success, negative on error
 *
 * @compress: compress stream to be drain
 */

int compress_partial_drain(FAR struct compress *compress);

/* compress_set_gapless_metadata: set gapless metadata of a compress strem
 *
 * return 0 on success, negative on error
 *
 * @compress: compress stream for which metadata has to set
 * @mdata: metadata encoder delay and  padding
 */

int compress_set_gapless_metadata(FAR struct compress *compress,
                                  FAR struct compr_gapless_mdata *mdata);

/* is_codec_supported:check if the given codec is supported
 * returns true when supported, false if not
 *
 * @card: sound card number
 * @device: device number
 * @flags: stream flags
 * @codec: codec type and parameters to be checked
 */

bool is_codec_supported(unsigned int card, unsigned int device,
                        unsigned int flags, FAR struct snd_codec *codec);

/* is_codec_supported_by_name:check if the given codec is supported
 * returns true when supported, false if not.
 * format of name is :
 *    hw:<card>,<device> for real hw compress node
 *    <plugin_libname>:<custom string> for virtual compress node
 *
 * @name: name of the compress node
 * @flags: stream flags
 * @codec: codec type and parameters to be checked
 */

bool is_codec_supported_by_name(FAR const char *name, unsigned int flags,
                                FAR struct snd_codec *codec);

/* compress_set_max_poll_wait: set the maximum time tinycompress
 * will wait for driver to signal a poll(). Interval is in
 * milliseconds.
 * Pass interval of -1 to disable timeout and make poll() wait
 * until driver signals.
 * If this function is not used the timeout defaults to 20 seconds.
 */

void compress_set_max_poll_wait(FAR struct compress *compress,
                                int milliseconds);

/* Enable or disable non-blocking mode for write and read */

void compress_nonblock(FAR struct compress *compress, int nonblock);

/* Wait for ring buffer to ready for next read or write */

int compress_wait(FAR struct compress *compress, int timeout_ms);

int is_compress_running(FAR struct compress *compress);

int is_compress_ready(FAR struct compress *compress);

/* Returns a human readable reason for the last error */

FAR const char *compress_get_error(FAR struct compress *compress);

/* compress_set_codec_params: set codec config intended for next track
 * if DSP has support to switch CODEC config during gapless playback
 *
 * return 0 on success, negative on error
 *
 * @compress: compress stream for which metadata has to set
 * @codec: stream config for next track
 */

int compress_set_codec_params(FAR struct compress *compress,
                              FAR struct snd_codec *codec);

/* compress_get_codec_params: get current codec info
 * return 0 on success, negative on error
 *
 * @compress: compress stream for which metadata has to set
 * @codec: stream config for next track
 */

int compress_get_codec_params(FAR struct compress *compress,
                              FAR struct snd_codec *codec);

/* compress_set_volume: set volume to driver
 *
 * return 0 on success, negative on error
 *
 * @compress: compress stream for which metadata has to set
 * @volume: volume to be set
 */

int compress_set_volume(FAR struct compress *compress, float volume);

/* compress_set_params: set parsms to driver
 *
 * return 0 on success, negative on error
 *
 * @compress: compress stream for which metadata has to set
 * @params: stream params to be set
 */

int compress_set_params(FAR struct compress *compress,
                        FAR const char *params);

/* compress_set_event_callback: set event callback function
 *
 * return 0 on success, negative on error
 *
 * @compress: compress stream for which callback has to set
 * @on_event: event callback
 * @cookie:   callback argument
 */

int compress_set_event_callback(FAR struct compress *compress,
                                compress_event_t on_event,
                                FAR void *cookie);

/* compress_get_current_config: get current config info from driver
 *
 * return 0 on success, negative on error
 *
 * @compress: compress stream for which metadata has to set
 * @config: pointer to current config
 */

/* compress_set_current_config: set config info to driver
 *
 * return 0 on success, negative on error
 *
* @compress: compress stream for which metadata has to set
 * @config: pointer to current config
 */

int compress_set_current_config(FAR struct compress *compress,
                                FAR struct compr_config *config);

int compress_get_current_config(FAR struct compress *compress,
                                FAR struct compr_config *config);

/* compress_get_file_descriptor: get the poll file descriptor
 * return 0 on success, negative on error
 *
 * @compress: compress stream for which metadata has to set
 */

int compress_get_file_descriptor(FAR struct compress *compress);

/* compress_poll_available: poll for available audio buffers
 * return 0 on success, negative on error
 *
 * @compress: compress stream for which metadata has to set
 */

int compress_poll_available(FAR struct compress *compress);

/* compress_flush: flush the compress stream
 * return 0 on success, negative on error
 *
 * @compress: compress stream to be flushed
 */

int compress_flush(FAR struct compress *compress);

#if defined(__cplusplus)
}
#endif

#endif /* __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_TINYCOMPRESS_TINYCOMPRESS_H */
