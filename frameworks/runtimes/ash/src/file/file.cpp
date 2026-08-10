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
#include "ash/file/file.h"
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>
#include <queue>
#include <utility>

namespace ash {

ScopedFD OpenFile(const std::string& path, OpenMode mode) {
  int flags = 0;
  int auth = 0;
  switch (mode) {
    case OpenMode::kRead:
      flags = O_RDONLY;
      break;
    case OpenMode::kWrite:
      flags = O_WRONLY | O_CREAT | O_TRUNC;
      auth = 0664;
      break;
    case OpenMode::kAppend:
      flags = O_WRONLY | O_CREAT | O_APPEND;
      auth = 0664;
      break;
  }

  return ScopedFD::Adopt(open(path.c_str(), flags, auth));
}

std::vector<std::string> ListFiles(const std::string& directory_path) {
  std::vector<std::string> result;
  ::DIR* dir = ::opendir(directory_path.c_str());
  if (dir == nullptr) {
    return result;
  }
  struct ::dirent* entry;
  while ((entry = ::readdir(dir)) != nullptr) {
    result.emplace_back(entry->d_name);
  }
  ::closedir(dir);
  return result;
}

std::vector<std::string> ListFilesRecursively(
    const std::string& directory_path) {
  std::vector<std::string> files;

  std::string root_path = directory_path.empty() ? "." : directory_path;
  if (!root_path.empty() && root_path.back() == '/') {
    root_path.pop_back();
  }

  std::queue<std::pair<std::string, std::string>> dir_queue;
  dir_queue.push({root_path, ""});

  while (!dir_queue.empty()) {
    auto [phys_path, rel_path] = dir_queue.front();
    dir_queue.pop();

    auto entries = ListFiles(phys_path);
    for (const auto& entry : entries) {
      if (entry == "." || entry == "..") {
        continue;
      }

      std::string child_phys = phys_path + "/" + entry;
      std::string child_rel = rel_path.empty() ? entry : rel_path + "/" + entry;

      if (IsDirectory(child_phys)) {
        if (entry[0] == '.')
          continue;
        dir_queue.push({child_phys, child_rel});
      } else {
        files.push_back(child_rel);
      }
    }
  }

  return files;
}

bool GetFileInfo(const ScopedFD& fd, FileInfo* info) {
  struct stat st;
  if (fstat(fd, &st) != 0)
    return false;
  info->size = st.st_size;
  return true;
}

bool GetFileInfo(const std::string& path, FileInfo* info) {
  struct stat st;
  if (stat(path.c_str(), &st) != 0)
    return false;
  info->size = st.st_size;
  return true;
}

bool ReadFile(const ScopedFD& fd,
              void* buf,
              size_t buf_size,
              size_t* bytes_read) {
  int n = 0;
  int r = 0;
  uint8_t* p = (uint8_t*)buf;

  while ((r = read(fd, p, buf_size)) > 0) {
    p += r;
    buf_size -= r;
    n += r;
  }

  if (bytes_read) {
    *bytes_read = n;
    return r == 0;
  }

  return buf_size == 0;
}

std::string ReadFileAsString(const ScopedFD& fd) {
  FileInfo info;
  if (!GetFileInfo(fd, &info))
    return "";

  std::string content(info.size, 0);
  if (!ReadFile(fd, &content[0], info.size))
    return "";
  return content;
}

bool WriteFile(const ScopedFD& fd, const void* buf, size_t bytes_write) {
  int r = 0;
  uint8_t* p = (uint8_t*)buf;

  // TODO(xuyan)：考虑使用writev
  while (bytes_write && (r = write(fd, p, bytes_write)) > 0) {
    p += r;
    bytes_write -= r;
  }

  return bytes_write == 0;
}

bool WriteFile(const std::string& path, const void* buf, size_t bytes_write) {
  ScopedFD fd = OpenFile(path.c_str(), OpenMode::kWrite);
  if (fd == nullptr)
    return false;
  return WriteFile(fd, buf, bytes_write);
}

bool CopyFile(const std::string& src, const std::string& dest) {
  ScopedFD src_fd = OpenFile(src.c_str(), OpenMode::kRead);

  if (src_fd == nullptr)
    return false;

  ScopedFD dest_fd = OpenFile(dest.c_str(), OpenMode::kWrite);

  if (dest_fd == nullptr)
    return false;

  uint8_t buf[1024];
  size_t bytes = 0;
  do {
    if (!ReadFile(src_fd, buf, sizeof(buf), &bytes)) {
      DeleteFile(dest);
      return false;
    }

    if (bytes && !WriteFile(dest_fd, buf, bytes)) {
      DeleteFile(dest);
      return false;
    }
  } while (bytes);

  return true;
}

bool DeleteFile(const std::string& path) {
  return unlink(path.c_str()) == 0;
}

bool CreateDirectory(const std::string& path) {
  if (path.empty())
    return true;
  if (!CreateDirectory(GetParentDirectory(path)))
    return false;
  if (IsDirectory(path))
    return true;
  return mkdir(path.c_str(), 0777) == 0;
}

bool DeleteDirectory(const std::string& path) {
  WalkDirectory(path, [](const std::string& subpath) {
    if (IsDirectory(subpath))
      DeleteDirectory(subpath);
    else
      DeleteFile(subpath);
  });
  return rmdir(path.c_str()) == 0;
}

void WalkDirectory(const std::string& path,
                   const std::function<void(const std::string&)>& callback) {
  DIR* dir = opendir(path.c_str());
  if (dir == nullptr)
    return;

  struct dirent* entry = nullptr;
  while ((entry = readdir(dir)) != nullptr) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;
    callback(path + "/" + entry->d_name);
  }

  closedir(dir);
}

bool IsDirectory(const std::string& path) {
  struct stat st;
  if (stat(path.c_str(), &st) != 0)
    return false;
  return S_ISDIR(st.st_mode);
}

std::string GetParentDirectory(const std::string& path) {
  size_t pos = path.find_last_of('/');
  if (pos == std::string::npos)
    return "";
  return path.substr(0, pos);
}

bool Seek(const ScopedFD& fd, int64_t offset, SeekMode mode) {
  int whence = SEEK_SET;
  if (mode == SeekMode::kCurrent)
    whence = SEEK_CUR;
  else if (mode == SeekMode::kEnd)
    whence = SEEK_END;
  return lseek(fd, offset, whence) != -1;
}

uint64_t Tell(const ScopedFD& fd) {
  return lseek(fd, 0, SEEK_CUR);
}

}  // namespace ash
