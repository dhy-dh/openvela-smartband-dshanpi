/*
 * Copyright (C) 2025 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SHELL_PROFILE_H
#define SHELL_PROFILE_H
#include "quickjs/quickjs.h"

#include "framework_log.h" // defined in feature lib

#if defined(CONFIG_QUICKAPP_QUALITY_PROFILER)
/* clang-format off */
#define PROFILE_LOG_TIMESTAMPE(name, dsc)           QuickProfileLogTimeStamp(QUICK_PROFILE_QUICKAPP_FRAMEWORK, name, dsc)
#define PROFILE_LOG_BEGIN(name, dsc)                QuickProfileLogBegin(QUICK_PROFILE_QUICKAPP_FRAMEWORK, name, dsc)
#define PROFILE_LOG_END(name, dsc)                  QuickProfileLogEnd(QUICK_PROFILE_QUICKAPP_FRAMEWORK, name, dsc)
#define PROFILE_LOG_MEM(name, size, count, dsc)     QuickProfileLogMemory(QUICK_PROFILE_QUICKAPP_FRAMEWORK, name, size, count, dsc)
#define PROFILE_LOG_ASYNC_BEGIN(name, dsc)          QuickProfileLogAsyncBegin(QUICK_PROFILE_QUICKAPP_FRAMEWORK, name, dsc)
#define PROFILE_LOG_ASYNC_END(name, dsc)            QuickProfileLogAsyncEnd(QUICK_PROFILE_QUICKAPP_FRAMEWORK, name, dsc)
#define PROFILE_LOG_FLUSH()                         QuickProfileLogFlush()
/* clang-format on */
#else
#define PROFILE_LOG_TIMESTAMPE(name, dsc)
#define PROFILE_LOG_BEGIN(name, dsc)
#define PROFILE_LOG_END(name, dsc)
#define PROFILE_LOG_MEM(name, size, count, dsc)
#define PROFILE_LOG_ASYNC_BEGIN(name, dsc)
#define PROFILE_LOG_ASYNC_END(name, dsc)
#define PROFILE_LOG_FLUSH()
#endif

#endif // AITOJS_PROFILE_H