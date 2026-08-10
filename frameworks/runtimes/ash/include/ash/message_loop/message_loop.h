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
#ifndef ASH_MESSAGE_LOOP_MESSAGE_LOOP_H_
#define ASH_MESSAGE_LOOP_MESSAGE_LOOP_H_

#include <memory>
#include "ash/macros/compiler_macros.h"
#include "ash/macros/disallow_copy.h"
#include "ash/message_loop/message_loop_listener.h"
#include "ash/message_loop/message_pump.h"
#include "ash/message_loop/message_queue.h"

#if defined(ASH_OS_NUTTX)
#include <uv.h>
#endif  // (ASH_OS_NUTTX)

#if defined(ASH_OS_ANDROID)
#include <android/looper.h>
#endif  // defined(ASH_OS_ANDROID)

namespace ash {

class MessageLoop {
 public:
  MessageLoop(std::unique_ptr<MessagePump> pump,
              std::shared_ptr<MessageQueue> queue);
  ~MessageLoop();

  std::shared_ptr<TaskRunner> GetTaskRunner();
  void Run();
  void Quit();

  static MessageLoop* Current();

  using FDWatchCB = MessagePump::FDWatchCB;
  void WatchFD(int fd,
               FDWatchCB on_can_read,
               FDWatchCB on_can_write,
               FDWatchCB on_error);
  void UnwatchFD(int fd);

  void AddListener(MessageLoopListener* listener);
  void RemoveListener(MessageLoopListener* listener);

  static std::unique_ptr<MessageLoop> Create();
  static std::unique_ptr<MessageLoop> CreateWithQueue(
      std::shared_ptr<MessageQueue> queue);

#if defined(ASH_OS_NUTTX)
  static std::unique_ptr<MessageLoop> CreateForUV(uv_loop_t* uv_loop = nullptr);
  static std::unique_ptr<MessageLoop> CreateForUVWithQueue(
      std::shared_ptr<MessageQueue> queue,
      uv_loop_t* uv_loop = nullptr);
#endif  // defined(ASH_OS_NUTTX)

#if defined(ASH_OS_ANDROID)
  static std::unique_ptr<MessageLoop> CreateForAndroid(
      ALooper* looper = nullptr);
  static std::unique_ptr<MessageLoop> CreateForAndroidWithQueue(
      std::shared_ptr<MessageQueue> queue,
      ALooper* looper = nullptr);
#endif  // defined(ASH_OS_ANDROID)

#if defined(ASH_OS_NUTTX)
  uv_loop_t* GetUVLoop();
#endif  // defined(ASH_OS_NUTTX)

 private:
  std::unique_ptr<MessagePump> pump_;
  std::shared_ptr<MessageQueue> queue_;
  std::shared_ptr<TaskRunner> task_runner_;
  ASH_DISALLOW_COPY_AND_MOVE(MessageLoop);
};

}  // namespace ash

#endif  // ASH_MESSAGE_LOOP_MESSAGE_LOOP_H_
