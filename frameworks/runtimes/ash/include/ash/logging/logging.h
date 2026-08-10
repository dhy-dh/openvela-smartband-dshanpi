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
#ifndef ASH_LOGGING_H_
#define ASH_LOGGING_H_

#include <sstream>
#include "ash/macros/disallow_copy.h"

#define ASH_LOG_STREAM(tag, level)                                     \
  ::ash::LogStream(tag, ::ash::LogLevel::k##level, __FILE__, __LINE__, \
                   __FUNCTION__)                                       \
      .stream()

#define ASH_LAZY_STREAM(tag, level, condition)                          \
  !(condition) ? (void)0                                                \
               : ::ash::LogStreamVoidify() &                            \
                     ::ash::LogStream(tag, ::ash::LogLevel::k##level,   \
                                      __FILE__, __LINE__, __FUNCTION__) \
                         .stream()

#define ASH_LOG_IF(tag, level, condition) ASH_LAZY_STREAM(tag, level, condition)

#define ASH_LOG(tag, level) ASH_LOG_IF(tag, level, true)

#define ASH_CHECK(condition) \
  ASH_LOG_IF("DEBUG", FATAL, !(condition)) << "check failed: " #condition " "
#define ASH_CHECK_EQ(val1, val2) ASH_CHECK((val1) == (val2))
#define ASH_CHECK_NE(val1, val2) ASH_CHECK((val1) != (val2))
#define ASH_CHECK_LT(val1, val2) ASH_CHECK((val1) < (val2))
#define ASH_CHECK_LE(val1, val2) ASH_CHECK((val1) <= (val2))
#define ASH_CHECK_GT(val1, val2) ASH_CHECK((val1) > (val2))
#define ASH_CHECK_GE(val1, val2) ASH_CHECK((val1) >= (val2))
#define ASH_UNREACHED() ASH_LOG(FATAL) << "UNREACHED"

#if defined(NDEBUG)
#define ASH_DLOG_IF(tag, level, condition) ASH_LOG_IF(tag, level, condition)
#define ASH_DLOG(tag, level) ASH_LOG(tag, level)
#define ASH_DCHECK(condition) ASH_CHECK(condition)
#define ASH_DCHECK_EQ(val1, val2) ASH_CHECK_EQ(val1, val2)
#define ASH_DCHECK_NE(val1, val2) ASH_CHECK_NE(val1, val2)
#define ASH_DCHECK_LT(val1, val2) ASH_CHECK_LT(val1, val2)
#define ASH_DCHECK_LE(val1, val2) ASH_CHECK_LE(val1, val2)
#define ASH_DCHECK_GT(val1, val2) ASH_CHECK_GT(val1, val2)
#define ASH_DCHECK_GE(val1, val2) ASH_CHECK_GE(val1, val2)
#else
#define ASH_DLOG_IF(tag, level, condition) \
  ASH_LOG_IF(tag, level, false && (condition))
#define ASH_DLOG(tag, level) ASH_LOG_IF(tag, level, false)
#define ASH_DCHECK(condition) ASH_DLOG_IF("DEBUG", FATAL, !(condition))
#define ASH_DCHECK_EQ(val1, val2) ASH_DCHECK(val1 == val2)
#define ASH_DCHECK_NE(val1, val2) ASH_DCHECK(val1 != val2)
#define ASH_DCHECK_LT(val1, val2) ASH_DCHECK(val1 < val2)
#define ASH_DCHECK_LE(val1, val2) ASH_DCHECK(val1 <= val2)
#define ASH_DCHECK_GT(val1, val2) ASH_DCHECK(val1 > val2)
#define ASH_DCHECK_GE(val1, val2) ASH_DCHECK(val1 >= val2)
#endif

namespace ash {

enum class LogLevel {
  kVERBOSE,
  kDEBUG,
  kINFO,
  kWARNING,
  kERROR,
  kFATAL,
};

class LogStream {
 public:
  LogStream(const char* tag,
            LogLevel level,
            const char* file,
            int line,
            const char* function);
  ~LogStream();

  std::ostream& stream() { return stream_; }

 private:
  const char* tag_;
  LogLevel level_;
  const char* file_;
  int line_;
  const char* function_;
  std::ostringstream stream_;
  ASH_DISALLOW_COPY_AND_MOVE(LogStream);
};

class LogStreamVoidify {
 public:
  LogStreamVoidify() = default;
  ~LogStreamVoidify() = default;
  void operator&(std::ostream&) {}
  ASH_DISALLOW_COPY_AND_MOVE(LogStreamVoidify);
};

}  // namespace ash

#endif  // ASH_LOGGING_H_
