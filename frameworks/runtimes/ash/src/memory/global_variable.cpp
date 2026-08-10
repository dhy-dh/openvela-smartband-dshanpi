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
#include "ash/memory/global_variable.h"
#include "ash/macros/compiler_macros.h"
#include "ash/memory/lazy_instance.h"

#if defined(ASH_OS_NUTTX) && !defined(CONFIG_BUILD_KERNEL)

#include <nuttx/tls_task.h>
#include <mutex>
#include "ash/crash/crash.h"

namespace ash {

namespace {

LazyInstance<VariableSegmentDefination> segment_defination =
    ASH_LAZY_INSTANCE_INITIALIZER;
LazyInstance<std::mutex> mutex = ASH_LAZY_INSTANCE_INITIALIZER;
int key = -1;

}  // namespace

VariableSegmentDefination* GetGlobalVariableSegmentDefination() {
  return segment_defination.Pointer();
}

uint8_t* GetGlobalVariableSegment() {
  std::lock_guard<std::mutex> lock(mutex.Get());
  if (key == -1) {
    key = task_tls_alloc([](void* segment) {
      GetGlobalVariableSegmentDefination()->DestroySegment(
          static_cast<uint8_t*>(segment));
    });
    if (key < 0) {
      CrashImmediately();
    }
  }

  uint8_t* segment = reinterpret_cast<uint8_t*>(task_tls_get_value(key));
  if (segment == nullptr) {
    segment = GetGlobalVariableSegmentDefination()->CreateSegment();
    if (task_tls_set_value(key, reinterpret_cast<uintptr_t>(segment)) != 0) {
      CrashImmediately();
    }
  }

  return segment;
}

}  // namespace ash

#endif  // defined(ASH_OS_NUTTX)
