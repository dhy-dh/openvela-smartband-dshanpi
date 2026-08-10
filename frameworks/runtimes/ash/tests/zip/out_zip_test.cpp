#include "ash/zip/out_zip.h"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include "ash/file/file.h"
#include "ash/zip/in_zip.h"

namespace ash {
namespace {

constexpr char TestContent[] = "Hello, this is a test file content!";
constexpr char TestContent2[] = "Another test file content for verification";
constexpr char kTestPath[] = "test_dir/test_file.txt";

class OutZipTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a temporary directory
    const char* tmpdir = getenv("TMPDIR");
    if (!tmpdir)
      tmpdir = "/tmp";
    temp_dir_ = std::string(tmpdir) + "/out_zip_test_XXXXXX";

    // Create unique temporary directory using mkdtemp
    char* dir_template = const_cast<char*>(temp_dir_.c_str());
    char* result = mkdtemp(dir_template);
    ASSERT_TRUE(result != nullptr);
    temp_dir_ = result;

    temp_zip_ = temp_dir_ + "/test.zip";
  }

  void TearDown() override {
    // Recursively delete temporary directory
    DeleteDirectory(temp_dir_);
  }

  std::string temp_dir_;
  std::string temp_zip_;
};

// Test creating empty zip file
TEST_F(OutZipTest, CreateEmptyZip) {
  auto zip = OutZip::Create();
  ASSERT_TRUE(zip->Write(temp_zip_));

  FileInfo info;
  ASSERT_TRUE(GetFileInfo(temp_zip_, &info));
  ASSERT_GT(info.size, static_cast<size_t>(0));
}

// Test adding a single stored file
TEST_F(OutZipTest, AddSingleStoredFile) {
  auto zip = OutZip::Create();

  auto data = std::make_unique<uint8_t[]>(sizeof(TestContent));
  std::copy(TestContent, TestContent + sizeof(TestContent), data.get());

  zip->Append(kTestPath, std::move(data), sizeof(TestContent),
              OutZip::CompressionMethod::kStore);

  ASSERT_TRUE(zip->Write(temp_zip_));

  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);
  EXPECT_EQ(in_zip->GetEntryCount(), static_cast<size_t>(1));

  const auto* entry = in_zip->GetEntry(kTestPath);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->compression_method(), InZip::CompressionMethod::kStore);
  EXPECT_EQ(entry->uncompressed_size(), sizeof(TestContent));

  auto loaded_data = in_zip->LoadEntry(entry);
  ASSERT_NE(loaded_data, nullptr);
  EXPECT_EQ(memcmp(loaded_data.get(), TestContent, sizeof(TestContent)), 0);
}

// Test adding a single compressed file
TEST_F(OutZipTest, AddSingleDeflatedFile) {
  auto zip = OutZip::Create();

  auto data = std::make_unique<uint8_t[]>(sizeof(TestContent));
  std::copy(TestContent, TestContent + sizeof(TestContent), data.get());

  zip->Append(kTestPath, std::move(data), sizeof(TestContent),
              OutZip::CompressionMethod::kDeflate);

  ASSERT_TRUE(zip->Write(temp_zip_));

  // Verify ZIP contents
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);
  EXPECT_EQ(in_zip->GetEntryCount(), static_cast<size_t>(1));

  const auto* entry = in_zip->GetEntry(kTestPath);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->compression_method(), InZip::CompressionMethod::kDeflate);
  EXPECT_EQ(entry->uncompressed_size(), sizeof(TestContent));

  auto loaded_data = in_zip->LoadEntry(entry);
  ASSERT_NE(loaded_data, nullptr);
  EXPECT_EQ(memcmp(loaded_data.get(), TestContent, sizeof(TestContent)), 0);
}

// Test adding multiple files
TEST_F(OutZipTest, AddMultipleFiles) {
  auto zip = OutZip::Create();

  // Add stored file
  auto data1 = std::make_unique<uint8_t[]>(sizeof(TestContent));
  std::copy(TestContent, TestContent + sizeof(TestContent), data1.get());
  zip->Append("file1.txt", std::move(data1), sizeof(TestContent),
              OutZip::CompressionMethod::kStore);

  // Add compressed file
  auto data2 = std::make_unique<uint8_t[]>(sizeof(TestContent));
  std::copy(TestContent, TestContent + sizeof(TestContent), data2.get());
  zip->Append("file2.txt", std::move(data2), sizeof(TestContent),
              OutZip::CompressionMethod::kDeflate);

  ASSERT_TRUE(zip->Write(temp_zip_));

  // Verify ZIP contents
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);
  EXPECT_EQ(in_zip->GetEntryCount(), static_cast<size_t>(2));

  EXPECT_NE(in_zip->GetEntry("file1.txt"), nullptr);
  EXPECT_NE(in_zip->GetEntry("file2.txt"), nullptr);
}

