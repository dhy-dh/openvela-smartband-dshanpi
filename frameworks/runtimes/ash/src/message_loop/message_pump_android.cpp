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
#include "ash/message_loop/message_pump_android.h"

#if defined(ASH_OS_ANDROID)

#include <fcntl.h>
#include <unistd.h>
#include "ash/logging.h"

namespace ash {

MessagePumpAndroid::MessagePumpAndroid(ALooper* looper) : looper_(looper) {
  if (looper_) {
    ALooper_acquire(looper_);
  } else {
    looper_ = ALooper_forThread();
  }

  if (!looper_) {
    ASH_LOG("ASH", ERROR) << "No valid looper.";
    return;
  }

  int fds[2];
  if (pipe(fds) < 0) {
    ASH_LOG("ASH", ERROR) << "Can't create pipe.";
    return;
  }

  in_ = ScopedFD::adopt(fds[0]);
  out_ = ScopedFD::adopt(fds[1]);

  int flags = 0;
  flags = fcntl(fds[0], F_GETFL);
  if (flags < 0) {
    ASH_LOG("ASH", ERROR) << "fcntl call failed.";
    return;
  }

  if (fcntl(fds[0], F_SETFL, flags | O_NONBLOCK) < 0) {
    ASH_LOG("ASH", ERROR) << "fcntl call failed.";
    return;
  }

  flags = fcntl(fds[1], F_GETFL);
  if (flags < 0) {
    ASH_LOG("ASH", ERROR) << "fcntl call failed.";
    return;
  }

  if (fcntl(fds[1], F_SETFL, flags | O_NONBLOCK) < 0) {
    ASH_LOG("ASH", ERROR) << "fcntl call failed.";
    return;
  }

  ALooper_addFd(looper_, in_, ALOOPER_POLL_CALLBACK, ALOOPER_EVENT_INPUT,
                &MessagePumpAndroid::Callback, this);
}

MessagePumpAndroid::~MessagePumpAndroid() {
  ALooper_removeFd(looper_, in_);
  ALooper_release(looper_);
}

void MessagePumpAndroid::Schedule() {
  char c = 0;
  write(out_, &c, 1);
}

void MessagePumpAndroid::Run() {}

void MessagePumpAndroid::Quit() {}

void MessagePumpAndroid::WatchFD(int fd,
                                 FDWatchCB on_can_read,
                                 FDWatchCB on_can_write,
                                 FDWatchCB on_error) {
  // TODO(xuyan): 添加实现
}

void MessagePumpAndroid::UnwatchFD(int fd) {
  // TODO(xuyan): 添加实现
}

int MessagePumpAndroid::Callback(int fd, int events, void* data) {
  MessagePumpAndroid* pump = static_cast<MessagePumpAndroid*>(data);

  char buf[64];
  while (read(pump->in_, buf, 64) > 0)
    ;

  // TODO(xuyan): 添加处理任务逻辑

  return 1;
}

}  // namespace ash

#endif  // defined(ASH_OS_ANDROID)
