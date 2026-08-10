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
#ifndef ASH_BUNDLE_BUNDLE_H_
#define ASH_BUNDLE_BUNDLE_H_

#include "ash/macros/compiler_macros.h"

#if defined(ASH_HAVE_ZLIB)

#include <memory>
#include <string>
#include "ash/macros/disallow_copy.h"
#include "ash/stream/seekable_raw_input_stream.h"
#include "ash/zip/in_zip.h"

namespace ash {

class Bundle;
using BundlePtr = std::unique_ptr<Bundle>;

class Bundle {
 public:
  struct Data {
    std::unique_ptr<uint8_t[]> data;
    uint32_t size;
  };

  Bundle(const std::string& root, std::unique_ptr<InZip> zip);
  ~Bundle() = default;

  bool Load(const std::string& name, Data* data, size_t extra_bytes = 0);

  std::unique_ptr<SeekableRawInputStream> LoadAsStream(const std::string& name);

  BundlePtr Clone();

  std::vector<std::string> ListFiles(const std::string& directory_path);

  static BundlePtr Create(const std::string& root);
  static BundlePtr Create(const std::string& root, const std::string& path);

 private:
  bool LoadFromZip(const std::string& name, Data* data, size_t extra_bytes);
  bool LoadFromDisk(const std::string& name, Data* data, size_t extra_bytes);

  std::string root_;
  std::unique_ptr<InZip> zip_;

  ASH_DISALLOW_COPY_AND_MOVE(Bundle);
};

}  // namespace ash

#endif  // defined(ASH_HAVE_ZLIB)

#endif  // ASH_BUNDLE_BUNDLE_H_
