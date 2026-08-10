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
#ifndef ASH_STREAM_RAW_INPUT_STREAM_H_
#define ASH_STREAM_RAW_INPUT_STREAM_H_

#include <limits>
#include "ash/macros/disallow_copy.h"

namespace ash {

class RawInputStream {
 public:
  RawInputStream() = default;
  virtual ~RawInputStream() = default;

  virtual uint32_t Read(void* buffer, uint32_t size) = 0;

  virtual bool IsEOF() = 0;

  virtual uint32_t GetAvailable() = 0;

  virtual uint32_t Skip(uint32_t size);

  static constexpr uint32_t kUnlimit = std::numeric_limits<uint32_t>::max();

  ASH_DISALLOW_COPY_AND_MOVE(RawInputStream);
};

}  // namespace ash

#endif  // ASH_STREAM_RAW_INPUT_STREAM_H_
