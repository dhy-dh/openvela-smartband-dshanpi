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
#ifndef ASH_ZIP_IN_ZIP_H_
#define ASH_ZIP_IN_ZIP_H_

#include "ash/macros/compiler_macros.h"

#if defined(ASH_HAVE_ZLIB)

#include <stdint.h>
#include <map>
#include <memory>
#include <string>
#include "ash/fds/scoped_fd.h"
#include "ash/macros/disallow_copy.h"
#include "ash/stream/seekable_raw_input_stream.h"

namespace ash {

class InZipEntryStream;

class InZip {
 public:
  enum class CompressionMethod {
    kStore = 0,
    kDeflate = 8,
  };

  class Entry {
   public:
    Entry(uint32_t offset,
          CompressionMethod compression_method,
          uint32_t uncompressed_size,
          uint32_t compressed_size);

    uint32_t offset() const { return offset_; }
    CompressionMethod compression_method() const { return compression_method_; }
    uint32_t uncompressed_size() const { return uncompressed_size_; }
    uint32_t compressed_size() const { return compressed_size_; }

   private:
    uint32_t offset_;
    CompressionMethod compression_method_;
    uint32_t uncompressed_size_;
    uint32_t compressed_size_;
  };

  class EntryMap {
   public:
    EntryMap(std::map<std::string, Entry> entries);
    ~EntryMap();

    Entry* GetEntry(const std::string& path);
    size_t GetEntryCount();
    std::vector<std::string> List(const std::string& prefix);

   private:
    std::map<std::string, Entry> entries_;
  };

  static std::unique_ptr<InZip> Open(const std::string& path);

  size_t GetEntryCount();
  const Entry* GetEntry(const std::string& path);
  std::unique_ptr<uint8_t[]> LoadEntry(const Entry* entry,
                                       size_t extra_size = 0);
  std::unique_ptr<uint8_t[]> LoadEntry(const std::string& path,
                                       size_t extra_size = 0);
  std::unique_ptr<InZipEntryStream> LoadEntryAsStream(const Entry* entry);
  std::unique_ptr<InZipEntryStream> LoadEntryAsStream(const std::string& path);

  bool ExtractEntry(const std::string& path, const std::string& dest);

  std::vector<std::string> List(const std::string& prefix);

  std::unique_ptr<InZip> Clone();

 private:
  InZip(const std::string& path,
        ScopedFD fd,
        std::shared_ptr<EntryMap> entry_map);

  std::string path_;
  ScopedFD fd_;
  std::shared_ptr<EntryMap> entry_map_;
  ASH_DISALLOW_COPY_AND_MOVE(InZip);
};

class InZipEntryStream : public SeekableRawInputStream {
 public:
  InZipEntryStream(ScopedFD fd, uint32_t offset, uint32_t size);
  ~InZipEntryStream();

  uint32_t Read(void* buffer, uint32_t size) override;
  bool IsEOF() override;
  uint32_t GetAvailable() override;
  uint32_t GetPosition() override;
  bool Seek(uint32_t position) override;
  uint32_t GetSize() override;

 private:
  ScopedFD fd_;
  uint32_t offset_;
  uint32_t size_;
  uint32_t position_;

  ASH_DISALLOW_COPY_AND_MOVE(InZipEntryStream);
};

}  // namespace ash

#endif  // defined(ASH_HAVE_ZLIB)

#endif  // ASH_ZIP_IN_ZIP_H_
