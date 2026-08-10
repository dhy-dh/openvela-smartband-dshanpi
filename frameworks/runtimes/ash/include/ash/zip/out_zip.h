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
#ifndef ASH_ZIP_OUT_ZIP_H_
#define ASH_ZIP_OUT_ZIP_H_

#include "ash/macros/compiler_macros.h"

#if defined(ASH_HAVE_ZLIB)

#include <stdint.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "ash/fds/scoped_fd.h"
#include "ash/functional/fn_once.h"
#include "ash/macros/disallow_copy.h"

namespace ash {

class OutZip {
 public:
  ~OutZip();

  enum class CompressionMethod {
    kStore = 0,
    kDeflate = 8,
  };

  static std::unique_ptr<OutZip> Create(
      CompressionMethod compression_method = CompressionMethod::kDeflate);

  void Append(const std::string& path,
              std::unique_ptr<uint8_t[]> data,
              size_t size);

  void Append(const std::string& path,
              std::unique_ptr<uint8_t[]> data,
              size_t size,
              CompressionMethod compression_method);

  struct Data {
    std::unique_ptr<uint8_t[]> data;
    size_t size;
  };

  using DataProvider = FnOnce<Data()>;

  void Append(const std::string& path, DataProvider provider);

  void Append(const std::string& path,
              DataProvider provider,
              CompressionMethod compression_method);

  bool Contains(const std::string& path);

  bool Write(const std::string& path);
  bool Write(const ScopedFD& fd);

  class Entry;

 private:
  OutZip(CompressionMethod compression_method);

  struct Record {
    std::string path;
    uint32_t offset;
    uint16_t compression_method;
    uint32_t uncompressed_size;
    uint32_t compressed_size;
  };

  bool WriteData(const ScopedFD& fd);
  bool WriteCentralDirectory(const ScopedFD& fd);
  bool WriteEndOfCentralDirectory(const ScopedFD& fd,
                                  uint32_t central_directory_offset);

  CompressionMethod compression_method_;
  std::vector<std::unique_ptr<Entry>> entries_;
  std::vector<Record> records_;
  ASH_DISALLOW_COPY_AND_MOVE(OutZip);
};

}  // namespace ash

#endif  // defined(ASH_HAVE_ZLIB)

#endif  // ASH_ZIP_OUT_ZIP_H_
