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
#ifndef ASH_TRACE_EVENT_TRACE_EVENT_H_
#define ASH_TRACE_EVENT_TRACE_EVENT_H_

namespace ash {

class TraceEvent {
 public:
  static void Begin(const char* name);
  static void End(const char* name);
};

class ScopedTraceEvent {
 public:
  ScopedTraceEvent(const char* name);
  ~ScopedTraceEvent();

 private:
  const char* name_;
};

#define __MAKE_SCOPED_TRACE_EVENT_NAME(x, y) x##y

#define MAKE_SCOPED_TRACE_EVENT_NAME(x, y) __MAKE_SCOPED_TRACE_EVENT_NAME(x, y)

#define SCOPED_TRACE_EVENT(name)                                     \
  ScopedTraceEvent MAKE_SCOPED_TRACE_EVENT_NAME(scoped_trace_event_, \
                                                __COUNTER__)(name)

}  // namespace ash

#endif  // ASH_TRACE_EVENT_TRACE_EVENT_H_
