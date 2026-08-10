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
#ifndef ASH_MEMORY_VARIABLE_SEGMENT_H_
#define ASH_MEMORY_VARIABLE_SEGMENT_H_

#include <functional>
#include "ash/macros/disallow_copy.h"

namespace ash {

/**
 * @class VariableSegmentDefination
 *
 * VariableSegmentDefination is a class that manages the memory of variables,
 * it is used by GlobalVariable and ThreadLocalVariable classes.
 *
 * VariableSegmentDefination records information , including the offset, the
 * onstrucor and the destructor of variables. Use RegisterVariable to register
 * variables.
 * CreateSegment method is used to allocate an memory to store all variables,
 * and invoke constructors for them.
 * DestroySegment method is used to invoke destructors for all variables and
 * free the memory.
 *
 * Don't use this class directly, due to the fact that the class is very
 * dangerous. RegisterVariable must be called before every CreateSegment
 * invocation, or it will cause crash.
 */
class VariableSegmentDefination {
 public:
  VariableSegmentDefination();
  ~VariableSegmentDefination();

  int RegisterVariable(size_t size,
                       std::function<void(void*)> constructor,
                       std::function<void(void*)> destructor);

  template <typename T, typename... Args>
  int RegisterVariableWithArgs(Args... args) {
    auto constructor = std::bind(
        [](void* addr, Args... extra_args) { new (addr) T(extra_args...); },
        std::placeholders::_1, std::move(args)...);
    auto destructor = [](void* addr) { reinterpret_cast<T*>(addr)->~T(); };
    return RegisterVariable(sizeof(T), std::move(constructor),
                            std::move(destructor));
  }

  template <typename T>
  int RegisterVariableWithInitializer(std::function<T()> initializer) {
    auto constructor = [initializer = std::move(initializer)](void* addr) {
      new (addr) T(initializer());
    };
    auto destructor = [](void* addr) { reinterpret_cast<T*>(addr)->~T(); };
    return RegisterVariable(sizeof(T), std::move(constructor),
                            std::move(destructor));
  }

  uint8_t* CreateSegment();
  void DestroySegment(uint8_t* segment);

 private:
  struct VariableDefination {
    int offset;
    std::function<void(void*)> constructor;
    std::function<void(void*)> destructor;
  };

 private:
  std::vector<VariableDefination> definations_;
  size_t total_size_;
#if !defined(NDEBUG)
  bool has_been_used_;
#endif  // !defined(NDEBUG)
  ASH_DISALLOW_COPY_AND_MOVE(VariableSegmentDefination);
};

}  // namespace ash

#endif  // ASH_MEMORY_VARIABLE_SEGMENT_H_
