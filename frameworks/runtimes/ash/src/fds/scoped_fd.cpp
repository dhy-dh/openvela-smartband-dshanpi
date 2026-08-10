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
#include "ash/fds/scoped_fd.h"
#include <unistd.h>
#include "ash/logging/logging.h"

namespace ash {

ScopedFD::ScopedFD() : fd_(-1) {}

ScopedFD::ScopedFD(std::nullptr_t) : fd_(-1) {}

ScopedFD::ScopedFD(int fd) : fd_(fd >= 0 ? dup(fd) : -1) {}

ScopedFD::ScopedFD(const ScopedFD& other)
    : fd_(other.fd_ >= 0 ? dup(other.fd_) : -1) {}

ScopedFD::ScopedFD(ScopedFD&& other) : fd_(other.fd_) {
  other.fd_ = -1;
}

ScopedFD::~ScopedFD() {
  Reset();
}

ScopedFD& ScopedFD::operator=(std::nullptr_t) {
  Reset();
  return *this;
}

ScopedFD& ScopedFD::operator=(int fd) {
  if (fd_ != fd) {
    Reset();
    fd_ = fd >= 0 ? dup(fd) : -1;
  }
  return *this;
}

ScopedFD& ScopedFD::operator=(const ScopedFD& other) {
  if (this != &other) {
    Reset();
    fd_ = other.fd_ >= 0 ? dup(other.fd_) : -1;
  }
  return *this;
}

ScopedFD& ScopedFD::operator=(ScopedFD&& other) {
  if (this != &other) {
    Reset();
    fd_ = other.fd_;
    other.fd_ = -1;
  }
  return *this;
}

bool ScopedFD::operator==(std::nullptr_t) const {
  return !IsValid();
}

bool ScopedFD::operator!=(std::nullptr_t) const {
  return IsValid();
}

ScopedFD ScopedFD::Adopt(int fd) {
  return ScopedFD(fd, 0);
}

bool ScopedFD::IsValid() const {
  return fd_ >= 0;
}

// TODO(xuyan): 考虑限制类型转换运算符的使用
ScopedFD::operator int() const {
  return fd_;
}

int ScopedFD::Release() {
  int fd = fd_;
  fd_ = -1;
  return fd;
}

ScopedFD::ScopedFD(int fd, int) : fd_(fd) {}

void ScopedFD::Reset() {
  if (fd_ >= 0) {
    int r = close(fd_);
    ASH_DCHECK_EQ(r, 0);
  }
  fd_ = -1;
}

}  // namespace ash
