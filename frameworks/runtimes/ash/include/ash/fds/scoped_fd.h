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
#ifndef ASH_FDS_SCOPED_FD_H_
#define ASH_FDS_SCOPED_FD_H_

#include <cstddef>

namespace ash {

class ScopedFD {
 public:
  ScopedFD();

  ScopedFD(std::nullptr_t);

  ScopedFD(int fd);

  ScopedFD(const ScopedFD& other);

  // TODO(xuyan): 考虑noexcept
  ScopedFD(ScopedFD&& other);

  ~ScopedFD();

  ScopedFD& operator=(std::nullptr_t);

  ScopedFD& operator=(int fd);

  ScopedFD& operator=(const ScopedFD& other);

  ScopedFD& operator=(ScopedFD&& other);

  // TODO(xuyan): 考虑nullptr == ScopedFD的使用方式
  bool operator==(std::nullptr_t) const;

  bool operator!=(std::nullptr_t) const;

  static ScopedFD Adopt(int fd);

  bool IsValid() const;

  operator int() const;

  int Release();

 private:
  ScopedFD(int fd, int);
  void Reset();

  int fd_;
};

}  // namespace ash

#endif  // ASH_FDS_SCOPED_FD_H_
