/****************************************************************************
 * apps/system/perf-tools/builtin-list.c
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

#include "evsel.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const char list_usage[] = "perf list [hw|sw|cache]";

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void default_print_start(void)
{
  printf("List of pre-defined events (to be used in -e):\n\n");
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

bool is_event_supported(uint8_t type, uint64_t config)
{
  bool ret = false;
  struct evsel_s *evsel;
  struct perf_event_attr_s attr =
    {
      .type = type,
      .config = config,
      .disabled = 1,
    };

  evsel = evsel_new(&attr);
  if (evsel)
    {
      ret = evsel_open(evsel, NULL) >= 0;
      evsel_delete(evsel);
    }

  return ret;
}

void print_hwcache_events(void)
{
  int type;
  int op;
  int res;
  int ret;
  uint64_t config;

  for (type = 0; type < PERF_COUNT_HW_CACHE_MAX; type++)
    {
      for (op = 0; op < PERF_COUNT_HW_CACHE_OP_MAX; op++)
        {
          if (!evsel_is_cache_op_valid(type, op))
            {
              continue;
            }

          for (res = 0; res < PERF_COUNT_HW_CACHE_RESULT_MAX; res++)
            {
              char name[64];
              evsel_hw_cache_type_op_res_name(type, op, res,
                                              name, sizeof(name));
              ret = parse_hw_cache_events(name, &config);
              if (ret || !is_event_supported(PERF_TYPE_HW_CACHE, config))
                {
                  continue;
                }

              printf("  %-32s\t [Hardware cache event]\n", name);
            }
        }
    }

  printf("\n");
}

void print_hw_events(void)
{
  int i;

  for (i = 0; i < PERF_COUNT_HW_MAX; i++)
    {
      if (!is_event_supported(PERF_TYPE_HARDWARE, i))
        {
          continue;
        }

      printf("  %-32s\t [Hardware event]\n", evsel_hw_names[i]);
    }

  printf("\n");
}

int cmd_list(int argc, FAR const char **argv)
{
  char *type = NULL;

  if (argc > 1)
    {
      type = (char *)argv[1];
    }
  else
    {
      default_print_start();
      print_hw_events();
      print_hwcache_events();
      return 0;
    }

  if (strcmp(type, "hw") == 0 ||
      strcmp(type, "hardware") == 0)
    {
      default_print_start();
      print_hw_events();
    }
  else if (strcmp(type, "cache") == 0)
    {
      default_print_start();
      print_hwcache_events();
    }
  else
    {
      printf("\n Usage: %s\n\n", list_usage);
    }

  return 0;
}
