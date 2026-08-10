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
#ifndef ASH_MESSAGE_LOOP_MESSAGE_PUMP_ANDROID_H_
#define ASH_MESSAGE_LOOP_MESSAGE_PUMP_ANDROID_H_

#if defined(ASH_OS_ANDROID)

#include <android/looper.h>
#include "ash/message_loop/message_pump.h"
#include "ash/scoped_fd.h"

namespace ash {

class MessagePumpAndroid : public MessagePump {
 public:
  MessagePumpAndroid(ALooper* looper);
  ~MessagePumpAndroid();

  void Schedule() override;

  void Run() override;
  void Quit() override;

  void WatchFD(int fd,
               FDWatchCB on_can_read,
               FDWatchCB on_can_write,
               FDWatchCB on_error) override;

  void UnwatchFD(int fd) override;

 private:
  static int Callback(int fd, int events, void* data);

  ALooper* looper_;
  ScopedFD in_;
  ScopedFD out_;
};

}  // namespace ash

#endif  // defined(ASH_OS_ANDROID)

#endif  // ASH_MESSAGE_LOOP_MESSAGE_PUMP_ANDROID_H_
