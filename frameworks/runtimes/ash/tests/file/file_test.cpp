#include "ash/file/file.h"
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <vector>

namespace ash {
namespace {

class FileTest : public ::testing::Test {
 protected:
  void SetUp() override {
    temp_dir_ = "/tmp/ash_file_test_" + std::to_string(getpid());
    CreateDirectory(temp_dir_);
  }

  void TearDown() override { DeleteDirectory(temp_dir_); }

  std::string temp_dir_;
};

// Helper function: Create temporary file
std::string CreateTempFile(const std::string& dir,
                           const std::string& content = "") {
  std::string path = dir + "/temp_file_" + std::to_string(rand()) + ".txt";
  std::ofstream file(path);
  file << content;
  file.close();
  return path;
}

// Helper function: Create temporary directory
std::string CreateTempDir(const std::string& parent_dir) {
  std::string dir = parent_dir + "/temp_dir_" + std::to_string(rand());
  CreateDirectory(dir);
  return dir;
}

// 1. OpenFile Test Cases
TEST_F(FileTest, OpenFile_ReadExistingFile) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  EXPECT_TRUE(fd.IsValid());
}

TEST_F(FileTest, OpenFile_WriteNewFile) {
  std::string path = temp_dir_ + "/new_file.txt";
  ScopedFD fd = OpenFile(path, OpenMode::kWrite);
  EXPECT_TRUE(fd.IsValid());
}

TEST_F(FileTest, OpenFile_AppendToExistingFile) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kAppend);
  EXPECT_TRUE(fd.IsValid());
}

TEST_F(FileTest, OpenFile_WriteDirectory) {
  std::string path = temp_dir_ + "/testdir";
  std::string dir = CreateTempDir(path);
  ScopedFD fd = OpenFile(dir, OpenMode::kWrite);
  EXPECT_FALSE(fd.IsValid());
}

TEST_F(FileTest, OpenFile_ReadNonExistingFile) {
  std::string path = temp_dir_ + "/non_existing.txt";
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  EXPECT_FALSE(fd.IsValid());
}

TEST_F(FileTest, OpenFile_AppendNonExistent) {
  ScopedFD fd = OpenFile(temp_dir_ + "/nonexistent.txt", OpenMode::kAppend);
  EXPECT_TRUE(fd.IsValid());
}

// 2. GetFileInfo Test Cases
TEST_F(FileTest, GetFileInfo_ByFD_ValidFile) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  FileInfo info;
  EXPECT_TRUE(GetFileInfo(fd, &info));
  EXPECT_EQ(info.size, static_cast<size_t>(5));  // "Hello" is 5 bytes
}

TEST_F(FileTest, GetFileInfo_ByFD_InvalidFD) {
  ScopedFD fd;  // Default invalid FD
  FileInfo info;
  EXPECT_FALSE(GetFileInfo(fd, &info));
}

TEST_F(FileTest, GetFileInfo_ByPath_ExistingFile) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  FileInfo info;
  EXPECT_TRUE(GetFileInfo(path, &info));
  EXPECT_EQ(info.size, static_cast<size_t>(5));
}

TEST_F(FileTest, GetFileInfo_ByPath_NonExistingFile) {
  std::string path = temp_dir_ + "/non_existing.txt";
  FileInfo info;
  EXPECT_FALSE(GetFileInfo(path, &info));
}

// 3. ListFiles Test Cases
TEST_F(FileTest, ListFiles_DirectoryWithFiles) {
  std::string dir = CreateTempDir(temp_dir_);
  CreateTempFile(dir, "file1");
  CreateTempFile(dir, "file2");
  std::vector<std::string> files = ListFiles(dir);
  EXPECT_EQ(files.size(), static_cast<size_t>(2));
}

TEST_F(FileTest, ListFiles_EmptyDirectory) {
  std::string dir = CreateTempDir(temp_dir_);
  std::vector<std::string> files = ListFiles(dir);
  EXPECT_EQ(files.size(), static_cast<size_t>(0));
}

TEST_F(FileTest, ListFiles_NonExistingDirectory) {
  std::string dir = temp_dir_ + "/non_existing_dir";
  std::vector<std::string> files = ListFiles(dir);
  EXPECT_TRUE(files.empty());
}

