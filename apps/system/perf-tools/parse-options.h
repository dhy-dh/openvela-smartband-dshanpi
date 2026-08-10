/****************************************************************************
 * apps/system/perf-tools/parse-options.h
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

#ifndef __APPS_SYSTEM_PERF_TOOLS_PARSE_OPTIONS_H
#define __APPS_SYSTEM_PERF_TOOLS_PARSE_OPTIONS_H

/****************************************************************************
 * Public Definitions
 ****************************************************************************/

enum parse_opt_type_e
{
  OPTION_END,
  OPTION_ARGUMENT,
  OPTION_STRING,
};

struct option_s
{
  enum parse_opt_type_e type;
  int short_name;
  FAR const char *long_name;
  FAR const char *help;
};

enum args_type_e
{
  STAT_ARGS_NONE,
  STAT_ARGS_ALL,
  STAT_ARGS_CPU,
  STAT_ARGS_EVENT,
  STAT_ARGS_TASK,
  STAT_ARGS_FORK,
  STAT_ARGS_HELP,
};

struct stat_args_s
{
  enum args_type_e type;
  int cpu;
  int pid;
  FAR char *events;
  FAR char *cmd;
  FAR char **cmd_args;
  int cmd_nr;
  int sec;
  bool call_chain;
  uint32_t buffer_size;
  uint64_t sample_period;
};

#define OPT_ARGUMENT(l, h) \
  { .type = OPTION_ARGUMENT, .long_name = (l), .help = (h) }
#define OPT_STRING(s, l, h) \
  { .type = OPTION_STRING, .short_name = (s), .long_name = (l), .help = (h) }

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

int parse_stat_options(int argc, FAR char **argv,
                       FAR struct stat_args_s *stat_args);

#endif /* __APPS_SYSTEM_PERF_TOOLS_PARSE_OPTIONS_H */
