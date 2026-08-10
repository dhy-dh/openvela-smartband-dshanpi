/****************************************************************************
 * apps/audioutils/tinycompress/include/tinycompress/compress_ops.h
 *
 * SPDX-License-Identifier: (LGPL-2.1-only OR BSD-3-Clause)
 * Copyright (c) 2020 The Linux Foundation. All rights reserved.
 *
 ****************************************************************************/

#ifndef __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_TINYCOMPRESS_COMPRESS_OPS_H
#define __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_TINYCOMPRESS_COMPRESS_OPS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <sound/compress_offload.h>
#include <tinycompress/tinycompress.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct compress_ops
{
  FAR void *(*open_by_name)(FAR const char *name, unsigned int flags,
                            FAR struct compr_config *config);
  void (*close)(FAR void *compress_data);
  int (*get_hpointer)(FAR void *compress_data, FAR unsigned int *avail,
                      FAR struct timespec *tstamp);
  int (*get_tstamp)(FAR void *compress_data, FAR unsigned int *samples,
                    FAR unsigned int *sampling_rate);
  int (*write)(FAR void *compress_data, FAR const void *buf, size_t size);
  int (*read)(FAR void *compress_data, FAR void *buf, size_t size);
  int (*start)(FAR void *compress_data);
  int (*stop)(FAR void *compress_data);
  int (*pause)(FAR void *compress_data);
  int (*resume)(FAR void *compress_data);
  int (*drain)(FAR void *compress_data);
  int (*reset)(FAR void *compress_data);
  int (*partial_drain)(FAR void *compress_data);
  int (*next_track)(FAR void *compress_data);
  int (*set_gapless_metadata)(FAR void *compress_data,
                              FAR struct compr_gapless_mdata *mdata);
  void (*set_max_poll_wait)(FAR void *compress_data, int milliseconds);
  void (*set_nonblock)(FAR void *compress_data, int nonblock);
  int (*wait)(FAR void *compress_data, int timeout_ms);
  bool (*is_codec_supported_by_name)(FAR const char *name,
                                     unsigned int flags,
                                     FAR struct snd_codec *codec);
  int (*is_compress_running)(FAR void *compress_data);
  int (*is_compress_ready)(FAR void *compress_data);
  FAR const char *(*get_error)(FAR void *compress_data);
  int (*set_codec_params)(FAR void *compress_data,
                          FAR struct snd_codec *codec);
  int (*get_codec_params)(FAR void *compress_data,
                          FAR struct snd_codec *codec);
  int (*set_volume)(FAR void *compress_data, float volume);
  int (*set_params)(FAR void *compress_data, FAR const char *params);
  int (*set_event_callback)(FAR void *compress_data, compress_event_t event,
                            FAR void *cookie);
  int (*set_current_config)(FAR void *compress_data,
                            FAR struct compr_config *config);
  int (*get_current_config)(FAR void *compress_data,
                            FAR struct compr_config *config);
  int (*get_file_descriptor)(FAR void *compress_data);
  int (*poll_available)(FAR void *compress_data);
  int (*flush)(FAR void *compress_data);
};

#ifdef __cplusplus
}
#endif

#endif /* __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_TINYCOMPRESS_COMPRESS_OPS_H */
