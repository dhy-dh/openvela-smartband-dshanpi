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
#ifndef ASH_MESSAGE_LOOP_MESSAGE_PUMP_H_
#define ASH_MESSAGE_LOOP_MESSAGE_PUMP_H_

#include <functional>
#include "ash/macros/disallow_copy.h"
#include "ash/message_loop/message_loop_listener.h"
#include "ash/time/time.h"
#ifdef ASH_OS_NUTTX
#include <uv.h>
#endif  // ASH_OS_NUTTX

namespace ash {

class MessageQueue;

class MessagePump {
 public:
  MessagePump();
  virtual ~MessagePump();

  virtual void Schedule() = 0;

  virtual void Run() = 0;

  virtual void Quit() = 0;

  using FDWatchCB = std::function<void(int)>;
  virtual void WatchFD(int fd,
                       FDWatchCB on_can_read,
                       FDWatchCB on_can_write,
                       FDWatchCB on_error) = 0;
  virtual void UnwatchFD(int fd) = 0;

  std::size_t GetTaskSize();

  void AddListener(MessageLoopListener* listener);
  void RemoveListener(MessageLoopListener* listener);

#if defined(ASH_OS_NUTTX)
  virtual uv_loop_t* GetUVLoop() = 0;
#endif  // defined(ASH_OS_NUTTX)

 protected:
  Duration Drive();
  void OnPreTask();
  void OnPostTask();

 private:
  MessageQueue* queue_;
  std::vector<MessageLoopListener*> listeners_;

  friend class MessageLoop;
  ASH_DISALLOW_COPY_AND_MOVE(MessagePump);
};

}  // namespace ash

#endif  // ASH_MESSAGE_LOOP_MESSAGE_PUMP_H_
