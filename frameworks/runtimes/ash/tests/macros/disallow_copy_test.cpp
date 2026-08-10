#include "ash/macros/disallow_copy.h"
#include <gtest/gtest.h>
#include <type_traits>
#include <utility>

class CopyDisallowed {
 public:
  CopyDisallowed() = default;
  explicit CopyDisallowed(int value) : data(value) {}
  int get() const { return data; }

  ASH_DISALLOW_COPY(CopyDisallowed);

 private:
  int data = 0;
};

class CopyMoveDisallowed {
 public:
  CopyMoveDisallowed() = default;
  explicit CopyMoveDisallowed(int value) : data(value) {}
  int get() const { return data; }

  ASH_DISALLOW_COPY_AND_MOVE(CopyMoveDisallowed);

 private:
  int data = 0;
};

class BaseClass {
 public:
  virtual ~BaseClass() = default;
};

class DerivedDisallowCopy : public BaseClass {
 public:
  DerivedDisallowCopy() = default;

  ASH_DISALLOW_COPY(DerivedDisallowCopy);
};

class CustomConstructor {
 public:
  explicit CustomConstructor(int value) : data(value) {}

  ASH_DISALLOW_COPY(CustomConstructor);

 private:
  int data;
};

// Verify type traits for copy-disallowed but move-allowed class
TEST(DisallowCopyTest, TypeTraitsForCopyDisallowed) {
  EXPECT_FALSE(std::is_copy_constructible_v<CopyDisallowed>);
  EXPECT_FALSE(std::is_copy_assignable_v<CopyDisallowed>);
  EXPECT_TRUE(std::is_move_constructible_v<CopyDisallowed>);
  EXPECT_TRUE(std::is_move_assignable_v<CopyDisallowed>);
  EXPECT_TRUE(std::is_default_constructible_v<CopyDisallowed>);
}

// Verify type traits for copy-and-move-disallowed class
TEST(DisallowCopyTest, TypeTraitsForCopyMoveDisallowed) {
  EXPECT_FALSE(std::is_copy_constructible_v<CopyMoveDisallowed>);
  EXPECT_FALSE(std::is_copy_assignable_v<CopyMoveDisallowed>);
  EXPECT_FALSE(std::is_move_constructible_v<CopyMoveDisallowed>);
  EXPECT_FALSE(std::is_move_assignable_v<CopyMoveDisallowed>);
  EXPECT_TRUE(std::is_default_constructible_v<CopyMoveDisallowed>);
}

// Verify move operations are allowed
TEST(DisallowCopyTest, AllowsMoveOperations) {
  CopyDisallowed obj1(42);
  CopyDisallowed obj2(std::move(obj1));
  CopyDisallowed obj3(0);
  obj3 = std::move(obj2);

  EXPECT_EQ(obj3.get(), 42);
}

// Verify compatibility with parameterized constructors
TEST(DisallowCopyTest, WorksWithParameterizedConstructors) {
  CustomConstructor obj(100);
  SUCCEED();
}

// Verify behavior in inheritance hierarchy
TEST(DisallowCopyTest, WorksInInheritanceHierarchy) {
  DerivedDisallowCopy obj;
  BaseClass* base_ptr = &obj;
  (void)base_ptr;
  SUCCEED();
}

// Verify default constructibility
TEST(DisallowCopyTest, DefaultConstructible) {
  CopyDisallowed obj1;
  CopyMoveDisallowed obj2;
  EXPECT_TRUE(std::is_default_constructible_v<CopyDisallowed>);
  EXPECT_TRUE(std::is_default_constructible_v<CopyMoveDisallowed>);
}

// Verify source object state after move
TEST(DisallowCopyTest, SourceObjectAfterMove) {
  CopyDisallowed obj1(100);
  CopyDisallowed obj2(std::move(obj1));

  // Source object state is undefined after move, but should be safely
  // destructible
  SUCCEED();
}
