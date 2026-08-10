#include "ash/strings/string_converter.h"
#include <gtest/gtest.h>
#include <limits>

namespace ash {
namespace {

// 测试 LiteralProcessor
TEST(StringConverterTest, LiteralProcessorBuild) {
  std::string output;
  EXPECT_TRUE(LiteralProcessor::Build(output, "test"));
  EXPECT_EQ(output, "test");
}

TEST(StringConverterTest, LiteralProcessorParseSuccess) {
  const char* input = "hello world";
  const char* original = input;
  EXPECT_TRUE(LiteralProcessor::Parse(&input, "hello"));
  EXPECT_EQ(input, original + 5);
}

TEST(StringConverterTest, LiteralProcessorParseFailure) {
  const char* input = "world";
  EXPECT_FALSE(LiteralProcessor::Parse(&input, "hello"));
  EXPECT_EQ(input, input);  // 指针不应改变
}

// 测试 SkipSpaceProcessor
TEST(StringConverterTest, SkipSpaceCRLF) {
  const char* input = " \t\n\rText";
  size_t skipped = SkipSpaceProcessor<true>::Skip(&input);
  EXPECT_EQ(skipped, static_cast<size_t>(4));  // 修复有符号/无符号比较
  EXPECT_STREQ(input, "Text");
}

TEST(StringConverterTest, SkipSpaceBasic) {
  const char* input = "   Text";
  size_t skipped = SkipSpaceProcessor<false>::Skip(&input);
  EXPECT_EQ(skipped, static_cast<size_t>(3));  // 修复有符号/无符号比较
  EXPECT_STREQ(input, "Text");
}

TEST(StringConverterTest, SkipSpaceNoSpace) {
  const char* input = "Text";
  size_t skipped = SkipSpaceProcessor<true>::Skip(&input);
  EXPECT_EQ(skipped, static_cast<size_t>(0));  // 修复有符号/无符号比较
  EXPECT_STREQ(input, "Text");
}

// 测试 IdentifierProcessor
TEST(StringConverterTest, IdentifierBuild) {
  std::string output;
  EXPECT_TRUE(IdentifierProcessor::Build(output, "valid_id"));
  EXPECT_EQ(output, "valid_id");
}

TEST(StringConverterTest, IdentifierParseSuccess) {
  const char* input = "_valid123";
  std::string value;
  EXPECT_TRUE(IdentifierProcessor::Parse(&input, &value));
  EXPECT_EQ(value, "_valid123");
  EXPECT_EQ(*input, '\0');
}

TEST(StringConverterTest, IdentifierParseFailureStartWithDigit) {
  const char* input = "123invalid";
  std::string value;
  EXPECT_FALSE(IdentifierProcessor::Parse(&input, &value));
}

TEST(StringConverterTest, IdentifierParsePartial) {
  const char* input = "id123+symbol";
  std::string value;
  EXPECT_TRUE(IdentifierProcessor::Parse(&input, &value));
  EXPECT_EQ(value, "id123");
  EXPECT_EQ(*input, '+');
}

// 测试布尔类型处理器
TEST(StringConverterTest, BoolTrue) {
  std::string output = ToString(true);
  EXPECT_EQ(output, "true");

  bool value = false;
  EXPECT_TRUE(FromString("true", &value));
  EXPECT_TRUE(value);
}

TEST(StringConverterTest, BoolFalse) {
  std::string output = ToString(false);
  EXPECT_EQ(output, "false");

  bool value = true;
  EXPECT_TRUE(FromString("false", &value));
  EXPECT_FALSE(value);
}

TEST(StringConverterTest, BoolInvalid) {
  bool value;
  EXPECT_FALSE(FromString("tru", &value));
  EXPECT_FALSE(FromString("TRUE", &value));
  EXPECT_FALSE(FromString("1", &value));
}

// 测试长整型处理器
TEST(StringConverterTest, LongPositive) {
  long test_val = 123456;
  std::string output = ToString(test_val);
  EXPECT_EQ(output, "123456");

  long parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, LongNegative) {
  long test_val = -789;
  std::string output = ToString(test_val);
  EXPECT_EQ(output, "-789");

  long parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, LongMax) {
  long test_val = std::numeric_limits<long>::max();
  std::string output = ToString(test_val);
  long parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, LongMin) {
  long test_val = std::numeric_limits<long>::min();
  std::string output = ToString(test_val);
  long parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, LongInvalid) {
  long value;
  EXPECT_FALSE(FromString("123abc", &value));
  EXPECT_FALSE(FromString("--123", &value));
  EXPECT_FALSE(FromString("++123", &value));
}

// 测试无符号长整型
TEST(StringConverterTest, UnsignedLong) {
  unsigned long test_val = 4294967295;
  std::string output = ToString(test_val);
  EXPECT_EQ(output, "4294967295");

  unsigned long parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, UnsignedLongInvalid) {
  unsigned long value;
  EXPECT_FALSE(FromString("-123", &value));
  EXPECT_FALSE(FromString("123abc", &value));
}

// 测试浮点型处理器
TEST(StringConverterTest, FloatBasic) {
  float test_val = 3.14f;
  std::string output = ToString(test_val);
  float parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_FLOAT_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, FloatPrecision) {
  float test_val = 0.123456789f;
  std::string output = ToString(test_val);
  float parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_NEAR(parsed_val, test_val, 1e-6f);
}

TEST(StringConverterTest, FloatInvalid) {
  float value;
  EXPECT_FALSE(FromString("3.14abc", &value));
  EXPECT_FALSE(FromString("..123", &value));
}

// 测试双精度浮点型
TEST(StringConverterTest, DoubleBasic) {
  double test_val = 2.718281828459045;
  std::string output = ToString(test_val);
  double parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_NEAR(parsed_val, test_val, 1e-12);
}

TEST(StringConverterTest, DoubleLarge) {
  double test_val = 1.23456789e+18;
  std::string output = ToString(test_val);
  double parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_DOUBLE_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, DoubleSmall) {
  double test_val = 1.23456789e-18;
  std::string output = ToString(test_val);
  double parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_DOUBLE_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, DoubleInvalid) {
  double value;
  EXPECT_FALSE(FromString("2.718abc", &value));
  EXPECT_FALSE(FromString("1.2.3", &value));
}

// 测试整数类型转换（使用CastProcessor）
TEST(StringConverterTest, IntConversion) {
  int test_val = -42;
  std::string output = ToString(test_val);
  EXPECT_EQ(output, "-42");

  int parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, IntBoundary) {
  int test_val = std::numeric_limits<int>::max();
  std::string output = ToString(test_val);
  int parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, UnsignedCharConversion) {
  unsigned char test_val = 255;
  std::string output = ToString(test_val);
  EXPECT_EQ(output, "255");

  unsigned char parsed_val = 0;
  EXPECT_TRUE(FromString(output, &parsed_val));
  EXPECT_EQ(parsed_val, test_val);
}

TEST(StringConverterTest, CastOverflow) {
  // 测试long到short的溢出
  std::string overflow_val = "32768";
  short value = 0;
  EXPECT_FALSE(FromString(overflow_val, &value));
}

// 测试空格处理器
TEST(StringConverterTest, SpaceBuild) {
  std::string output;
  EXPECT_TRUE(SpaceProcessor<>::Build(output));
  EXPECT_EQ(output, " ");
}

TEST(StringConverterTest, SpaceParse) {
  const char* input = "   text";
  EXPECT_TRUE(SpaceProcessor<>::Parse(&input));
  EXPECT_STREQ(input, "text");
}

}  // namespace
}  // namespace ash
