#include "ash/zip/in_zip.h"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include "ash/file/file.h"
#include "ash/zip/out_zip.h"

namespace ash {
namespace {

constexpr char kTestContent[] = "Hello, this is a test file content!";
constexpr char kTestContent2[] = "Another test file content for verification";
constexpr char kTestDir[] = "test_directory/";
constexpr char kTestDir2[] = "nested/directory/";

class InZipTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create temporary directory
    const char* tmpdir = getenv("TMPDIR");
    if (!tmpdir)
      tmpdir = "/tmp";
    temp_dir_ = std::string(tmpdir) + "/in_zip_test_XXXXXX";

    char* dir_template = const_cast<char*>(temp_dir_.c_str());
    char* result = mkdtemp(dir_template);
    ASSERT_TRUE(result != nullptr);
    temp_dir_ = result;

    temp_zip_ = temp_dir_ + "/test.zip";
    extracted_dir_ = temp_dir_ + "/extracted";
    CreateDirectory(extracted_dir_);

    // Create test ZIP file
    CreateTestZip();
  }

  void TearDown() override { DeleteDirectory(temp_dir_); }

  void CreateTestZip() {
    auto zip = OutZip::Create();

    // Add text file
    auto data1 = std::make_unique<uint8_t[]>(sizeof(kTestContent));
    std::copy(kTestContent, kTestContent + sizeof(kTestContent), data1.get());
    zip->Append("file1.txt", std::move(data1), sizeof(kTestContent));

    // Add another text file
    auto data2 = std::make_unique<uint8_t[]>(sizeof(kTestContent2));
    std::copy(kTestContent2, kTestContent2 + sizeof(kTestContent2),
              data2.get());
    zip->Append("dir/file2.txt", std::move(data2), sizeof(kTestContent2));

    // Add empty file
    auto empty_data = std::make_unique<uint8_t[]>(0);
    zip->Append("empty.txt", std::move(empty_data), 0);

    // Add large file
    const size_t kLargeSize = 1024 * 1024;  // 1MB
    auto large_data = std::make_unique<uint8_t[]>(kLargeSize);
    std::fill(large_data.get(), large_data.get() + kLargeSize, 0x55);
    zip->Append("large.bin", std::move(large_data), kLargeSize);

    ASSERT_TRUE(zip->Write(temp_zip_));

    FileInfo info;
    ASSERT_TRUE(GetFileInfo(temp_zip_, &info));
    ASSERT_GT(info.size, static_cast<size_t>(0));
  }

  std::string temp_dir_;
  std::string temp_zip_;
  std::string extracted_dir_;
};

// Test opening existing ZIP file
TEST_F(InZipTest, OpenExistingZip) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);
}

// Test opening non-existent ZIP file
TEST_F(InZipTest, OpenNonExistingZip) {
  auto in_zip = InZip::Open("/path/to/nonexistent.zip");
  ASSERT_EQ(in_zip, nullptr);
}

// Test getting entry count
TEST_F(InZipTest, GetEntryCount) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  EXPECT_EQ(in_zip->GetEntryCount(), static_cast<size_t>(4));
}

// Test getting existing file entries
TEST_F(InZipTest, GetExistingFileEntry) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const auto* entry = in_zip->GetEntry("file1.txt");
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->uncompressed_size(), sizeof(kTestContent));

  entry = in_zip->GetEntry("dir/file2.txt");
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->uncompressed_size(), sizeof(kTestContent2));

  entry = in_zip->GetEntry("empty.txt");
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->uncompressed_size(), static_cast<size_t>(0));

  entry = in_zip->GetEntry("large.bin");
  ASSERT_NE(entry, nullptr);
  EXPECT_GT(entry->uncompressed_size(), static_cast<size_t>(0));
}

