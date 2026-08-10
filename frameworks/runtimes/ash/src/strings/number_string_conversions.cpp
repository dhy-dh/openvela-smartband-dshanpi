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
#include "ash/strings/number_string_conversions.h"
#include <iomanip>
#include <sstream>

namespace ash {
// TODO(xuyan): 考虑使用性能刚好的std::from_chars和std::to_chars

std::string DoubleToString(double value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

std::string IntegerToString(int value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

std::string IntToHexString(uint32_t value, uint32_t width) {
  std::ostringstream oss;
  oss << std::hex << std::setw(width) << std::setfill('0') << value;
  return oss.str();
}

}  // namespace ash
