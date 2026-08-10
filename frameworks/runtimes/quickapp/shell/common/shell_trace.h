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

#ifndef _QUICKAPP_SHELL_TRACE_H
#define _QUICKAPP_SHELL_TRACE_H

#ifdef __NuttX__
#include <nuttx/sched_note.h>

#define SHELL_NOTE_PRINTF(format, ...) \
    sched_note_printf(NOTE_TAG_ALWAYS, format, ##__VA_ARGS__)
#define SHELL_NOTE_BEGIN() \
    sched_note_begin(NOTE_TAG_ALWAYS)
#define SHELL_NOTE_END() \
    sched_note_end(NOTE_TAG_ALWAYS)
#define SHELL_NOTE_BEGIN_STR(str) \
    sched_note_beginex(NOTE_TAG_ALWAYS, str)
#define SHELL_NOTE_END_STR(str) \
    sched_note_endex(NOTE_TAG_ALWAYS, str)
#define SHELL_NOTE_MARK(str) \
    sched_note_mark(NOTE_TAG_ALWAYS, str)
#else
#define SHELL_NOTE_PRINTF(format, ...)
#define SHELL_NOTE_BEGIN()
#define SHELL_NOTE_END()
#define SHELL_NOTE_BEGIN_STR(str)
#define SHELL_NOTE_END_STR(str)
#define SHELL_NOTE_MARK(str)
#endif /* __Nuttx__ */

#endif // _QUICKAPP_SHELL_TRACE_H