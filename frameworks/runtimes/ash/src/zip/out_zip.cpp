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
#include "ash/zip/out_zip.h"

#if defined(ASH_HAVE_ZLIB)

#include "ash/file/file.h"
#include "ash/logging/logging.h"
#include "zlib.h"

namespace ash {

namespace {

bool Compress(const ScopedFD& fd,
              const uint8_t* data,
              size_t size,
              uint32_t* compressed_size) {
  z_stream stream;
  int err;
  const size_t kBufferSize = 1024 * 4;
  uint8_t buffer[kBufferSize];

  stream.zalloc = Z_NULL;
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;

  *compressed_size = 0;
  err = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8,
                     Z_DEFAULT_STRATEGY);
  if (err != Z_OK)
    return false;

  stream.next_in = (Bytef*)data;
  stream.avail_in = size;
  do {
    stream.avail_out = kBufferSize;
    stream.next_out = (Bytef*)buffer;

    err = deflate(&stream, stream.avail_in ? Z_NO_FLUSH : Z_FINISH);
    if (err != Z_OK && err != Z_STREAM_END) {
      deflateEnd(&stream);
      return false;
    }

    size_t have = kBufferSize - stream.avail_out;
    if (have > 0) {
      if (!WriteFile(fd, buffer, have)) {
        deflateEnd(&stream);
        return false;
      }
      *compressed_size += have;
    }
  } while (stream.avail_out == 0 || err != Z_STREAM_END);

  return deflateEnd(&stream) == Z_OK;
}

}  // namespace

class OutZip::Entry {
 public:
  Entry(const std::string& path, CompressionMethod compression_method)
      : path_(path), compression_method_(compression_method) {}

  virtual ~Entry() = default;

  const std::string& path() const { return path_; }
  CompressionMethod compression_method() const { return compression_method_; }

  virtual bool IsDirectory() = 0;
  virtual std::unique_ptr<uint8_t[]> GetData() = 0;
  virtual size_t GetSize() = 0;

 private:
  std::string path_;
  CompressionMethod compression_method_;
};

class DirectoryEntry : public OutZip::Entry {
 public:
  DirectoryEntry(const std::string& path)
      : Entry(path, OutZip::CompressionMethod::kStore) {}

  bool IsDirectory() override { return true; }
  std::unique_ptr<uint8_t[]> GetData() override { return nullptr; }
  size_t GetSize() override { return 0; }
};

class DataEntry : public OutZip::Entry {
 public:
  DataEntry(const std::string& path,
            std::unique_ptr<uint8_t[]> data,
            size_t size,
            OutZip::CompressionMethod compression_method)
      : Entry(path, compression_method), data_(std::move(data)), size_(size) {}

  bool IsDirectory() override { return false; }
  std::unique_ptr<uint8_t[]> GetData() override { return std::move(data_); }
  size_t GetSize() override { return size_; }

 private:
  std::unique_ptr<uint8_t[]> data_;
  size_t size_;
};

class DataProviderEntry : public OutZip::Entry {
 public:
  DataProviderEntry(const std::string& path,
                    OutZip::DataProvider provider,
                    OutZip::CompressionMethod compression_method)
      : Entry(path, compression_method),
        provider_(std::move(provider)),
        size_(0) {}

  bool IsDirectory() override { return false; }

  std::unique_ptr<uint8_t[]> GetData() override {
    TakeDataIfNeeded();
    return std::move(data_);
  }

  size_t GetSize() override {
    TakeDataIfNeeded();
    return size_;
  }

 private:
  void TakeDataIfNeeded() {
    if (provider_) {
      OutZip::Data data = std::move(provider_)();
      data_ = std::move(data.data);
      size_ = data.size;
    }
  }

  OutZip::DataProvider provider_;
  std::unique_ptr<uint8_t[]> data_;
  size_t size_;
};

OutZip::OutZip(CompressionMethod compression_method)
    : compression_method_(compression_method) {}

OutZip::~OutZip() = default;

std::unique_ptr<OutZip> OutZip::Create(CompressionMethod compression_method) {
  return std::unique_ptr<OutZip>(new OutZip(compression_method));
}

void OutZip::Append(const std::string& path,
                    std::unique_ptr<uint8_t[]> data,
                    size_t size) {
  Append(path, std::move(data), size, compression_method_);
}

