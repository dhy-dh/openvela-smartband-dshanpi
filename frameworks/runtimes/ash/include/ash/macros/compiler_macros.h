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
#ifndef ASH_MACROS_COMPILER_MACROS_H_
#define ASH_MACROS_COMPILER_MACROS_H_

#if defined(__NuttX__)
#include <nuttx/config.h>
#endif  // defined(__NuttX__)

/**
 * Macros to detect the OS.
 *
 * ASH_OS_NUTTX: NuttX
 * ASH_OS_LINUX: Linux
 * ASH_OS_ANDROID: Android
 * ASH_OS_WINDOWS: Windows
 * ASH_OS_UNIX: Unix, including Linux, Android, NuttX, etc.
 */
#if defined(__NuttX__)
#define ASH_OS_NUTTX
#define ASH_OS_UNIX
#elif defined(__ANDROID__)
#define ASH_OS_ANDROID
#define ASH_OS_UNIX
#elif defined(__linux__)
#define ASH_OS_LINUX
#define ASH_OS_UNIX
#elif defined(_WIN32) || defined(_WIN64)
#define ASH_OS_WINDOWS
#else
static_assert(false, "OS is not supported.")
#endif

/**
 * Macros to detect the cpu architecture.
 *
 * ASH_ARCH_X86: x86
 * ASH_ARCH_X64: x86_64
 * ASH_ARCH_ARM: arm
 * ASH_ARCH_ARM64: arm64
 *
 * ASH_ARCH_32: 32-bit architecture
 * ASH_ARCH_64: 64-bit architecture
 */
#if defined(__x86_64__) || defined(_M_X64)
#define ASH_ARCH_X64
#define ASH_ARCH_64
#elif defined(__i386__) || defined(_M_IX86)
#define ASH_ARCH_X86
#define ASH_ARCH_32
#elif defined(__arm__) || defined(_M_ARM)
#define ASH_ARCH_ARM
#define ASH_ARCH_32
#elif defined(__aarch64__) || defined(_M_ARM64)
#define ASH_ARCH_ARM64
#define ASH_ARCH_64
#else
    static_assert(false, "Architecture is not supported.")
#endif

/**
 * Macros to detect the compiler.
 *
 * ASH_COMPILER_GCC: GCC
 * ASH_COMPILER_CLANG: Clang
 * ASH_COMPILER_MSVC: MSVC
 */
#if defined(__GNUC__)
#define ASH_COMPILER_GCC
#elif defined(__clang__)
#define ASH_COMPILER_CLANG
#elif defined(_MSC_VER)
#define ASH_COMPILER_MSVC
#else
static_assert(false, "Compiler is not supported.")
#endif

#ifndef ASH_HAVE_ZLIB
#if defined(CONFIG_LIBASH)
#define ASH_HAVE_ZLIB
#endif  // defined(CONFIG_LIBASH)
#endif  // ASH_HAVE_ZLIB

#endif  // ASH_MACROS_COMPILER_MACROS_H_
