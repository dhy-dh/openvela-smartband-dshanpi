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
#ifndef ASH_STREAM_INPUT_STREAM_H_
#define ASH_STREAM_INPUT_STREAM_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include "ash/macros/disallow_copy.h"

namespace ash {

class InputStream {
 public:
  InputStream(uint8_t* data, size_t size);
  ~InputStream();

  size_t available() const;

  void Skip(size_t size);
  uint8_t ReadU8();
  uint16_t ReadU16();
  uint32_t ReadU32();
  uint64_t ReadU64();

  std::string ReadString(size_t length);

 private:
  uint8_t* begin_;
  uint8_t* end_;
  ASH_DISALLOW_COPY_AND_MOVE(InputStream);
};

}  // namespace ash

#endif  // ASH_STREAM_INPUT_STREAM_H_