TEST_F(FileTest, ListFilesRecursively_EmptyDirectory) {
  auto files = ListFilesRecursively(temp_dir_);
  EXPECT_TRUE(files.empty());
}

TEST_F(FileTest, ListFilesRecursively_SingleFile) {
  CreateTempFile(temp_dir_, "test");

  auto files = ListFilesRecursively(temp_dir_);
  ASSERT_EQ(files.size(), static_cast<size_t>(1));

  size_t pos = files[0].find_last_of('/');
  std::string filename =
      (pos == std::string::npos) ? files[0] : files[0].substr(pos + 1);

  EXPECT_TRUE(filename.find("temp_file_") == 0);
  EXPECT_TRUE(filename.find(".txt") == filename.size() - 4);
}

TEST_F(FileTest, ListFilesRecursively_FlatStructure) {
  std::string file1 = CreateTempFile(temp_dir_);
  std::string file2 = CreateTempFile(temp_dir_);
  std::string subdir = CreateTempDir(temp_dir_);
  std::string file3 = CreateTempFile(subdir);

  auto files = ListFilesRecursively(temp_dir_);
  ASSERT_EQ(files.size(), static_cast<size_t>(3));

  std::string rel_file3 = file3.substr(temp_dir_.size() + 1);

  std::set<std::string> fileSet(files.begin(), files.end());
  EXPECT_TRUE(fileSet.find(file1.substr(temp_dir_.size() + 1)) !=
              fileSet.end());
  EXPECT_TRUE(fileSet.find(file2.substr(temp_dir_.size() + 1)) !=
              fileSet.end());
  EXPECT_TRUE(fileSet.find(rel_file3) != fileSet.end());
}

TEST_F(FileTest, ListFilesRecursively_DeepNesting) {
  std::string current = temp_dir_;
  for (int i = 0; i < 5; i++) {
    current = CreateTempDir(current);
  }
  std::string deepFile = CreateTempFile(current);

  auto files = ListFilesRecursively(temp_dir_);
  ASSERT_EQ(files.size(), static_cast<size_t>(1));

  std::string relPath = deepFile.substr(temp_dir_.size() + 1);
  EXPECT_EQ(files[0], relPath);
}

TEST_F(FileTest, ListFilesRecursively_OnlyFilesNoDirectories) {
  for (int i = 0; i < 5; i++) {
    CreateTempFile(temp_dir_);
  }

  auto files = ListFilesRecursively(temp_dir_);
  EXPECT_EQ(files.size(), static_cast<size_t>(5));

  for (const auto& file : files) {
    EXPECT_EQ(file.find('/'), std::string::npos);
  }
}

TEST_F(FileTest, ListFilesRecursively_OnlyDirectoriesNoFiles) {
  std::string current = temp_dir_;
  for (int i = 0; i < 3; i++) {
    current = CreateTempDir(current);
  }

  auto files = ListFilesRecursively(temp_dir_);
  EXPECT_TRUE(files.empty());
}

TEST_F(FileTest, ListFilesRecursively_SpecialFilenames) {
  std::string specialDir = CreateTempDir(temp_dir_);
  std::string file1 = specialDir + "/file with spaces.txt";
  std::ofstream(file1.c_str()) << "test";

  std::string file2 = specialDir + "/file!@#$%^&*()_+.txt";
  std::ofstream(file2.c_str()) << "test";

  std::string file3 = specialDir + "/.hidden_file";
  std::ofstream(file3.c_str()) << "test";

  auto files = ListFilesRecursively(temp_dir_);
  ASSERT_EQ(files.size(), static_cast<size_t>(3));

  std::string relDir = specialDir.substr(temp_dir_.size() + 1);
  std::set<std::string> expected = {relDir + "/file with spaces.txt",
                                    relDir + "/file!@#$%^&*()_+.txt",
                                    relDir + "/.hidden_file"};

  std::set<std::string> actual(files.begin(), files.end());
  EXPECT_EQ(actual, expected);
}

TEST_F(FileTest, ListFilesRecursively_SkipDotDirectories) {
  std::string hiddenDir = temp_dir_ + "/.hidden_dir";
  CreateDirectory(hiddenDir);
  CreateTempFile(hiddenDir);

  std::string normalDir = CreateTempDir(temp_dir_);
  std::string normalFile = CreateTempFile(normalDir);

  auto files = ListFilesRecursively(temp_dir_);

  std::string expected = normalFile.substr(temp_dir_.size() + 1);

  ASSERT_EQ(files.size(), static_cast<size_t>(1));
  EXPECT_EQ(files[0], expected);
}

