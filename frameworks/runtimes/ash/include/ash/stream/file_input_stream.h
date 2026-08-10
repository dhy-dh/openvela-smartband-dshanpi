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
#ifndef ASH_STREAM_FILE_INPUT_STREAM_H_
#define ASH_STREAM_FILE_INPUT_STREAM_H_

#include <memory>
#include "ash/fds/scoped_fd.h"
#include "ash/macros/disallow_copy.h"
#include "ash/stream/seekable_raw_input_stream.h"

namespace ash {

class FileInputStream;
using FileInputStreamPtr = std::unique_ptr<FileInputStream>;

class FileInputStream : public SeekableRawInputStream {
 public:
  FileInputStream(ScopedFD fd);
  ~FileInputStream();

  uint32_t Read(void* buffer, uint32_t size) override;
  bool IsEOF() override;
  uint32_t GetAvailable() override;
  uint32_t GetPosition() override;
  bool Seek(uint32_t position) override;
  uint32_t GetSize() override;

  static FileInputStreamPtr Create(const std::string& path);
  static FileInputStreamPtr Create(ScopedFD fd);

 private:
  ScopedFD fd_;

  ASH_DISALLOW_COPY_AND_MOVE(FileInputStream);
};

}  // namespace ash

#endif  // ASH_STREAM_FILE_INPUT_STREAM_H_
