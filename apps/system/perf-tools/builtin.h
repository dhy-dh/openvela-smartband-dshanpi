/****************************************************************************
 * apps/system/perf-tools/builtin.h
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

#ifndef __APPS_SYSTEM_PERF_TOOLS_BUILTIN_H
#define __APPS_SYSTEM_PERF_TOOLS_BUILTIN_H

/****************************************************************************
 * Public Definitions
 ****************************************************************************/

#define PERF_DEFAULT_RUN_TIME    5
#define PERF_SAVE_FILE_NAME "perf.data"

/****************************************************************************
 * Public Data
 ****************************************************************************/

extern const char perf_usage_string[];

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void cmd_list_help(void);
int cmd_help(int argc, FAR const char **argv);
int cmd_list(int argc, FAR const char **argv);
int cmd_record(int argc, FAR const char **argv);
int cmd_stat(int argc, FAR const char **argv);
int cmd_version(int argc, FAR const char **argv);
int cmd_write(int argc, FAR const char **argv);

#endif /* __APPS_SYSTEM_PERF_TOOLS_BUILTIN_H */
