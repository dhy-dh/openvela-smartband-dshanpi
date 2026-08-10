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
#include "ash/zip/zip_file.h"
#include "ash/macros/compiler_macros.h"

#if defined(ASH_OS_NUTTX)
#include <unistd.h>
#include "ash/file/file.h"
#include "ash/logging/logging.h"
#include "minizip/unzip.h"
#include "minizip/zip.h"

namespace ash {

ZipFile::ZipFile(const std::string& path)
    : ZipFile(OpenFile(path, OpenMode::kRead)) {}

ZipFile::ZipFile(ScopedFD fd) : fd_(std::move(fd)) {
  if (fd_ == nullptr)
    return;

  uint32_t magic;
  uint16_t version;
  uint16_t flag;
  uint16_t compression_method;
  uint16_t modification_time;
  uint16_t modification_date;
  uint32_t crc;
  uint32_t compressed_size;
  uint32_t uncompressed_size;
  uint16_t file_name_length;
  uint16_t extra_field_length;
  char file_name[256];

  do {
    if (read(fd_, &magic, 4) != 4 || magic != 0x04034b50)
      break;

    if (read(fd_, &version, 2) != 2)
      break;

    if (read(fd_, &flag, 2) != 2)
      break;

    if (read(fd_, &compression_method, 2) != 2)
      break;

    if (read(fd_, &modification_time, 2) != 2)
      break;

    if (read(fd_, &modification_date, 2) != 2)
      break;

    if (read(fd_, &crc, 4) != 4)
      break;

    if (read(fd_, &compressed_size, 4) != 4)
      break;

    if (read(fd_, &uncompressed_size, 4) != 4)
      break;

    if (read(fd_, &file_name_length, 2) != 2)
      break;

    if (read(fd_, &extra_field_length, 2) != 2)
      break;

    if (read(fd_, file_name, file_name_length) != file_name_length)
      break;

    off_t offset = lseek(fd_, extra_field_length, SEEK_CUR);
    if (offset < 0)
      break;

    if (lseek(fd_, uncompressed_size, SEEK_CUR) < 0)
      break;

    if (flag != 0 || compression_method != 0)
      continue;

    std::string path(file_name, file_name_length);
    ASH_LOG("ASH", INFO) << "AddEntry: " << path;
    entries_.try_emplace(
        std::move(path),
        EntryInfo{static_cast<uint32_t>(offset), uncompressed_size});
  } while (true);
}

ZipFile::~ZipFile() = default;

const ZipFile::EntryInfo* ZipFile::getEntry(const std::string& path) {
  auto it = entries_.find(path);
  if (it == entries_.end())
    return nullptr;

  return &it->second;
}

bool ZipFile::loadEntry(const EntryInfo* entry, void* buffer, uint32_t size) {
  if (size > entry->size)
    return false;

  if (lseek(fd_, entry->offset, SEEK_SET) < 0)
    return false;
  if (!ReadFile(fd_, buffer, size))
    return false;
  return true;
}

bool repackZip(const char* src, const char* dest) {
  unzFile src_file = unzOpen(src);
  if (!src_file)
    return false;

  zipFile dest_file = zipOpen(dest, APPEND_STATUS_CREATE);
  if (!dest_file) {
    unzClose(src_file);
    return false;
  }

  if (unzGoToFirstFile(src_file) != UNZ_OK) {
    unzClose(src_file);
    zipClose(dest_file, nullptr);
    return false;
  }

  unz_file_info unz_info;
  zip_fileinfo zip_info;
  char name[256];
  char buf[1024];
  int r = 0;

  do {
    unzGetCurrentFileInfo(src_file, &unz_info, name, sizeof(name), nullptr, 0,
                          nullptr, 0);

    if (unz_info.compressed_size == 0)
      continue;

    zip_info.dosDate = unz_info.dosDate;
    zip_info.internal_fa = unz_info.internal_fa;
    zip_info.external_fa = unz_info.external_fa;

    zipOpenNewFileInZip(dest_file, name, &zip_info, nullptr, 0, nullptr, 0,
                        nullptr, 0, 0);

    unzOpenCurrentFile(src_file);

    while ((r = unzReadCurrentFile(src_file, buf, sizeof(buf))) > 0) {
      zipWriteInFileInZip(dest_file, buf, r);
    }

    unzCloseCurrentFile(src_file);
    zipCloseFileInZip(dest_file);
  } while (unzGoToNextFile(src_file) == UNZ_OK);

  unzClose(src_file);
  zipClose(dest_file, nullptr);
  return true;
}

}  // namespace ash
#endif  // defined(ASH_OS_NUTTX)