TEST_F(FileTest, ListFilesRecursively_NonExistentDirectory) {
  auto files = ListFilesRecursively(temp_dir_ + "/non_existent");
  EXPECT_TRUE(files.empty());
}

TEST_F(FileTest, ListFilesRecursively_FileInsteadOfDirectory) {
  std::string testFile = CreateTempFile(temp_dir_);
  auto files = ListFilesRecursively(testFile);
  EXPECT_TRUE(files.empty());
}

TEST_F(FileTest, ListFilesRecursively_PermissionDenied) {
  std::string restrictedDir = CreateTempDir(temp_dir_);
  std::string accessibleFile = CreateTempFile(temp_dir_);
  std::string restrictedFile = CreateTempFile(restrictedDir);

  chmod(restrictedDir.c_str(), 0000);

  auto files = ListFilesRecursively(temp_dir_);

  chmod(restrictedDir.c_str(), 0700);

  for (const auto& file : files) {
    std::cout << "File: " << file << std::endl;
  }

  ASSERT_EQ(files.size(), static_cast<size_t>(2));
}

// 4. ReadFile Test Cases
TEST_F(FileTest, ReadFile_FullFile) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  char buf[6];
  size_t bytes_read;
  EXPECT_TRUE(ReadFile(fd, buf, 5, &bytes_read));
  EXPECT_EQ(bytes_read, static_cast<size_t>(5));
  buf[5] = '\0';
  EXPECT_STREQ(buf, "Hello");
}

TEST_F(FileTest, ReadFile_PartialRead) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  char buf[3];
  size_t bytes_read;
  EXPECT_TRUE(ReadFile(fd, buf, 2, &bytes_read));
  EXPECT_EQ(bytes_read, static_cast<size_t>(2));
  buf[2] = '\0';
  EXPECT_STREQ(buf, "He");
}

TEST_F(FileTest, ReadFile_EmptyFile) {
  std::string path = CreateTempFile(temp_dir_, "");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  char buf[1];
  size_t bytes_read;
  EXPECT_TRUE(ReadFile(fd, buf, 1, &bytes_read));
  EXPECT_EQ(bytes_read, static_cast<size_t>(0));
}

// 5. ReadFileAsString Test Cases
TEST_F(FileTest, ReadFileAsString_NonEmptyFile) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  std::string content = ReadFileAsString(fd);
  EXPECT_EQ(content, "Hello");
}

TEST_F(FileTest, ReadFileAsString_EmptyFile) {
  std::string path = CreateTempFile(temp_dir_, "");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  std::string content = ReadFileAsString(fd);
  EXPECT_TRUE(content.empty());
}

TEST_F(FileTest, ReadFileAsString_InvalidFD) {
  ScopedFD fd;  // Default invalid FD
  std::string content = ReadFileAsString(fd);
  EXPECT_TRUE(content.empty());
}

// 6. WriteFile (FD version) Test Cases
TEST_F(FileTest, WriteFile_ToFile_Success) {
  std::string path = temp_dir_ + "/write_test.txt";
  {
    ScopedFD fd = OpenFile(path, OpenMode::kWrite);
    const char* data = "Hello";
    EXPECT_TRUE(WriteFile(fd, data, 5));
  }  // fd destroyed here, auto-closes file descriptor
  ScopedFD read_fd = OpenFile(path, OpenMode::kRead);
  std::string content = ReadFileAsString(read_fd);
  EXPECT_EQ(content, "Hello");
}

TEST_F(FileTest, WriteFile_AppendToFile) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  {
    ScopedFD fd = OpenFile(path, OpenMode::kAppend);
    const char* data = " World";
    EXPECT_TRUE(WriteFile(fd, data, 6));
  }  // fd destroyed here, auto-closes file descriptor
  ScopedFD read_fd = OpenFile(path, OpenMode::kRead);
  std::string content = ReadFileAsString(read_fd);
  EXPECT_EQ(content, "Hello World");
}

