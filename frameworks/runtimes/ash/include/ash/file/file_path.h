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
#ifndef ASH_FILE_FILE_PATH_H_
#define ASH_FILE_FILE_PATH_H_

#include <string>
#include "ash/macros/compiler_macros.h"

namespace ash {

class FilePath {
 public:
  explicit FilePath(const std::string& path, bool force_unix = false);
  explicit FilePath(const FilePath& directory,
                    const std::string& name,
                    bool force_unix = false);

  bool IsRelative() const;
  bool IsDirectory() const;
  FilePath directory() const;
  std::string name() const;
  const std::string& basename() const;
  const std::string& extension() const;
  std::string path() const;

 private:
  void ParseFrom(const std::string& path, bool force_unix);
  void ParseFrom(const FilePath& directory,
                 const std::string& name,
                 bool force_unix);

  std::string root_;  // Drive letter (D:\ eg.) for windows or / for unix
  std::string directory_;
  std::string basename_;
  std::string extension_;
#if defined(ASH_OS_WINDOWS)
  bool force_unix_;
#endif  // defined(AS_OS_WINDOWS)
};

}  // namespace ash

#endif  // ASH_FILE_FILE_PATH_H_
