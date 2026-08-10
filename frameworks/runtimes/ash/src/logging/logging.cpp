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
#include "ash/logging/logging.h"
#include <cassert>
#include <iostream>
#include "ash/crash/crash.h"
#include "ash/macros/compiler_macros.h"

#if defined(ASH_OS_NUTTX)
#include <syslog.h>
#endif  // defined(ASH_OS_NUTTX)

namespace ash {

LogStream::LogStream(const char* tag,
                     LogLevel level,
                     const char* file,
                     int line,
                     const char* function)
    : tag_(tag), level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
#ifdef __NuttX__
  int level = LOG_DEBUG;
  switch (level_) {
    case LogLevel::kVERBOSE:
      level = LOG_NOTICE;
      break;
    case LogLevel::kDEBUG:
      level = LOG_DEBUG;
      break;
    case LogLevel::kINFO:
      level = LOG_INFO;
      break;
    case LogLevel::kWARNING:
      level = LOG_WARNING;
      break;
    case LogLevel::kERROR:
      level = LOG_ERR;
      break;
    case LogLevel::kFATAL:
      level = LOG_CRIT;
      break;
  };
  syslog(level, "[%s] (%s:%d %s) %s", tag_, file_, line_, function_,
         stream_.str().c_str());
#else
  std::string level_str;
  switch (level_) {
    case LogLevel::kVERBOSE:
      level_str = "VERBOSE";
      break;
    case LogLevel::kDEBUG:
      level_str = "DEBUG";
      break;
    case LogLevel::kINFO:
      level_str = "INFO";
      break;
    case LogLevel::kWARNING:
      level_str = "WARNING";
      break;
    case LogLevel::kERROR:
      level_str = "ERROR";
      break;
    case LogLevel::kFATAL:
      level_str = "FATAL";
      break;
  };
  std::cout << "[" << tag_ << "] (" << file_ << ":" << line_ << " " << function_
            << ") " << stream_.str() << std::endl;
#endif

  if (level_ == LogLevel::kFATAL)
    CrashImmediately();
}

}  // namespace ash
