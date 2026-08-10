#include "ash/time/duration.h"
#include <gtest/gtest.h>

namespace ash {

class DurationTest : public testing::Test {
 protected:
  void SetUp() override {
    zero_ = Duration::Zero();
    inf_ = Duration::Infinity();
  }

  Duration zero_;
  Duration inf_;
};

// Test default constructor
TEST_F(DurationTest, DefaultConstructor) {
  Duration d;
  EXPECT_EQ(d.ToNano(), 0);
}

// Test nanosecond conversions
TEST_F(DurationTest, NanoConversions) {
  // Integer version
  Duration d1 = Duration::FromNano(123456789);
  EXPECT_EQ(d1.ToNano(), 123456789);
  EXPECT_EQ(d1.ToMicro(), 123456);
  EXPECT_EQ(d1.ToMilli(), 123);
  EXPECT_EQ(d1.ToSeconds(), 0);

  // Floating-point version
  Duration d2 = Duration::FromNanoF(123456789.5);
  EXPECT_EQ(d2.ToNano(), 123456789);  // Truncate decimal part
  EXPECT_DOUBLE_EQ(d2.ToNanoF(), 123456789.0);
}

// Test microsecond conversions
TEST_F(DurationTest, MicroConversions) {
  // Integer version
  Duration d1 = Duration::FromMicro(123456);
  EXPECT_EQ(d1.ToNano(), 123456000);
  EXPECT_EQ(d1.ToMicro(), 123456);
  EXPECT_DOUBLE_EQ(d1.ToMicroF(), 123456.0);

  // Floating-point version (with decimal)
  Duration d2 = Duration::FromMicroF(123.456);
  EXPECT_EQ(d2.ToNano(), 123456);  // 123.456 * 1000 = 123456
  EXPECT_DOUBLE_EQ(d2.ToMicroF(), 123.456);
}

// Test millisecond conversions
TEST_F(DurationTest, MilliConversions) {
  // Integer version
  Duration d1 = Duration::FromMilli(123);
  EXPECT_EQ(d1.ToNano(), 123000000);
  EXPECT_EQ(d1.ToMilli(), 123);
  EXPECT_DOUBLE_EQ(d1.ToMilliF(), 123.0);

  // Floating-point version (with decimal)
  Duration d2 = Duration::FromMilliF(123.456);
  EXPECT_EQ(d2.ToNano(), 123456000);  // 123.456 * 1000000 = 123456000
  EXPECT_DOUBLE_EQ(d2.ToMilliF(), 123.456);
}

// Test second conversions
TEST_F(DurationTest, SecondConversions) {
  // Integer version
  Duration d1 = Duration::FromSeconds(5);
  EXPECT_EQ(d1.ToNano(), 5000000000);
  EXPECT_EQ(d1.ToSeconds(), 5);
  EXPECT_DOUBLE_EQ(d1.ToSecondsF(), 5.0);

  // Floating-point version (with decimal)
  Duration d2 = Duration::FromSecondsF(2.5);
  EXPECT_EQ(d2.ToNano(), 2500000000);  // 2.5 * 1e9 = 2500000000
  EXPECT_DOUBLE_EQ(d2.ToSecondsF(), 2.5);
}

// Test boundary values
TEST_F(DurationTest, BoundaryValues) {
  // Zero value
  EXPECT_EQ(zero_.ToNano(), 0);
  EXPECT_TRUE(zero_ == Duration::FromNano(0));

  // Infinity value
  const int64_t max_int64 = std::numeric_limits<int64_t>::max();
  EXPECT_EQ(inf_.ToNano(), max_int64);
  EXPECT_TRUE(inf_ == Duration::FromNano(max_int64));

  // Negative value conversion
  Duration neg = Duration::FromNano(-1000);
  EXPECT_EQ(neg.ToMicro(), -1);
  EXPECT_DOUBLE_EQ(neg.ToMicroF(), -1.0);
}

// Test operators
TEST_F(DurationTest, Operators) {
  Duration d1 = Duration::FromSeconds(10);
  Duration d2 = Duration::FromSeconds(5);

  // Equality/inequality operators
  EXPECT_TRUE(d1 != d2);
  EXPECT_TRUE(d1 == Duration::FromSeconds(10));

  // Addition operator
  Duration sum = d1 + d2;
  EXPECT_EQ(sum.ToSeconds(), 15);

  // Subtraction operator
  Duration diff = d1 - d2;
  EXPECT_EQ(diff.ToSeconds(), 5);

  // Special operations with zero/infinity
  EXPECT_EQ(d1 + zero_, d1);
}

// Test mixed-unit operations
TEST_F(DurationTest, MixedUnitOperations) {
  Duration nano = Duration::FromNano(500);
  Duration micro = Duration::FromMicro(1);
  Duration milli = Duration::FromMilli(1);
  Duration sec = Duration::FromSeconds(1);

  // Mixed addition
  Duration sum1 = nano + micro;
  EXPECT_EQ(sum1.ToNano(), 1500);

  // Mixed subtraction
  Duration diff1 = milli - micro;
  EXPECT_EQ(diff1.ToMicro(), 999);

  // Cross-unit comparisons
  EXPECT_TRUE(Duration::FromMilli(1000) == sec);
  EXPECT_TRUE(Duration::FromMicro(1000000) == sec);
}

}  // namespace ash
