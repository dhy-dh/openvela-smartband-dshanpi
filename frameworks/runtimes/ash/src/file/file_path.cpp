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
#include "ash/file/file_path.h"
#include "ash/logging/logging.h"

namespace ash {

FilePath::FilePath(const std::string& path, bool force_unix) {
  ParseFrom(path, force_unix);
}

FilePath::FilePath(const FilePath& directory,
                   const std::string& name,
                   bool force_unix) {
  ParseFrom(directory, name, force_unix);
}

bool FilePath::IsRelative() const {
  return root_.empty();
}

bool FilePath::IsDirectory() const {
#if defined(__WINDOWS__)
  const char kSep == force_unix_ ? '/' : '\\';
#else
  const char kSep = '/';
#endif
  return !basename_.empty() && basename_.back() == kSep;
}

FilePath FilePath::directory() const {
#if defined(__WINDOWS__)
  return FilePath(root_ + directory_, force_unix_);
#else
  return FilePath(root_ + directory_);
#endif
}

std::string FilePath::name() const {
  return basename_ + extension_;
}

const std::string& FilePath::basename() const {
  return basename_;
}

const std::string& FilePath::extension() const {
  return extension_;
}

std::string FilePath::path() const {
  return root_ + directory_ + basename_ + extension_;
}

void FilePath::ParseFrom(const std::string& path, bool force_unix) {
  const char* p = path.c_str();
  const char kUnixSep = '/';
  const char kWinSep = '\\';
#if defined(__WINDOWS__)
  force_unix_ = force_unix;
  const char kSep = force_unix ? kUnixSep : kWinSep;
  bool is_win = !force_unix;
#else
  const char kSep = kUnixSep;
  bool is_win = false;
#endif

  if (is_win && p[0] && p[1] == ':') {
    root_.append(1, std::toupper(p[0]));
    root_.append(1, ':');
    root_.append(1, kSep);
    p += 2;
  }

  if ((p[0] == kUnixSep || p[0] == kWinSep) && root_.empty()) {
    root_.append(1, kSep);
  }

  while (*p == kUnixSep || *p == kWinSep)
    ++p;
  if (*p == 0)
    return;

  const char* q = p;
  const char* r = q;

  do {
    p = q;
    while (*q && *q != kUnixSep && *q != kWinSep)
      ++q;
    r = q;
    while (*q == kUnixSep || *q == kWinSep)
      ++q;
    if (*q) {
      directory_.append(p, r);
      directory_.append(1, kSep);
    }
  } while (*q);

  if (*r == kUnixSep || *r == kWinSep) {
    basename_.append(p, r);
    basename_.append(1, kSep);
  } else {
    for (q = r - 1; q > p && *q != '.'; --q)
      ;
    if (p == q) {
      basename_.append(p, r);
    } else {
      basename_.append(p, q);
      extension_.append(q, r);
    }
  }
}

void FilePath::ParseFrom(const FilePath& path,
                         const std::string& name,
                         bool force_unix) {
  if (path.IsDirectory()) {
    ParseFrom(path.path() + name, force_unix);
  } else {
    ParseFrom(path.path() + "/" + name, force_unix);
  }
}

}  // namespace ash
