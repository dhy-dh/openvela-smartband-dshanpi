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
#include "ash/time/duration.h"
#include <limits>

namespace ash {

Duration::Duration() : nanos_(0) {}

Duration::Duration(int64_t nanos) : nanos_(nanos) {}

Duration::~Duration() {}

int64_t Duration::ToNano() const {
  return nanos_;
}

double Duration::ToNanoF() const {
  return static_cast<double>(nanos_);
}

int64_t Duration::ToMicro() const {
  return nanos_ / 1000;
}

double Duration::ToMicroF() const {
  return static_cast<double>(nanos_) / 1000.0;
}

int64_t Duration::ToMilli() const {
  return nanos_ / 1000000;
}

double Duration::ToMilliF() const {
  return static_cast<double>(nanos_) / 1000000.0;
}

int64_t Duration::ToSeconds() const {
  return nanos_ / 1000000000;
}

double Duration::ToSecondsF() const {
  return static_cast<double>(nanos_) / 1000000000.0;
}

Duration Duration::FromNano(int64_t nanos) {
  return Duration(nanos);
}

Duration Duration::FromNanoF(double nanos) {
  return Duration(static_cast<int64_t>(nanos));
}

Duration Duration::FromMicro(int64_t micros) {
  return Duration(micros * 1000);
}

Duration Duration::FromMicroF(double micros) {
  return Duration(static_cast<int64_t>(micros * 1000));
}

Duration Duration::FromMilli(int64_t millis) {
  return Duration(millis * 1000000);
}

Duration Duration::FromMilliF(double millis) {
  return Duration(static_cast<int64_t>(millis * 1000000));
}

Duration Duration::FromSeconds(int64_t seconds) {
  return Duration(seconds * 1000000000);
}

Duration Duration::FromSecondsF(double seconds) {
  return Duration(static_cast<int64_t>(seconds * 1000000000));
}

bool Duration::operator==(const Duration& other) const {
  return nanos_ == other.nanos_;
}

bool Duration::operator!=(const Duration& other) const {
  return nanos_ != other.nanos_;
}

Duration Duration::operator+(const Duration& other) const {
  return Duration(nanos_ + other.nanos_);
}

Duration Duration::operator-(const Duration& other) const {
  return Duration(nanos_ - other.nanos_);
}

Duration Duration::Zero() {
  return Duration();
}

Duration Duration::Infinity() {
  return Duration(std::numeric_limits<int64_t>::max());
}

}  // namespace ash
