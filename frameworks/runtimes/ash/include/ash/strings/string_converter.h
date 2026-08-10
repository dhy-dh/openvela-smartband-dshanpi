/*
 * Copyright (C) 2025 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef ASH_STRINGS_STRING_CONVERTER_H_
#define ASH_STRINGS_STRING_CONVERTER_H_

#include <string>

namespace ash {

template <typename T>
struct StringProcessor;

template <typename T, typename Processor = StringProcessor<T>>
struct StringConverter {
  static std::string ToString(const T& value) {
    std::string output;
    if (!Processor::Build(output, value))
      return "";
    return output;
  }

  static bool FromString(const std::string& input, T* value) {
    const char* p = input.c_str();
    return Processor::Parse(&p, value) && *p == '\0';
  }
};

struct LiteralProcessor {
  static bool Build(std::string& output, const char* literal);
  static bool Parse(const char** input, const char* literal);
};

template <typename T, typename U, typename Processor = StringProcessor<U>>
struct CastProcessor {
  static bool Build(std::string& output, T value) {
    return Processor::Build(output, static_cast<U>(value));
  }

  static bool Parse(const char** input, T* value) {
    const char* p = *input;
    U u;
    if (!Processor::Parse(&p, &u))
      return false;
    T t = static_cast<T>(u);
    if (static_cast<U>(t) != u)
      return false;
    *value = t;
    *input = p;
    return true;
  }
};

template <bool crlf = false>
struct SkipSpaceProcessor;

template <>
struct SkipSpaceProcessor<true> {
  static size_t Skip(const char** input);
};

template <>
struct SkipSpaceProcessor<false> {
  static size_t Skip(const char** input);
};

template <bool crlf = false>
struct SpaceProcessor {
  static bool Build(std::string& output) {
    output.append(" ");
    return true;
  }

  static bool Parse(const char** input) {
    return SkipSpaceProcessor<crlf>::Skip(input) > 0;
  }
};

struct IdentifierProcessor {
  static bool Build(std::string& output, const std::string& value);
  static bool Parse(const char** input, std::string* value);
};

template <>
struct StringProcessor<bool> {
  static bool Build(std::string& output, bool value);
  static bool Parse(const char** input, bool* value);
};

template <>
struct StringProcessor<signed char> : CastProcessor<signed char, long> {};

template <>
struct StringProcessor<unsigned char>
    : CastProcessor<unsigned char, unsigned long> {};

template <>
struct StringProcessor<short> : CastProcessor<short, long> {};

template <>
struct StringProcessor<unsigned short>
    : CastProcessor<unsigned short, unsigned long> {};

template <>
struct StringProcessor<int> : CastProcessor<int, long> {};

template <>
struct StringProcessor<unsigned int>
    : CastProcessor<unsigned int, unsigned long> {};

template <>
struct StringProcessor<long> {
  static bool Build(std::string& output, long value);
  static bool Parse(const char** input, long* value);
};

template <>
struct StringProcessor<unsigned long> {
  static bool Build(std::string& output, unsigned long value);
  static bool Parse(const char** input, unsigned long* value);
};

template <>
struct StringProcessor<float> {
  static bool Build(std::string& output, float value);
  static bool Parse(const char** input, float* value);
};

template <>
struct StringProcessor<double> {
  static bool Build(std::string& output, double value);
  static bool Parse(const char** input, double* value);
};

template <typename T>
std::string ToString(const T& value) {
  return StringConverter<T>::ToString(value);
}

template <typename T>
bool FromString(const std::string& input, T* value) {
  return StringConverter<T>::FromString(input, value);
}

}  // namespace ash

#endif  // ASH_STRINGS_STRING_CONVERTER_H_