void OutZip::Append(const std::string& path,
                    std::unique_ptr<uint8_t[]> data,
                    size_t size,
                    CompressionMethod compression_method) {
  auto it = std::find_if(entries_.begin(), entries_.end(),
                         [&path](const std::unique_ptr<Entry>& entry) {
                           return entry->path() == path;
                         });
  if (it != entries_.end()) {
    entries_.erase(it);
  }

  entries_.emplace_back(
      new DataEntry(path, std::move(data), size, compression_method));
}

void OutZip::Append(const std::string& path, DataProvider provider) {
  Append(path, std::move(provider), compression_method_);
}

void OutZip::Append(const std::string& path,
                    DataProvider provider,
                    CompressionMethod compression_method) {
  auto it = std::find_if(entries_.begin(), entries_.end(),
                         [&path](const std::unique_ptr<Entry>& entry) {
                           return entry->path() == path;
                         });
  if (it != entries_.end()) {
    entries_.erase(it);
  }
  entries_.emplace_back(
      new DataProviderEntry(path, std::move(provider), compression_method));
}

bool OutZip::Write(const std::string& path) {
  ScopedFD fd = OpenFile(path, OpenMode::kWrite);
  if (!fd.IsValid())
    return false;
  return Write(fd);
}

bool OutZip::Write(const ScopedFD& fd) {
  if (!WriteData(fd)) {
    records_.clear();
    return false;
  }

  uint32_t directory_size_pos = Tell(fd);
  if (!WriteCentralDirectory(fd)) {
    records_.clear();
    return false;
  }

  if (!WriteEndOfCentralDirectory(fd, directory_size_pos)) {
    records_.clear();
    return false;
  }

  records_.clear();
  return true;
}

bool OutZip::Contains(const std::string& path) {
  return std::find_if(entries_.begin(), entries_.end(),
                      [&](const std::unique_ptr<Entry>& entry) {
                        return entry->path() == path;
                      }) != entries_.end();
}

bool OutZip::WriteData(const ScopedFD& fd) {
  uint32_t magic = 0x04034b50;
  uint16_t version = 10;
  uint16_t flag = 0;
  uint16_t compression_method;
  uint16_t modification_time = 0;
  uint16_t modification_date = 0;
  uint32_t crc = 0;
  uint32_t compressed_size;
  uint32_t uncompressed_size;
  uint16_t file_name_length;
  uint16_t extra_field_length = 0;

  for (auto& entry : entries_) {
    uint32_t offset = Tell(fd);
    uncompressed_size = entry->GetSize();
    std::unique_ptr<uint8_t[]> uncompressed_data = entry->GetData();

    if (!WriteFile(fd, &magic, sizeof(magic))) {
      ASH_LOG("ASH", ERROR) << "Failed to write magic number";
      return false;
    }

    if (!WriteFile(fd, &version, sizeof(version))) {
      ASH_LOG("ASH", ERROR) << "Failed to write version";
      return false;
    }

    if (!WriteFile(fd, &flag, sizeof(flag))) {
      ASH_LOG("ASH", ERROR) << "Failed to write flag";
      return false;
    }

    compression_method = (uint16_t)entry->compression_method();
    if (!WriteFile(fd, &compression_method, sizeof(compression_method))) {
      ASH_LOG("ASH", ERROR) << "Failed to write compression method";
      return false;
    }

    if (!WriteFile(fd, &modification_time, sizeof(modification_time))) {
      ASH_LOG("ASH", ERROR) << "Failed to write modification time";
      return false;
    }

    if (!WriteFile(fd, &modification_date, sizeof(modification_date))) {
      ASH_LOG("ASH", ERROR) << "Failed to write modification date";
      return false;
    }

    crc = crc32(0, uncompressed_data.get(), uncompressed_size);
    if (!WriteFile(fd, &crc, sizeof(crc))) {
      ASH_LOG("ASH", ERROR) << "Failed to write crc";
      return false;
    }

    uint64_t compressed_size_pos = Tell(fd);

    // Write zero for compressed size.
    // Write actual compressed size later.
    compressed_size = 0;
    if (!WriteFile(fd, &compressed_size, sizeof(compressed_size))) {
      ASH_LOG("ASH", ERROR) << "Failed to write compressed size";
      return false;
    }

    if (!WriteFile(fd, &uncompressed_size, sizeof(uncompressed_size))) {
      ASH_LOG("ASH", ERROR) << "Failed to write uncompressed size";
      return false;
    }

    file_name_length = entry->path().size();
    if (!WriteFile(fd, &file_name_length, sizeof(file_name_length))) {
      ASH_LOG("ASH", ERROR) << "Failed to write file name length";
      return false;
    }

    if (!WriteFile(fd, &extra_field_length, sizeof(extra_field_length))) {
      ASH_LOG("ASH", ERROR) << "Failed to write extra field length";
      return false;
    }

    if (!WriteFile(fd, entry->path().c_str(), file_name_length)) {
      ASH_LOG("ASH", ERROR) << "Failed to write file name";
      return false;
    }

    if (compression_method == Z_BINARY) {
      if (!WriteFile(fd, uncompressed_data.get(), uncompressed_size)) {
        ASH_LOG("ASH", ERROR) << "Failed to write data";
        return false;
      }
      compressed_size = uncompressed_size;
    } else if (compression_method == Z_DEFLATED) {
      if (!Compress(fd, uncompressed_data.get(), uncompressed_size,
                    &compressed_size)) {
        ASH_LOG("ASH", ERROR) << "Failed to compress data";
        return false;
      }
    } else {
      ASH_LOG("ASH", ERROR)
          << "Unsupported compression method: " << compression_method;
      return false;
    }

    uint64_t end_pos = Tell(fd);
    if (!Seek(fd, compressed_size_pos, SeekMode::kBegin)) {
      ASH_LOG("ASH", ERROR) << "Failed to seek to compressed size position";
      return false;
    }

    if (!WriteFile(fd, &compressed_size, sizeof(compressed_size))) {
      ASH_LOG("ASH", ERROR) << "Failed to write compressed size";
      return false;
    }

    if (!Seek(fd, end_pos, SeekMode::kBegin)) {
      ASH_LOG("ASH", ERROR) << "Failed to seek to end position";
      return false;
    }

    records_.push_back(Record{entry->path(), offset, compression_method,
                              uncompressed_size, compressed_size});
  }

  return true;
}

