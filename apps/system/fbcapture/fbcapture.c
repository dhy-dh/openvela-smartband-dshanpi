/****************************************************************************
 * apps/system/fbcapture/fbcapture.c
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

#include <errno.h>
#include <fcntl.h>
#include <nuttx/cache.h>
#include <nuttx/config.h>
#include <nuttx/video/fb.h>
#include <poll.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

/****************************************************************************
 * Preprocessor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct fb_state_s
{
    int fd;
    struct fb_videoinfo_s vinfo;
    struct fb_planeinfo_s pinfo;
    FAR void *memory;
};
struct fb_param_s
{
    FAR const char *dev_path;
    FAR const char *output_path;
};

begin_packed_struct
struct bmp_file_header_s
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t off_bits;
} end_packed_struct;

begin_packed_struct
struct bmp_info_header_s
{
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    int32_t  x_pels_per_meter;
    int32_t  y_pels_per_meter;
    uint32_t clr_used;
    uint32_t clr_important;
} end_packed_struct;

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * show_usage
 ****************************************************************************/

/****************************************************************************
 * Name: show_usage
 *
 * Description:
 *   Show usage of this tool.
 *
 * Input Parameters:
 *   progname - The name of this tool.
 *   exitcode - The exit code to return.
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

static void show_usage(FAR const char *progname, int exitcode)
{
    printf("\nUsage: %s"
           " -d <string> -o <string> -s\n",
        progname);

    printf("\nWhere:\n");
    printf("  -d <string> Framebuffer device path.\n");
    printf("  -o <string> Output file path.\n");

    exit(exitcode);
}

/****************************************************************************
 * Name: parse_commandline
 *
 * Description:
 *   Parse command line options.
 *
 * Input Parameters:
 *   argc - The number of command line arguments.
 *   argv - The command line arguments.
 *   param - The parameter structure to be filled.
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

static void parse_commandline(int argc, FAR char **argv,
                              FAR struct fb_param_s *param)
{
  int ch;

  /* Set default param */

  memset(param, 0, sizeof(struct fb_param_s));
  param->dev_path = "/dev/fb0";
  param->output_path = "/data/capture.bmp";

  while ((ch = getopt(argc, argv, "d:o:h")) != -1)
    {
      switch (ch)
        {
          case 'd':
            param->dev_path = optarg;
            break;

          case 'o':
            param->output_path = optarg;
            break;

          case 'h':
            show_usage(argv[0], EXIT_FAILURE);
            break;

          case '?':
            printf("Unknown option: %c\n", optopt);
            show_usage(argv[0], EXIT_FAILURE);
            break;

          default:
            break;
        }
    }
}

/****************************************************************************
 * Name: fb_close
 *
 * Description:
 *   Close the framebuffer device.
 *
 * Input Parameters:
 *   state - The framebuffer state structure.
 *
 * Returned Value:
 *   None
 *
 ****************************************************************************/

static void fb_close(FAR struct fb_state_s *state)
{
  if (state->memory)
    {
      munmap(state->memory, state->pinfo.fblen);
      state->memory = NULL;
    }

  if (state->fd >= 0)
    {
      close(state->fd);
      state->fd = -1;
    }
}

/****************************************************************************
 * Name: fb_open
 *
 * Description:
 *   Open the framebuffer device.
 *
 * Input Parameters:
 *   state - The framebuffer state structure.
 *   path - The framebuffer device path.
 *
 * Returned Value:
 *   0 on success, -1 on failure.
 *
 ****************************************************************************/

static int fb_open(FAR struct fb_state_s *state, FAR const char *path)
{
  memset(state, 0, sizeof(struct fb_state_s));

  state->fd = open(path, O_RDWR | O_CLOEXEC);
  if (state->fd < 0)
    {
      printf("Failed to open framebuffer device: %s, error: %d\n",
              path, errno);
      goto failed;
    }

  /* Get fixed screen information */

  if (ioctl(state->fd, FBIOGET_VIDEOINFO, &state->vinfo) < 0)
    {
      printf("ioctl FBIOGET_VIDEOINFO failed: %d\n", errno);
      goto failed;
    }

  /* Get variable screen information */

  if (ioctl(state->fd, FBIOGET_PLANEINFO, &state->pinfo) < 0)
    {
      printf("ioctl FBIOGET_PLANEINFO failed: %d\n", errno);
      goto failed;
    }

  /* Map the device to memory */

  state->memory = mmap(NULL, state->pinfo.fblen, PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_FILE, state->fd, 0);
  if (state->memory == MAP_FAILED)
    {
      printf("mmap failed: %d\n", errno);
      goto failed;
    }

  return 0;

failed:
  fb_close(state);
  return -1;
}

