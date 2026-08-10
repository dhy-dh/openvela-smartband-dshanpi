#include "ash/bundle/bundle.h"
#include <algorithm>
#include <cstring>
#include <set>
#include <vector>
#include "ash/file/file.h"
#include "ash/zip/out_zip.h"
#include "gtest/gtest.h"

namespace ash {
namespace {

constexpr char txtContent[] = "Hello Bundle!";
constexpr char txtContent2[] = "Another file";

class BundleTest : public ::testing::Test {
 protected:
  void SetUp() override {
    temp_dir_ = "/data/local/tmp/bundle_test";
    CreateDirectory(temp_dir_);
  }

  void TearDown() override { DeleteDirectory(temp_dir_); }

  void CreateDiskFile(const std::string& name, const std::string& content) {
    std::string file_path = temp_dir_ + "/" + name;
    std::string parent_dir = GetParentDirectory(file_path);

    if (!parent_dir.empty() && !IsDirectory(parent_dir)) {
      CreateDirectory(parent_dir);
    }

    WriteFile(file_path, content.data(), content.size());
  }

  void CreateDiskDirectory(const std::string& name) {
    std::string dir_path = temp_dir_ + "/" + name;
    CreateDirectory(dir_path);
  }

  void CreateZipFile(
      const std::string& zip_name,
      const std::vector<std::pair<std::string, std::string>>& files) {
    std::string zip_path = temp_dir_ + "/" + zip_name;

    auto out_zip = OutZip::Create();
    for (const auto& [name, content] : files) {
      auto data = std::make_unique<uint8_t[]>(content.size());
      memcpy(data.get(), content.data(), content.size());
      out_zip->Append(name, std::move(data), content.size(),
                      OutZip::CompressionMethod::kStore);
    }

    ASSERT_TRUE(out_zip->Write(zip_path));
  }

  bool FileExistsInList(const std::vector<std::string>& files,
                        const std::string& name) {
    return std::find(files.begin(), files.end(), name) != files.end();
  }

  int CountFileInList(const std::vector<std::string>& files,
                      const std::string& name) {
    return std::count(files.begin(), files.end(), name);
  }