// Test getting non-existent entries
TEST_F(InZipTest, GetNonExistingEntry) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const auto* entry = in_zip->GetEntry("nonexistent.txt");
  ASSERT_EQ(entry, nullptr);

  entry = in_zip->GetEntry("nonexistent_directory/");
  ASSERT_EQ(entry, nullptr);
}

// Test loading text entry content
TEST_F(InZipTest, LoadTextEntry) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const auto* entry = in_zip->GetEntry("file1.txt");
  ASSERT_NE(entry, nullptr);

  auto data = in_zip->LoadEntry(entry);
  ASSERT_NE(data, nullptr);

  // Compare content excluding null terminator
  EXPECT_EQ(memcmp(data.get(), kTestContent, strlen(kTestContent)), 0);
}

// Test loading entry with extra space
TEST_F(InZipTest, LoadEntryWithExtraSpace) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const auto* entry = in_zip->GetEntry("file1.txt");
  ASSERT_NE(entry, nullptr);

  const size_t extra_size = 10;
  auto data = in_zip->LoadEntry(entry, extra_size);
  ASSERT_NE(data, nullptr);

  // Verify original content
  EXPECT_EQ(memcmp(data.get(), kTestContent, strlen(kTestContent)), 0);
}

// Test loading empty entry
TEST_F(InZipTest, LoadEmptyEntry) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const auto* entry = in_zip->GetEntry("empty.txt");
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->uncompressed_size(), static_cast<size_t>(0));

  auto data = in_zip->LoadEntry(entry);
  ASSERT_NE(data, nullptr);
}

// Test loading large file entry
TEST_F(InZipTest, LoadLargeEntry) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const auto* entry = in_zip->GetEntry("large.bin");
  ASSERT_NE(entry, nullptr);

  auto data = in_zip->LoadEntry(entry);
  ASSERT_NE(data, nullptr);

  // Verify content
  for (size_t i = 0; i < entry->uncompressed_size(); i++) {
    EXPECT_EQ(data[i], 0x55);
  }
}

// Test loading entries by path
TEST_F(InZipTest, LoadEntryByPath) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  auto data = in_zip->LoadEntry("file1.txt");
  ASSERT_NE(data, nullptr);
  EXPECT_EQ(memcmp(data.get(), kTestContent, strlen(kTestContent)), 0);

  data = in_zip->LoadEntry("dir/file2.txt");
  ASSERT_NE(data, nullptr);
  EXPECT_EQ(memcmp(data.get(), kTestContent2, strlen(kTestContent2)), 0);

  data = in_zip->LoadEntry("empty.txt");
  ASSERT_NE(data, nullptr);
}

// Test extracting file entry to destination path
TEST_F(InZipTest, ExtractFileEntry) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const std::string dest_path = extracted_dir_ + "/extracted_file.txt";

  // Extract file
  ASSERT_TRUE(in_zip->ExtractEntry("file1.txt", dest_path));

  // Verify file content
  ScopedFD fd = OpenFile(dest_path, OpenMode::kRead);
  ASSERT_TRUE(fd.IsValid());

  std::string content = ReadFileAsString(fd);
  // Compare excluding null terminator
  EXPECT_EQ(content.substr(0, strlen(kTestContent)), kTestContent);
}

// Test extracting to existing file
TEST_F(InZipTest, ExtractToExistingFile) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const std::string dest_path = extracted_dir_ + "/existing_file.txt";

  // Create target file
  const char content[] = "Existing content";
  ASSERT_TRUE(WriteFile(dest_path, content, sizeof(content)));

  // Extract file (should overwrite)
  ASSERT_TRUE(in_zip->ExtractEntry("file1.txt", dest_path));

  // Verify file content is overwritten
  ScopedFD fd = OpenFile(dest_path, OpenMode::kRead);
  ASSERT_TRUE(fd.IsValid());

  std::string new_content = ReadFileAsString(fd);
  // Compare excluding null terminator
  EXPECT_EQ(new_content.substr(0, strlen(kTestContent)), kTestContent);
}

