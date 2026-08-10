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
#include "ash/time/time.h"
#include <chrono>

namespace ash {

Time::Time() : nanos_(0) {}

Time::Time(int64_t nanos) : nanos_(nanos) {}

Time::~Time() = default;

int64_t Time::ToNano() const {
  return nanos_;
}

double Time::ToNanoF() const {
  return static_cast<double>(nanos_);
}

int64_t Time::ToMicro() const {
  return nanos_ / 1000;
}

double Time::ToMicroF() const {
  return static_cast<double>(nanos_) / 1000;
}

int64_t Time::ToMilli() const {
  return nanos_ / 1000000;
}

double Time::ToMilliF() const {
  return static_cast<double>(nanos_) / 1000000;
}

int64_t Time::ToSeconds() const {
  return nanos_ / 1000000000;
}

double Time::ToSecondsF() const {
  return static_cast<double>(nanos_) / 1000000000;
}

Time Time::FromNano(int64_t nanos) {
  return Time(nanos);
}

Time Time::FromNanoF(double nanos) {
  return Time(static_cast<int64_t>(nanos));
}

Time Time::FromMicro(int64_t micros) {
  return Time(micros * 1000);
}

Time Time::FromMicroF(double micros) {
  return Time(static_cast<int64_t>(micros * 1000));
}

Time Time::FromMilli(int64_t millis) {
  return Time(millis * 1000000);
}

Time Time::FromMilliF(double millis) {
  return Time(static_cast<int64_t>(millis * 1000000));
}

Time Time::FromSeconds(int64_t seconds) {
  return Time(seconds * 1000000000);
}

Time Time::FromSecondsF(double seconds) {
  return Time(static_cast<int64_t>(seconds * 1000000000));
}

Time Time::Now() {
  return Time(std::chrono::duration_cast<std::chrono::nanoseconds>(
                  std::chrono::steady_clock::now().time_since_epoch())
                  .count());
}

Time Time::Forever() {
  return Time(std::numeric_limits<int64_t>::max());
}

Time Time::operator+(const Duration& duration) const {
  return Time(nanos_ + duration.ToNano());
}

Time Time::operator-(const Duration& duration) const {
  return Time(nanos_ - duration.ToNano());
}

Duration Time::operator-(const Time& time) const {
  return Duration::FromNano(nanos_ - time.ToNano());
}

bool Time::operator==(const Time& time) const {
  return nanos_ == time.ToNano();
}

bool Time::operator!=(const Time& time) const {
  return nanos_ != time.ToNano();
}

bool Time::operator<(const Time& time) const {
  return nanos_ < time.ToNano();
}

bool Time::operator<=(const Time& time) const {
  return nanos_ <= time.ToNano();
}

bool Time::operator>(const Time& time) const {
  return nanos_ > time.ToNano();
}

bool Time::operator>=(const Time& time) const {
  return nanos_ >= time.ToNano();
}

}  // namespace ash
