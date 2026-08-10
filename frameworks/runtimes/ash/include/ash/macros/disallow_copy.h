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

/**
 * @file disallow_copy.h
 *
 * The class DisallowCopy is used to disallow copy constructor and copy
 * assignment. Any object of subclass of DisallowCopy can not be copied,
 * but still can be moved.
 *
 * Example:
 *   class A : public DisallowCopy {
 *     ...
 *   };
 *
 *   A a;
 *   A b = a; // Compile error.
 *   A c = std::move(a); // OK.
 *
 * The class DisallowCopyAndMove is used to disallow copy constructor, copy
 * assignment, move constructor and move assignment. Any object of subclass of
 * DisallowCopyAndMove can not be copied or moved.
 *
 * Example:
 *   class A : public DisallowCopyAndMove {
 *     ...
 *   };
 *
 *   A a;
 *   A b = a; // Compile error.
 *   A c = std::move(a); // Compile error.
 */

#ifndef ASH_MEMORY_DISALLOW_COPY_H_
#define ASH_MEMORY_DISALLOW_COPY_H_

namespace ash {

#define ASH_DISALLOW_COPY(TypeName)              \
  TypeName(const TypeName&) = delete;            \
  TypeName& operator=(const TypeName&) = delete; \
  TypeName(TypeName&&) = default;                \
  TypeName& operator=(TypeName&&) = default

#define ASH_DISALLOW_COPY_AND_MOVE(TypeName)     \
  TypeName(const TypeName&) = delete;            \
  TypeName(TypeName&&) = delete;                 \
  TypeName& operator=(const TypeName&) = delete; \
  TypeName& operator=(TypeName&&) = delete

}  // namespace ash

#endif  // ASH_MEMORY_DISALLOW_COPY_H_