// Test adding files using DataProvider
TEST_F(OutZipTest, AddWithDataProvider) {
  auto zip = OutZip::Create();

  zip->Append("provider.txt", [&]() {
    auto data = std::make_unique<uint8_t[]>(sizeof(TestContent));
    std::copy(TestContent, TestContent + sizeof(TestContent), data.get());
    return OutZip::Data{std::move(data), sizeof(TestContent)};
  });

  ASSERT_TRUE(zip->Write(temp_zip_));

  // Verify ZIP contents
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);
  EXPECT_EQ(in_zip->GetEntryCount(), static_cast<size_t>(1));

  const auto* entry = in_zip->GetEntry("provider.txt");
  ASSERT_NE(entry, nullptr);

  auto loaded_data = in_zip->LoadEntry(entry);
  ASSERT_NE(loaded_data, nullptr);
  EXPECT_STREQ(reinterpret_cast<char*>(loaded_data.get()), TestContent);
}

// Test Contains method
TEST_F(OutZipTest, ContainsPath) {
  auto zip = OutZip::Create();

  auto data = std::make_unique<uint8_t[]>(sizeof(TestContent));
  zip->Append(kTestPath, std::move(data), sizeof(TestContent));

  EXPECT_TRUE(zip->Contains(kTestPath));
  EXPECT_FALSE(zip->Contains("non_existent_path.txt"));
}

// Test adding empty file
TEST_F(OutZipTest, AddEmptyFile) {
  auto zip = OutZip::Create();

  auto data = std::make_unique<uint8_t[]>(0);
  zip->Append("empty.txt", std::move(data), 0);

  ASSERT_TRUE(zip->Write(temp_zip_));

  // Verify ZIP contents
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const auto* entry = in_zip->GetEntry("empty.txt");
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->uncompressed_size(), static_cast<size_t>(0));

  if (entry->compression_method() == InZip::CompressionMethod::kStore) {
    EXPECT_EQ(entry->compressed_size(), static_cast<size_t>(0));
  } else {
    EXPECT_EQ(entry->compressed_size(),
              static_cast<size_t>(2));  // Size for deflated empty file
  }
}

// Test compressing large files
TEST_F(OutZipTest, AddLargeFile) {
  auto zip = OutZip::Create();

  const size_t kLargeSize = 1024 * 1024;  // 1MB
  auto large_data = std::make_unique<uint8_t[]>(kLargeSize);
  std::fill(large_data.get(), large_data.get() + kLargeSize, 0x55);

  zip->Append("large.bin", std::move(large_data), kLargeSize,
              OutZip::CompressionMethod::kDeflate);

  ASSERT_TRUE(zip->Write(temp_zip_));

  // Verify ZIP contents
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const auto* entry = in_zip->GetEntry("large.bin");
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->uncompressed_size(), kLargeSize);
  EXPECT_LT(entry->compressed_size(), kLargeSize);
}

// Test default compression method
TEST_F(OutZipTest, DefaultCompressionMethod) {
  auto zip = OutZip::Create();

  const char kData[] = "Test default compression";
  auto data = std::make_unique<uint8_t[]>(sizeof(kData));
  memcpy(data.get(), kData, sizeof(kData));

  // Use default compression method
  zip->Append("default.txt", std::move(data), sizeof(kData));

  ASSERT_TRUE(zip->Write(temp_zip_));

  // Verify compression method (default should be kDeflate)
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const auto* entry = in_zip->GetEntry("default.txt");
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->compression_method(), InZip::CompressionMethod::kDeflate);
}

// Test mixed compression methods
TEST_F(OutZipTest, MixedCompressionMethods) {
  auto zip = OutZip::Create();

  // Add stored file
  auto data_store = std::make_unique<uint8_t[]>(sizeof(TestContent));
  std::copy(TestContent, TestContent + sizeof(TestContent), data_store.get());
  zip->Append("store.txt", std::move(data_store), sizeof(TestContent),
              OutZip::CompressionMethod::kStore);

  // Add compressed file
  auto data_deflate = std::make_unique<uint8_t[]>(sizeof(TestContent));
  std::copy(TestContent, TestContent + sizeof(TestContent), data_deflate.get());
  zip->Append("deflate.txt", std::move(data_deflate), sizeof(TestContent),
              OutZip::CompressionMethod::kDeflate);

  // Add file with default compression
  auto data_default = std::make_unique<uint8_t[]>(sizeof(TestContent));
  std::copy(TestContent, TestContent + sizeof(TestContent), data_default.get());
  zip->Append("default.txt", std::move(data_default), sizeof(TestContent));

  ASSERT_TRUE(zip->Write(temp_zip_));

  FileInfo info;
  ASSERT_TRUE(GetFileInfo(temp_zip_, &info));
  ASSERT_GT(info.size, static_cast<size_t>(0));
}

