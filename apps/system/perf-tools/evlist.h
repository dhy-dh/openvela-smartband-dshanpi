/****************************************************************************
 * apps/system/perf-tools/evlist.h
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

#ifndef __APPS_SYSTEM_PERF_TOOLS_EVLIST_H
#define __APPS_SYSTEM_PERF_TOOLS_EVLIST_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <time.h>
#include <nuttx/list.h>
#include <nuttx/perf.h>

#include "builtin.h"
#include "evsel.h"

/****************************************************************************
 * Public Definitions
 ****************************************************************************/

struct perf_evlist_s
{
  struct list_node entries;
  int nr_entries;
};

struct evlist_s
{
  struct perf_evlist_s core;
  int sec;
  int cpu;
  int pid;
  int type;
  bool system_wide;
  bool defult_attrs;
  FAR struct perf_event_attr_s *attrs;
  uint64_t sample_period;
};

struct evlist_count_s
{
  uint64_t count;
  FAR const char *name;
};

#define evlist_for_each_evsel(evlist, node, func, status) \
  do { \
    FAR struct perf_evsel_s *psel = NULL; \
    FAR struct evsel_s *evsel = NULL; \
    FAR struct list_node *node; \
    FAR struct list_node *list = &(evlist->core.entries); \
    for(node = (list)->next; node != (list); node = node->next) \
      { \
        psel = list_container_of(node, struct perf_evsel_s, node), \
        evsel = list_container_of(psel, struct evsel_s, core); \
        status = func(evsel, evlist); \
      } \
    } while(0)

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

int evlist_delete(FAR struct evlist_s *evlist);
FAR struct evlist_s *evlist_new(void);
int evlist_add_attrs(FAR struct evlist_s *evlist,
                     FAR struct perf_event_attr_s *attrs,
                     size_t nr_attrs, int cpu);
int evlist_print_counters(FAR struct evlist_s *evlist);

#endif /* __APPS_SYSTEM_PERF_TOOLS_EVLIST_H */
