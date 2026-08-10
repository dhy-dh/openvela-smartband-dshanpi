/****************************************************************************
 * apps/system/perf-tools/evsel.c
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

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <nuttx/nuttx.h>

#include "evsel.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define C(x)            PERF_COUNT_HW_CACHE_##x
#define CACHE_READ      (1 << C(OP_READ))
#define CACHE_WRITE     (1 << C(OP_WRITE))
#define CACHE_PREFETCH  (1 << C(OP_PREFETCH))
#define COP(x)          (1 << x)

/****************************************************************************
 * Private data
 ****************************************************************************/

/****************************************************************************
 * cache operation stat
 * L1I : Read and prefetch only
 * ITLB and BPU : Read-only
 ****************************************************************************/

const unsigned long evsel_hw_cache_stat[C(MAX)] =
{
  [C(L1D)]  = (CACHE_READ | CACHE_WRITE | CACHE_PREFETCH),
  [C(L1I)]  = (CACHE_READ | CACHE_PREFETCH),
  [C(LL)]   = (CACHE_READ | CACHE_WRITE | CACHE_PREFETCH),
  [C(DTLB)] = (CACHE_READ | CACHE_WRITE | CACHE_PREFETCH),
  [C(ITLB)] = (CACHE_READ),
  [C(BPU)]  = (CACHE_READ),
  [C(NODE)] = (CACHE_READ | CACHE_WRITE | CACHE_PREFETCH),
};

FAR const char *evsel_hw_cache[PERF_COUNT_HW_CACHE_MAX] =
{
  [PERF_COUNT_HW_CACHE_L1D]  = "L1-dcache",
  [PERF_COUNT_HW_CACHE_L1I]  = "L1-icache",
  [PERF_COUNT_HW_CACHE_LL]   = "LLC",
  [PERF_COUNT_HW_CACHE_DTLB] = "dTLB",
  [PERF_COUNT_HW_CACHE_ITLB] = "iTLB",
  [PERF_COUNT_HW_CACHE_BPU]  = "branch",
  [PERF_COUNT_HW_CACHE_NODE] = "node",
};

FAR const char *evsel_hw_cache_op[PERF_COUNT_HW_CACHE_OP_MAX] =
{
  [PERF_COUNT_HW_CACHE_OP_READ]     = "load",
  [PERF_COUNT_HW_CACHE_OP_WRITE]    = "store",
  [PERF_COUNT_HW_CACHE_OP_PREFETCH] = "prefetch",
};

const char *evsel_hw_cache_result[PERF_COUNT_HW_CACHE_RESULT_MAX] =
{
  [PERF_COUNT_HW_CACHE_RESULT_ACCESS] = "refs",
  [PERF_COUNT_HW_CACHE_RESULT_MISS]   = "misses",
};

/****************************************************************************
 * Public data
 ****************************************************************************/

FAR const char *const evsel_hw_names[PERF_COUNT_HW_MAX] =
{
  "cycles",
  "instructions",
  "cache-references",
  "cache-misses",
  "branches",
  "branch-misses",
  "bus-cycles",
  "stalled-cycles-frontend",
  "stalled-cycles-backend",
  "ref-cycles",
};

