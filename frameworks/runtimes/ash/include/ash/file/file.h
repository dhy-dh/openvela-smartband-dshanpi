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
#ifndef ASH_FILE_FILE_H_
#define ASH_FILE_FILE_H_

#include <stdint.h>
#include <functional>
#include <string>
#include "ash/fds/scoped_fd.h"

namespace ash {

// TODO(xuyan): 考虑提供获取错误信息的接口

enum class OpenMode {
  kRead,
  kWrite,
  kAppend,
};

ScopedFD OpenFile(const std::string& path, OpenMode mode);

struct FileInfo {
  uint64_t size;
};

bool GetFileInfo(const ScopedFD& fd, FileInfo* info);
bool GetFileInfo(const std::string& path, FileInfo* info);

std::vector<std::string> ListFiles(const std::string& directory_path);
std::vector<std::string> ListFilesRecursively(
    const std::string& directory_path);

bool ReadFile(const ScopedFD& fd,
              void* buf,
              size_t buf_size,
              size_t* bytes_read = nullptr);

std::string ReadFileAsString(const ScopedFD& fd);

bool WriteFile(const ScopedFD& fd, const void* buf, size_t bytes_write);
bool WriteFile(const std::string& path, const void* buf, size_t bytes_write);

bool CopyFile(const std::string& src, const std::string& dest);

bool DeleteFile(const std::string& path);

bool CreateDirectory(const std::string& path);

bool DeleteDirectory(const std::string& path);

void WalkDirectory(const std::string& path,
                   const std::function<void(const std::string&)>& callback);

bool IsDirectory(const std::string& path);

std::string GetParentDirectory(const std::string& path);

enum SeekMode {
  kBegin,
  kCurrent,
  kEnd,
};

bool Seek(const ScopedFD& fd, int64_t offset, SeekMode mode);

uint64_t Tell(const ScopedFD& fd);

}  // namespace ash

#endif  // ASH_FILE_FILE_H_
