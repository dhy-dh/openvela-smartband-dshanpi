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

#ifndef QWRAPPER_H
#define QWRAPPER_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Developers are required to implement the following interfaces.
 */

/**
 * @brief 获取包路径
 */
const char* QWrapperGetAppPath(char* buf, size_t max);
const char* QWrapperGetBaseDataDir(const char* package_name, char* buf, size_t max);

#ifdef __cplusplus
}
#endif

#endif // QWRAPPER_H
