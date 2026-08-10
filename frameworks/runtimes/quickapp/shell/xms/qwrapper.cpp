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

#include "qwrapper.h"

#include <filesystem>

#include "common/shell_log.h"
#include "pm/PackageManager.h"

namespace fs = std::filesystem;

static fs::path get_app_path()
{
#ifdef __NuttX__
    const char* path = CONFIG_HAP_APP_PATH;
#else
    const char* path = currentPath()
#endif
    fs::path app_path(path);

    app_path /= "app";

#ifndef __NuttX__
    free(path);
#endif

    return app_path;
}

extern "C" const char* QWrapperGetAppPath(char* buf, size_t max)
{
    // 获取非预装 quickapp 基础路径
    auto app_path = get_app_path();
    auto path_str = app_path.string();
    max = std::min(max, path_str.length());

    strncpy(buf, path_str.c_str(), max);
    buf[max] = 0;
    return buf;
}

extern "C" const char* QWrapperGetBaseDataDir(const char* package_name, char* buf, size_t max)
{
    if (!package_name)
        return nullptr;

    os::pm::PackageManager pm;
    os::pm::PackageInfo package_info;

    if (pm.getPackageInfo(package_name, &package_info)) {
        SHELL_LOG_ERROR("FrameworkXms::getPackagePath : packagename:%s is not installed!!!", package_name);
        return nullptr;
    }
    strncpy(buf, package_info.installedPath.c_str(), max - 1);
    return buf;
}
