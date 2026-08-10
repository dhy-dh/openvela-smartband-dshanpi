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
#include "ash/stream/raw_input_stream.h"

namespace ash {

uint32_t RawInputStream::Skip(uint32_t size) {
  uint8_t buffer[1024];
  uint32_t bytes_read = 0;
  uint32_t bytes_skipped = 0;
  do {
    bytes_read = Read(buffer, sizeof(buffer));
    bytes_skipped += bytes_read;
  } while (bytes_read == sizeof(buffer));
  return bytes_skipped;
}

}  // namespace ash
