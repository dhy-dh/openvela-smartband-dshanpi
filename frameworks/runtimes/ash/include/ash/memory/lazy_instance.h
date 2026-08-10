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
#ifndef ASH_MEMORY_LAZY_INSTANCE_H_
#define ASH_MEMORY_LAZY_INSTANCE_H_

#include <atomic>
#include "ash/macros/disallow_copy.h"

namespace ash {

namespace lazy_instance_internal {
inline std::atomic<uintptr_t>& asAtomic(uintptr_t& a) {
  return reinterpret_cast<std::atomic<uintptr_t>&>(a);
}
}  // namespace lazy_instance_internal

/**
 * LazyInstance is a thread-safe class to declare a lazy initialized variable.
 *
 * Example:
 *   LazyInstance<std::string> str = LAZY_INSTANCE_INITIALIZER;
 *   ...
 *   str.Get() = "hello";  // Set
 *   const std::string& a = str.Get();  // Get
 *   const std::string* b = str.Pointer();  // Get pointer
 *
 * The constructor of T will be called when the first time Get() or Pointer()
 * is called.
 */
template <typename T>
class LazyInstance {
 public:
  LazyInstance() = default;

  ~LazyInstance() {
    uintptr_t ptr = lazy_instance_internal::asAtomic(ptr_).load();
    if (ptr == kInitial)
      return;
    while (ptr == kCreating)
      ptr = lazy_instance_internal::asAtomic(ptr_).load();
    reinterpret_cast<T*>(ptr)->~T();
  }

  T* Pointer() {
    uintptr_t p = lazy_instance_internal::asAtomic(ptr_).load();
    if (p > kCreating)
      return reinterpret_cast<T*>(p);

    while (p <= kCreating) {
      if (p == kCreating) {
        p = lazy_instance_internal::asAtomic(ptr_).load();
        continue;
      }

      // p == kInitial
      if (lazy_instance_internal::asAtomic(ptr_).compare_exchange_weak(
              p, kCreating)) {
        p = reinterpret_cast<uintptr_t>(new (storage_) T);
        lazy_instance_internal::asAtomic(ptr_).store(p);
      }
    }

    return reinterpret_cast<T*>(p);
  }

  T& Get() { return *Pointer(); }

 private:
  static constexpr uintptr_t kInitial = 0;
  static constexpr uintptr_t kCreating = 1;
  uintptr_t ptr_;
  alignas(T) char storage_[sizeof(T)];
  ASH_DISALLOW_COPY_AND_MOVE(LazyInstance);
};

}  // namespace ash

#define ASH_LAZY_INSTANCE_INITIALIZER \
  {}

#endif  // ASH_MEMORY_LAZY_INSTANCE_H_
