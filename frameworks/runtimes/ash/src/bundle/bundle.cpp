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
#include "ash/bundle/bundle.h"

#if defined(ASH_HAVE_ZLIB)

#include "ash/file/file.h"
#include "ash/stream/file_input_stream.h"

namespace ash {

BundlePtr Bundle::Create(const std::string& root) {
  std::string path = root + "/base.rpk";
  return Create(root, path);
}

BundlePtr Bundle::Create(const std::string& root, const std::string& path) {
  std::unique_ptr<InZip> zip = InZip::Open(path);
  // zip can be nullptr
  return std::make_unique<Bundle>(root, std::move(zip));
}

Bundle::Bundle(const std::string& root, std::unique_ptr<InZip> zip)
    : root_(root), zip_(std::move(zip)) {}

BundlePtr Bundle::Clone() {
  return std::make_unique<Bundle>(root_, zip_ ? zip_->Clone() : nullptr);
}

bool Bundle::Load(const std::string& name, Data* data, size_t extra_bytes) {
  if (LoadFromZip(name, data, extra_bytes))
    return true;
  return LoadFromDisk(name, data, extra_bytes);
}

std::unique_ptr<SeekableRawInputStream> Bundle::LoadAsStream(
    const std::string& name) {
  if (zip_) {
    std::unique_ptr<InZipEntryStream> stream = zip_->LoadEntryAsStream(name);
    if (stream)
      return stream;
  }

  return FileInputStream::Create(root_ + "/" + name);
}

bool Bundle::LoadFromZip(const std::string& name,
                         Data* data,
                         size_t extra_bytes) {
  if (!zip_)
    return false;

  const InZip::Entry* entry = zip_->GetEntry(name);
  if (!entry)
    return false;

  std::unique_ptr<uint8_t[]> p(zip_->LoadEntry(entry, extra_bytes));
  if (!p)
    return false;

  data->data = std::move(p);
  data->size = entry->uncompressed_size();
  return true;
}

bool Bundle::LoadFromDisk(const std::string& name,
                          Data* data,
                          size_t extra_bytes) {
  ScopedFD fd = OpenFile(root_ + "/" + name, OpenMode::kRead);
  if (fd == nullptr) {
    return false;
  }

  FileInfo info;
  if (!GetFileInfo(fd, &info))
    return false;

  data->size = info.size;
  data->data =
      std::unique_ptr<uint8_t[]>(new uint8_t[data->size + extra_bytes]);

  bool r = ReadFile(fd, data->data.get(), data->size);

  return r;
}

std::vector<std::string> Bundle::ListFiles(const std::string& directory_path) {
  std::vector<std::string> result;
  if (zip_) {
    std::string prefix = directory_path.empty() ? "" : directory_path + "/";
    std::vector<std::string> result_in_zip = zip_->List(prefix);
    result.insert(result.end(), result_in_zip.begin(), result_in_zip.end());
  }

  std::vector<std::string> result_in_disk =
      ash::ListFilesRecursively(root_ + "/" + directory_path);
  for (const std::string& item : result_in_disk) {
    if (std::find(result.begin(), result.end(), item) == result.end()) {
      result.push_back(item);
    }
  }
  return result;
}

}  // namespace ash

#endif  // defined(ASH_HAVE_ZLIB)