bool OutZip::WriteCentralDirectory(const ScopedFD& fd) {
  uint32_t magic = 0x02014b50;
  for (const Record& record : records_) {
    if (!WriteFile(fd, &magic, sizeof(magic))) {
      ASH_LOG("ASH", ERROR) << "Failed to write magic number";
      return false;
    }

    uint16_t version_made_by = 20;
    if (!WriteFile(fd, &version_made_by, sizeof(version_made_by))) {
      ASH_LOG("ASH", ERROR) << "Failed to write version made by";
      return false;
    }

    uint16_t version_needed_to_extract = 20;
    if (!WriteFile(fd, &version_needed_to_extract,
                   sizeof(version_needed_to_extract))) {
      ASH_LOG("ASH", ERROR) << "Failed to write version needed to extract";
      return false;
    }

    uint16_t flag = 0;
    if (!WriteFile(fd, &flag, sizeof(flag))) {
      ASH_LOG("ASH", ERROR) << "Failed to write flag";
      return false;
    }

    uint16_t compression_method = record.compression_method;
    if (!WriteFile(fd, &compression_method, sizeof(compression_method))) {
      ASH_LOG("ASH", ERROR) << "Failed to write compression method";
      return false;
    }

    uint16_t modification_time = 0;
    if (!WriteFile(fd, &modification_time, sizeof(modification_time))) {
      ASH_LOG("ASH", ERROR) << "Failed to write modification time";
      return false;
    }

    uint16_t modification_date = 0;
    if (!WriteFile(fd, &modification_date, sizeof(modification_date))) {
      ASH_LOG("ASH", ERROR) << "Failed to write modification date";
      return false;
    }

    uint32_t crc = 0;
    if (!WriteFile(fd, &crc, sizeof(crc))) {
      ASH_LOG("ASH", ERROR) << "Failed to write crc";
      return false;
    }

    uint32_t compressed_size = record.compressed_size;
    if (!WriteFile(fd, &compressed_size, sizeof(compressed_size))) {
      ASH_LOG("ASH", ERROR) << "Failed to write compressed size";
      return false;
    }

    uint32_t uncompressed_size = record.uncompressed_size;
    if (!WriteFile(fd, &uncompressed_size, sizeof(uncompressed_size))) {
      ASH_LOG("ASH", ERROR) << "Failed to write uncompressed size";
      return false;
    }

    uint16_t file_name_length = record.path.size();
    if (!WriteFile(fd, &file_name_length, sizeof(file_name_length))) {
      ASH_LOG("ASH", ERROR) << "Failed to write file name length";
      return false;
    }

    uint16_t extra_field_length = 0;
    if (!WriteFile(fd, &extra_field_length, sizeof(extra_field_length))) {
      ASH_LOG("ASH", ERROR) << "Failed to write extra field length";
      return false;
    }

    uint16_t file_comment_length = 0;
    if (!WriteFile(fd, &file_comment_length, sizeof(file_comment_length))) {
      ASH_LOG("ASH", ERROR) << "Failed to write file comment length";
      return false;
    }

    uint16_t disk_number_start = 0;
    if (!WriteFile(fd, &disk_number_start, sizeof(disk_number_start))) {
      ASH_LOG("ASH", ERROR) << "Failed to write disk number start";
      return false;
    }

    uint16_t internal_file_attributes = 0;
    if (!WriteFile(fd, &internal_file_attributes,
                   sizeof(internal_file_attributes))) {
      ASH_LOG("ASH", ERROR) << "Failed to write internal file attributes";
      return false;
    }

    uint32_t external_file_attributes = 0;
    if (!WriteFile(fd, &external_file_attributes,
                   sizeof(external_file_attributes))) {
      ASH_LOG("ASH", ERROR) << "Failed to write external file attributes";
      return false;
    }

    uint32_t relative_offset_of_local_header = record.offset;
    if (!WriteFile(fd, &relative_offset_of_local_header,
                   sizeof(relative_offset_of_local_header))) {
      ASH_LOG("ASH", ERROR)
          << "Failed to write relative offset of local header";
      return false;
    }

    if (!WriteFile(fd, record.path.c_str(), file_name_length)) {
      ASH_LOG("ASH", ERROR) << "Failed to write file name";
      return false;
    }
  }
  return true;
}