TEST_F(FileTest, WriteFile_InvalidFD) {
  ScopedFD fd;  // Default invalid FD
  const char* data = "Hello";
  EXPECT_FALSE(WriteFile(fd, data, 5));
}

// 7. WriteFile (path version) Test Cases
TEST_F(FileTest, WriteFile_ByPath_Success) {
  std::string path = temp_dir_ + "/write_by_path.txt";
  const char* data = "Test";
  EXPECT_TRUE(WriteFile(path, data, 4));
  std::string content = ReadFileAsString(OpenFile(path, OpenMode::kRead));
  EXPECT_EQ(content, "Test");
}

TEST_F(FileTest, WriteFile_ByPath_ExistingFile) {
  std::string path = CreateTempFile(temp_dir_, "Old");
  const char* data = "New";
  EXPECT_TRUE(WriteFile(path, data, 3));
  std::string content = ReadFileAsString(OpenFile(path, OpenMode::kRead));
  EXPECT_EQ(content, "New");
}

// 8. CopyFile Test Cases
TEST_F(FileTest, CopyFile_Success) {
  std::string src = CreateTempFile(temp_dir_, "Source");
  std::string dest = temp_dir_ + "/dest.txt";
  EXPECT_TRUE(CopyFile(src, dest));
  std::string content = ReadFileAsString(OpenFile(dest, OpenMode::kRead));
  EXPECT_EQ(content, "Source");
}

TEST_F(FileTest, CopyFile_SourceNotExist) {
  std::string src = temp_dir_ + "/non_existing.txt";
  std::string dest = temp_dir_ + "/dest.txt";
  EXPECT_FALSE(CopyFile(src, dest));
}

TEST_F(FileTest, CopyFile_DestinationDirectoryNotExist) {
  std::string src = CreateTempFile(temp_dir_, "Source");
  std::string dest = temp_dir_ + "/non_existing_dir/dest.txt";
  EXPECT_FALSE(CopyFile(src, dest));
}

// 9. DeleteFile Test Cases
TEST_F(FileTest, DeleteFile_Success) {
  std::string path = CreateTempFile(temp_dir_);
  EXPECT_TRUE(DeleteFile(path));
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  EXPECT_FALSE(fd.IsValid());
}

TEST_F(FileTest, DeleteFile_NonExistingFile) {
  std::string path = temp_dir_ + "/non_existing.txt";
  EXPECT_FALSE(DeleteFile(path));
}

TEST_F(FileTest, DeleteFile_Directory) {
  std::string dir = CreateTempDir(temp_dir_);
  EXPECT_FALSE(DeleteFile(dir));
}

// 10. CreateDirectory Test Cases
TEST_F(FileTest, CreateDirectory_NewDirectory) {
  std::string dir = temp_dir_ + "/new_dir";
  EXPECT_TRUE(CreateDirectory(dir));
  EXPECT_TRUE(IsDirectory(dir));
}

TEST_F(FileTest, CreateDirectory_ExistingDirectory) {
  std::string dir = CreateTempDir(temp_dir_);
  EXPECT_TRUE(CreateDirectory(dir));  // Existing directory should return true
}

TEST_F(FileTest, CreateDirectory_NestedDirectories) {
  std::string dir = temp_dir_ + "/nested/dir";
  EXPECT_TRUE(CreateDirectory(dir));
  EXPECT_TRUE(IsDirectory(dir));
}

// 11. DeleteDirectory Test Cases
TEST_F(FileTest, DeleteDirectory_EmptyDirectory) {
  std::string dir = CreateTempDir(temp_dir_);
  EXPECT_TRUE(DeleteDirectory(dir));
  EXPECT_FALSE(IsDirectory(dir));
}

TEST_F(FileTest, DeleteDirectory_NonEmptyDirectory) {
  std::string dir = CreateTempDir(temp_dir_);
  CreateTempFile(dir, "file1");
  EXPECT_TRUE(DeleteDirectory(dir));
  EXPECT_FALSE(IsDirectory(dir));
}

TEST_F(FileTest, DeleteDirectory_NonExistingDirectory) {
  std::string dir = temp_dir_ + "/non_existing_dir";
  EXPECT_FALSE(DeleteDirectory(dir));
}

