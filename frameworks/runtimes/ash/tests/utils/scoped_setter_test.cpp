#include "ash/utils/scoped_setter.h"
#include <gtest/gtest.h>
#include <string>

namespace ash {
namespace {

// Basic functionality test: Verifies setting and restoring
TEST(ScopedSetterTest, BasicFunctionality) {
  int value = 10;
  {
    ScopedSetter<int> setter(&value, 20);
    EXPECT_EQ(value, 20);  // Value modified inside scope
  }
  EXPECT_EQ(value, 10);  // Value restored after scope ends
}

TEST(ScopedSetterTest, BoolTypeRestore) {
  bool flag = false;

  {
    ScopedSetter<bool> setter(&flag, true);
    EXPECT_TRUE(flag);
  }

  EXPECT_FALSE(flag);
}

// Nested scopes test: Verifies multi-level restore order
TEST(ScopedSetterTest, NestedScopes) {
  int value = 5;
  {
    ScopedSetter<int> outer(&value, 15);
    EXPECT_EQ(value, 15);
    {
      ScopedSetter<int> inner(&value, 25);
      EXPECT_EQ(value, 25);
    }  // Inner scope ends, restores outer scope value
    EXPECT_EQ(value, 15);
  }  // Outer scope ends, restores original value
  EXPECT_EQ(value, 5);
}

// Multiple variables independent management test
TEST(ScopedSetterTest, MultipleVariables) {
  int a = 1, b = 2;
  {
    ScopedSetter<int> setter_a(&a, 10);
    ScopedSetter<int> setter_b(&b, 20);
    EXPECT_EQ(a, 10);
    EXPECT_EQ(b, 20);
  }  // Both setters go out of scope simultaneously
  EXPECT_EQ(a, 1);
  EXPECT_EQ(b, 2);
}

// Custom type test (Verifies non-POD type behavior)
TEST(ScopedSetterTest, CustomType) {
  struct Point {
    int x, y;
    bool operator==(const Point& other) const {
      return x == other.x && y == other.y;
    }
  };

  Point origin{0, 0};
  {
    ScopedSetter<Point> set_point(&origin, Point{3, 4});
    EXPECT_EQ(origin, (Point{3, 4}));
  }
  EXPECT_EQ(origin, (Point{0, 0}));
}

// String type test (Verifies complex type behavior)
TEST(ScopedSetterTest, StringType) {
  std::string str = "original";
  {
    ScopedSetter<std::string> set_str(&str, "temporary");
    EXPECT_EQ(str, "temporary");
  }
  EXPECT_EQ(str, "original");
}

}  // namespace
}  // namespace ash
