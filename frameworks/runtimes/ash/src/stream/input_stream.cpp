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
#include "ash/stream/input_stream.h"
#include "ash/logging/logging.h"
namespace ash {

InputStream::InputStream(uint8_t* data, size_t size)
    : begin_(data), end_(data + size) {}

InputStream::~InputStream() = default;

size_t InputStream::available() const {
  return end_ - begin_;
}

void InputStream::Skip(size_t size) {
  begin_ += size;
}

uint8_t InputStream::ReadU8() {
  return *begin_++;
}

uint16_t InputStream::ReadU16() {
  uint16_t lo = ReadU8();
  uint16_t hi = ReadU8();
  return lo | (hi << 8);
}

uint32_t InputStream::ReadU32() {
  uint32_t lo = ReadU16();
  uint32_t hi = ReadU16();
  return lo | (hi << 16);
}

uint64_t InputStream::ReadU64() {
  uint64_t lo = ReadU32();
  uint64_t hi = ReadU32();
  return lo | (hi << 32);
}

std::string InputStream::ReadString(size_t length) {
  std::string result(reinterpret_cast<char*>(begin_), length);
  begin_ += length;
  return result;
}

}  // namespace ash