// Test writing to file descriptor
TEST_F(OutZipTest, WriteToFileDescriptor) {
  auto zip = OutZip::Create();

  auto data = std::make_unique<uint8_t[]>(sizeof(TestContent));
  std::copy(TestContent, TestContent + sizeof(TestContent), data.get());
  zip->Append(kTestPath, std::move(data), sizeof(TestContent));

  ScopedFD fd = OpenFile(temp_zip_, OpenMode::kWrite);
  ASSERT_TRUE(fd.IsValid());
  ASSERT_TRUE(zip->Write(fd));

  // Verify write operation
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);
  EXPECT_NE(in_zip->GetEntry(kTestPath), nullptr);
}

// Test writing to invalid file descriptor
TEST_F(OutZipTest, WriteToInvalidFileDescriptor) {
  auto zip = OutZip::Create();

  auto data = std::make_unique<uint8_t[]>(sizeof(TestContent));
  zip->Append(kTestPath, std::move(data), sizeof(TestContent));

  ScopedFD invalid_fd;
  ASSERT_FALSE(zip->Write(invalid_fd));
}

// Test writing to invalid path
TEST_F(OutZipTest, WriteToInvalidPath) {
  auto zip = OutZip::Create();

  auto data = std::make_unique<uint8_t[]>(sizeof(TestContent));
  zip->Append(kTestPath, std::move(data), sizeof(TestContent));

  // Attempt to write to non-existent directory
  ASSERT_FALSE(zip->Write("/invalid/path/test.zip"));
}

// Test directory operations
TEST_F(OutZipTest, DirectoryOperations) {
  // Create nested directory structure
  std::string nested_dir = temp_dir_ + "/a/b/c";
  ASSERT_TRUE(CreateDirectory(nested_dir));

  // Verify directory creation
  ASSERT_TRUE(IsDirectory(nested_dir));

  // Create test file
  std::string test_file = nested_dir + "/test.txt";
  const char content[] = "Test file content";
  ASSERT_TRUE(WriteFile(test_file, content, sizeof(content)));

  // Verify file write
  FileInfo info;
  ASSERT_TRUE(GetFileInfo(test_file, &info));
  ASSERT_EQ(info.size, sizeof(content));

  // Clean up
  ASSERT_TRUE(DeleteDirectory(temp_dir_ + "/a"));
}

// Test filenames with special characters
TEST_F(OutZipTest, SpecialCharacterFilenames) {
  auto zip = OutZip::Create();

  const char kData[] = "Special characters";
  auto data = std::make_unique<uint8_t[]>(sizeof(kData));
  memcpy(data.get(), kData, sizeof(kData));

  const std::string filename = "文件@#$%^&()_+-=[]{};',.txt";
  zip->Append(filename, std::move(data), sizeof(kData));

  ASSERT_TRUE(zip->Write(temp_zip_));

  // Verify ZIP contents
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  const auto* entry = in_zip->GetEntry(filename);
  ASSERT_NE(entry, nullptr);
}

// Test file size correctness
TEST_F(OutZipTest, FileSizeCorrectness) {
  auto zip = OutZip::Create();

  const size_t kSizes[] = {0, 1, 10, 100, 1000, 10000};
  for (size_t size : kSizes) {
    auto data = std::make_unique<uint8_t[]>(size);
    std::fill(data.get(), data.get() + size, 0x55);

    std::string filename = "size_" + std::to_string(size) + ".bin";
    zip->Append(filename, std::move(data), size);
  }

  ASSERT_TRUE(zip->Write(temp_zip_));

  // Verify ZIP contents
  auto in_zip = InZip::Open(temp_zip_);
  ASSERT_NE(in_zip, nullptr);

  for (size_t size : kSizes) {
    std::string filename = "size_" + std::to_string(size) + ".bin";
    const auto* entry = in_zip->GetEntry(filename);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->uncompressed_size(), size);
  }
}

}  // namespace
}  // namespace ash
