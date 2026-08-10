#include "ash/macros/compiler_macros.h"
#include "gtest/gtest.h"

// OS Macro Tests =======================================================

// Test NuttX OS definition
#if defined(ASH_OS_NUTTX)
TEST(CompilerMacrosTest, NuttxOsDefined) {
// Verify both NuttX and UNIX macros are defined
#if defined(ASH_OS_NUTTX) && defined(ASH_OS_UNIX)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_OS_NUTTX and ASH_OS_UNIX should both be defined on NuttX";
#endif
}
#endif

// Test Linux OS definition
#if defined(ASH_OS_LINUX)
TEST(CompilerMacrosTest, LinuxOsDefined) {
// Verify both Linux and UNIX macros are defined
#if defined(ASH_OS_LINUX) && defined(ASH_OS_UNIX)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_OS_LINUX and ASH_OS_UNIX should both be defined on Linux";
#endif
}
#endif

// Test Android OS definition
#if defined(ASH_OS_ANDROID)
TEST(CompilerMacrosTest, AndroidOsDefined) {
// Verify both Android and UNIX macros are defined
#if defined(ASH_OS_ANDROID) && defined(ASH_OS_UNIX)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_OS_ANDROID and ASH_OS_UNIX should both be defined on Android";
#endif
}
#endif

// Test Windows OS definition
#if defined(ASH_OS_WINDOWS)
TEST(CompilerMacrosTest, WindowsOsDefined) {
// Verify Windows macro is defined
#if defined(ASH_OS_WINDOWS)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_OS_WINDOWS should be defined on Windows";
#endif

// Verify UNIX macro is NOT defined
#if !defined(ASH_OS_UNIX)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_OS_UNIX should not be defined on Windows";
#endif
}
#endif

// Test Unix system consistency
TEST(CompilerMacrosTest, UnixSystemConsistency) {
#if defined(ASH_OS_NUTTX) || defined(ASH_OS_LINUX) || defined(ASH_OS_ANDROID)
// Verify UNIX macro is defined
#if defined(ASH_OS_UNIX)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_OS_UNIX should be defined on Unix systems";
#endif
#else
// Verify UNIX macro is NOT defined
#if !defined(ASH_OS_UNIX)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_OS_UNIX should not be defined on non-Unix systems";
#endif
#endif
}

// Test OS macros mutual exclusivity
TEST(CompilerMacrosTest, OsMacrosMutuallyExclusive) {
  int osCount = 0;
#if defined(ASH_OS_NUTTX)
  osCount++;
#endif
#if defined(ASH_OS_LINUX)
  osCount++;
#endif
#if defined(ASH_OS_ANDROID)
  osCount++;
#endif
#if defined(ASH_OS_WINDOWS)
  osCount++;
#endif
  EXPECT_EQ(osCount, 1);
}

// CPU Architecture Macro Tests
// ======================================================

// Test x86_64 architecture definition
#if defined(ASH_ARCH_X64)
TEST(CompilerMacrosTest, X64ArchDefined) {
#if defined(ASH_ARCH_64) && defined(ASH_ARCH_64) && !defined(ASH_ARCH_32)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_ARCH_X64 should be defined on Unix systems";
#endif
}
#endif

// Test x86 architecture definition
#if defined(ASH_ARCH_X86)
TEST(CompilerMacrosTest, X86ArchDefined) {
#if defined(ASH_ARCH_X86) && defined(ASH_ARCH_32) && !defined(ASH_ARCH_64)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_ARCH_X86 should be defined on Unix systems";
#endif
}
#endif

// Test ARM architecture definition
#if defined(ASH_ARCH_ARM)
TEST(CompilerMacrosTest, ArmArchDefined) {
#if defined(ASH_ARCH_ARM) && defined(ASH_ARCH_32) && !defined(ASH_ARCH_64)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_ARCH_ARM should be defined on Unix systems";
#endif
}
#endif

// Test ARM64 architecture definition
#if defined(ASH_ARCH_ARM64)
TEST(CompilerMacrosTest, Arm64ArchDefined) {
#if defined(ASH_ARCH_ARM64) && defined(ASH_ARCH_64) && !defined(ASH_ARCH_32)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_ARCH_ARM64 should be defined on Unix systems";
#endif
}
#endif

