/****************************************************************************
 * apps/builtin/exec_builtin.c
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

#include <spawn.h>
#include <fcntl.h>
#include <errno.h>
#include <debug.h>

#include "builtin/builtin.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: exec_builtin
 *
 * Description:
 *   Executes builtin applications registered during 'make context' time.
 *   New application is run in a separate task context (and thread).
 *
 * Input Parameter:
 *   filename      - Name of the linked-in binary to be started.
 *   argv          - Argument list
 *   param         - Parameters for execute.
 *
 * Returned Value:
 *   This is an end-user function, so it follows the normal convention:
 *   Returns the PID of the exec'ed module.  On failure, it returns
 *   -1 (ERROR) and sets errno appropriately.
 *
 ****************************************************************************/

int exec_builtin(FAR const char *appname, FAR char * const *argv,
                 FAR const struct nsh_param_s *param)
{
  FAR const struct builtin_s *builtin;
  FAR posix_spawnattr_t *attr = NULL;
#ifndef CONFIG_NSH_DISABLE_PRLIMIT
  posix_spawnattr_t tmp;
#endif
  posix_spawn_file_actions_t file_actions;
  pid_t pid;
  int index;
  int ret;

  /* Verify that an application with this name exists */

  index = builtin_isavail(appname);
  if (index < 0)
    {
      ret = ENOENT;
      goto errout_with_errno;
    }

  /* Get information about the builtin */

  builtin = builtin_for_index(index);
  if (builtin == NULL)
    {
      ret = ENOENT;
      goto errout_with_errno;
    }

  ret = posix_spawn_file_actions_init(&file_actions);
  if (ret != 0)
    {
      goto errout_with_attrs;
    }

  if (param)
    {
#ifndef CONFIG_NSH_DISABLE_PRLIMIT
      if (param->prlimit.used)
        {
          ret = posix_spawnattr_init(&tmp);
          if (ret != 0)
            {
              goto errout_with_actions;
            }

          attr = &tmp;
          if (param->prlimit.priority)
            {
              struct sched_param sched;

              sched_getparam(0, &sched);
              sched.sched_priority = param->prlimit.priority;
              ret = posix_spawnattr_setschedparam(&tmp, &sched);
              if (ret != 0)
                {
                  goto errout_with_attrs;
                }
            }

          if (param->prlimit.stacksize)
            {
              ret = posix_spawnattr_setstacksize(&tmp,
                                                 param->prlimit.stacksize);
              if (ret != 0)
                {
                  goto errout_with_attrs;
                }
            }

          if (param->prlimit.heapsize)
            {
              ret = posix_spawnattr_setheapsize(&tmp,
                                                param->prlimit.heapsize);
              if (ret != 0)
                {
                  goto errout_with_attrs;
                }
            }
        }
#endif

      /* Is input being redirected? */

      if (param->file_in)
        {
          /* Set up to close open redirfile and set to stdin (0) */

          ret = posix_spawn_file_actions_addopen(&file_actions, 0,
                                                 param->file_in,
                                                 param->oflags_in, 0);
          if (ret != 0)
            {
              serr("ERROR: posix_spawn_file_actions_addopen failed: %d\n",
                   ret);
              goto errout_with_attrs;
            }
        }
#ifdef CONFIG_NSH_PIPELINE
      else if (param->fd_in != -1)
        {
          ret = posix_spawn_file_actions_adddup2(&file_actions,
                                                 param->fd_in, 0);
          if (ret != 0)
            {
              serr("ERROR: posix_spawn_file_actions_adddup2 failed: %d\n",
                   ret);
              goto errout_with_attrs;
            }
        }
#endif

      /* Is output being redirected? */

      if (param->file_out)
        {
          /* Set up to close open redirfile and set to stdout (1) */

          ret = posix_spawn_file_actions_addopen(&file_actions, 1,
                                                 param->file_out,
                                                 param->oflags_out, 0644);
          if (ret != 0)
            {
              serr("ERROR: posix_spawn_file_actions_addopen failed: %d\n",
                   ret);
              goto errout_with_attrs;
            }
        }
#ifdef CONFIG_NSH_PIPELINE
      else if (param->fd_out != -1)
        {
          ret = posix_spawn_file_actions_adddup2(&file_actions,
                                                 param->fd_out, 1);
          if (ret != 0)
            {
              serr("ERROR: posix_spawn_file_actions_adddup2 failed: %d\n",
                   ret);
              goto errout_with_attrs;
            }
        }
#endif
    }

#ifdef CONFIG_LIBC_EXECFUNCS
  /* Load and execute the application. */

  ret = posix_spawn(&pid, builtin->name, &file_actions, attr, argv, NULL);
  if (ret != 0 && builtin->main != NULL)
#endif
    {
      /* Start the built-in */

      pid = task_spawn(builtin->name, builtin->main, &file_actions,
                       attr, argv ? &argv[1] : NULL, NULL);
      ret = pid < 0 ? -pid : 0;
    }

  if (ret != 0)
    {
      serr("ERROR: task_spawn failed: %d\n", ret);
      goto errout_with_attrs;
    }

  /* Free attributes and file actions.  Ignoring return values in the case
   * of an error.
   */

  /* Return the task ID of the new task if the task was successfully
   * started.  Otherwise, ret will be ERROR (and the errno value will
   * be set appropriately).
   */

#ifndef CONFIG_NSH_DISABLE_PRLIMIT
  if (attr != NULL)
    {
      posix_spawnattr_destroy(attr);
    }
#endif

  posix_spawn_file_actions_destroy(&file_actions);

  return pid;

errout_with_attrs:
#ifndef CONFIG_NSH_DISABLE_PRLIMIT
  if (attr != NULL)
    {
      posix_spawnattr_destroy(attr);
    }
#endif

errout_with_actions:
  posix_spawn_file_actions_destroy(&file_actions);

errout_with_errno:
  errno = ret;
  return ERROR;
}
