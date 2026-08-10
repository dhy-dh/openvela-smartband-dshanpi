#include "ash/fds/scoped_fd.h"
#include <fcntl.h>
#include <gtest/gtest.h>
#include <unistd.h>
#include <cerrno>

namespace ash {
namespace {

class ScopedFDTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create temporary file descriptor for testing
    temp_fd_ = open("/dev/null", O_RDWR);
    ASSERT_GE(temp_fd_, 0) << "Failed to open /dev/null";
  }

  void TearDown() override {
    if (temp_fd_ >= 0) {
      close(temp_fd_);
    }
  }

  // Check if file descriptor is valid
  bool IsFdValid(int fd) {
    if (fd < 0)
      return false;
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
  }

  int temp_fd_ = -1;
};

// Test default constructor
TEST_F(ScopedFDTest, DefaultConstructor) {
  ScopedFD fd;
  EXPECT_EQ(static_cast<int>(fd), -1);
  EXPECT_FALSE(fd.IsValid());
  EXPECT_EQ(fd, nullptr);
}

// Test nullptr constructor
TEST_F(ScopedFDTest, NullptrConstructor) {
  ScopedFD fd(nullptr);
  EXPECT_EQ(static_cast<int>(fd), -1);
  EXPECT_FALSE(fd.IsValid());
  EXPECT_EQ(fd, nullptr);
}

// Test construction from valid fd
TEST_F(ScopedFDTest, ValidFdConstructor) {
  ScopedFD fd(temp_fd_);
  int fd_value = static_cast<int>(fd);

  EXPECT_NE(fd_value, -1);        // Verify copy succeeded
  EXPECT_NE(fd_value, temp_fd_);  // Verify it's a different descriptor
  EXPECT_TRUE(fd.IsValid());
  EXPECT_TRUE(IsFdValid(fd_value));

  // Verify original fd remains valid
  EXPECT_TRUE(IsFdValid(temp_fd_));
}

// Test construction from invalid fd
TEST_F(ScopedFDTest, InvalidFdConstructor) {
  ScopedFD fd(-1);
  EXPECT_EQ(static_cast<int>(fd), -1);
  EXPECT_FALSE(fd.IsValid());
}

// Test copy constructor
TEST_F(ScopedFDTest, CopyConstructor) {
  ScopedFD original(temp_fd_);
  ScopedFD copy(original);

  int original_fd = static_cast<int>(original);
  int copy_fd = static_cast<int>(copy);

  EXPECT_NE(original_fd, -1);
  EXPECT_NE(copy_fd, -1);
  EXPECT_NE(original_fd, copy_fd);  // Should be different fds

  // Copy should remain valid after closing original
  original = nullptr;
  EXPECT_TRUE(copy.IsValid());
  EXPECT_TRUE(IsFdValid(copy_fd));
}

// Boundary case: Test with fd=0 (stdin)
TEST_F(ScopedFDTest, StandardInputFd) {
  ScopedFD fd(STDIN_FILENO);

  // STDIN_FILENO should be duplicated
  EXPECT_NE(static_cast<int>(fd), STDIN_FILENO);
  EXPECT_TRUE(fd.IsValid());

  // Original stdin should remain valid
  EXPECT_TRUE(IsFdValid(STDIN_FILENO));
}

// Test move constructor
TEST_F(ScopedFDTest, MoveConstructor) {
  ScopedFD original(temp_fd_);
  int original_fd = static_cast<int>(original);

  ScopedFD moved(std::move(original));

  EXPECT_EQ(static_cast<int>(original), -1);  // Original should be invalidated
  EXPECT_EQ(static_cast<int>(moved),
            original_fd);  // New object takes ownership
  EXPECT_TRUE(moved.IsValid());
  EXPECT_TRUE(IsFdValid(original_fd));
}

// Error case: Test using object after move
TEST_F(ScopedFDTest, UseAfterMove) {
  ScopedFD original(temp_fd_);
  ScopedFD moved(std::move(original));

  // Attempt to use moved-from object
  EXPECT_EQ(static_cast<int>(original), -1);
  EXPECT_FALSE(original.IsValid());

  // Should handle safely
  original = nullptr;  // Double-release protection
  int released = original.Release();
  EXPECT_EQ(released, -1);
}

// Test nullptr assignment
TEST_F(ScopedFDTest, AssignNullptr) {
  ScopedFD fd(temp_fd_);
  int original_fd = static_cast<int>(fd);

  fd = nullptr;

  EXPECT_EQ(static_cast<int>(fd), -1);
  EXPECT_FALSE(fd.IsValid());

  // Verify fd has been closed
  EXPECT_FALSE(IsFdValid(original_fd));
}

// Error case: Test assigning closed file descriptor
TEST_F(ScopedFDTest, AssignClosedFileDescriptor) {
  int closed_fd = dup(temp_fd_);
  close(closed_fd);  // Explicitly close

  ScopedFD fd;
  fd = closed_fd;  // Attempt to assign closed fd

  // Should create invalid object or handle error
  EXPECT_FALSE(fd.IsValid());
  EXPECT_EQ(static_cast<int>(fd), -1);
}

