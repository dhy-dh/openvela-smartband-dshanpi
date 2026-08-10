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
#ifndef ASH_TIME_DURATION_H_
#define ASH_TIME_DURATION_H_

#include <stdint.h>

namespace ash {

class Duration {
 public:
  Duration();
  ~Duration();

  int64_t ToNano() const;
  double ToNanoF() const;
  int64_t ToMicro() const;
  double ToMicroF() const;
  int64_t ToMilli() const;
  double ToMilliF() const;
  int64_t ToSeconds() const;
  double ToSecondsF() const;

  static Duration FromNano(int64_t nanos);
  static Duration FromNanoF(double nanos);
  static Duration FromMicro(int64_t micros);
  static Duration FromMicroF(double micros);
  static Duration FromMilli(int64_t millis);
  static Duration FromMilliF(double millis);
  static Duration FromSeconds(int64_t seconds);
  static Duration FromSecondsF(double seconds);

  static Duration Zero();
  static Duration Infinity();

  bool operator==(const Duration& other) const;
  bool operator!=(const Duration& other) const;
  Duration operator+(const Duration& other) const;
  Duration operator-(const Duration& other) const;

 private:
  Duration(int64_t nanos);
  int64_t nanos_;
};

}  // namespace ash

#endif  // ASH_TIME_DURATION_H_
