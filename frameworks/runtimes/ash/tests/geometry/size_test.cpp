#include "ash/geometry/size.h"
#include <gtest/gtest.h>
#include <cfloat>
#include <climits>

namespace ash {
namespace {

// ====================== SizeTest (int type) ======================
class SizeTest : public testing::Test {
 protected:
  void SetUp() override {}
};

TEST_F(SizeTest, DefaultConstructor) {
  Size size;
  EXPECT_EQ(size.width(), 0);
  EXPECT_EQ(size.height(), 0);
}

TEST_F(SizeTest, ParameterizedConstructor) {
  Size size(10, 20);
  EXPECT_EQ(size.width(), 10);
  EXPECT_EQ(size.height(), 20);
}

TEST_F(SizeTest, NegativeValues) {
  Size size(-5, -10);
  EXPECT_EQ(size.width(), -5);
  EXPECT_EQ(size.height(), -10);
}

TEST_F(SizeTest, MaxIntValues) {
  Size size(INT_MAX, INT_MIN);
  EXPECT_EQ(size.width(), INT_MAX);
  EXPECT_EQ(size.height(), INT_MIN);
}

TEST_F(SizeTest, EqualityOperatorTrue) {
  Size size1(100, 200);
  Size size2(100, 200);
  EXPECT_TRUE(size1 == size2);
}

TEST_F(SizeTest, EqualityOperatorFalseWidth) {
  Size size1(100, 200);
  Size size2(101, 200);
  EXPECT_FALSE(size1 == size2);
}

TEST_F(SizeTest, EqualityOperatorFalseHeight) {
  Size size1(100, 200);
  Size size2(100, 201);
  EXPECT_FALSE(size1 == size2);
}

TEST_F(SizeTest, InequalityOperatorTrueWidth) {
  Size size1(100, 200);
  Size size2(101, 200);
  EXPECT_TRUE(size1 != size2);
}

TEST_F(SizeTest, InequalityOperatorTrueHeight) {
  Size size1(100, 200);
  Size size2(100, 201);
  EXPECT_TRUE(size1 != size2);
}

TEST_F(SizeTest, InequalityOperatorFalse) {
  Size size1(100, 200);
  Size size2(100, 200);
  EXPECT_FALSE(size1 != size2);
}

TEST_F(SizeTest, ZeroSize) {
  Size size(0, 0);
  EXPECT_EQ(size.width(), 0);
  EXPECT_EQ(size.height(), 0);
}

// ====================== SizeFTest (float type) ======================
class SizeFTest : public testing::Test {
 protected:
  void SetUp() override {}
};

TEST_F(SizeFTest, DefaultConstructor) {
  SizeF size;
  EXPECT_FLOAT_EQ(size.width(), 0.0f);
  EXPECT_FLOAT_EQ(size.height(), 0.0f);
}

TEST_F(SizeFTest, ParameterizedConstructor) {
  SizeF size(10.5f, 20.7f);
  EXPECT_FLOAT_EQ(size.width(), 10.5f);
  EXPECT_FLOAT_EQ(size.height(), 20.7f);
}

TEST_F(SizeFTest, NegativeValues) {
  SizeF size(-5.5f, -10.25f);
  EXPECT_FLOAT_EQ(size.width(), -5.5f);
  EXPECT_FLOAT_EQ(size.height(), -10.25f);
}

TEST_F(SizeFTest, FloatPrecision) {
  SizeF size(0.1f + 0.2f, 1.0f / 3.0f);
  EXPECT_FLOAT_EQ(size.width(), 0.3f);
  EXPECT_FLOAT_EQ(size.height(), 1.0f / 3.0f);
}

TEST_F(SizeFTest, MaxFloatValues) {
  SizeF size(FLT_MAX, FLT_MIN);
  EXPECT_FLOAT_EQ(size.width(), FLT_MAX);
  EXPECT_FLOAT_EQ(size.height(), FLT_MIN);
}

TEST_F(SizeFTest, EqualityOperatorTrue) {
  SizeF size1(10.5f, 20.7f);
  SizeF size2(10.5f, 20.7f);
  EXPECT_TRUE(size1 == size2);
}

TEST_F(SizeFTest, EqualityOperatorFalseWidth) {
  SizeF size1(10.5f, 20.7f);
  SizeF size2(10.6f, 20.7f);
  EXPECT_FALSE(size1 == size2);
}

TEST_F(SizeFTest, EqualityOperatorFalseHeight) {
  SizeF size1(10.5f, 20.7f);
  SizeF size2(10.5f, 20.8f);
  EXPECT_FALSE(size1 == size2);
}

TEST_F(SizeFTest, InequalityOperatorTrue) {
  SizeF size1(10.5f, 20.7f);
  SizeF size2(10.5f, 20.71f);
  EXPECT_TRUE(size1 != size2);
}

TEST_F(SizeFTest, SpecialFloatValues) {
  SizeF size1(NAN, INFINITY);
  SizeF size2(NAN, INFINITY);
  // NaN != NaN, INFINITY == INFINITY
  EXPECT_FALSE(size1 == size2);
  EXPECT_TRUE(size1 != size2);
}

}  // namespace
}  // namespace ash