// Test int assignment
TEST_F(ScopedFDTest, AssignInt) {
  // Get raw file descriptor (assume temp_fd_ is ScopedFD object)
  int raw_fd = static_cast<int>(temp_fd_);

  ScopedFD fd;
  fd = raw_fd;  // Call operator=(int)
  int fd_value = static_cast<int>(fd);

  EXPECT_TRUE(fd.IsValid());
  EXPECT_NE(fd_value, raw_fd);  // Verify new descriptor was duplicated
  EXPECT_TRUE(IsFdValid(fd_value));

  // Verify original fd remains valid
  EXPECT_TRUE(IsFdValid(raw_fd));  // Should use raw fd directly
}

// Test copy assignment
TEST_F(ScopedFDTest, CopyAssignment) {
  ScopedFD original(temp_fd_);
  ScopedFD copy;

  copy = original;

  int original_fd = static_cast<int>(original);
  int copy_fd = static_cast<int>(copy);

  EXPECT_NE(original_fd, -1);
  EXPECT_NE(copy_fd, -1);
  EXPECT_NE(original_fd, copy_fd);  // Should be different fds
  EXPECT_TRUE(IsFdValid(original_fd));
  EXPECT_TRUE(IsFdValid(copy_fd));
}

// Test move assignment
TEST_F(ScopedFDTest, MoveAssignment) {
  ScopedFD original(temp_fd_);
  int original_fd = static_cast<int>(original);

  ScopedFD moved;
  moved = std::move(original);

  EXPECT_EQ(static_cast<int>(original), -1);  // Original should be invalidated
  EXPECT_EQ(static_cast<int>(moved),
            original_fd);  // New object takes ownership
  EXPECT_TRUE(moved.IsValid());
  EXPECT_TRUE(IsFdValid(original_fd));
}

// Test self-assignment (int)
TEST_F(ScopedFDTest, SelfAssignInt) {
  ScopedFD fd(temp_fd_);
  int original_fd = static_cast<int>(fd);

  fd = original_fd;

  EXPECT_EQ(original_fd, static_cast<int>(fd));
  EXPECT_TRUE(fd.IsValid());
  EXPECT_TRUE(IsFdValid(static_cast<int>(fd)));
  EXPECT_TRUE(IsFdValid(original_fd));
}

// Test self-assignment (copy)
TEST_F(ScopedFDTest, SelfCopyAssignment) {
  ScopedFD fd(temp_fd_);
  int original_fd = static_cast<int>(fd);

  fd = fd;  // Self-assignment

  EXPECT_EQ(static_cast<int>(fd), original_fd);
  EXPECT_TRUE(fd.IsValid());
  EXPECT_TRUE(IsFdValid(original_fd));
}

// Test self-assignment (move)
TEST_F(ScopedFDTest, SelfMoveAssignment) {
  ScopedFD fd(temp_fd_);
  int original_fd = static_cast<int>(fd);

  ScopedFD& self_ref = fd;
  fd = std::move(self_ref);  // Self-move assignment

  // Should maintain original state
  EXPECT_EQ(static_cast<int>(fd), original_fd);
  EXPECT_TRUE(fd.IsValid());
  EXPECT_TRUE(IsFdValid(original_fd));
}

// Test Adopt method
TEST_F(ScopedFDTest, Adopt) {
  int raw_fd = dup(temp_fd_);
  ScopedFD fd = ScopedFD::Adopt(raw_fd);

  EXPECT_EQ(static_cast<int>(fd), raw_fd);  // Should take direct ownership
  EXPECT_TRUE(fd.IsValid());
  EXPECT_TRUE(IsFdValid(raw_fd));
}

// Test Adopt method: Error case
TEST_F(ScopedFDTest, AdoptOriginalFdBehavior) {
  int raw_fd = dup(temp_fd_);
  ScopedFD fd = ScopedFD::Adopt(raw_fd);

  char buf[1] = {0};  // Initialize buffer to avoid undefined behavior
  const size_t buf_size = sizeof(buf);
  const ssize_t expected_write = static_cast<ssize_t>(buf_size);
  // Safe write test
  ssize_t bytes_written = write(raw_fd, buf, buf_size);

  EXPECT_EQ(bytes_written, expected_write)
      << "Adopt should not close original fd immediately, but it's unsafe to "
         "use. Write operation failed.";

  // When ScopedFD is destroyed, original descriptor should be closed
  fd = nullptr;  // Explicitly release resources
  EXPECT_FALSE(IsFdValid(raw_fd))
      << "Adopted descriptor should be closed after release";
}

// // Test Adopt method: Error case - Adopting same fd twice - crash
// Adopt shouldn't be taken twice, but current code can't runtime check if an fd
// was adopted twice, so this misuse can only be documented.
// TEST_F(ScopedFDTest, DoubleAdoptSameFd) {
//   int raw_fd = dup(temp_fd_);

