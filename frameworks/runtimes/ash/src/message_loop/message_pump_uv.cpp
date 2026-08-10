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
#include "ash/message_loop/message_pump_uv.h"
#include "ash/macros/compiler_macros.h"

#if defined(ASH_OS_NUTTX)

#include "ash/logging/logging.h"

namespace ash {

class MessagePumpUV::FDWatcher {
 public:
  using FDWatchCB = MessagePump::FDWatchCB;

  FDWatcher(int fd,
            FDWatchCB on_can_read,
            FDWatchCB on_can_write,
            FDWatchCB on_error,
            uv_loop_t* loop);

  ~FDWatcher();

  void Start(FDWatchCB on_can_read, FDWatchCB on_can_write, FDWatchCB on_error);

 private:
  static void UVPollCB(uv_poll_t* handle, int status, int events);
  static void HandleError(uv_poll_t* handle);
  static void HandleReadable(uv_poll_t* handle);
  static void HandleWritable(uv_poll_t* handle);
  static void HandleDisconnect(uv_poll_t* handle);

  uv_poll_t* poll_;
  int fd_;
  FDWatchCB on_can_read_;
  FDWatchCB on_can_write_;
  FDWatchCB on_error_;
};

MessagePumpUV::MessagePumpUV(uv_loop_t* uv_loop) : loop_(uv_loop) {
  if (!loop_) {
    loop_ = &own_loop_;
    ASH_CHECK_EQ(uv_loop_init(loop_), 0);
  }

  async_ = static_cast<uv_async_t*>(calloc(1, sizeof(uv_async_t)));
  ASH_CHECK_EQ(uv_async_init(loop_, async_, &MessagePumpUV::AsyncCB), 0);
  uv_handle_set_data((uv_handle_t*)async_, this);
  timer_ = static_cast<uv_timer_t*>(calloc(1, sizeof(uv_timer_t)));
  ASH_CHECK_EQ(uv_timer_init(loop_, timer_), 0);
  uv_handle_set_data((uv_handle_t*)timer_, this);
  prepare_ = static_cast<uv_prepare_t*>(calloc(1, sizeof(uv_prepare_t)));
  ASH_CHECK_EQ(uv_prepare_init(loop_, prepare_), 0);
  uv_handle_set_data((uv_handle_t*)prepare_, this);
  ASH_CHECK_EQ(uv_prepare_start(prepare_, PrepareCB), 0);
  check_ = static_cast<uv_check_t*>(calloc(1, sizeof(uv_check_t)));
  ASH_CHECK_EQ(uv_check_init(loop_, check_), 0);
  uv_handle_set_data((uv_handle_t*)check_, this);
  ASH_CHECK_EQ(uv_check_start(check_, CheckCB), 0);
}

MessagePumpUV::~MessagePumpUV() {
  // Clear watchers to free uv_polls.
  watchers_.clear();
  ASH_CHECK_EQ(uv_timer_stop(timer_), 0);
  uv_close((uv_handle_t*)timer_, [](uv_handle_t* handle) { free(handle); });
  uv_close(reinterpret_cast<uv_handle_t*>(async_),
           [](uv_handle_t* handle) { free(handle); });
  ASH_CHECK_EQ(uv_prepare_stop(prepare_), 0);
  uv_close(reinterpret_cast<uv_handle_t*>(prepare_),
           [](uv_handle_t* handle) { free(handle); });
  ASH_CHECK_EQ(uv_check_stop(check_), 0);
  uv_close(reinterpret_cast<uv_handle_t*>(check_),
           [](uv_handle_t* handle) { free(handle); });
  if (loop_ == &own_loop_) {
    ASH_CHECK_EQ(uv_loop_close(loop_), 0);
  }
}

void MessagePumpUV::Schedule() {
  uv_async_send(async_);
}

void MessagePumpUV::Run() {
  // MessagePumpUV::Run can only be called if the uv loop is owned by the
  // MessagePumpUV object.
  ASH_CHECK_EQ(loop_, &own_loop_);
  ASH_CHECK_EQ(uv_run(loop_, UV_RUN_DEFAULT), 0);
}

void MessagePumpUV::Quit() {
  // MessagePumpUV::Quit can only be called if the uv loop is owned by the
  // MessagePumpUV object.
  ASH_CHECK_EQ(loop_, &own_loop_);
  uv_stop(loop_);
}

void MessagePumpUV::WatchFD(int fd,
                            FDWatchCB on_can_read,
                            FDWatchCB on_can_write,
                            FDWatchCB on_error) {
  auto it = watchers_.find(fd);
  if (it == watchers_.end()) {
    watchers_.emplace(fd,
                      std::make_unique<FDWatcher>(fd, std::move(on_can_read),
                                                  std::move(on_can_write),
                                                  std::move(on_error), loop_));
  } else {
    it->second->Start(std::move(on_can_read), std::move(on_can_write),
                      std::move(on_error));
  }
}

void MessagePumpUV::UnwatchFD(int fd) {
  auto it = watchers_.find(fd);
  ASH_DCHECK(it != watchers_.end());
  watchers_.erase(it);
}

void MessagePumpUV::AsyncCB(uv_async_t* handle) {
  MessagePumpUV* pump =
      static_cast<MessagePumpUV*>(uv_handle_get_data((uv_handle_t*)handle));
  pump->RunCB();
}

void MessagePumpUV::TimerCB(uv_timer_t* timer) {
  MessagePumpUV* pump =
      static_cast<MessagePumpUV*>(uv_handle_get_data((uv_handle_t*)timer));
  pump->RunCB();
}

void MessagePumpUV::PrepareCB(uv_prepare_t* prepare) {
  MessagePumpUV* pump =
      static_cast<MessagePumpUV*>(uv_handle_get_data((uv_handle_t*)prepare));
  pump->OnPreTask();
}

void MessagePumpUV::CheckCB(uv_check_t* check) {
  MessagePumpUV* pump =
      static_cast<MessagePumpUV*>(uv_handle_get_data((uv_handle_t*)check));
  pump->OnPostTask();
}

void MessagePumpUV::RunCB() {
  Duration delay = Drive();
  uv_timer_stop(timer_);
  if (delay == Duration::Infinity())
    return;
  uv_timer_start(timer_, &MessagePumpUV::TimerCB, delay.ToMilli(), 0);
}

MessagePumpUV::FDWatcher::FDWatcher(int fd,
                                    FDWatchCB on_can_read,
                                    FDWatchCB on_can_write,
                                    FDWatchCB on_error,
                                    uv_loop_t* loop)
    : fd_(fd) {
  poll_ = static_cast<uv_poll_t*>(calloc(1, sizeof(uv_poll_t)));
  ASH_CHECK_EQ(uv_poll_init(loop, poll_, fd), 0);
  uv_handle_set_data((uv_handle_t*)poll_, this);
  Start(std::move(on_can_read), std::move(on_can_write), std::move(on_error));
}

MessagePumpUV::FDWatcher::~FDWatcher() {
  ASH_CHECK_EQ(uv_poll_stop(poll_), 0);
  uv_handle_set_data((uv_handle_t*)poll_, nullptr);
  uv_close(reinterpret_cast<uv_handle_t*>(poll_),
           [](uv_handle_t* handle) { free(handle); });
}

void MessagePumpUV::FDWatcher::Start(FDWatchCB on_can_read,
                                     FDWatchCB on_can_write,
                                     FDWatchCB on_error) {
  int event = 0;
  if (on_can_read)
    event |= UV_READABLE;
  if (on_can_write)
    event |= UV_WRITABLE;
  if (on_error)
    event |= UV_DISCONNECT;
  on_can_read_ = std::move(on_can_read);
  on_can_write_ = std::move(on_can_write);
  on_error_ = std::move(on_error);
  ASH_CHECK_EQ(uv_poll_start(poll_, event, &MessagePumpUV::FDWatcher::UVPollCB),
               0);
}

void MessagePumpUV::FDWatcher::HandleError(uv_poll_t* handle) {
  FDWatcher* watcher =
      reinterpret_cast<FDWatcher*>(uv_handle_get_data((uv_handle_t*)handle));

  if (watcher && watcher->on_error_) {
    int fd = watcher->fd_;
    watcher->on_error_(fd);
  }
}

void MessagePumpUV::FDWatcher::HandleReadable(uv_poll_t* handle) {
  FDWatcher* watcher =
      reinterpret_cast<FDWatcher*>(uv_handle_get_data((uv_handle_t*)handle));

  if (watcher && watcher->on_can_read_) {
    int fd = watcher->fd_;
    watcher->on_can_read_(fd);
  }
}

void MessagePumpUV::FDWatcher::HandleWritable(uv_poll_t* handle) {
  FDWatcher* watcher =
      reinterpret_cast<FDWatcher*>(uv_handle_get_data((uv_handle_t*)handle));

  if (watcher && watcher->on_can_write_) {
    int fd = watcher->fd_;
    watcher->on_can_write_(fd);
  }
}

void MessagePumpUV::FDWatcher::HandleDisconnect(uv_poll_t* handle) {
  FDWatcher* watcher =
      reinterpret_cast<FDWatcher*>(uv_handle_get_data((uv_handle_t*)handle));

  if (watcher && watcher->on_error_) {
    int fd = watcher->fd_;
    watcher->on_error_(fd);
  }
}

void MessagePumpUV::FDWatcher::UVPollCB(uv_poll_t* handle,
                                        int status,
                                        int events) {
  if (status != 0) {
    HandleError(handle);
    return;
  }

  if (events & UV_READABLE) {
    HandleReadable(handle);
  }
  if (events & UV_WRITABLE) {
    HandleWritable(handle);
  }
  if (events & UV_DISCONNECT) {
    HandleDisconnect(handle);
  }
}

uv_loop_t* MessagePumpUV::GetUVLoop() {
  return loop_;
}

}  // namespace ash

#endif  // defined(ASH_OS_NUTTX)