  std::string temp_dir_;
};

TEST_F(BundleTest, CreateWithDefaultZip) {
  CreateZipFile("base.rpk", {{"test.txt", txtContent}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  EXPECT_NE(bundle, nullptr);
}

TEST_F(BundleTest, CreateWithoutZip) {
  BundlePtr bundle = Bundle::Create(temp_dir_);
  EXPECT_NE(bundle, nullptr);
}

TEST_F(BundleTest, CreateWithCustomZip) {
  CreateZipFile("custom.rpk", {{"custom.txt", txtContent}});

  BundlePtr bundle = Bundle::Create(temp_dir_, temp_dir_ + "/custom.rpk");
  EXPECT_NE(bundle, nullptr);
}

TEST_F(BundleTest, LoadFromZip) {
  CreateZipFile("base.rpk", {{"zip_file.txt", txtContent}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  ASSERT_NE(bundle, nullptr);

  Bundle::Data data;
  EXPECT_TRUE(bundle->Load("zip_file.txt", &data));

  std::string content(reinterpret_cast<const char*>(data.data.get()),
                      data.size);
  EXPECT_EQ(content, txtContent);
}

TEST_F(BundleTest, LoadFromDisk) {
  CreateDiskFile("disk_file.txt", txtContent2);

  BundlePtr bundle = Bundle::Create(temp_dir_);
  ASSERT_NE(bundle, nullptr);

  Bundle::Data data;
  EXPECT_TRUE(bundle->Load("disk_file.txt", &data));

  std::string content(reinterpret_cast<const char*>(data.data.get()),
                      data.size);
  EXPECT_EQ(content, txtContent2);
}

TEST_F(BundleTest, LoadWithExtraBytes) {
  CreateZipFile("base.rpk", {{"file.txt", "Test"}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data;
  const size_t extra_bytes = 10;
  EXPECT_TRUE(bundle->Load("file.txt", &data, extra_bytes));

  EXPECT_EQ(data.size, static_cast<size_t>(4));
  EXPECT_NE(data.data, nullptr);

  memset(data.data.get() + data.size, 0, extra_bytes);
}

TEST_F(BundleTest, LoadZipPrecedence) {
  CreateZipFile("base.rpk", {{"common.txt", "From ZIP"}});
  CreateDiskFile("common.txt", "From Disk");

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data;
  EXPECT_TRUE(bundle->Load("common.txt", &data));

  std::string content(reinterpret_cast<const char*>(data.data.get()),
                      data.size);
  EXPECT_EQ(content, "From ZIP");
}

TEST_F(BundleTest, LoadLargeFile) {
  std::string large_content(1024 * 1024, 'A');
  CreateZipFile("base.rpk", {{"large.bin", large_content}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data;
  EXPECT_TRUE(bundle->Load("large.bin", &data));

  EXPECT_EQ(data.size, large_content.size());
  EXPECT_EQ(memcmp(data.data.get(), large_content.data(), data.size), 0);
}

TEST_F(BundleTest, LoadEmptyFile) {
  CreateZipFile("base.rpk", {{"empty.txt", ""}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data;
  EXPECT_TRUE(bundle->Load("empty.txt", &data));

  EXPECT_EQ(data.size, static_cast<size_t>(0));
  EXPECT_NE(data.data, nullptr);
}

TEST_F(BundleTest, LoadNonExistentFile) {
  CreateZipFile("base.rpk", {{"exist.txt", txtContent}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data;
  EXPECT_FALSE(bundle->Load("non_existent.txt", &data));
}

TEST_F(BundleTest, CloneBundle) {
  CreateZipFile("base.rpk", {{"clone.txt", "Cloned"}});

  BundlePtr bundle1 = Bundle::Create(temp_dir_);
  BundlePtr bundle2 = bundle1->Clone();

  Bundle::Data data;
  EXPECT_TRUE(bundle2->Load("clone.txt", &data));

  std::string content(reinterpret_cast<const char*>(data.data.get()),
                      data.size);
  EXPECT_EQ(content, "Cloned");
}

TEST_F(BundleTest, CloneWithoutZip) {
  CreateDiskFile("disk_clone.txt", "Disk Clone");

  BundlePtr bundle1 = Bundle::Create(temp_dir_);
  BundlePtr bundle2 = bundle1->Clone();

  Bundle::Data data;
  EXPECT_TRUE(bundle2->Load("disk_clone.txt", &data));

  std::string content(reinterpret_cast<const char*>(data.data.get()),
                      data.size);
  EXPECT_EQ(content, "Disk Clone");
}

TEST_F(BundleTest, ListFilesFromZip) {
  CreateZipFile("base.rpk", {{"file1.txt", "1"},
                             {"sub/file2.txt", "2"},
                             {"sub/sub2/file3.txt", "3"}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  auto files = bundle->ListFiles("");

  EXPECT_TRUE(FileExistsInList(files, "file1.txt"));
  EXPECT_TRUE(FileExistsInList(files, "sub/file2.txt"));
  EXPECT_TRUE(FileExistsInList(files, "sub/sub2/file3.txt"));
}

TEST_F(BundleTest, ListFilesFromDisk) {
  CreateDiskFile("disk1.txt", "1");
  CreateDiskFile("sub/disk2.txt", "2");
  CreateDiskFile("sub/sub2/disk3.txt", "3");

  BundlePtr bundle = Bundle::Create(temp_dir_);
  auto files = bundle->ListFiles("");

  EXPECT_TRUE(FileExistsInList(files, "disk1.txt"));
  EXPECT_TRUE(FileExistsInList(files, "sub/disk2.txt"));
  EXPECT_TRUE(FileExistsInList(files, "sub/sub2/disk3.txt"));
}

TEST_F(BundleTest, ListFilesMerge) {
  CreateZipFile("base.rpk",
                {{"zip_only.txt", "ZIP"}, {"common.txt", "ZIP version"}});
  CreateDiskFile("disk_only.txt", "Disk");
  CreateDiskFile("common.txt", "Disk version");

  BundlePtr bundle = Bundle::Create(temp_dir_);
  auto files = bundle->ListFiles("");

  EXPECT_TRUE(FileExistsInList(files, "zip_only.txt"));
  EXPECT_TRUE(FileExistsInList(files, "disk_only.txt"));
  EXPECT_TRUE(FileExistsInList(files, "common.txt"));
}

TEST_F(BundleTest, ListFilesSubdirectory) {
  CreateZipFile(
      "base.rpk",
      {{"sub/zip1.txt", "1"}, {"sub/zip2.txt", "2"}, {"nosub/zip3.txt", "3"}});

  CreateDiskFile("sub/disk1.txt", "A");
  CreateDiskFile("sub/disk2.txt", "B");

  BundlePtr bundle = Bundle::Create(temp_dir_);
  auto files = bundle->ListFiles("sub");

  EXPECT_TRUE(FileExistsInList(files, "sub/zip1.txt"));
  EXPECT_TRUE(FileExistsInList(files, "sub/zip2.txt"));
  EXPECT_TRUE(FileExistsInList(files, "disk1.txt"));
  EXPECT_TRUE(FileExistsInList(files, "disk2.txt"));
}

TEST_F(BundleTest, ListFilesEmpty) {
  BundlePtr bundle = Bundle::Create(temp_dir_);
  auto files = bundle->ListFiles("");
  EXPECT_TRUE(files.empty());
}

TEST_F(BundleTest, LoadCorruptedZip) {
  std::string zip_path = temp_dir_ + "/base.rpk";
  WriteFile(zip_path, "This is not a valid ZIP file", 29);

  BundlePtr bundle = Bundle::Create(temp_dir_);

  Bundle::Data data;
  EXPECT_FALSE(bundle->Load("any_file.txt", &data));

  CreateDiskFile("disk_file.txt", txtContent2);
  EXPECT_TRUE(bundle->Load("disk_file.txt", &data));

  std::string content(reinterpret_cast<const char*>(data.data.get()),
                      data.size);
  EXPECT_EQ(content, txtContent2);
}

TEST_F(BundleTest, LoadWithZeroExtraBytes) {
  CreateZipFile("base.rpk", {{"file.txt", "Content"}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data;
  EXPECT_TRUE(bundle->Load("file.txt", &data, 0));

  EXPECT_EQ(data.size, static_cast<size_t>(7));
  EXPECT_NE(data.data, nullptr);
}

TEST_F(BundleTest, LoadNonexistentDirectory) {
  CreateZipFile("base.rpk", {{"file.txt", txtContent}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data;
  EXPECT_FALSE(bundle->Load("nonexistent_dir/file.txt", &data));
}

TEST_F(BundleTest, ListFilesNonexistentDirectory) {
  CreateZipFile("base.rpk", {{"file.txt", txtContent}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  auto files = bundle->ListFiles("nonexistent_dir");
  EXPECT_TRUE(files.empty());
}

TEST_F(BundleTest, LoadDirectory) {
  CreateDiskDirectory("test_dir");

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data;
  EXPECT_FALSE(bundle->Load("test_dir", &data));
}

TEST_F(BundleTest, ListFilesIncludeDirectories) {
  CreateDiskDirectory("dir1");
  CreateDiskFile("dir1/file.txt", "Content");

  BundlePtr bundle = Bundle::Create(temp_dir_);
  auto files = bundle->ListFiles("");

  EXPECT_TRUE(FileExistsInList(files, "dir1/file.txt"));
}

TEST_F(BundleTest, LoadSameFileMultipleTimes) {
  CreateZipFile("base.rpk", {{"multi.txt", "Multi"}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data1, data2;

  EXPECT_TRUE(bundle->Load("multi.txt", &data1));
  EXPECT_TRUE(bundle->Load("multi.txt", &data2));

  std::string content1(reinterpret_cast<const char*>(data1.data.get()),
                       data1.size);
  std::string content2(reinterpret_cast<const char*>(data2.data.get()),
                       data2.size);
  EXPECT_EQ(content1, "Multi");
  EXPECT_EQ(content2, "Multi");
}

TEST_F(BundleTest, LoadFileWithSpecialCharacters) {
  const std::string fileName = "file with spaces.txt";
  CreateDiskFile(fileName, "Special");

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data;
  EXPECT_TRUE(bundle->Load(fileName, &data));

  std::string content(reinterpret_cast<const char*>(data.data.get()),
                      data.size);
  EXPECT_EQ(content, "Special");
}

TEST_F(BundleTest, LoadVeryLargeFile) {
  std::string large_content(15 * 1024 * 1024, 'B');
  CreateZipFile("base.rpk", {{"very_large.bin", large_content}});

  BundlePtr bundle = Bundle::Create(temp_dir_);
  Bundle::Data data;
  EXPECT_TRUE(bundle->Load("very_large.bin", &data));

  EXPECT_EQ(data.size, large_content.size());
  EXPECT_EQ(memcmp(data.data.get(), large_content.data(), data.size), 0);
}

TEST_F(BundleTest, CloneAfterOriginalDestroyed) {
  CreateZipFile("base.rpk", {{"clone_test.txt", "Original"}});

  BundlePtr bundle1 = Bundle::Create(temp_dir_);
  BundlePtr bundle2 = bundle1->Clone();
  bundle1.reset();

  Bundle::Data data;
  EXPECT_TRUE(bundle2->Load("clone_test.txt", &data));

  std::string content(reinterpret_cast<const char*>(data.data.get()),
                      data.size);
  EXPECT_EQ(content, "Original");
}

}  // namespace
}  // namespace ash
