/****************************************************************************
 * apps/audioutils/tinycompress/include/tinycompress/tinywave.h
 *
 * SPDX-License-Identifier: (LGPL-2.1-only OR BSD-3-Clause)
 *
 * wave header and parsing
 *
 * Copyright 2020 NXP
 *
 ****************************************************************************/

#ifndef __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_TINYCOMPRESS_TINYWAVE_H
#define __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_TINYCOMPRESS_TINYWAVE_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

begin_packed_struct
struct riff_chunk
{
  char desc[4];
  uint32_t size;
}
end_packed_struct;

begin_packed_struct
struct wave_header
{
  begin_packed_struct
  struct
  {
    struct riff_chunk chunk;
    char format[4];
  }
  end_packed_struct riff;

  begin_packed_struct
  struct
  {
    struct riff_chunk chunk;
    uint16_t type;
    uint16_t channels;
    uint32_t rate;
    uint32_t byterate;
    uint16_t blockalign;
    uint16_t samplebits;
  }
  end_packed_struct fmt;

  begin_packed_struct
  struct
  {
    struct riff_chunk chunk;
  }
  end_packed_struct data;
}
end_packed_struct;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void init_wave_header(FAR struct wave_header *header, uint16_t channels,
                      uint32_t rate, uint16_t samplebits);
void size_wave_header(FAR struct wave_header *header, uint32_t size);
int parse_wave_header(FAR struct wave_header *header,
                      FAR unsigned int *channels, FAR unsigned int *rate,
                      FAR unsigned int *format);

#ifdef __cplusplus
}
#endif

#endif /* __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_TINYCOMPRESS_TINYWAVE_H */
