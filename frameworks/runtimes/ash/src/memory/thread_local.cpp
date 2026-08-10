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
#include "ash/memory/thread_local.h"

#if defined(ASH_THREAD_LOCAL_USE_VARIABLE_SEGMENT)

#include <pthread.h>
#include <mutex>
#include "ash/crash/crash.h"
#include "ash/memory/global_variable.h"
#include "ash/memory/lazy_instance.h"

namespace ash {

namespace {

LazyInstance<VariableSegmentDefination> segment_defination =
    ASH_LAZY_INSTANCE_INITIALIZER;
GlobalVariable<std::mutex> mutex;
GlobalVariable<pthread_key_t> key = -1;

}  // namespace

VariableSegmentDefination* GetThreadLocalSegmentDefination() {
  return segment_defination.Pointer();
}

uint8_t* GetThreadLocalSegment() {
  {
    std::lock_guard<std::mutex> lock(mutex.Get());
    if (key.Get() == -1) {
      int r = pthread_key_create(&key.Get(), [](void* segment) {
        GetThreadLocalSegmentDefination()->DestroySegment(
            static_cast<uint8_t*>(segment));
      });
      if (r < 0) {
        CrashImmediately();
      }
    }
  }

  uint8_t* segment = static_cast<uint8_t*>(pthread_getspecific(key.Get()));
  if (segment == nullptr) {
    segment = GetThreadLocalSegmentDefination()->CreateSegment();
    if (pthread_setspecific(key.Get(), segment) < 0) {
      CrashImmediately();
    }
  }
  return segment;
}

}  // namespace ash

#endif  // defined(ASH_THREAD_LOCAL_USE_VARIABLE_SEGMENT)
