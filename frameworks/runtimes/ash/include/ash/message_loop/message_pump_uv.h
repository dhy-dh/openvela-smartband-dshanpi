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
#ifndef ASH_MESSAGE_LOOP_MESSAGE_PUMP_UV_H_
#define ASH_MESSAGE_LOOP_MESSAGE_PUMP_UV_H_

#include "ash/macros/compiler_macros.h"

#if defined(ASH_OS_NUTTX)

#include <uv.h>
#include <map>
#include "ash/message_loop/message_pump.h"

namespace ash {

class MessagePumpUV : public MessagePump {
 public:
  MessagePumpUV(uv_loop_t* uv_loop);
  ~MessagePumpUV() override;

  void Schedule() override;

  void Run() override;
  void Quit() override;

  void WatchFD(int fd,
               FDWatchCB on_can_read,
               FDWatchCB on_can_write,
               FDWatchCB on_error) override;

  void UnwatchFD(int fd) override;

  uv_loop_t* GetUVLoop() override;

 private:
  class FDWatcher;

  static void AsyncCB(uv_async_t* async);
  static void TimerCB(uv_timer_t* timer);
  static void PrepareCB(uv_prepare_t* prepare);
  static void CheckCB(uv_check_t* check);

  void RunCB();

  uv_loop_t* loop_;
  uv_loop_t own_loop_;
  uv_async_t* async_;
  uv_timer_t* timer_;
  uv_prepare_t* prepare_;
  uv_check_t* check_;

  std::map<int, std::unique_ptr<FDWatcher>> watchers_;
};

}  // namespace ash

#endif  // defined(ASH_OS_NUTTX)

#endif  // ASH_MESSAGE_LOOP_MESSAGE_PUMP_UV_H_
