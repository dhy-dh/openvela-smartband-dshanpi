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
#ifndef ASH_ZIP_ZIP_FILE_H_
#define ASH_ZIP_ZIP_FILE_H_

#include <map>
#include <string>
#include "ash/fds/scoped_fd.h"
#include "ash/macros/compiler_macros.h"

#if defined(ASH_OS_NUTTX)
namespace ash {

class ZipFile {
 public:
  struct EntryInfo {
    uint32_t offset;
    uint32_t size;
  };

  explicit ZipFile(const std::string& path);
  explicit ZipFile(ScopedFD fd);
  ~ZipFile();

  const EntryInfo* getEntry(const std::string& path);

  bool loadEntry(const EntryInfo* entry, void* buffer, uint32_t size);

  const std::map<std::string, EntryInfo>& getEntryMap() const {
    return entries_;
  };

 private:
  ScopedFD fd_;
  std::map<std::string, EntryInfo> entries_;
};

bool repackZip(const char* src, const char* dest);

}  // namespace ash

#endif  // defined(ASH_OS_NUTTX)

#endif  // ASH_ZIP_ZIP_FILE_H_