// 12. WalkDirectory Test Cases
TEST_F(FileTest, WalkDirectory_DirectoryWithFiles) {
  std::string dir = CreateTempDir(temp_dir_);
  CreateTempFile(dir, "file1");
  CreateTempFile(dir, "file2");
  std::vector<std::string> walked;
  WalkDirectory(dir,
                [&walked](const std::string& path) { walked.push_back(path); });
  EXPECT_EQ(walked.size(), static_cast<size_t>(2));
}

TEST_F(FileTest, WalkDirectory_EmptyDirectory) {
  std::string dir = CreateTempDir(temp_dir_);
  std::vector<std::string> walked;
  WalkDirectory(dir,
                [&walked](const std::string& path) { walked.push_back(path); });
  EXPECT_TRUE(walked.empty());
}

TEST_F(FileTest, WalkDirectory_NonExistingDirectory) {
  std::string dir = temp_dir_ + "/non_existing_dir";
  std::vector<std::string> walked;
  WalkDirectory(dir,
                [&walked](const std::string& path) { walked.push_back(path); });
  EXPECT_TRUE(walked.empty());
}

// 13. IsDirectory Test Cases
TEST_F(FileTest, IsDirectory_Directory) {
  std::string dir = CreateTempDir(temp_dir_);
  EXPECT_TRUE(IsDirectory(dir));
}

TEST_F(FileTest, IsDirectory_File) {
  std::string path = CreateTempFile(temp_dir_);
  EXPECT_FALSE(IsDirectory(path));
}

TEST_F(FileTest, IsDirectory_NonExistingPath) {
  std::string path = temp_dir_ + "/non_existing";
  EXPECT_FALSE(IsDirectory(path));
}

// 14. GetParentDirectory Test Cases
TEST_F(FileTest, GetParentDirectory_FilePath) {
  std::string path = temp_dir_ + "/file.txt";
  std::string parent = GetParentDirectory(path);
  EXPECT_EQ(parent, temp_dir_);
}

TEST_F(FileTest, GetParentDirectory_RootDirectory) {
  std::string path = "/";
  std::string parent = GetParentDirectory(path);
  EXPECT_EQ(parent, "");
}

TEST_F(FileTest, GetParentDirectory_NoSlash) {
  std::string path = "file.txt";
  std::string parent = GetParentDirectory(path);
  EXPECT_EQ(parent, "");
}

// 15. Seek Test Cases
TEST_F(FileTest, Seek_FromBegin) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  EXPECT_TRUE(Seek(fd, 2, SeekMode::kBegin));
  char buf[4];
  size_t bytes_read;
  EXPECT_TRUE(ReadFile(fd, buf, 3, &bytes_read));
  buf[3] = '\0';
  EXPECT_STREQ(buf, "llo");
}

TEST_F(FileTest, Seek_FromCurrent) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  char buf[3];
  size_t bytes_read;
  EXPECT_TRUE(ReadFile(fd, buf, 2, &bytes_read));  // Read "He"
  EXPECT_TRUE(Seek(fd, 1, SeekMode::kCurrent));    // Skip 'l'
  EXPECT_TRUE(ReadFile(fd, buf, 2, &bytes_read));
  buf[2] = '\0';
  EXPECT_STREQ(buf, "lo");
}

TEST_F(FileTest, Seek_FromEnd) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  EXPECT_TRUE(Seek(fd, -2, SeekMode::kEnd));
  char buf[3];
  size_t bytes_read;
  EXPECT_TRUE(ReadFile(fd, buf, 2, &bytes_read));
  buf[2] = '\0';
  EXPECT_STREQ(buf, "lo");
}

// 16. Tell Test Cases
TEST_F(FileTest, Tell_AfterSeek) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  EXPECT_TRUE(Seek(fd, 3, SeekMode::kBegin));
  EXPECT_EQ(Tell(fd), static_cast<size_t>(3));
}

TEST_F(FileTest, Tell_AtBeginning) {
  std::string path = CreateTempFile(temp_dir_, "Hello");
  ScopedFD fd = OpenFile(path, OpenMode::kRead);
  EXPECT_EQ(Tell(fd), static_cast<size_t>(0));
}

TEST_F(FileTest, Tell_InvalidFD) {
  ScopedFD fd;  // Default invalid FD
  EXPECT_EQ(Tell(fd),
            static_cast<uint64_t>(-1));  // ScopedFD returns -1 for invalid
}

}  // namespace
}  // namespace ash
