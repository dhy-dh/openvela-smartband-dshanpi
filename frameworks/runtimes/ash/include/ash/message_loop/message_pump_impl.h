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
#ifndef ASH_MESSAGE_LOOP_MESSAGE_PUMP_IMPL_H_
#define ASH_MESSAGE_LOOP_MESSAGE_PUMP_IMPL_H_

#include <sys/epoll.h>
#include <map>
#include "ash/logging/logging.h"
#include "ash/message_loop/message_pump.h"
#include "ash/time/time.h"

namespace ash {

class MessagePumpImpl : public MessagePump {
 public:
  MessagePumpImpl();
  virtual ~MessagePumpImpl();

  void Schedule() override;

  void Run() override;

  void Quit() override;

  void WatchFD(int fd,
               FDWatchCB on_can_read,
               FDWatchCB on_can_write,
               FDWatchCB on_error) override;

  void UnwatchFD(int fd) override;

  struct FDWatchCBs {
    FDWatchCB on_can_read_;
    FDWatchCB on_can_write_;
    FDWatchCB on_error_;
  };

#ifdef ASH_OS_NUTTX
  uv_loop_t* GetUVLoop() override;
#endif  // ASH_OS_NUTTX

 private:
  void HandleReadable(int fd);
  void HandleWritable(int fd);
  void HandleError(int fd);

  bool running_;
  int epoll_;
  int pipefd_[2];
  std::map<int, FDWatchCBs> fd_cbs_;
};

}  // namespace ash

#endif  // ASH_MESSAGE_LOOP_MESSAGE_PUMP_IMPL_H_