int default_hw_config[DEFAULT_HW_CONFIG_NUM] =
{
  PERF_COUNT_HW_CPU_CYCLES,
  PERF_COUNT_HW_INSTRUCTIONS,
  PERF_COUNT_HW_BRANCH_INSTRUCTIONS,
  PERF_COUNT_HW_BRANCH_MISSES,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void perf_evsel_init(FAR struct perf_evsel_s *evsel,
                            FAR struct perf_event_attr_s *attr)
{
  list_initialize(&evsel->node);
  evsel->attr = *attr;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int parse_aliases(FAR const char *str,
                  FAR const char *const names[],
                  int size, FAR int *longest)
{
  int n;
  int i;

  *longest = -1;
  for (i = 0; i < size; i++)
    {
      n = strlen(names[i]);
      if (n > *longest && !strncasecmp(str, names[i], n))
        {
          *longest = n;
        }

      if (*longest > 0)
        {
          return i;
        }
    }

  return -1;
}

int parse_hw_cache_events(FAR const char *name, FAR uint64_t *config)
{
  int len;
  int cache_type = -1;
  int cache_op = -1;
  int cache_result = -1;
  const char *str = name;
  const char *name_end = &name[strlen(name) + 1];

  cache_type = parse_aliases(str, evsel_hw_cache,
                             PERF_COUNT_HW_CACHE_MAX, &len);
  if (cache_type == -1)
    {
      return -EINVAL;
    }

  str += len + 1;
  if (str < name_end)
    {
      cache_op = parse_aliases(str, evsel_hw_cache_op,
                               PERF_COUNT_HW_CACHE_OP_MAX, &len);
      if (cache_op >= 0)
        {
          if (!evsel_is_cache_op_valid(cache_type, cache_op))
            {
              return -EINVAL;
            }

          str += len + 1;
        }
      else
        {
          return -EINVAL;
        }
    }

  if (str < name_end)
    {
      cache_result = parse_aliases(str, evsel_hw_cache_result,
                                   PERF_COUNT_HW_CACHE_RESULT_MAX, &len);
      if (cache_result < 0)
        {
          return -EINVAL;
        }
    }

  if (cache_op == -1)
    {
      cache_op = PERF_COUNT_HW_CACHE_OP_READ;
    }

  if (cache_result == -1)
    {
      cache_result = PERF_COUNT_HW_CACHE_RESULT_ACCESS;
    }

  if (config)
    {
      *config = cache_type | (cache_op << 8) | (cache_result << 16);
    }

  return 0;
}

int evsel_hw_cache_type_op_res_name(uint8_t type, uint8_t op, uint8_t result,
                                    FAR char *bf, size_t size)
{
  if (result)
    {
      return snprintf(bf, size, "%s-%s-%s", evsel_hw_cache[type],
                       evsel_hw_cache_op[op],
                       evsel_hw_cache_result[result]);
    }

  return snprintf(bf, size, "%s-%s", evsel_hw_cache[type],
                       evsel_hw_cache_op[op]);
}

bool evsel_is_cache_op_valid(uint8_t type, uint8_t op)
{
  if (evsel_hw_cache_stat[type] & COP(op))
    {
      return true;
    }
  else
    {
      return false;
    }
}

int evsel_hw_cache_name(FAR struct evsel_s *evsel, FAR char *buf,
                              size_t size)
{
  uint64_t config = evsel->core.attr.config;
  uint8_t op;
  uint8_t result;
  uint8_t type = (config >>  0) & 0xff;
  const char *err = "unknown-ext-hardware-cache-type";

  if (type >= PERF_COUNT_HW_CACHE_MAX)
    {
      goto out_err;
    }

  op = (config >>  8) & 0xff;
  err = "unknown-ext-hardware-cache-op";
  if (op >= PERF_COUNT_HW_CACHE_OP_MAX)
    {
      goto out_err;
    }

  result = (config >> 16) & 0xff;
  err = "unknown-ext-hardware-cache-result";
  if (result >= PERF_COUNT_HW_CACHE_RESULT_MAX)
    {
      goto out_err;
    }

  err = "invalid-cache";
  if (!evsel_is_cache_op_valid(type, op))
    {
      goto out_err;
    }

  return evsel_hw_cache_type_op_res_name(type, op, result, buf, size);

out_err:
  return snprintf(buf, size, "%s", err);
}

int evsel_raw_name(FAR struct evsel_s *evsel, FAR char *buf,
                              size_t size)
{
  return snprintf(buf, size, "r%" PRIx64, evsel->core.attr.config);
}

int evsel_hw_name(FAR struct evsel_s *evsel, FAR char *buf,
                              size_t size)
{
  uint64_t config = evsel->core.attr.config;

  if (config < PERF_COUNT_HW_MAX && evsel_hw_names[config])
    {
      return snprintf(buf, size, "%s", evsel_hw_names[config]);
    }
  else
    {
      return snprintf(buf, size, "%s", "unknown-hardware");
    }
}

FAR const char *evsel_name(FAR struct evsel_s *evsel)
{
  char buf[128];

  if (!evsel)
    {
      goto out_unknown;
    }

  switch (evsel->core.attr.type)
    {
      case PERF_TYPE_HARDWARE:
        evsel_hw_name(evsel, buf, sizeof(buf));
        break;
      case PERF_TYPE_HW_CACHE:
        evsel_hw_cache_name(evsel, buf, sizeof(buf));
        break;
      case PERF_TYPE_RAW:
        evsel_raw_name(evsel, buf, sizeof(buf));
        break;
      default:
        snprintf(buf, sizeof(buf), "unknown attr type: %d",
                  evsel->core.attr.type);
        break;
    }

  evsel->name = strdup(buf);
  if (evsel->name)
    {
      return evsel->name;
    }

out_unknown:
  return "unknown";
}

void evsel_init(FAR struct evsel_s *evsel,
                FAR struct perf_event_attr_s *attr)
{
  perf_evsel_init(&evsel->core, attr);
  evsel->evlist = NULL;
}

FAR struct evsel_s *evsel_new(FAR struct perf_event_attr_s *attr)
{
  FAR struct evsel_s *evsel = zalloc(sizeof(struct evsel_s));

  if (!evsel)
    {
      return NULL;
    }

  evsel_init(evsel, attr);

  return evsel;
}

void evsel_delete(FAR struct evsel_s *evsel)
{
  if (!evsel)
    {
      return;
    }

  free(evsel);
}

int evsel_read_counter(FAR struct evsel_s *evsel,
                       FAR struct evlist_s *evlist)
{
  uint64_t count;

  if (read(evsel->core.evfd, &count, sizeof(count)) < 0)
    {
      return -EINVAL;
    }

  evsel->core.count = count;

  return 0;
}

int evsel_open(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  evsel->core.evfd = perf_event_open(&evsel->core.attr, evsel->core.pid,
                                     evsel->core.cpu, -1, O_CLOEXEC);
  if (evsel->core.evfd < 0)
    {
      return -ENOENT;
    }

  return 0;
}

int evsel_close(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  if (evsel->core.evfd < 0)
    {
      return -EINVAL;
    }

  close(evsel->core.evfd);

  return 0;
}

int evsel_reset(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  if (evsel->core.evfd < 0)
    {
      return -EINVAL;
    }

  ioctl(evsel->core.evfd, PERF_EVENT_IOC_RESET, 0);

  return 0;
}

int evsel_enable(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  if (evsel->core.evfd < 0)
    {
      return -EINVAL;
    }

  ioctl(evsel->core.evfd, PERF_EVENT_IOC_ENABLE, 0);

  return 0;
}

int evsel_disable(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  if (evsel->core.evfd < 0)
    {
      return -EINVAL;
    }

  ioctl(evsel->core.evfd, PERF_EVENT_IOC_DISABLE, 0);

  return 0;
}

int evsel_count_start(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  int status;

  status = evsel_open(evsel, evlist);
  if (status)
    {
      return status;
    }

  evsel_reset(evsel, evlist);
  evsel_enable(evsel, evlist);

  return status;
}

int evsel_count_end(FAR struct evsel_s *evsel, FAR struct evlist_s *evlist)
{
  evsel_disable(evsel, evlist);
  evsel_close(evsel, evlist);

  return 0;
}
