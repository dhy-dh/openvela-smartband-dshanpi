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
#ifndef ASH_DEVICE_INFO_DEVICE_INFO_H_
#define ASH_DEVICE_INFO_DEVICE_INFO_H_

#include <string>

namespace ash {

class DeviceInfo {
 public:
  static void init();

  static const std::string& brand();
  static const std::string& manufacturer();
  static const std::string& model();
  static const std::string& product();
  static const std::string& osType();
  static const std::string& osVersionName();
  static const std::string& language();
  static const std::string& region();
  static const std::string& did();
  static const std::string& screenShape();
  static const std::string& deviceType();
  static int osVersionCode();
  static int screenWidth();
  static int screenHeight();
  static float screenDensity();
#ifdef CONFIG_QUICKAPP_TEST_FRAMEWORK
  static int bpp();
#endif
};

}  // namespace ash

#endif  // ASH_DEVICE_INFO_DEVICE_INFO_H_
