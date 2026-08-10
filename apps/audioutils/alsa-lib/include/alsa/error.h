/****************************************************************************
 * apps/audioutils/alsa-lib/include/alsa/error.h
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

#ifndef __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ALSA_ERROR_H
#define __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ALSA_ERROR_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <syslog.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define snd_log(level, fmt, args...)                                         \
  syslog(level, "[SND][%s:%d] " fmt, __func__, __LINE__, ##args)

#define snd_log_none(fmt, args...)                                           \
  do                                                                         \
    {                                                                        \
      if (0)                                                                 \
        {                                                                    \
          syslog(LOG_ERR, fmt, ##args);                                      \
        }                                                                    \
    }                                                                        \
  while (0)

#if defined(CONFIG_AUDIOUTILS_ALSA_LIB_LOG_DEBUG)
#define SNDDEBUG(fmt, args...)           snd_log(LOG_DEBUG, fmt, ##args)
#define SNDINFO(fmt, args...)            snd_log(LOG_INFO, fmt, ##args)
#define SNDWARN(fmt, args...)            snd_log(LOG_WARNING, fmt, ##args)
#define SNDERR(fmt, args...)             snd_log(LOG_ERR, fmt, ##args)
#elif defined(CONFIG_AUDIOUTILS_ALSA_LIB_LOG_INFO)
#define SNDDEBUG(fmt, args...)           snd_log_none(fmt, ##args)
#define SNDINFO(fmt, args...)            snd_log(LOG_INFO, fmt, ##args)
#define SNDWARN(fmt, args...)            snd_log(LOG_WARNING, fmt, ##args)
#define SNDERR(fmt, args...)             snd_log(LOG_ERR, fmt, ##args)
#elif defined(CONFIG_AUDIOUTILS_ALSA_LIB_LOG_WARN)
#define SNDDEBUG(fmt, args...)           snd_log_none(fmt, ##args)
#define SNDINFO(fmt, args...)            snd_log_none(fmt, ##args)
#define SNDWARN(fmt, args...)            snd_log(LOG_WARNING, fmt, ##args)
#define SNDERR(fmt, args...)             snd_log(LOG_ERR, fmt, ##args)
#elif defined(CONFIG_AUDIOUTILS_ALSA_LIB_LOG_ERR)
#define SNDDEBUG(fmt, args...)           snd_log_none(fmt, ##args)
#define SNDINFO(fmt, args...)            snd_log_none(fmt, ##args)
#define SNDWARN(fmt, args...)            snd_log_none(fmt, ##args)
#define SNDERR(fmt, args...)             snd_log(LOG_ERR, fmt, ##args)
#else
#define SNDDEBUG(fmt, args...)           snd_log_none(fmt, ##args)
#define SNDINFO(fmt, args...)            snd_log_none(fmt, ##args)
#define SNDWARN(fmt, args...)            snd_log_none(fmt, ##args)
#define SNDERR(fmt, args...)             snd_log_none(fmt, ##args)
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

FAR const char *snd_strerror(int errnum);

#ifdef __cplusplus
}
#endif

#endif /* __APPS_AUDIOUTILS_ALSA_LIB_INCLUDE_ALSA_ERROR_H */