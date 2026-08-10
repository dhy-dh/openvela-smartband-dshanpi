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
#ifndef ASH_STREAM_SEEKABLE_RAW_INPUT_STREAM_H_
#define ASH_STREAM_SEEKABLE_RAW_INPUT_STREAM_H_

#include "ash/macros/disallow_copy.h"
#include "ash/stream/raw_input_stream.h"

namespace ash {

class SeekableRawInputStream : public RawInputStream {
 public:
  SeekableRawInputStream() = default;
  virtual ~SeekableRawInputStream() = default;

  virtual uint32_t GetPosition() = 0;

  virtual bool Seek(uint32_t position) = 0;

  virtual uint32_t GetSize() = 0;

  ASH_DISALLOW_COPY_AND_MOVE(SeekableRawInputStream);
};

}  // namespace ash

#endif  // ASH_STREAM_SEEKABLE_RAW_INPUT_STREAM_H_