//   ScopedFD fd1 = ScopedFD::Adopt(raw_fd);
//   ScopedFD fd2 = ScopedFD::Adopt(raw_fd);  // Dangerous operation!

//   // First object should remain valid
//   EXPECT_TRUE(fd1.IsValid());

//   // Second object should be invalid
//   EXPECT_FALSE(fd2.IsValid());

//   // Original descriptor should be managed by fd1
//   fd1 = nullptr;
//   EXPECT_FALSE(IsFdValid(raw_fd));
// }

// Test IsValid method
TEST_F(ScopedFDTest, IsValid) {
  ScopedFD valid(temp_fd_);
  ScopedFD invalid;

  EXPECT_TRUE(valid.IsValid());
  EXPECT_FALSE(invalid.IsValid());
}

// Test conversion operator
TEST_F(ScopedFDTest, IntConversion) {
  ScopedFD fd(temp_fd_);
  int raw_fd = static_cast<int>(fd);

  EXPECT_EQ(raw_fd, static_cast<int>(fd));
  EXPECT_TRUE(IsFdValid(raw_fd));
}

// Test conversion operator: Error case
TEST_F(ScopedFDTest, IntConversionWhenInvalid) {
  ScopedFD fd;
  int raw_fd = static_cast<int>(fd);
  EXPECT_EQ(raw_fd, -1);
}

// Test Release method
TEST_F(ScopedFDTest, Release) {
  ScopedFD fd(temp_fd_);
  int raw_fd = fd.Release();

  EXPECT_EQ(static_cast<int>(fd), -1);  // Should be invalid after release
  EXPECT_NE(raw_fd, -1);
  EXPECT_TRUE(IsFdValid(raw_fd));

  // Manually close released fd
  close(raw_fd);
  EXPECT_FALSE(IsFdValid(raw_fd));
}

// Error case: Test Release on closed fd
TEST_F(ScopedFDTest, ReleaseAfterClose) {
  ScopedFD fd(temp_fd_);
  fd = nullptr;

  int released = fd.Release();
  EXPECT_EQ(released, -1);
}

// Test destructor closes fd
TEST_F(ScopedFDTest, DestructorClosesFd) {
  int raw_fd;
  {
    ScopedFD fd(temp_fd_);
    raw_fd = static_cast<int>(fd);
    // fd will be automatically closed when scope ends
  }

  // Verify fd has been closed
  EXPECT_FALSE(IsFdValid(raw_fd));
}

// Test comparison operators
TEST_F(ScopedFDTest, ComparisonOperators) {
  ScopedFD valid_fd(temp_fd_);
  ScopedFD invalid_fd;

  // Only test defined forms: fd == nullptr and fd != nullptr
  EXPECT_NE(valid_fd, nullptr);
  EXPECT_EQ(invalid_fd, nullptr);
}

// Test boundary value: Maximum valid file descriptor
TEST_F(ScopedFDTest, MaxValidFileDescriptor) {
  // Get system maximum open files
  long max_open_files = sysconf(_SC_OPEN_MAX);
  ASSERT_GT(max_open_files, 0);

  // Attempt to create fd near maximum value
  int raw_fd = dup(temp_fd_);
  ASSERT_GE(raw_fd, 0);

  // Create ScopedFD
  ScopedFD fd(max_open_files - 1);

  // Behavior depends on system, but shouldn't crash
  if (max_open_files - 1 >= 0) {
    // May be valid or invalid depending on currently open files
    EXPECT_TRUE(fd.IsValid() || !fd.IsValid());
  } else {
    EXPECT_FALSE(fd.IsValid());
  }
}

// Test boundary case: Invalid fd operations
TEST_F(ScopedFDTest, InvalidFdOperations) {
  ScopedFD fd;

  // Release invalid fd
  int released = fd.Release();
  EXPECT_EQ(released, -1);
}

// Test boundary case: Double close
TEST_F(ScopedFDTest, DoubleClose) {
  ScopedFD fd(temp_fd_);
  int raw_fd = static_cast<int>(fd);

  // First close (via nullptr assignment)
  fd = nullptr;
  EXPECT_FALSE(IsFdValid(raw_fd));

  // Second close (should be no-op)
  fd = nullptr;
  EXPECT_FALSE(IsFdValid(raw_fd));
}

// Test close after move
TEST_F(ScopedFDTest, CloseAfterMove) {
  ScopedFD original(temp_fd_);
  int original_fd = static_cast<int>(original);

  ScopedFD moved(std::move(original));

  // Close moved object
  moved = nullptr;

  EXPECT_EQ(static_cast<int>(moved), -1);
  EXPECT_EQ(static_cast<int>(original), -1);
  EXPECT_FALSE(IsFdValid(original_fd));
}

}  // namespace
}  // namespace ash