// Test bit-width consistency
TEST(CompilerMacrosTest, ArchitectureWidthConsistency) {
#if defined(ASH_ARCH_64) && !defined(ASH_ARCH_32)
  EXPECT_TRUE(true);

#elif defined(ASH_ARCH_32)
#if !defined(ASH_ARCH_64)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_ARCH_32 should be defined on Unix systems";
#endif
#endif
}

// Test architecture macros mutual exclusivity
TEST(CompilerMacrosTest, ArchMacrosMutuallyExclusive) {
  int archCount = 0;
#if defined(ASH_ARCH_X64)
  archCount++;
#endif
#if defined(ASH_ARCH_X86)
  archCount++;
#endif
#if defined(ASH_ARCH_ARM)
  archCount++;
#endif
#if defined(ASH_ARCH_ARM64)
  archCount++;
#endif

  EXPECT_EQ(archCount, 1) << "Only one ARCH macro should be defined";
}

// Compiler Macro Tests ========================================================

// Test GCC compiler definition
#if defined(ASH_COMPILER_GCC)
TEST(CompilerMacrosTest, GccCompilerDefined) {
#if !defined(ASH_COMPILER_CLANG) && !defined(ASH_COMPILER_MSVC)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_COMPILER_GCC should be defined on Unix systems";
#endif
}
#endif

// Test Clang compiler definition
#if defined(ASH_COMPILER_CLANG)
TEST(CompilerMacrosTest, ClangCompilerDefined) {
#if !defined(ASH_COMPILER_GCC) && !defined(ASH_COMPILER_MSVC)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_COMPILER_CLANG should be defined on Unix systems";
#endif
}
#endif

// Test MSVC compiler definition
#if defined(ASH_COMPILER_MSVC)
TEST(CompilerMacrosTest, MsvcCompilerDefined) {
#if !defined(ASH_COMPILER_GCC) && !defined(ASH_COMPILER_CLANG)
  EXPECT_TRUE(true);
#else
  FAIL() << "ASH_COMPILER_MSVC should be defined on Unix systems";
#endif
}
#endif

// Test compiler macros mutual exclusivity
TEST(CompilerMacrosTest, CompilerMacrosMutuallyExclusive) {
  int compilerCount = 0;
#if defined(ASH_COMPILER_GCC)
  compilerCount++;
#endif
#if defined(ASH_COMPILER_CLANG)
  compilerCount++;
#endif
#if defined(ASH_COMPILER_MSVC)
  compilerCount++;
#endif

  EXPECT_EQ(compilerCount, 1) << "Only one compiler macro should be defined";
}

// Combination Verification Tests
// ===========================================================

// Test OS-architecture combination validity
TEST(CompilerMacrosTest, OsArchCombination) {
#if defined(ASH_OS_WINDOWS)
#if defined(ASH_ARCH_X64) || defined(ASH_ARCH_X86)
  EXPECT_TRUE(true);
#else
  FAIL() << "Windows only supports x86/x86 architectures";
#endif
#elif defined(ASH_OS_ANDROID) || defined(ASH_OS_LINUX)
#if defined(ASH_ARCH_ARM64) || defined(ASH_ARCH_ARM) || \
    defined(ASH_ARCH_X64) || defined(ASH_ARCH_X86)
  EXPECT_TRUE(true);
#else
  FAIL() << "Linux/Android only supports ARM64/ARM/x86/x86 architectures";
#endif
#elif defined(ASH_OS_NUTTX)
#if defined(ASH_ARCH_ARM) || defined(ASH_ARCH_ARM64) || \
    defined(ASH_ARCH_X64) || defined(ASH_ARCH_X86)
  EXPECT_TRUE(true);
#else
  FAIL() << "NuttX only supports ARM/ARM64 architectures";
#endif
#endif
}

// Test compiler-OS combination validity
TEST(CompilerMacrosTest, CompilerOsCombination) {
#if defined(ASH_OS_WINDOWS)
#if defined(ASH_COMPILER_MSVC)
  EXPECT_TRUE(true);
#else
  FAIL() << "Windows primarily uses MSVC";
#endif
#elif defined(ASH_OS_NUTTX)
#if defined(ASH_COMPILER_GCC)
  EXPECT_TRUE(true);
#else
  FAIL() << "NuttX primarily uses GCC";
#endif
#else
#if defined(ASH_COMPILER_GCC) || defined(ASH_COMPILER_CLANG)
  EXPECT_TRUE(true);
#else
  FAIL() << "Unix-like systems use GCC or Clang";
#endif
#endif
}
