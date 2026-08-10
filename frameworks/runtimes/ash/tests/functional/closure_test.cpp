#include "ash/functional/closure.h"
#include <gtest/gtest.h>
#include <memory>

namespace ash {
namespace {

// Test case: Basic functionality of Closure
TEST(ClosureTest, BasicFunctionality) {
  int counter = 0;
  Closure closure = [&] { counter++; };

  closure();
  EXPECT_EQ(counter, 1);

  closure();
  EXPECT_EQ(counter, 2);
}

// Test case: Closure with return value
TEST(ClosureTest, WithReturnValue) {
  int value = 0;
  Closure closure = [&] { value = 42; };
  closure();
  EXPECT_EQ(value, 42);
}

// Test case: Verify if closure is empty
TEST(ClosureTest, CheckEmptyClosure) {
  Closure empty;
  EXPECT_FALSE(static_cast<bool>(empty));
}

// Test case: Copy semantics of Closure
TEST(ClosureTest, CopySemantics) {
  int counter = 0;
  Closure original = [&] { counter++; };
  Closure copy = original;

  original();
  EXPECT_EQ(counter, 1);

  copy();
  EXPECT_EQ(counter, 2);
}

// Test case: Basic functionality of OnceClosure
TEST(OnceClosureTest, BasicFunctionality) {
  int counter = 0;
  OnceClosure closure = [&] { counter = 42; };

  std::move(closure)();
  EXPECT_EQ(counter, 42);
}

// Test case: Move assignment of OnceClosure
TEST(OnceClosureTest, MoveAssignment) {
  int counter = 0;
  OnceClosure source = [&] { counter = 100; };
  OnceClosure target;

  target = std::move(source);
  std::move(target)();
  EXPECT_EQ(counter, 100);
}

// Test case: Resource cleanup in OnceClosure
TEST(OnceClosureTest, ResourceCleanup) {
  struct Resource {
    bool* destroyed;
    ~Resource() { *destroyed = true; }
  };

  bool destroyed = false;
  {
    OnceClosure closure = [res = Resource{&destroyed}] {};
    std::move(closure)();
  }
  EXPECT_TRUE(destroyed);
}

// Test case: Value capture in OnceClosure
TEST(OnceClosureTest, CaptureByValue) {
  int value = 10;
  OnceClosure closure = [value] { EXPECT_EQ(value, 10); };
  std::move(closure)();
}

// Test case: Reference capture in OnceClosure
TEST(OnceClosureTest, CaptureByReference) {
  int value = 10;
  OnceClosure closure = [&value] { value = 20; };
  std::move(closure)();
  EXPECT_EQ(value, 20);
}

// Closure containing OnceClosure (using shared pointer)
TEST(ClosureConversionTest, ClosureContainingOnce) {
  int counter = 0;

  // Manage OnceClosure lifecycle with shared pointer
  auto once_ptr = std::make_shared<OnceClosure>([&] { counter = 5; });

  Closure closure = [once_ptr]() mutable { std::move (*once_ptr)(); };

  closure();
  EXPECT_EQ(counter, 5);
}

// Multiple calls to contained OnceClosure (using shared pointer)
TEST(ClosureConversionTest, MultipleCallsToContainedOnce) {
  int counter = 0;

  // Manage lifecycle with make_shared
  auto once_ptr = std::make_shared<OnceClosure>([&] { counter++; });

  Closure closure = [once_ptr]() mutable {
    // Create local copy for each call
    if (auto local_once = std::exchange(once_ptr, nullptr)) {
      std::move (*local_once)();
    }
  };

  closure();
  EXPECT_EQ(counter, 1);

  // Subsequent calls have no effect
  closure();
  EXPECT_EQ(counter, 1);
}

// Test case: Moving Closure
TEST(ClosureTest, MoveClosure) {
  int counter = 0;
  Closure original = [&] { counter++; };
  Closure moved = std::move(original);

  moved();
  EXPECT_EQ(counter, 1);

  // Original object should still be invokable (std::function guarantee)
  original();
  EXPECT_EQ(counter, 2);
}

// Test case: Large capture object
TEST(ClosureTest, LargeCapture) {
  struct LargeObject {
    char data[1024];
  };

  LargeObject obj;
  Closure closure = [&obj] {};
  closure();  // Verify no crash
}

}  // namespace
}  // namespace ash
