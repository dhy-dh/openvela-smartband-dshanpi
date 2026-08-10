/****************************************************************************
 * apps/system/perf-tools/perf.c
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
#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>

#include <nuttx/sched.h>

#include "builtin.h"
#include "parse-options.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct cmd_struct_s
{
  FAR const char *cmd;
  FAR int (*fn)(int, FAR const char **);
  int option;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct cmd_struct_s commands[] =
{
  { "help",    cmd_help,    0 },
  { "list",    cmd_list,    0 },
  { "record",  cmd_record,  0 },
  { "stat",    cmd_stat,    0 },
  { "version", cmd_version, 0 },
  { "write",   cmd_write,   0 },
};

static struct option_s options[] =
{
  OPT_ARGUMENT("help", "help"),
  OPT_ARGUMENT("version", "version"),
  OPT_ARGUMENT("list-cmds", "list-cmds"),
  OPT_ARGUMENT("list-opts", "list-opts"),
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

const char perf_usage_string[] =
  "perf [--version] [--help] [OPTIONS] COMMAND [ARGS]";

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static inline bool strstarts(FAR const char *str, FAR const char *prefix)
{
  return strncmp(str, prefix, strlen(prefix)) == 0;
}

static int run_argv(int argc, FAR const char **argv)
{
  FAR const char *cmd = argv[0];
  unsigned int i;

  /* Turn "perf cmd --help" into "perf help cmd" */

  if (argc > 1 && !strcmp(argv[1], "--help"))
    {
      argv[1] = argv[0];
      argv[0] = cmd = "help";
    }

  for (i = 0; i < nitems(commands); i++)
    {
      FAR struct cmd_struct_s *p = commands + i;
      if (p->fn == NULL)
        {
          continue;
        }

      if (strcmp(p->cmd, cmd))
        {
          continue;
        }

      exit(p->fn(argc, argv));
    }

  printf("perf: '%s' is not a perf-command. See 'perf --help'.\n", cmd);
  exit(-EINVAL);
}

static int handle_options(FAR const char ***argv, FAR int *argc)
{
  int handled = 0;

  while (*argc > 0)
    {
      const char *cmd = (*argv)[0];
      if (cmd[0] != '-')
        {
          break;
        }

      if (!strcmp(cmd, "--help") || !strcmp(cmd, "--version"))
        {
          break;
        }

      if (!strcmp(cmd, "-h"))
        {
          (*argv)[0] = "--help";
          break;
        }

      if (!strcmp(cmd, "-v"))
        {
          (*argv)[0] = "--version";
          break;
        }

      if (!strcmp(cmd, "--list-cmds"))
        {
          unsigned int i;

          for (i = 0; i < nitems(commands); i++)
            {
              FAR struct cmd_struct_s *p = commands + i;
              printf("%s ", p->cmd);
            }

          printf("\n");
          exit(0);
        }
      else if (!strcmp(cmd, "--list-opts"))
        {
          unsigned int i;

          for (i = 0; i < nitems(options); i++)
            {
              FAR struct option_s *p = options + i;
              printf("--%s ", p->long_name);
            }

          printf("\n");
          exit(0);
        }
      else
        {
          fprintf(stderr, "Unknown option: %s\n", cmd);
          fprintf(stderr, "\n Usage: %s\n", perf_usage_string);
          exit(-EINVAL);
        }

      (*argv)++;
      (*argc)--;
      handled++;
    }

  return handled;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, FAR const char **argv)
{
  argv++;
  argc--;
  handle_options(&argv, &argc);

  if (argc == 0)
    {
      cmd_list_help();
      return 0;
    }

  if (strstarts(argv[0], "--"))
    {
      argv[0] += 2;
    }

  while (1)
    {
      run_argv(argc, argv);
    }

  return 0;
}
