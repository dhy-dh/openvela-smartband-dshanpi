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
#include "ash/stream/file_input_stream.h"
#include "ash/file/file.h"
#include "ash/logging/logging.h"

namespace ash {

FileInputStream::FileInputStream(ScopedFD fd) : fd_(std::move(fd)) {}

FileInputStream::~FileInputStream() = default;

FileInputStreamPtr FileInputStream::Create(ScopedFD fd) {
  if (!fd.IsValid())
    return nullptr;
  return std::make_unique<FileInputStream>(std::move(fd));
}

FileInputStreamPtr FileInputStream::Create(const std::string& path) {
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  return Create(std::move(fd));
}

uint32_t FileInputStream::Read(void* buffer, uint32_t size) {
  size_t bytes_read = 0;
  if (!ReadFile(fd_, buffer, size, &bytes_read))
    return 0;
  return bytes_read;
}

bool FileInputStream::IsEOF() {
  return GetAvailable() == 0;
}

uint32_t FileInputStream::GetAvailable() {
  return GetSize() - GetPosition();
}

uint32_t FileInputStream::GetPosition() {
  return Tell(fd_);
}

bool FileInputStream::Seek(uint32_t position) {
  return ash::Seek(fd_, position, SeekMode::kBegin);
}

uint32_t FileInputStream::GetSize() {
  FileInfo info;
  ASH_CHECK(GetFileInfo(fd_, &info));
  return info.size;
}

}  // namespace ash
