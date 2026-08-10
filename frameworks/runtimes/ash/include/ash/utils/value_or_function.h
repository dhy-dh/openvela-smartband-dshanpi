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
#ifndef ASH_UTILS_VALUE_OR_FUNCTION_H_
#define ASH_UTILS_VALUE_OR_FUNCTION_H_

#include <functional>
#include <variant>

namespace ash {

template <typename T>
class ValueOrFunction {
  // TODO(xuyan): 限制模板参数类型
 public:
  ValueOrFunction() : value_(T()) {}
  ValueOrFunction(const T& value) : value_(value) {}
  ValueOrFunction(std::function<T()> value) : value_(value) {}

  bool IsValue() const { return value_.index() == 0; }
  bool IsFunction() const { return value_.index() == 1; }

  T GetValue() const { return std::get<T>(value_); }

  std::function<T()> GetFunction() const {
    return std::get<std::function<T()>>(value_);
  }

  T operator()() const {
    if (IsValue()) {
      return GetValue();
    } else {
      return GetFunction()();
    }
  }

  template <typename U>
  ValueOrFunction<U> Transform(std::function<U(T)> transform) const {
    if (IsValue()) {
      return transform(GetValue());
    } else {
      return std::function(
          [func = GetFunction(), transform]() { return transform(func()); });
    }
  }

 private:
  std::variant<T, std::function<T()>> value_;
};

}  // namespace ash

#endif  // ASH_UTILS_VALUE_OR_FUNCTION_H_
