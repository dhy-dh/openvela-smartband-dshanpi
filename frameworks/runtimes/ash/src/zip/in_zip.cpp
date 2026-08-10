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
#include "ash/zip/in_zip.h"

#if defined(ASH_HAVE_ZLIB)

#include "ash/file/file.h"
#include "ash/logging/logging.h"
#include "ash/stream/input_stream.h"
#include "ash/trace_event/trace_event.h"
#include "zlib.h"

namespace ash {

namespace {

constexpr size_t kMinEcdrSize = 22;
constexpr size_t kMaxEcdrSize = 65536;

bool Uncompress(const uint8_t* dest,
                size_t dest_size,
                const uint8_t* src,
                size_t src_size) {
  z_stream stream;
  int err;
  stream.next_in = (Bytef*)src;
  stream.avail_in = src_size;
  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;

  err = inflateInit2(&stream, -MAX_WBITS);
  if (err != Z_OK)
    return err;

  stream.next_out = (Bytef*)dest;
  stream.avail_out = dest_size;

  do {
    err = inflate(&stream, Z_NO_FLUSH);
  } while (err == Z_OK);

  inflateEnd(&stream);

  return err == Z_STREAM_END;
}

}  // namespace

InZip::Entry::Entry(uint32_t offset,
                    CompressionMethod compression_method,
                    uint32_t uncompressed_size,
                    uint32_t compressed_size)
    : offset_(offset),
      compression_method_(compression_method),
      uncompressed_size_(uncompressed_size),
      compressed_size_(compressed_size) {}

InZip::EntryMap::EntryMap(std::map<std::string, Entry> entries)
    : entries_(std::move(entries)) {}

InZip::EntryMap::~EntryMap() = default;

InZip::Entry* InZip::EntryMap::GetEntry(const std::string& path) {
  std::string path_copy(path);
  if (!path_copy.empty() && path_copy[0] == '/')
    path_copy.erase(0, 1);

  auto it = entries_.find(path_copy);
  if (it == entries_.end()) {
    return nullptr;
  }
  return &it->second;
}

size_t InZip::EntryMap::GetEntryCount() {
  return entries_.size();
}

std::vector<std::string> InZip::EntryMap::List(const std::string& prefix) {
  std::vector<std::string> result;
  for (auto it = entries_.begin(); it != entries_.end(); ++it) {
    if (it->first.compare(0, prefix.size(), prefix) == 0) {
      result.push_back(it->first);
    }
  }
  return result;
}

std::unique_ptr<InZip> InZip::Open(const std::string& path) {
  SCOPED_TRACE_EVENT("InZip::Open");

  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  if (!fd.IsValid()) {
    return nullptr;
  }

  std::map<std::string, Entry> entries;

  // TODO(xuyan): Find a better way to locate end of central directory record.
  bool found = false;
  for (size_t i = 0; i < kMaxEcdrSize; ++i) {
    if (!Seek(fd, -static_cast<int>(kMinEcdrSize + i), SeekMode::kEnd)) {
      ASH_LOG("ASH", ERROR) << "Failed to seek to end of file";
      return nullptr;
    }

    uint32_t magic;
    if (!ReadFile(fd, &magic, sizeof(magic))) {
      ASH_LOG("ASH", ERROR) << "Failed to read magic";
      return nullptr;
    }

    if (magic == 0x06054b50) {
      found = true;
      break;
    }
  }

  if (!found) {
    ASH_LOG("ASH", ERROR) << "Failed to find end of central directory record";
    return nullptr;
  }

  uint8_t ecdr[18];
  if (!ReadFile(fd, ecdr, sizeof(ecdr))) {
    ASH_LOG("ASH", ERROR) << "Failed to read end of central directory record";
    return nullptr;
  }

  InputStream ecdr_stream(ecdr, sizeof(ecdr));
  ecdr_stream.Skip(8);
  uint32_t central_directory_size = ecdr_stream.ReadU32();
  uint32_t central_directory_offset = ecdr_stream.ReadU32();

  if (!Seek(fd, central_directory_offset, SeekMode::kBegin)) {
    ASH_LOG("ASH", ERROR) << "Failed to seek to central directory";
    return nullptr;
  }

  std::unique_ptr<uint8_t[]> central_directory_data(
      new uint8_t[central_directory_size]);
  if (!ReadFile(fd, central_directory_data.get(), central_directory_size)) {
    ASH_LOG("ASH", ERROR) << "Failed to read central directory";
    return nullptr;
  }

  InputStream central_directory_stream(central_directory_data.get(),
                                       central_directory_size);

  while (central_directory_stream.available() >= 46) {
    uint32_t magic = central_directory_stream.ReadU32();
    if (magic != 0x02014b50)
      break;
    central_directory_stream.Skip(6);
    uint16_t compression_method = central_directory_stream.ReadU16();
    central_directory_stream.Skip(8);
    uint32_t compressed_size = central_directory_stream.ReadU32();
    uint32_t uncompressed_size = central_directory_stream.ReadU32();
    uint16_t file_name_length = central_directory_stream.ReadU16();
    uint16_t extra_field_length = central_directory_stream.ReadU16();
    uint16_t file_comment_length = central_directory_stream.ReadU16();
    central_directory_stream.Skip(8);
    uint32_t offset = central_directory_stream.ReadU32();
    if (central_directory_stream.available() <
        size_t(file_name_length + extra_field_length + file_comment_length)) {
      ASH_LOG("ASH", ERROR) << "Invalid central directory record";
      break;
    }
    std::string file_name(
        central_directory_stream.ReadString(file_name_length));
    central_directory_stream.Skip(extra_field_length + file_comment_length);

    if (file_name.empty() || file_name.back() == '/')
      continue;

    if (compression_method != 0 && compression_method != 8) {
      ASH_LOG("ASH", ERROR)
          << "Unsupported compression method: " << compression_method << " for "
          << file_name;
      continue;
    }

    entries.insert(std::make_pair(
        file_name, Entry(offset + 30 + file_name_length + extra_field_length,
                         static_cast<CompressionMethod>(compression_method),
                         uncompressed_size, compressed_size)));
  }

  std::shared_ptr<EntryMap> entry_map =
      std::make_shared<EntryMap>(std::move(entries));
  return std::unique_ptr<InZip>(
      new InZip(path, std::move(fd), std::move(entry_map)));
}

size_t InZip::GetEntryCount() {
  return entry_map_->GetEntryCount();
}

const InZip::Entry* InZip::GetEntry(const std::string& path) {
  return entry_map_->GetEntry(path);
}

std::unique_ptr<uint8_t[]> InZip::LoadEntry(const Entry* entry,
                                            size_t extra_size) {
  SCOPED_TRACE_EVENT("InZip::LoadEntry");

  if (!Seek(fd_, entry->offset(), SeekMode::kBegin)) {
    ASH_LOG("ASH", ERROR) << "Failed to seek to entry";
    return nullptr;
  }

  std::unique_ptr<uint8_t[]> compressed_data(
      new uint8_t[entry->compressed_size() + extra_size]);
  if (!ReadFile(fd_, compressed_data.get(), entry->compressed_size())) {
    ASH_LOG("ASH", ERROR) << "Failed to read data";
    return nullptr;
  }

  if (entry->compression_method() == CompressionMethod::kStore) {
    return compressed_data;
  }

  if (entry->compression_method() != CompressionMethod::kDeflate) {
    ASH_LOG("ASH", ERROR) << "Unsupported compression method";
    return nullptr;
  }

  std::unique_ptr<uint8_t[]> uncompressed_data(
      new uint8_t[entry->uncompressed_size() + extra_size]);
  if (!Uncompress(uncompressed_data.get(), entry->uncompressed_size(),
                  compressed_data.get(), entry->compressed_size())) {
    ASH_LOG("ASH", ERROR) << "Failed to uncompress data";
    return nullptr;
  }

  return uncompressed_data;
}

std::unique_ptr<uint8_t[]> InZip::LoadEntry(const std::string& path,
                                            size_t extra_size) {
  const Entry* entry = GetEntry(path);
  if (!entry) {
    return nullptr;
  }
  return LoadEntry(entry, extra_size);
}

std::unique_ptr<InZipEntryStream> InZip::LoadEntryAsStream(const Entry* entry) {
  // Only support uncompressed data
  if (entry->compression_method() != CompressionMethod::kStore) {
    return nullptr;
  }

  return std::make_unique<InZipEntryStream>(fd_, entry->offset(),
                                            entry->compressed_size());
}

std::unique_ptr<InZipEntryStream> InZip::LoadEntryAsStream(
    const std::string& path) {
  const Entry* entry = GetEntry(path);
  if (!entry) {
    return nullptr;
  }
  return LoadEntryAsStream(entry);
}

bool InZip::ExtractEntry(const std::string& path, const std::string& dest) {
  const Entry* entry = GetEntry(path);
  if (!entry) {
    ASH_LOG("ASH", ERROR) << "Entry not found" << path;
    return false;
  }

  std::unique_ptr<uint8_t[]> data = LoadEntry(entry);
  if (!data) {
    ASH_LOG("ASH", ERROR) << "Failed to load entry";
    return false;
  }

  CreateDirectory(GetParentDirectory(dest));

  if (!WriteFile(dest, data.get(), entry->uncompressed_size())) {
    ASH_LOG("ASH", ERROR) << "Failed to write file";
    return false;
  }

  return true;
}

std::vector<std::string> InZip::List(const std::string& prefix) {
  return entry_map_->List(prefix);
}

std::unique_ptr<InZip> InZip::Clone() {
  ScopedFD fd = OpenFile(path_, OpenMode::kRead);
  if (!fd.IsValid())
    return nullptr;
  return std::unique_ptr<InZip>(new InZip(path_, std::move(fd), entry_map_));
}

InZip::InZip(const std::string& path,
             ScopedFD fd,
             std::shared_ptr<EntryMap> entry_map)
    : path_(path), fd_(std::move(fd)), entry_map_(std::move(entry_map)) {}

InZipEntryStream::InZipEntryStream(ScopedFD fd, uint32_t offset, uint32_t size)
    : fd_(std::move(fd)), offset_(offset), size_(size), position_(0) {}

InZipEntryStream::~InZipEntryStream() = default;

uint32_t InZipEntryStream::Read(void* buffer, uint32_t size) {
  size = std::min(size, GetAvailable());

  ASH_CHECK(ash::Seek(fd_, offset_ + position_, SeekMode::kBegin));

  size_t bytes_read = 0;
  ASH_CHECK(ReadFile(fd_, buffer, size, &bytes_read));
  ASH_CHECK(bytes_read == size);
  position_ += size;
  return size;
}

bool InZipEntryStream::IsEOF() {
  return GetAvailable() == 0;
}

uint32_t InZipEntryStream::GetAvailable() {
  return offset_ + size_ - position_;
}

uint32_t InZipEntryStream::GetPosition() {
  return position_;
}

bool InZipEntryStream::Seek(uint32_t position) {
  if (position > offset_ + size_)
    return false;

  position_ = position;
  return true;
}

uint32_t InZipEntryStream::GetSize() {
  return size_;
}

}  // namespace ash

#endif  // defined(ASH_HAVE_ZLIB)
