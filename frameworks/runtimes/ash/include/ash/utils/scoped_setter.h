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
#ifndef ASH_UTILS_SCOPED_SETTER_H_
#define ASH_UTILS_SCOPED_SETTER_H_

#include "ash/macros/disallow_copy.h"

namespace ash {

template <typename T>
class ScopedSetter {
 public:
  ScopedSetter(T* ptr, T value) : ptr_(ptr), old_value_(*ptr) { *ptr_ = value; }

  ~ScopedSetter() { *ptr_ = old_value_; }

 private:
  T* ptr_;
  T old_value_;
  ASH_DISALLOW_COPY_AND_MOVE(ScopedSetter);
};

}  // namespace ash

#endif  // ASH_UTILS_SCOPED_SETTER_H_
