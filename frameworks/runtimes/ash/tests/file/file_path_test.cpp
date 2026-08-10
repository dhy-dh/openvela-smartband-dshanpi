#include "ash/file/file_path.h"
#include <gtest/gtest.h>

namespace ash {
namespace {
// Test absolute path construction
TEST(FilePathTest, ConstructFromString) {
  FilePath unix_path("/usr/local/bin");
  EXPECT_FALSE(unix_path.IsDirectory());
  EXPECT_EQ(unix_path.path(), "/usr/local/bin");

  FilePath unix_dir("/usr/local/bin/");
  EXPECT_TRUE(unix_dir.IsDirectory());
  EXPECT_EQ(unix_dir.path(), "/usr/local/bin/");

  FilePath win_path("C:\\Windows\\System32", false);
  EXPECT_FALSE(win_path.IsDirectory());

  FilePath win_dir("C:\\Windows\\System32\\", false);
  EXPECT_TRUE(win_dir.IsDirectory());

#if defined(__WINDOWS__)
  EXPECT_EQ(win_path.path(), "C:\\Windows\\System32");
  EXPECT_EQ(win_dir.path(), "C:\\Windows\\System32\\");
#else
  EXPECT_EQ(win_path.path(), "C:/Windows/System32");
  EXPECT_EQ(win_dir.path(), "C:/Windows/System32/");
#endif
}

// Test combined path constructor
TEST(FilePathTest, CombinePaths) {
  FilePath base("/var/log/");
  FilePath combined(base, "app.log");

  EXPECT_EQ(combined.path(), "/var/log/app.log");
}

// Test relative path construction
TEST(FilePathTest, RelativePath) {
  FilePath path("docs/report.pdf");

  EXPECT_TRUE(path.IsRelative());
  EXPECT_EQ(path.directory().path(), "docs/");
  EXPECT_EQ(path.basename(), "report");
  EXPECT_EQ(path.extension(), ".pdf");
  EXPECT_EQ(path.path(), "docs/report.pdf");
}

// Test relative path determination
TEST(FilePathTest, IsRelative) {
  FilePath relative("docs/readme.md");
  EXPECT_TRUE(relative.IsRelative());

  FilePath absolute("/var/log");
  EXPECT_FALSE(absolute.IsRelative());
}

// Test directory detection
TEST(FilePathTest, IsDirectory) {
  FilePath dir_path("/tmp/logs/");
  FilePath file_path("/tmp/log.txt");

  EXPECT_TRUE(dir_path.IsDirectory());
  EXPECT_FALSE(file_path.IsDirectory());
}

// Test directory component extraction
TEST(FilePathTest, DirectoryComponent) {
  FilePath path("/usr/local/bin/gcc");
  EXPECT_EQ(path.directory().path(), "/usr/local/bin/");

  FilePath win_path("C:\\Program Files\\Git");
  EXPECT_EQ(win_path.directory().path(), "C:/Program Files/");
}

// Test full filename extraction
TEST(FilePathTest, NameComponent) {
  FilePath path("/usr/local/bin/gcc");
  EXPECT_EQ(path.name(), "gcc");

  FilePath path_with_ext("/usr/bin/bash.exe");
  EXPECT_EQ(path_with_ext.name(), "bash.exe");
}

// Test basename extraction
TEST(FilePathTest, BasenameComponent) {
  FilePath path("/usr/local/bin/gcc");
  EXPECT_EQ(path.basename(), "gcc");

  FilePath path_with_ext("/usr/bin/bash.exe");
  EXPECT_EQ(path_with_ext.basename(), "bash");
}

// Test extension extraction
TEST(FilePathTest, ExtensionComponent) {
  FilePath path("/usr/bin/bash.exe");
  EXPECT_EQ(path.extension(), ".exe");

  FilePath path_no_ext("/usr/bin/bash");
  EXPECT_EQ(path_no_ext.extension(), "");
}

// Test basic path parsing
TEST(FilePathTest, BasicParsing) {
  FilePath path("/home/user/file.txt");

  EXPECT_FALSE(path.IsRelative());
  EXPECT_EQ(path.directory().path(), "/home/user/");
  EXPECT_EQ(path.basename(), "file");
  EXPECT_EQ(path.extension(), ".txt");
  EXPECT_EQ(path.path(), "/home/user/file.txt");
}

// Test Windows style path
#if defined(__WINDOWS__)
TEST(FilePathTest, WindowsStylePath) {
  FilePath path("C:\\Program Files\\app.exe", false);

  EXPECT_FALSE(path.IsRelative());
  EXPECT_EQ(path.directory().path(), "C:\\Program Files\\");
  EXPECT_EQ(path.basename(), "app");
  EXPECT_EQ(path.extension(), ".exe");
  EXPECT_EQ(path.path(), "C:\\Program Files\\app.exe");
}
#endif

// Test forcing Unix style
TEST(FilePathTest, ForceUnixStyle) {
  FilePath win_path("C:\\Users\\file", true);  // Force Unix style

  EXPECT_TRUE(win_path.IsRelative());
  EXPECT_EQ(win_path.directory().path(), "C:/Users/");
  EXPECT_EQ(win_path.basename(), "file");
  EXPECT_TRUE(win_path.extension().empty());
  EXPECT_EQ(win_path.path(), "C:/Users/file");
}

// Test filename and extension separation in multiple cases
TEST(FilePathTest, FilenameAndExtension) {
  FilePath f1("archive.tar.gz");
  FilePath f2(".hidden_file");
  FilePath f3("no_extension");

  EXPECT_EQ(f1.basename(), "archive.tar");
  EXPECT_EQ(f1.extension(), ".gz");
  EXPECT_EQ(f2.basename(), ".hidden_file");
  EXPECT_TRUE(f2.extension().empty());
  EXPECT_EQ(f3.basename(), "no_extension");
  EXPECT_TRUE(f3.extension().empty());
}

// Test edge cases
TEST(FilePathTest, EdgeCases) {
  // Empty path
  FilePath empty("");
  EXPECT_TRUE(empty.IsRelative());
  EXPECT_TRUE(empty.path().empty());

  // Root directory
  FilePath root("/");
  EXPECT_EQ(root.path(), "/");
  EXPECT_EQ(root.directory().path(), "/");
  EXPECT_TRUE(root.basename().empty());
  EXPECT_TRUE(root.extension().empty());
}

// Test directory names with dots
TEST(FilePathTest, DotsInDirectory) {
  FilePath path("/home/user.with.dots/file");
  EXPECT_EQ(path.directory().path(), "/home/user.with.dots/");
  EXPECT_EQ(path.basename(), "file");
}

// Test mixed separators
TEST(FilePathTest, MixedSeparators) {
  FilePath mixed("C:/Program\\ Files/My\\App/config.ini", false);

  EXPECT_EQ(mixed.path(), "C:/Program/ Files/My/App/config.ini");
  EXPECT_EQ(mixed.basename(), "config");
  EXPECT_EQ(mixed.extension(), ".ini");
}

}  // namespace
}  // namespace ash
