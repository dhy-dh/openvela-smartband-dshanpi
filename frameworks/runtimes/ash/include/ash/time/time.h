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
#ifndef ASH_TIME_TIME_H_
#define ASH_TIME_TIME_H_

#include <cstdint>
#include "ash/time/duration.h"

namespace ash {

class Time {
 public:
  Time();
  ~Time();

  int64_t ToNano() const;
  double ToNanoF() const;
  int64_t ToMicro() const;
  double ToMicroF() const;
  int64_t ToMilli() const;
  double ToMilliF() const;
  int64_t ToSeconds() const;
  double ToSecondsF() const;

  static Time FromNano(int64_t nanos);
  static Time FromNanoF(double nanos);
  static Time FromMicro(int64_t micros);
  static Time FromMicroF(double micros);
  static Time FromMilli(int64_t millis);
  static Time FromMilliF(double millis);
  static Time FromSeconds(int64_t seconds);
  static Time FromSecondsF(double seconds);
  static Time Now();
  static Time Forever();

  Time operator+(const Duration& duration) const;
  Time operator-(const Duration& duration) const;
  Duration operator-(const Time& time) const;

  bool operator==(const Time& time) const;
  bool operator!=(const Time& time) const;
  bool operator<(const Time& time) const;
  bool operator<=(const Time& time) const;
  bool operator>(const Time& time) const;
  bool operator>=(const Time& time) const;

 private:
  Time(int64_t nanos);
  int64_t nanos_;
};

}  // namespace ash

#endif  // ASH_TIME_TIME_H_
