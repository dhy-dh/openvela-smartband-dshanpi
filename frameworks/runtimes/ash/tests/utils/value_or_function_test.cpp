// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Warray-bounds"
#include "ash/utils/value_or_function.h"
#include <gtest/gtest.h>
#include <string>

namespace ash {
namespace {

// Test functionality with integer type
TEST(ValueOrFunctionTest, IntValue) {
  ValueOrFunction<int> vof(42);
  EXPECT_TRUE(vof.IsValue());
  EXPECT_FALSE(vof.IsFunction());
  EXPECT_EQ(vof.GetValue(), 42);
  EXPECT_EQ(vof(), 42);
}

TEST(ValueOrFunctionTest, IntFunction) {
  ValueOrFunction<int> vof([] { return 100; });
  EXPECT_FALSE(vof.IsValue());
  EXPECT_TRUE(vof.IsFunction());
  EXPECT_EQ(vof.GetFunction()(), 100);
  EXPECT_EQ(vof(), 100);
}

TEST(ValueOrFunctionTest, DefaultInt) {
  ValueOrFunction<int> vof;
  EXPECT_TRUE(vof.IsValue());
  EXPECT_EQ(vof(), 0);
}

TEST(ValueOrFunctionTest, IntTransformValue) {
  ValueOrFunction<int> original(10);
  auto transformed = original.Transform<int>([](int x) { return x * 2; });

  EXPECT_EQ(transformed(), 20);
}

TEST(ValueOrFunctionTest, IntTransformFunction) {
  ValueOrFunction<int> original([] { return 5; });
  auto transformed = original.Transform<int>([](int x) { return x + 3; });

  EXPECT_EQ(transformed(), 8);
  EXPECT_TRUE(transformed.IsFunction());
}

// Test functionality with string type
TEST(ValueOrFunctionTest, StringValue) {
  ValueOrFunction<std::string> vof("Hello");
  EXPECT_TRUE(vof.IsValue());
  EXPECT_EQ(vof(), "Hello");
}

TEST(ValueOrFunctionTest, StringFunction) {
  ValueOrFunction<std::string> vof([] { return "World"; });
  EXPECT_TRUE(vof.IsFunction());
  EXPECT_EQ(vof(), "World");
}

// TEST(ValueOrFunctionTest, StringTransformValue) {
//   ValueOrFunction<std::string> original("abc");
//   auto transformed = original.Transform<size_t>(
//       [](const std::string& s) { return s.length(); });

//   EXPECT_EQ(transformed(), static_cast<size_t>(3));
// }

// TEST(ValueOrFunctionTest, StringTransformFunction) {
//   ValueOrFunction<std::string> original([] { return "test"; });
//   auto transformed =
//       original.Transform<size_t>([](const std::string& s) { return s.size();
//       });

//   EXPECT_EQ(transformed(), static_cast<size_t>(4));
// }

// Test complex types and state transitions
// TEST(ValueOrFunctionTest, ChainedTransform) {
//   ValueOrFunction<int> first([] { return 2; });
//   auto second = first.Transform<float>([](int x) { return x * 1.5f; });
//   auto third =
//       second.Transform<std::string>([](float f) { return std::to_string(f);
//       });

//   EXPECT_EQ(third(), "3.000000");
// }

TEST(ValueOrFunctionTest, StatefulFunction) {
  int counter = 0;
  ValueOrFunction<int> vof([&counter] { return ++counter; });

  EXPECT_EQ(vof(), 1);
  EXPECT_EQ(vof(), 2);
  EXPECT_EQ(vof(), 3);
}

// Test move semantics
TEST(ValueOrFunctionTest, MoveSemantics) {
  ValueOrFunction<std::string> source([] { return "movable"; });
  ValueOrFunction<std::string> dest = std::move(source);

  EXPECT_EQ(dest(), "movable");
}

// Test empty function object
TEST(ValueOrFunctionTest, EmptyFunction) {
  ValueOrFunction<int> vof(std::function<int()>{});
  EXPECT_TRUE(vof.IsFunction());
}

// Test const correctness
TEST(ValueOrFunctionTest, ConstCorrectness) {
  const ValueOrFunction<int> cvof(99);
  EXPECT_EQ(cvof(), 99);

  const ValueOrFunction<int> cf([] { return 88; });
  EXPECT_EQ(cf(), 88);
}

}  // namespace
}  // namespace ash
   // #pragma GCC diagnostic pop
