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
#include "ash/memory/variable_segment.h"
#include "ash/crash/crash.h"
#include "ash/macros/compiler_macros.h"

namespace ash {

namespace {

// Maximum size of a segment: 1MB.
const size_t kMaxSegmentSize = 1024 * 1024;

inline size_t GetAlignFromSize(size_t size) {
  if (size <= 1)
    return 1;
  if (size <= 2)
    return 2;
#if defined(ASH_ARCH_32)
  return 4;
#elif defined(ASH_ARCH_64)
  if (size <= 4)
    return 4;
  return 8;
#endif
}

inline size_t AlignTo(size_t size, size_t align) {
  return (size + align - 1) & ~(align - 1);
}

}  // namespace

VariableSegmentDefination::VariableSegmentDefination()
    : total_size_(0)
#if !defined(NDEBUG)
      ,
      has_been_used_(false)
#endif  // !defined(NDEBUG)
{
}

VariableSegmentDefination::~VariableSegmentDefination() = default;

int VariableSegmentDefination::RegisterVariable(
    size_t size,
    std::function<void(void*)> constructor,
    std::function<void(void*)> destructor) {
#if !defined(NDEBUG)
  if (has_been_used_) {
    CrashImmediately();
  }
#endif  // !defined(NDEBUG)
  size_t align = GetAlignFromSize(size);
  total_size_ = AlignTo(total_size_, align);
  int offset = total_size_;
  total_size_ += size;

  if (total_size_ > kMaxSegmentSize) {
    // Can not use logging here, because logging depends on this class.
    CrashImmediately();
  }

  definations_.push_back({
      offset,
      std::move(constructor),
      std::move(destructor),
  });

  return offset;
}

uint8_t* VariableSegmentDefination::CreateSegment() {
#if !defined(NDEBUG)
  has_been_used_ = true;
#endif  // !defined(NDEBUG)
  size_t align = GetAlignFromSize(8);
  size_t size = AlignTo(total_size_, align);
  uint8_t* segment = static_cast<uint8_t*>(malloc(size));
  memset(segment, 0, size);
  if (!segment) {
    CrashImmediately();
  }
  for (auto& defination : definations_) {
    defination.constructor(segment + defination.offset);
  }
  return segment;
}

void VariableSegmentDefination::DestroySegment(uint8_t* segment) {
  for (auto& defination : definations_) {
    defination.destructor(segment + defination.offset);
  }
  free(segment);
}

}  // namespace ash
