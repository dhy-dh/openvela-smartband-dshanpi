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
#include "ash/message_loop/message_pump_impl.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#define TAG "MessagePumpImpl"

namespace ash {

MessagePumpImpl::MessagePumpImpl() : running_(true) {
  epoll_ = epoll_create(16);
  ASH_CHECK(epoll_ >= 0);
  ASH_CHECK(pipe(pipefd_) == 0);

  fcntl(pipefd_[0], F_SETFL, O_NONBLOCK);

  struct epoll_event event {};
  event.events = EPOLLIN | EPOLLET;
  event.data.fd = pipefd_[0];
  epoll_ctl(epoll_, EPOLL_CTL_ADD, pipefd_[0], &event);
}

MessagePumpImpl::~MessagePumpImpl() {
  close(pipefd_[0]);
  close(pipefd_[1]);
  close(epoll_);
}

void MessagePumpImpl::Schedule() {
  uint8_t expirations = 0;
  write(pipefd_[1], &expirations, sizeof(expirations));
}

void MessagePumpImpl::HandleReadable(int fd) {
  auto it = fd_cbs_.find(fd);
  if (it != fd_cbs_.end() && it->second.on_can_read_) {
    it->second.on_can_read_(fd);
  }
}

void MessagePumpImpl::HandleWritable(int fd) {
  auto it = fd_cbs_.find(fd);
  if (it != fd_cbs_.end() && it->second.on_can_write_) {
    it->second.on_can_write_(fd);
  }
}

void MessagePumpImpl::HandleError(int fd) {
  auto it = fd_cbs_.find(fd);
  if (it != fd_cbs_.end() && it->second.on_error_) {
    it->second.on_error_(fd);
  }
}

void MessagePumpImpl::Run() {
  while (running_) {
    Duration delay = Duration::Infinity();
    if (GetTaskSize() > 0) {
      OnPreTask();
      delay = Drive();
      OnPostTask();
    }

    if (!running_)
      break;

    int64_t milli = std::max(delay.ToMilli(), int64_t(0));
    if (milli > int64_t(std::numeric_limits<int>::max()))
      milli = -1;

    struct epoll_event events[16];
    int nfd = epoll_wait(epoll_, events, 16, (int)milli);
    if (nfd < 0)
      continue;
    for (int i = 0; i < nfd; ++i) {
      if (events[i].data.fd == pipefd_[0]) {
        uint8_t expirations[16];
        while (true) {
          int r = read(pipefd_[0], expirations, sizeof(expirations));
          if (r == -1 && errno == EAGAIN)
            break;
          if (r == -1) {
            running_ = false;
            break;
          }
        }
      } else {
        int fd = events[i].data.fd;

        if (events[i].events & EPOLLIN) {
          HandleReadable(fd);
        }
        if (events[i].events & EPOLLOUT) {
          HandleWritable(fd);
        }
        if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP) {
          HandleError(fd);
        }
        continue;
      }
    }
  }
}

void MessagePumpImpl::Quit() {
  running_ = false;
}

void MessagePumpImpl::WatchFD(int fd,
                              FDWatchCB on_can_read,
                              FDWatchCB on_can_write,
                              FDWatchCB on_error) {
  ASH_CHECK_NE(on_can_read || on_can_write || on_error, false)
      << "fd: " << fd << ", all cbs are null!";
  struct epoll_event event {};
  if (on_can_read)
    event.events |= EPOLLIN;
  if (on_can_write)
    event.events |= EPOLLOUT;
  if (on_error)
    event.events |= EPOLLERR;
  event.data.fd = fd;

  int mod = fd_cbs_.find(fd) == fd_cbs_.end() ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
  int ret = epoll_ctl(epoll_, mod, fd, &event);
  ASH_CHECK_GE(ret, 0) << "WatchFD " << fd << " err: " << strerror(errno);
  fd_cbs_.insert_or_assign(
      fd, FDWatchCBs{std::move(on_can_read), std::move(on_can_write),
                     std::move(on_error)});
  return;
}

void MessagePumpImpl::UnwatchFD(int fd) {
  int ret = epoll_ctl(epoll_, EPOLL_CTL_DEL, fd, nullptr);
  ASH_CHECK_GE(ret, 0) << "UnwatchFD " << fd << " err: " << strerror(errno);
  fd_cbs_.erase(fd);
  return;
}

#ifdef ASH_OS_NUTTX
uv_loop_t* MessagePumpImpl::GetUVLoop() {
  return nullptr;  // Not implemented
}
#endif  // ASH_OS_NUTTX

}  // namespace ash
