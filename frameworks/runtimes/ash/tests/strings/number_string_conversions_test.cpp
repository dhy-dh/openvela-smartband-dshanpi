#include "ash/strings/number_string_conversions.h"
#include <gtest/gtest.h>
#include <limits>

namespace ash {

// DoubleToString tests
// Regular cases
TEST(NumberStringConversionTest, DoubleToStringTest_NormalCases) {
  EXPECT_EQ(DoubleToString(123.456), "123.456");
  EXPECT_EQ(DoubleToString(-789.12), "-789.12");
}

// Boundary value tests
TEST(NumberStringConversionTest, DoubleToStringTest_BoundaryValues) {
  EXPECT_EQ(DoubleToString(0.0), "0");
  EXPECT_EQ(DoubleToString(1e308), "1e+308");
  EXPECT_EQ(DoubleToString(1e-308), "1e-308");
}

// Special values tests
TEST(NumberStringConversionTest, DoubleToStringTest_SpecialValues) {
  EXPECT_EQ(DoubleToString(std::numeric_limits<double>::infinity()), "inf");
  EXPECT_EQ(DoubleToString(-std::numeric_limits<double>::infinity()), "-inf");
  EXPECT_TRUE(
      DoubleToString(std::numeric_limits<double>::quiet_NaN()).find("nan") !=
      std::string::npos);
}

// Precision tests
TEST(NumberStringConversionTest, DoubleToStringTest_PrecisionCases) {
  EXPECT_EQ(DoubleToString(123.456789),
            "123.457");  // Default 6-digit precision
  EXPECT_EQ(DoubleToString(0.125), "0.125");
  EXPECT_EQ(DoubleToString(1.0 / 3.0),
            "0.333333");  // Default precision behavior
}

// IntegerToString tests
// Regular cases
TEST(NumberStringConversionTest, IntegerToStringTest_NormalCases) {
  EXPECT_EQ(IntegerToString(123), "123");
  EXPECT_EQ(IntegerToString(-456), "-456");
}

// Boundary value tests
TEST(NumberStringConversionTest, IntegerToStringTest_BoundaryValues) {
  EXPECT_EQ(IntegerToString(0), "0");
  EXPECT_EQ(IntegerToString(std::numeric_limits<int>::max()), "2147483647");
  EXPECT_EQ(IntegerToString(std::numeric_limits<int>::min()), "-2147483648");
}

// Special cases tests
TEST(NumberStringConversionTest, IntegerToStringTest_SpecialCases) {
  EXPECT_EQ(IntegerToString(1), "1");
  EXPECT_EQ(IntegerToString(-1), "-1");
  EXPECT_EQ(IntegerToString(-0), "0");  // Verify -0 handling
}

// IntToHexString tests
// Regular cases
TEST(NumberStringConversionTest, IntToHexStringTest_NormalCases) {
  EXPECT_EQ(IntToHexString(0x123, 4), "0123");
  EXPECT_EQ(IntToHexString(0x123, 2), "123");
}

// Boundary width tests
TEST(NumberStringConversionTest, IntToHexStringTest_BoundaryWidths) {
  EXPECT_EQ(IntToHexString(0, 4), "0000");
  EXPECT_EQ(IntToHexString(0xFFFFFFFF, 8), "ffffffff");
  EXPECT_EQ(IntToHexString(0x123, 1), "123");  // Minimum width
}

// Exceptional width tests
TEST(NumberStringConversionTest, IntToHexStringTest_ExceptionalWidths) {
  EXPECT_EQ(IntToHexString(0x123, 0), "123");  // Width 0 special handling
  EXPECT_EQ(IntToHexString(0x123, 0xFFFFFFFF), "123");  // Excessive width
}

// Special value tests
TEST(NumberStringConversionTest, IntToHexStringTest_SpecialValues) {
  EXPECT_EQ(IntToHexString(0, 4), "0000");
  EXPECT_EQ(IntToHexString(0xABC, 4), "0abc");
  EXPECT_EQ(IntToHexString(0x12345678, 8), "12345678");
}

// Exceptional scenarios tests
// NaN variants testing
TEST(NumberStringConversionTest, DoubleToStringTest_NaNVariants) {
  EXPECT_EQ(DoubleToString(std::numeric_limits<double>::quiet_NaN()), "nan");
}

}  // namespace ash
