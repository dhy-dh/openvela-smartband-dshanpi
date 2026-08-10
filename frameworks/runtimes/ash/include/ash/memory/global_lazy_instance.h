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
#ifndef ASH_MEMORY_GLOBAL_LAZY_H_
#define ASH_MEMORY_GLOBAL_LAZY_H_
#include "ash/memory/global_variable.h"
#include "ash/memory/lazy_instance.h"
#include "global_variable.h"
#include "lazy_instance.h"

namespace ash {
template <typename T>
class GlobalLazyInstance {
 public:
  GlobalLazyInstance() = default;
  ~GlobalLazyInstance() = default;

  T& Get() { return global_lazy_variable_.Get().Get(); }

 private:
  GlobalVariable<LazyInstance<T>> global_lazy_variable_;
  ASH_DISALLOW_COPY_AND_MOVE(GlobalLazyInstance);
};
}  // namespace ash
#endif  // ASH_MEMORY_GLOBAL_LAZY_H_
