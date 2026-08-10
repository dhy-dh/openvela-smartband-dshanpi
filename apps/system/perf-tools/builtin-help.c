/****************************************************************************
 * apps/system/perf-tools/builtin-help.c
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>

#include "builtin.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct cmd_help_s
{
  const char name[16];
  const char help[80];
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct cmd_help_s g_cmd_help[] =
{
  {"list", "List all symbolic event types"},
  {"record", "Run a command and record its profile into perf.data"},
  {"stat", "Run a command and gather performance counter statistics"},
  {"version", "Display the version of perf binary"},
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void cmd_list_help(void)
{
  size_t i = 0;
  size_t longest = 0;

  printf("\n usage: %s\n\n", perf_usage_string);

  for (i = 0; i < nitems(g_cmd_help); i++)
    {
      if (longest < strlen(g_cmd_help[i].name))
        {
          longest = strlen(g_cmd_help[i].name);
        }
    }

  printf(" The most commonly used perf commands are:\n");

  for (i = 0; i < nitems(g_cmd_help); i++)
    {
      printf("   %-*s   %s\n", (int)longest, g_cmd_help[i].name,
                               g_cmd_help[i].help);
    }

  printf("\n");
}

int cmd_help(int argc, FAR const char **argv)
{
  cmd_list_help();
  return 0;
}
