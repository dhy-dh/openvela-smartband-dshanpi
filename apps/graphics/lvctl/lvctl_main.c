/****************************************************************************
 * apps/graphics/lvctl/lvctl_main.c
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

#include <nuttx/config.h>

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <lvgl/lvgl.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: print_func
 ****************************************************************************/

static void print_func(FAR const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

/****************************************************************************
 * Name: send_cmd
 ****************************************************************************/

static int send_cmd(FAR const void * cmd, size_t size)
{
  int sock = socket(PF_LOCAL, SOCK_STREAM, 0);
  struct sockaddr_un addr;
  int ret = OK;

  if (sock < 0)
    {
      perror("Failed to create socket");
      return -errno;
    }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_LOCAL;
  strncpy(addr.sun_path, LV_NUTTX_CONTROL_PIPE_NAME,
          sizeof(addr.sun_path) - 1);

  if (connect(sock, (FAR struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
      perror("Failed to connect to LVGL control server");
      ret = -errno;
      goto errout;
    }

  if (send(sock, cmd, size, 0) < 0)
    {
      perror("Failed to send command");
      ret = -errno;
      goto errout;
    }
  else
    {
      printf("Command sent successfully\n");
    }

errout:
  close(sock);
  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: lvctl_main
 *
 * Description:
 *   Main entry point for the LVGL control utility.
 *
 ****************************************************************************/

int main(int argc, FAR const char *argv[])
{
  int ret;
  lv_remote_ctrl_args_t args;

  if (lv_remote_ctrl_args_init(&args, argc - 1, argv + 1) != LV_RESULT_OK)
    {
      lv_remote_ctrl_show_help(argv[0], print_func);
      return 1;
    }

  ret = send_cmd(&args, sizeof(args));
  if (ret < 0)
    {
      printf("Failed to send command: %s\n", strerror(ret));
      return 1;
    }

  return 0;
}
