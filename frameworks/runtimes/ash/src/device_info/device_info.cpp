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
#include "ash/device_info/device_info.h"
#include "ash/logging/logging.h"
#include "ash/macros/compiler_macros.h"

#if defined(ASH_OS_NUTTX)
#include "uv_ext.h"
#endif  // defined(ASH_OS_NUTTX)

#define LOG_TAG "DeviceInfo"

namespace ash {

namespace {

std::string g_brand;
std::string g_manufacturer;
std::string g_model;
std::string g_product;
std::string g_os_type;
std::string g_os_version_name;
std::string g_language;
std::string g_region;
std::string g_did;
std::string g_screen_shape;
std::string g_device_type;
int g_os_version_code = 0;
int g_screen_width = 0;
int g_screen_height = 0;
float g_screen_density = 0;
#ifdef CONFIG_QUICKAPP_TEST_FRAMEWORK
int g_bpp = 0;
#endif
}  // namespace

void DeviceInfo::init() {
#if defined(ASH_OS_NUTTX)
  uv_devinfo_t devinfo;
  memset(&devinfo, 0, sizeof(devinfo));
  uv_getdeviceinfo(&devinfo);
  g_brand = devinfo.brand;
  g_manufacturer = devinfo.manufacturer;
  g_model = devinfo.model;
  g_product = devinfo.product;
  g_os_type = devinfo.ostype;
  g_os_version_name = devinfo.osversionname;
  g_language = devinfo.language;
  g_region = devinfo.region;
  g_did = devinfo.did;
  g_screen_shape = devinfo.screenshape;
  g_device_type = devinfo.devicetype;
  g_os_version_code = devinfo.osversioncode;
  g_screen_width = devinfo.screenwidth;
  g_screen_height = devinfo.screenheight;
  g_screen_density = devinfo.screendensity;
#ifdef CONFIG_QUICKAPP_TEST_FRAMEWORK
  g_bpp = devinfo.bpp;
#endif
#else   // defined(ASH_OS_NUTTX)
  ASH_LOG(LOG_TAG, FATAL) << "Unsupported OS.";
#endif  // defined(ASH_OS_NUTTX)
}

const std::string& DeviceInfo::brand() {
  return g_brand;
}

const std::string& DeviceInfo::manufacturer() {
  return g_manufacturer;
}

const std::string& DeviceInfo::model() {
  return g_model;
}

const std::string& DeviceInfo::product() {
  return g_product;
}

const std::string& DeviceInfo::osType() {
  return g_os_type;
}

const std::string& DeviceInfo::osVersionName() {
  return g_os_version_name;
}

const std::string& DeviceInfo::language() {
  return g_language;
}

const std::string& DeviceInfo::region() {
  return g_region;
}

const std::string& DeviceInfo::did() {
  return g_did;
}

const std::string& DeviceInfo::screenShape() {
  return g_screen_shape;
}

const std::string& DeviceInfo::deviceType() {
  return g_device_type;
}

int DeviceInfo::osVersionCode() {
  return g_os_version_code;
}

int DeviceInfo::screenWidth() {
  return g_screen_width;
}

int DeviceInfo::screenHeight() {
  return g_screen_height;
}

float DeviceInfo::screenDensity() {
  return g_screen_density;
}

#ifdef CONFIG_QUICKAPP_TEST_FRAMEWORK
int DeviceInfo::bpp() {
  return g_bpp;
}
#endif

}  // namespace ash
