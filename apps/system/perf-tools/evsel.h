/****************************************************************************
 * apps/system/perf-tools/evsel.h
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

#ifndef __APPS_SYSTEM_PERF_TOOLS_EVSEL_H
#define __APPS_SYSTEM_PERF_TOOLS_EVSEL_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/list.h>
#include <nuttx/perf.h>

#include "evlist.h"

/****************************************************************************
 * Public Definitions
 ****************************************************************************/

struct perf_evsel_s
{
  struct list_node node;
  struct perf_event_attr_s attr;
  int evfd;
  int cpu;
  int pid;
  uint64_t count;
  FAR uint64_t *id;
  uint32_t ids;
  uint32_t id_offset;
  int fd_array[CONFIG_SMP_NCPUS];
};

struct evsel_s
{
  struct perf_evsel_s core;
  FAR struct evlist_s *evlist;
  FAR char *name;
};

#define DEFAULT_HW_CONFIG_NUM 4

/****************************************************************************
 * Public Data
 ****************************************************************************/

FAR extern const char *const evsel_hw_names[PERF_COUNT_HW_MAX];
extern int default_hw_config[DEFAULT_HW_CONFIG_NUM];
FAR extern const char *evsel_hw_cache[PERF_COUNT_HW_CACHE_MAX];
FAR extern const char *evsel_hw_cache_op[PERF_COUNT_HW_CACHE_OP_MAX];
FAR extern const char *evsel_hw_cache_result[PERF_COUNT_HW_CACHE_RESULT_MAX];

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

int parse_hw_cache_events(FAR const char *name, FAR uint64_t *config);
int evsel_hw_cache_type_op_res_name(uint8_t type, uint8_t op, uint8_t result,
                                    FAR char *bf, size_t size);
bool evsel_is_cache_op_valid(uint8_t type, uint8_t op);
int evsel_hw_cache_name(FAR struct evsel_s *evsel, FAR char *buf,
                              size_t size);
FAR const char *evsel_name(FAR struct evsel_s *evsel);
FAR struct evsel_s *evsel_new(FAR struct perf_event_attr_s *attr);
void evsel_delete(FAR struct evsel_s *evsel);
int evsel_read_counter(FAR struct evsel_s *evsel,
                       FAR struct evlist_s *evlist);
int evsel_open(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);
int evsel_close(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);
int evsel_reset(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);
int evsel_enable(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);
int evsel_disable(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);
int evsel_count_start(FAR struct evsel_s *evsel,
                      FAR struct evlist_s *evlist);
int evsel_count_end(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist);

#endif /* __APPS_SYSTEM_PERF_TOOLS_EVSEL_H */
