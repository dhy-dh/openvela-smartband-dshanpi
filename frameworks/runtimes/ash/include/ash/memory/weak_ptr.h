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
#ifndef ASH_MEMORY_WEAK_PTR_H_
#define ASH_MEMORY_WEAK_PTR_H_

#include <memory>
#include "ash/memory/lifecycle_watcher.h"

namespace ash {

template <typename T>
class WeakPtrImpl : public LifecycleWatcher {
 public:
  WeakPtrImpl(T* ptr) : ptr_(ptr) {}

  ~WeakPtrImpl() override = default;

  bool IsAlive() override { return !!ptr_; }

  void Reset() { ptr_ = nullptr; }

  T* Get() { return ptr_; }

  T* operator->() { return ptr_; }

 private:
  T* ptr_;
};

template <typename T>
class WeakPtr {
 public:
  WeakPtr() = default;

  WeakPtr(const WeakPtr&) = default;

  WeakPtr(std::shared_ptr<WeakPtrImpl<T>> ptr) : ptr_(std::move(ptr)) {}

  ~WeakPtr() = default;

  bool IsAlive() const { return ptr_ != nullptr && ptr_->IsAlive(); }

  T* Get() const { return ptr_ != nullptr ? ptr_->Get() : nullptr; }

 private:
  std::shared_ptr<WeakPtrImpl<T>> ptr_;
};

template <typename T>
class SupportsWeakPtr {
 public:
  SupportsWeakPtr(T* owner) : owner_(std::make_shared<WeakPtrImpl<T>>(owner)) {}

  WeakPtr<T> AsWeakPtr() { return WeakPtr<T>(owner_); }

  void Reset() { owner_->Reset(); }

 protected:
  ~SupportsWeakPtr() { owner_->Reset(); }

 private:
  std::shared_ptr<WeakPtrImpl<T>> owner_;
};

}  // namespace ash

#endif  // ASH_MEMORY_WEAK_PTR_H_
