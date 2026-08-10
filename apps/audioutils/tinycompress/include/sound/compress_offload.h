/****************************************************************************
 * apps/audioutils/tinycompress/include/sound/compress_offload.h
 *
 * This header was automatically generated from a Linux kernel header
 * of the same name, to make information necessary for userspace to
 * call into the kernel available to libc.  It contains only constants,
 * structures, and macros generated from the original header, and thus,
 * contains no copyrightable information.
 *
 * To edit the content of this header, modify the corresponding
 * source file (e.g. under external/kernel-headers/original/) then
 * run bionic/libc/kernel/tools/update_all.py
 *
 * Any manual change here will be lost the next time this script will
 * be run. You've been warned!
 *
 ****************************************************************************/

#ifndef __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_SOUND_COMPRESS_OFFLOAD_H
#define __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_SOUND_COMPRESS_OFFLOAD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <sound/compress_params.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define SNDRV_COMPRESS_VERSION SNDRV_PROTOCOL_VERSION(0, 2, 0)

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct snd_compressed_buffer
{
  uint32_t fragment_size;
  uint32_t fragments;
};

struct snd_compr_params
{
  struct snd_compressed_buffer buffer;
  struct snd_codec codec;
  uint8_t no_wake_mode;
};

struct snd_compr_tstamp
{
  uint32_t byte_offset;
  uint32_t copied_total;
  uint32_t pcm_frames;
  uint32_t pcm_io_frames;
  uint32_t sampling_rate;
};

struct snd_compr_avail
{
  uint64_t avail;
  struct snd_compr_tstamp tstamp;
};

enum snd_compr_direction
{
  SND_COMPRESS_PLAYBACK = 0,
  SND_COMPRESS_CAPTURE
};

struct snd_compr_caps
{
  uint32_t num_codecs;
  uint32_t direction;
  uint32_t min_fragment_size;
  uint32_t max_fragment_size;
  uint32_t min_fragments;
  uint32_t max_fragments;
  uint32_t codecs[MAX_NUM_CODECS];
  uint32_t reserved[11];
};

struct snd_compr_codec_caps
{
  uint32_t codec;
  uint32_t num_descriptors;
  struct snd_codec_desc descriptor[MAX_NUM_CODEC_DESCRIPTORS];
};

enum
{
  SNDRV_COMPRESS_ENCODER_PADDING = 1,
  SNDRV_COMPRESS_ENCODER_DELAY = 2,
};

struct snd_compr_metadata
{
  uint32_t key;
  uint32_t value[8];
};

#ifdef __cplusplus
}
#endif

#endif /* __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_SOUND_COMPRESS_OFFLOAD_H */
