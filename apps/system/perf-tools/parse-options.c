/****************************************************************************
 * apps/system/perf-tools/parse-options.c
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

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>

#include <nuttx/sched.h>

#include "builtin.h"
#include "parse-options.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int check_and_atoi(char *arg)
{
  int i;

  for (i = 0; arg[i] != '\0'; i++)
    {
      if (!isdigit(arg[i]))
        {
          return -EINVAL;
        }
    }

  return atoi(arg);
}

int parse_stat_options(int argc, FAR char **argv,
                       FAR struct stat_args_s *stat_args)
{
  int opt;
  int ret;

  stat_args->pid = -1;
  stat_args->cpu = -1;
  stat_args->cmd_nr = argc - 1;

  while ((opt = getopt(argc, argv, ":aC:p:e:c:m:g")) != -1)
    {
      switch (opt)
        {
          case 'a':
            if (stat_args->type == STAT_ARGS_NONE ||
                stat_args->type == STAT_ARGS_CPU)
              {
                stat_args->type = STAT_ARGS_ALL;
                stat_args->cpu = -1;
                stat_args->pid = -1;
                stat_args->events = NULL;
              }

            stat_args->cmd_nr -= 1;
            break;
          case 'C':
            if (stat_args->type == STAT_ARGS_NONE)
              {
                stat_args->type = STAT_ARGS_CPU;
                stat_args->cpu = check_and_atoi(optarg);
                stat_args->pid = -1;
                stat_args->events = NULL;
              }
            else if (stat_args->type == STAT_ARGS_EVENT)
              {
                stat_args->cpu = check_and_atoi(optarg);
                stat_args->pid = -1;
              }
            else if (stat_args->type == STAT_ARGS_TASK)
              {
                printf("PID/TID switch overriding CPU\n");
                stat_args->cpu = check_and_atoi(optarg);
              }

            stat_args->cmd_nr -= 2;
            break;
          case 'p':
            if (stat_args->type == STAT_ARGS_NONE)
              {
                stat_args->type = STAT_ARGS_TASK;
                stat_args->pid = check_and_atoi(optarg);
                stat_args->cpu = -1;
                stat_args->events = NULL;
              }
            else if (stat_args->type == STAT_ARGS_EVENT)
              {
                stat_args->pid = check_and_atoi(optarg);
                stat_args->cpu = -1;
              }
            else if (stat_args->type == STAT_ARGS_CPU)
              {
                printf("PID/TID switch overriding CPU\n");
                stat_args->type = STAT_ARGS_TASK;
                stat_args->pid = check_and_atoi(optarg);
              }

            stat_args->cmd_nr -= 2;
            break;
          case 'e':
            if (stat_args->type == STAT_ARGS_NONE ||
                stat_args->type == STAT_ARGS_ALL)
              {
                stat_args->cpu = -1;
                stat_args->pid = -1;
              }
            else if (stat_args->type == STAT_ARGS_CPU)
              {
                stat_args->pid = -1;
              }

            stat_args->type = STAT_ARGS_EVENT;
            stat_args->events = optarg;
            stat_args->cmd_nr -= 2;
            break;
          case 'h':
            stat_args->type = STAT_ARGS_HELP;
            break;
          case 'c':
            stat_args->sample_period = check_and_atoi(optarg);
            stat_args->cmd_nr -= 2;
            break;
          case 'm':
            stat_args->buffer_size = check_and_atoi(optarg);
            stat_args->cmd_nr -= 2;
            break;
          case 'g':
            stat_args->call_chain = true;
            stat_args->cmd_nr -= 1;
            break;
          default:
            return -EINVAL;
        }
    }

  stat_args->sec = 0;
  stat_args->cmd = argv[optind];
  stat_args->cmd_args = &argv[optind];

  if (argc == 1)
    {
      stat_args->type = STAT_ARGS_ALL;
      stat_args->cpu = -1;
      stat_args->pid = -1;
      stat_args->events = NULL;
      stat_args->cmd_nr -= 1;
    }
  else if (stat_args->cmd_nr > 0)
    {
      if (stat_args->cmd && !strcmp(stat_args->cmd, "sleep"))
        {
          stat_args->sec = PERF_DEFAULT_RUN_TIME;
          if (stat_args->type == STAT_ARGS_NONE)
            {
              stat_args->cpu = -1;
              stat_args->pid = -1;
              stat_args->events = NULL;
              stat_args->cmd_nr -= 1;
            }

          if (stat_args->cmd_nr == 2)
            {
              stat_args->sec = check_and_atoi(stat_args->cmd_args[1]);
              if (stat_args->sec < 0)
                {
                  return -EINVAL;
                }
            }
          else
            {
              printf("sleep: invalid time interval\n");
              return -EINVAL;
            }

          stat_args->cmd = NULL;
          stat_args->cmd_nr = 0;
        }
      else
        {
          if (stat_args->type == STAT_ARGS_NONE)
            {
              stat_args->type = STAT_ARGS_FORK;
              stat_args->cpu = -1;
              stat_args->pid = _SCHED_GETPID();
              stat_args->events = NULL;
              stat_args->cmd_nr -= 1;
            }
          else if (stat_args->type == STAT_ARGS_CPU ||
                   stat_args->type == STAT_ARGS_EVENT)
            {
              stat_args->type = STAT_ARGS_FORK;
              stat_args->pid = _SCHED_GETPID();
            }
          else if (stat_args->type == STAT_ARGS_TASK)
            {
              stat_args->cmd = NULL;
              stat_args->cmd_nr = 0;
            }
        }
    }

  if (stat_args->cpu >= CONFIG_SMP_NCPUS || stat_args->cpu < -1)
    {
      printf(" Perf can support %d CPUs.\n", CONFIG_SMP_NCPUS);
      return -EINVAL;
    }

  if (stat_args->pid > 0)
    {
      ret = kill(stat_args->pid, 0);
      if (ret == -1 && errno == ESRCH)
        {
          printf("The specified process does not exist\n");
          return -EINVAL;
        }
      else if (ret != 0)
        {
          printf("Error trying to determine whether the process exists\n");
        }
    }
  else if (stat_args->pid != -1)
    {
      return -EINVAL;
    }

  return 0;
}
