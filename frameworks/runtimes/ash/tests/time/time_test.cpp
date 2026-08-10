#include "ash/time/time.h"
#include <gtest/gtest.h>
#include <limits>
#include "ash/time/duration.h"

namespace ash {
namespace {

constexpr int64_t kMaxInt64 = std::numeric_limits<int64_t>::max();

// Test default constructor and basic conversions
TEST(TimeTest, DefaultAndBasicConversions) {
  Time time;
  EXPECT_EQ(time.ToNano(), 0);
  EXPECT_DOUBLE_EQ(time.ToNanoF(), 0.0);
  EXPECT_EQ(time.ToMicro(), 0);
  EXPECT_DOUBLE_EQ(time.ToMicroF(), 0.0);
  EXPECT_EQ(time.ToMilli(), 0);
  EXPECT_DOUBLE_EQ(time.ToMilliF(), 0.0);
  EXPECT_EQ(time.ToSeconds(), 0);
  EXPECT_DOUBLE_EQ(time.ToSecondsF(), 0.0);
}

// Test FromNano functions
TEST(TimeTest, FromNano) {
  Time t1 = Time::FromNano(123'456'789);
  EXPECT_EQ(t1.ToNano(), 123'456'789);

  Time t2 = Time::FromNanoF(123'456'789.5);
  EXPECT_EQ(t2.ToNano(), 123'456'789);
}

// Test FromMicro functions
TEST(TimeTest, FromMicro) {
  Time t1 = Time::FromMicro(123'456);
  EXPECT_EQ(t1.ToNano(), 123'456'000);
  EXPECT_EQ(t1.ToMicro(), 123'456);

  Time t2 = Time::FromMicroF(123'456.789);
  EXPECT_EQ(t2.ToNano(), 123'456'789);
}

// Test FromMilli functions
TEST(TimeTest, FromMilli) {
  Time t1 = Time::FromMilli(123);
  EXPECT_EQ(t1.ToNano(), 123'000'000);
  EXPECT_EQ(t1.ToMilli(), 123);

  Time t2 = Time::FromMilliF(123.456);
  EXPECT_EQ(t2.ToNano(), 123'456'000);
}

// Test FromSeconds functions
TEST(TimeTest, FromSeconds) {
  Time t1 = Time::FromSeconds(42);
  EXPECT_EQ(t1.ToNano(), 42'000'000'000);
  EXPECT_EQ(t1.ToSeconds(), 42);

  Time t2 = Time::FromSecondsF(42.123456789);
  EXPECT_EQ(t2.ToNano(), 42'123'456'789);
}

// Test boundary value handling
TEST(TimeTest, BoundaryValues) {
  // Maximum integer value
  Time t1 = Time::FromNano(kMaxInt64);
  EXPECT_EQ(t1.ToNano(), kMaxInt64);

  // Float boundary (test overflow)
  Time t2 = Time::FromSecondsF(static_cast<double>(kMaxInt64) / 1e9);
  EXPECT_LT(t2.ToNano(),
            kMaxInt64);  // Float precision loss should result in smaller value

  // Negative input
  Time t3 = Time::FromNano(-100);
  EXPECT_GE(t3.ToNano(), -100);
}

// Test Now() function
TEST(TimeTest, Now) {
  Time now = Time::Now();
  EXPECT_GT(now.ToNano(), 0);
}

// Test Forever() function
TEST(TimeTest, Forever) {
  Time forever = Time::Forever();
  EXPECT_EQ(forever.ToNano(), kMaxInt64);

  Time now = Time::Now();
  EXPECT_LT(now.ToNano(), forever.ToNano());
}

// Test arithmetic operators
TEST(TimeTest, ArithmeticOperations) {
  Time base = Time::FromSeconds(100);
  Duration d = Duration::FromSeconds(25);

  // Addition
  Time added = base + d;
  EXPECT_EQ(added.ToSeconds(), 125);

  // Subtraction
  Time subtracted = base - d;
  EXPECT_EQ(subtracted.ToSeconds(), 75);

  // Time difference
  Time later = Time::FromSeconds(150);
  Duration diff = later - base;
  EXPECT_EQ(diff.ToSeconds(), 50);
}

// Test comparison operators
TEST(TimeTest, ComparisonOperators) {
  Time t1 = Time::FromSeconds(100);
  Time t2 = Time::FromSeconds(200);

  EXPECT_TRUE(t1 < t2);
  EXPECT_TRUE(t2 > t1);
  EXPECT_TRUE(t1 <= t1);
  EXPECT_TRUE(t2 >= t2);
  EXPECT_TRUE(t1 == t1);
  EXPECT_TRUE(t1 != t2);

  // Boundary comparison
  Time forever = Time::Forever();
  EXPECT_LT(t2, forever);
  EXPECT_NE(forever, t1);
}

// Test operator combinations
TEST(TimeTest, OperatorCombinations) {
  Time start = Time::FromSeconds(10);
  Duration d1 = Duration::FromSeconds(5);
  Duration d2 = Duration::FromSeconds(3);

  Time result = start + d1 - d2;
  EXPECT_EQ(result.ToSeconds(), 12);

  Time end = Time::FromSeconds(15);
  EXPECT_EQ((end - start).ToSeconds(), 5);
}

}  // namespace
}  // namespace ash