// Test extracting to invalid path
TEST_F(InZipTest, ExtractToInvalidPath) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  // Attempt to extract to invalid path
  ASSERT_FALSE(in_zip->ExtractEntry("file1.txt", "/invalid/path/file.txt"));
}

// Test extracting non-existent entry
TEST_F(InZipTest, ExtractNonExistingEntry) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const std::string dest_path = extracted_dir_ + "/nonexistent.txt";

  // Attempt to extract non-existent entry
  ASSERT_FALSE(in_zip->ExtractEntry("nonexistent.txt", dest_path));
}

// Test listing all entries
TEST_F(InZipTest, ListAllEntries) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  auto entries = in_zip->List("");
  ASSERT_EQ(entries.size(), static_cast<size_t>(4));

  // Verify all entries exist
  EXPECT_NE(std::find(entries.begin(), entries.end(), "file1.txt"),
            entries.end());
  EXPECT_NE(std::find(entries.begin(), entries.end(), "dir/file2.txt"),
            entries.end());
  EXPECT_NE(std::find(entries.begin(), entries.end(), "empty.txt"),
            entries.end());
  EXPECT_NE(std::find(entries.begin(), entries.end(), "large.bin"),
            entries.end());
}

// Test listing entries by prefix
TEST_F(InZipTest, ListEntriesWithPrefix) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  // List entries with "dir/" prefix
  auto entries = in_zip->List("dir/");
  ASSERT_EQ(entries.size(), static_cast<size_t>(1));
  EXPECT_EQ(entries[0], "dir/file2.txt");

  // List non-existent prefix
  entries = in_zip->List("nonexistent/");
  EXPECT_TRUE(entries.empty());
}

// Test cloning InZip object
TEST_F(InZipTest, CloneInZip) {
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  auto clone = in_zip->Clone();
  ASSERT_NE(clone, nullptr);

  // Verify clone has same entry count
  EXPECT_EQ(clone->GetEntryCount(), in_zip->GetEntryCount());

  // Verify clone can load entries
  const auto* entry = clone->GetEntry("file1.txt");
  ASSERT_NE(entry, nullptr);

  auto data = clone->LoadEntry(entry);
  ASSERT_NE(data, nullptr);
  EXPECT_EQ(memcmp(data.get(), kTestContent, strlen(kTestContent)), 0);
}

// Test handling corrupted ZIP file
TEST_F(InZipTest, CorruptedZipFile) {
  // Create corrupted ZIP file
  const char corrupted_content[] = "This is not a valid ZIP file";
  ASSERT_TRUE(
      WriteFile(temp_zip_, corrupted_content, sizeof(corrupted_content)));

  // Attempt to open corrupted ZIP file
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_EQ(in_zip, nullptr);
}

// Test partially corrupted ZIP file
TEST_F(InZipTest, PartiallyCorruptedZipFile) {
  // Create valid ZIP file
  CreateTestZip();

  // Append corrupted data
  ScopedFD fd = OpenFile(temp_zip_, OpenMode::kAppend);
  ASSERT_TRUE(fd.IsValid());
  const char corrupted_content[] = "Corrupted data appended";
  ASSERT_TRUE(WriteFile(fd, corrupted_content, sizeof(corrupted_content)));

  // Attempt to open partially corrupted ZIP file
  auto in_zip = InZip::Open(temp_zip_);

  // Depending on implementation, may partially succeed or fail completely
  if (in_zip) {
    // If opened successfully, verify at least one entry exists
    EXPECT_GT(in_zip->GetEntryCount(), static_cast<size_t>(0));
  } else {
    // Failure is also acceptable behavior
    SUCCEED()
        << "Partially corrupted ZIP file failed to open, which is acceptable";
  }
}

