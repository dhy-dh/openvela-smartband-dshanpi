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
#ifndef ASH_MEMORY_RAW_PTR_H_
#define ASH_MEMORY_RAW_PTR_H_

#include <cstddef>
#include <utility>

namespace ash {

template <typename T>
class RawPtr {
 public:
  constexpr RawPtr() : ptr_(nullptr) {}
  explicit RawPtr(T* ptr) : ptr_(ptr) {}
  RawPtr(std::nullptr_t) : ptr_(nullptr) {}
  RawPtr(const RawPtr& other) = default;

  template <typename U>
  RawPtr(U* ptr) : ptr_(ptr) {}

  template <typename U>
  RawPtr(const RawPtr<U>& other) : ptr_(other.Get()) {}

  ~RawPtr() = default;

  RawPtr& operator=(const RawPtr& other) = default;

  RawPtr& operator=(std::nullptr_t) {
    Reset();
    return *this;
  }

  template <typename U>
  RawPtr& operator=(U* ptr) {
    ptr_ = ptr;
    return *this;
  }

  template <typename U>
  RawPtr& operator=(const RawPtr<U>& other) {
    ptr_ = other.Get();
    return *this;
  }

  T& operator*() { return *ptr_; }
  const T& operator*() const { return *ptr_; }

  T* operator->() { return ptr_; }
  const T* operator->() const { return ptr_; }

  operator bool() const { return ptr_ != nullptr; }

  bool operator==(const RawPtr& other) const { return ptr_ == other.ptr_; }

  template <typename U>
  bool operator==(const RawPtr<U>& other) const {
    return ptr_ == other.Get();
  }

  bool operator==(std::nullptr_t) const { return ptr_ == nullptr; }

  bool operator==(const T* ptr) const { return ptr_ == ptr; }

  bool operator!=(const RawPtr& other) const { return !(*this == other); }

  template <typename U>
  bool operator!=(const RawPtr<U>& other) const {
    return !(*this == other);
  }

  bool operator!=(std::nullptr_t) const { return ptr_ != nullptr; }

  bool operator!=(const T* ptr) const { return ptr_ != ptr; }

  bool operator<(const RawPtr& other) const { return ptr_ < other.ptr_; }

  template <typename U>
  bool operator<(const RawPtr<U>& other) const {
    return ptr_ < other.Get();
  }

  bool operator<(const T* ptr) const { return ptr_ < ptr; }

  bool operator>(const RawPtr& other) const { return other < *this; }

  template <typename U>
  bool operator>(const RawPtr<U>& other) const {
    return other < *this;
  }

  bool operator>(const T* ptr) const { return ptr < ptr_; }

  bool operator<=(const RawPtr& other) const { return !(other < *this); }

  template <typename U>
  bool operator<=(const RawPtr<U>& other) const {
    return !(other < *this);
  }

  bool operator<=(const T* ptr) const { return !(ptr < ptr_); }

  bool operator>=(const RawPtr& other) const { return !(*this < other); }

  template <typename U>
  bool operator>=(const RawPtr<U>& other) const {
    return !(*this < other);
  }

  bool operator>=(const T* ptr) const { return !(ptr_ < ptr); }

  T* Get() const { return ptr_; }

  void Reset() { ptr_ = nullptr; }
  void Reset(T* ptr) { ptr_ = ptr; }

 private:
  T* ptr_;
};

template <typename T>
bool operator==(std::nullptr_t, const RawPtr<T>& rhs) {
  return rhs.Get() == nullptr;
}

template <typename T>
bool operator!=(std::nullptr_t, const RawPtr<T>& rhs) {
  return rhs.Get() != nullptr;
}

template <typename T>
bool operator==(const T* lhs, const RawPtr<T>& rhs) {
  return lhs == rhs.Get();
}

template <typename T>
bool operator!=(const T* lhs, const RawPtr<T>& rhs) {
  return lhs != rhs.Get();
}

template <typename T>
bool operator<(const T* lhs, const RawPtr<T>& rhs) {
  return lhs < rhs.Get();
}

template <typename T>
bool operator>(const T* lhs, const RawPtr<T>& rhs) {
  return rhs < lhs;
}

template <typename T>
bool operator<=(const T* lhs, const RawPtr<T>& rhs) {
  return !(lhs > rhs);
}

template <typename T>
bool operator>=(const T* lhs, const RawPtr<T>& rhs) {
  return !(lhs < rhs);
}

}  // namespace ash

#endif  // ASH_MEMORY_RAW_PTR_H_
