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

/**
 * @file thread_local.h
 *
 * Provides classes to support thread scoped variables both on nuttx simulator
 * other platforms.
 *
 * On nuttx simulator, the thread_local keyword is not implemented well, so we
 * supply a alternative implementation. This is a workaround, once the bug
 * about thread_local is fixed, we can remove this workaround.
 *
 * THREAD_LOCAL macro is used to declare a thread local variable.
 * Example:
 *   THREAD_LOCAL(int) i = 0;
 *   ...
 *   i.Get() = 1;  // Set
 *   int var = i.Get();  // Get
 *
 * Use THREAD_LOCAL_WITH_INITIALIZER macro to declare a thread local variable
 * with an initializer which will be called on each thread.
 *
 * Example:
 *   THREAD_LOCAL_WITH_INITIALIZER(int) i = []() { return 0; };
 *
 * THREAD_LOCAL/THREAD_LOCAL_WITH_INITIALIZER can be used with variables which
 * can be used with thread_local keyword, including global variables, static
 * local variables and static member variables. They can't be used with local
 * variables and member variables.
 */
#ifndef ASH_MEMORY_THREAD_LOCAL_H_
#define ASH_MEMORY_THREAD_LOCAL_H_

#include "ash/macros/compiler_macros.h"
#include "ash/macros/disallow_copy.h"
#include "ash/memory/variable_segment.h"

namespace ash {

#if defined(ASH_OS_NUTTX)
#define ASH_THREAD_LOCAL_USE_VARIABLE_SEGMENT
#endif  // defined(ASH_OS_NUTTX)

#if defined(ASH_THREAD_LOCAL_USE_VARIABLE_SEGMENT)

VariableSegmentDefination* GetThreadLocalSegmentDefination();
uint8_t* GetThreadLocalSegment();

template <typename T>
class ThreadLocal {
 public:
  template <typename... Args>
  ThreadLocal(Args&&... args)
      : offset_(GetThreadLocalSegmentDefination()->RegisterVariableWithArgs<T>(
            std::forward<Args>(args)...)) {}
  ~ThreadLocal() = default;

  T& Get() { return *reinterpret_cast<T*>(GetThreadLocalSegment() + offset_); }

 private:
  int offset_;
  ASH_DISALLOW_COPY_AND_MOVE(ThreadLocal);
};

template <typename T>
class ThreadLocalWithInitializer {
 public:
  template <typename F>
  ThreadLocalWithInitializer(F initializer)
      : offset_(GetThreadLocalSegmentDefination()->RegisterVariableWithArgs<T>(
            std::move(initializer))) {}
  ~ThreadLocalWithInitializer() = default;

  T& Get() { return *reinterpret_cast<T*>(GetThreadLocalSegment() + offset_); }

 private:
  int offset_;
  ASH_DISALLOW_COPY_AND_MOVE(ThreadLocalWithInitializer);
};

#define THREAD_LOCAL(type) ThreadLocal<type>
#define THREAD_LOCAL_WITH_INITIALIZER(type) ThreadLocalWithInitializer<type>

#else  // defined(ASH_OS_NUTTX) && defined(CONFIG_ARCH_SIM)

template <typename T>
class ThreadLocal {
 public:
  template <typename... Args>
  ThreadLocal(Args&&... args) : value_(std::forward<Args>(args)...) {}
  ~ThreadLocal() = default;

  T& Get() { return value_; }

 private:
  T value_;
  ASH_DISALLOW_COPY_AND_MOVE(ThreadLocal);
};

template <typename T>
class ThreadLocalWithInitializer {
 public:
  template <typename F>
  ThreadLocalWithInitializer(F initializer) : value_(initializer()) {}
  ~ThreadLocalWithInitializer() = default;

 private:
  T value_;
  ASH_DISALLOW_COPY_AND_MOVE(ThreadLocalWithInitializer);
};

#define THREAD_LOCAL(type) thread_local ThreadLocal<type>
#define THREAD_LOCAL_WITH_INITIALIZER(type) \
  thread_local ThreadLocalWithInitializer<type>

#endif  // defined(ASH_THREAD_LOCAL_USE_VARIABLE_SEGMENT)

}  // namespace ash

#endif  // ASH_MEMORY_THREAD_LOCAL_H_