// Test Chinese filename support
TEST_F(InZipTest, ChineseFileName) {
  const std::string chinese_file = "中文文件.txt";
  const char content[] = "中文内容测试";

  // Create new ZIP file
  auto zip = OutZip::Create();
  auto data = std::make_unique<uint8_t[]>(sizeof(content));
  std::copy(content, content + sizeof(content), data.get());
  zip->Append(chinese_file, std::move(data), sizeof(content));
  ASSERT_TRUE(zip->Write(temp_zip_));

  // Open ZIP file
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  // Verify Chinese filename entry
  const auto* entry = in_zip->GetEntry(chinese_file);
  ASSERT_NE(entry, nullptr);

  // Load content
  auto loaded_data = in_zip->LoadEntry(entry);
  ASSERT_NE(loaded_data, nullptr);
  EXPECT_EQ(memcmp(loaded_data.get(), content, sizeof(content)), 0);
}

// Test long path support
TEST_F(InZipTest, LongPathSupport) {
  const std::string long_path =
      "a/very/long/path/that/exceeds/typical/limits/"
      "and/needs/to/be/tested/for/proper/handling.txt";
  const char content[] = "Long path test content";

  // Create new ZIP file
  auto zip = OutZip::Create();
  auto data = std::make_unique<uint8_t[]>(sizeof(content));
  std::copy(content, content + sizeof(content), data.get());
  zip->Append(long_path, std::move(data), sizeof(content));
  ASSERT_TRUE(zip->Write(temp_zip_));

  // Open ZIP file
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  // Verify long path entry
  const auto* entry = in_zip->GetEntry(long_path);
  ASSERT_NE(entry, nullptr);

  // Load content
  auto loaded_data = in_zip->LoadEntry(entry);
  ASSERT_NE(loaded_data, nullptr);
  EXPECT_EQ(memcmp(loaded_data.get(), content, sizeof(content)), 0);
}

// Test compression method recognition
TEST_F(InZipTest, CompressionMethodRecognition) {
  // Create file with store compression method
  auto store_zip = OutZip::Create(OutZip::CompressionMethod::kStore);
  auto store_data = std::make_unique<uint8_t[]>(sizeof(kTestContent));
  std::copy(kTestContent, kTestContent + sizeof(kTestContent),
            store_data.get());
  store_zip->Append("store.txt", std::move(store_data), sizeof(kTestContent));
  ASSERT_TRUE(store_zip->Write(temp_zip_));

  // Open ZIP file
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  // Verify compression method
  const auto* entry = in_zip->GetEntry("store.txt");
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->compression_method(), InZip::CompressionMethod::kStore);
}

// Test duplicate entries handling
TEST_F(InZipTest, DuplicateEntries) {
  // Create ZIP file with duplicate entries
  auto zip = OutZip::Create();

  // Add two files with same name
  auto data1 = std::make_unique<uint8_t[]>(sizeof(kTestContent));
  std::copy(kTestContent, kTestContent + sizeof(kTestContent), data1.get());
  zip->Append("duplicate.txt", std::move(data1), sizeof(kTestContent));

  auto data2 = std::make_unique<uint8_t[]>(sizeof(kTestContent2));
  std::copy(kTestContent2, kTestContent2 + sizeof(kTestContent2), data2.get());
  zip->Append("duplicate.txt", std::move(data2), sizeof(kTestContent2));

  ASSERT_TRUE(zip->Write(temp_zip_));

  // Open ZIP file
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  // Verify only one entry is returned (last added)
  auto entries = in_zip->List("");
  ASSERT_EQ(std::count(entries.begin(), entries.end(), "duplicate.txt"), 1);

  // Verify content is the last added content (kTestContent2)
  auto loaded_data = in_zip->LoadEntry("duplicate.txt");
  ASSERT_NE(loaded_data, nullptr);

  // Compare content excluding null terminator
  EXPECT_STREQ(reinterpret_cast<char*>(loaded_data.get()), kTestContent2);
}

}  // namespace
}  // namespace ash
