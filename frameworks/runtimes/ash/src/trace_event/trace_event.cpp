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
#include "ash/trace_event/trace_event.h"
#include "ash/macros/compiler_macros.h"

#if defined(ASH_OS_NUTTX)
#include <nuttx/sched_note.h>
#endif  // defined(ASH_OS_NUTTX)

namespace ash {

void TraceEvent::Begin(const char* name) {
#if defined(ASH_OS_NUTTX) && defined(CONFIG_SYSTEM_TRACE)
  sched_note_beginex(NOTE_TAG_ALWAYS, name);
#endif  // defined(ASH_OS_NUTTX) && defined(CONFIG_SYSTEM_TRACE)
}

void TraceEvent::End(const char* name) {
#if defined(ASH_OS_NUTTX) && defined(CONFIG_SYSTEM_TRACE)
  sched_note_endex(NOTE_TAG_ALWAYS, name);
#endif  // defined(ASH_OS_NUTTX) && defined(CONFIG_SYSTEM_TRACE)
}

ScopedTraceEvent::ScopedTraceEvent(const char* name) : name_(name) {
  TraceEvent::Begin(name_);
}

ScopedTraceEvent::~ScopedTraceEvent() {
  TraceEvent::End(name_);
}

}  // namespace ash