/****************************************************************************
 * fb_capture_save
 *
 * Description:
 *   Capture the framebuffer and save it as a PNG file.
 *
 * Input Parameters:
 *   state - The framebuffer state structure.
 *   path - The output file path.
 *
 * Returned Value:
 *   0 on success, -1 on failure.
 *
 ****************************************************************************/

static int fb_capture_save(FAR struct fb_state_s *state,
                           FAR const char *path)
{
  FAR const void *src_buffer = state->memory;
  uint32_t src_stride = state->pinfo.stride;
  int fd = -1;
  int ret = -1;
  bool need_convert = false;
  uint32_t bmp_stride = 0;
  uint32_t padding = 0;
  struct bmp_file_header_s file_hdr;
  struct bmp_info_header_s info_hdr;

  uint32_t width = state->vinfo.xres;
  uint32_t height = state->vinfo.yres;
  uint16_t bpp = 32;
  ssize_t written;
  FAR const uint8_t *row_ptr;
  uint32_t x;
  uint32_t y;

  printf("Taking screenshot of '%s' ...\n", path);

  switch (state->vinfo.fmt)
    {
      case FB_FMT_RGB16_565:
          bpp = 16;
          break;
      case FB_FMT_RGB24:
          bpp = 24;
          break;
      case FB_FMT_RGBA32:
          break;
      case FB_FMT_RGB32:
          need_convert = true;
          break;
      default:
          printf("Unsupported color format: %d\n", state->vinfo.fmt);
          return -1;
    }

  bmp_stride = width * (bpp / 8);
  padding = (4 - (bmp_stride % 4)) % 4;
  bmp_stride += padding;

  memset(&file_hdr, 0, sizeof(file_hdr));
  file_hdr.type = 0x4d42;
  file_hdr.size = sizeof(file_hdr) + sizeof(info_hdr) + bmp_stride * height;
  file_hdr.off_bits = sizeof(file_hdr) + sizeof(info_hdr);

  memset(&info_hdr, 0, sizeof(info_hdr));
  info_hdr.size = sizeof(info_hdr);
  info_hdr.width = width;
  info_hdr.height = height;
  info_hdr.planes = 1;
  info_hdr.bit_count = bpp;

  up_invalidate_dcache(
      (uintptr_t)state->memory,
      (uintptr_t)state->memory + state->pinfo.stride * height);

  if (need_convert)
  {
    for (y = 0; y < height; y++)
    {
      FAR uint32_t *dest = state->memory + y * state->pinfo.stride;
      for (x = 0; x < width; x++)
        {
            *dest = *dest | 0xff000000;
            dest++;
        }
    }
  }

  fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fd < 0)
    {
      printf("Failed to create file, errno: %d\n", errno);
      goto cleanup;
    }

  /* Write headers */

  written = write(fd, &file_hdr, sizeof(file_hdr));
  if (written != sizeof(file_hdr))
    {
      printf("File header write failed, written %zd bytes,\
             expected %zu bytes, errno: %d\n",
             written, sizeof(file_hdr), errno);
      goto cleanup;
    }

  written = write(fd, &info_hdr, sizeof(info_hdr));
  if (written != sizeof(info_hdr))
    {
      printf("Info header write failed, written %zd bytes,\
             expected %zu bytes,errno: %d\n",
             written, sizeof(info_hdr), errno);
      goto cleanup;
    }

  /* Write pixel data (bottom-up) */

  row_ptr = src_buffer + (height - 1) * src_stride;

  for (y = 0; y < height; y++)
    {
      const size_t expect_bytes = bmp_stride - padding;
      written = write(fd, row_ptr, expect_bytes);
      if (written != expect_bytes)
        {
          printf("Pixel data write failed, written %zd bytes,\
                  expected %zu bytes, errno: %d\n",
                  written, expect_bytes, errno);
          goto cleanup;
        }

      if (padding > 0)
        {
          const uint32_t zero_pad = 0;
          written = write(fd, &zero_pad, padding);

          if (written != padding)
            {
              printf("Padding write failed, written %zd bytes,\
                      expected %zu bytes, errno: %d\n",
                      written, (size_t)padding, errno);
              goto cleanup;
            }
        }

      row_ptr -= src_stride;
    }

  ret = 0;
  printf("Success\n");

cleanup:
  if (fd >= 0)
    {
      close(fd);
    }

  return ret ? -1 : 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  struct fb_state_s state;
  struct fb_param_s param;
  parse_commandline(argc, argv, &param);

  if (fb_open(&state, param.dev_path) != 0)
    {
      return EXIT_FAILURE;
    }

  fb_capture_save(&state, param.output_path);

  fb_close(&state);
  return OK;
}