bool OutZip::WriteEndOfCentralDirectory(const ScopedFD& fd,
                                        uint32_t central_directory_offset) {
  uint32_t magic = 0x06054b50;
  uint16_t disk_number = 0;
  uint16_t central_directory_disk_number = 0;
  uint16_t number_of_entries_on_disk = 0;
  uint16_t number_of_entries = records_.size();
  uint32_t central_directory_size_on_disk = Tell(fd) - central_directory_offset;
  uint16_t comment_length = 0;

  if (!WriteFile(fd, &magic, sizeof(magic))) {
    ASH_LOG("ASH", ERROR) << "Failed to write magic number";
    return false;
  }

  if (!WriteFile(fd, &disk_number, sizeof(disk_number))) {
    ASH_LOG("ASH", ERROR) << "Failed to write disk number";
    return false;
  }

  if (!WriteFile(fd, &central_directory_disk_number,
                 sizeof(central_directory_disk_number))) {
    ASH_LOG("ASH", ERROR) << "Failed to write central directory disk number";
    return false;
  }

  if (!WriteFile(fd, &number_of_entries_on_disk,
                 sizeof(number_of_entries_on_disk))) {
    ASH_LOG("ASH", ERROR) << "Failed to write number of entries on disk";
    return false;
  }

  if (!WriteFile(fd, &number_of_entries, sizeof(number_of_entries))) {
    ASH_LOG("ASH", ERROR) << "Failed to write number of entries";
    return false;
  }

  if (!WriteFile(fd, &central_directory_size_on_disk,
                 sizeof(central_directory_size_on_disk))) {
    ASH_LOG("ASH", ERROR) << "Failed to write central directory size on disk";
    return false;
  }

  if (!WriteFile(fd, &central_directory_offset,
                 sizeof(central_directory_offset))) {
    ASH_LOG("ASH", ERROR) << "Failed to write central directory offset";
    return false;
  }

  if (!WriteFile(fd, &comment_length, sizeof(comment_length))) {
    ASH_LOG("ASH", ERROR) << "Failed to write comment length";
    return false;
  }

  return true;
}

}  // namespace ash

#endif  // defined(ASH_HAVE_ZLIB)
