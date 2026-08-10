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
#ifndef ASH_CMESSAGE_LOOP_MESSAGE_LOOP_H_
#define ASH_CMESSAGE_LOOP_MESSAGE_LOOP_H_

#include "ash/macros/compiler_macros.h"
#include "ash/task_runner/ctask_runner.h"

#if defined(ASH_OS_NUTTX)
#include <uv.h>
#endif  // (ASH_OS_NUTTX)

#if defined(ASH_OS_ANDROID)
#include <android/looper.h>
#endif  // defined(ASH_OS_ANDROID)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MessageLoop MessageLoop;
typedef struct MessageQueue MessageQueue;
typedef void (*FDWatchCB)(int);

MessageLoop* MessageLoop_Create(void);
MessageLoop* MessageLoop_CreateWithQueue(MessageQueue* queue);
void MessageLoop_Destroy(MessageLoop* loop);

#if defined(ASH_OS_NUTTX)
MessageLoop* MessageLoop_CreateForUV(uv_loop_t* uv_loop);
MessageLoop* MessageLoop_CreateForUVWithQueue(MessageQueue* queue,
                                              uv_loop_t* uv_loop);
uv_loop_t* MessageLoop_GetUVLoop(MessageLoop* loop);
#endif  // defined(ASH_OS_NUTTX)

#if defined(ASH_OS_ANDROID)
MessageLoop* CreateForAndroid(ALooper* looper);
MessageLoop* CreateForAndroidWithQueue(MessageQueue* queue, ALooper* looper);
#endif  // defined(ASH_OS_ANDROID)

void MessageLoop_Run(MessageLoop* loop);
void MessageLoop_Quit(MessageLoop* loop);

TaskRunner* MessageLoop_GetTaskRunner(MessageLoop* loop);

MessageLoop* MessageLoop_Current(void);

void MessageLoop_WatchFD(MessageLoop* loop,
                         int fd,
                         FDWatchCB on_can_read,
                         FDWatchCB on_can_write,
                         FDWatchCB on_error);
void MessageLoop_UnwatchFD(MessageLoop* loop, int fd);

#ifdef __cplusplus
}
#endif

#endif  // ASH_MESSAGE_LOOP_MESSAGE_LOOP_H_
