# ##############################################################################
# cmake/nuttx_add_rust.cmake
#
# Licensed to the Apache Software Foundation (ASF) under one or more contributor
# license agreements.  See the NOTICE file distributed with this work for
# additional information regarding copyright ownership.  The ASF licenses this
# file to you under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#
# ##############################################################################

include(nuttx_parse_function_args)

# Check if required LLVM parameters are defined If any are missing, warn and
# skip the entire Rust logic
if(NOT DEFINED LLVM_ARCHTYPE
   OR NOT DEFINED LLVM_ABITYPE
   OR NOT DEFINED LLVM_CPUTYPE)
  message(
    WARNING
      "Missing required LLVM parameters for Rust support. Skipping Rust build logic. "
      "Required: LLVM_ARCHTYPE='${LLVM_ARCHTYPE}', LLVM_ABITYPE='${LLVM_ABITYPE}', LLVM_CPUTYPE='${LLVM_CPUTYPE}'"
  )
  return()
endif()

# ~~~
# Convert architecture type to Rust NuttX target
#
# Supported architectures:
#   - aarch64: aarch64-unknown-nuttx
#   - armv7a: armv7a-nuttx-eabi, armv7a-nuttx-eabihf
#   - thumbv6m: thumbv6m-nuttx-eabi
#   - thumbv7a: thumbv7a-nuttx-eabi, thumbv7a-nuttx-eabihf
#   - thumbv7m: thumbv7m-nuttx-eabi
#   - thumbv7em: thumbv7em-nuttx-eabihf
#   - thumbv8m.main: thumbv8m.main-nuttx-eabi, thumbv8m.main-nuttx-eabihf
#   - thumbv8m.base: thumbv8m.base-nuttx-eabi, thumbv8m.base-nuttx-eabihf
#   - riscv32: riscv32imc/imac/imafc-unknown-nuttx-elf
#   - riscv64: riscv64imac/imafdc-unknown-nuttx-elf
#   - x86: i686-unknown-nuttx
#   - x86_64: x86_64-unknown-nuttx
#
# Inputs:
#   ARCHTYPE - Architecture type (e.g. thumbv7m, riscv32)
#   ABITYPE  - ABI type (e.g. eabi, eabihf)
#   CPUTYPE  - CPU type (e.g. cortex-m4, sifive-e20)
#
# Output:
#   OUTPUT   - Rust target triple (e.g. riscv32imac-unknown-nuttx-elf,
#             thumbv7m-nuttx-eabi, thumbv7em-nuttx-eabihf)
# ~~~

function(nuttx_rust_target_triple ARCHTYPE ABITYPE CPUTYPE OUTPUT)
  if(ARCHTYPE STREQUAL "x86_64")
    set(TARGET_TRIPLE "${NUTTX_APPS_DIR}/tools/x86_64-unknown-nuttx.json")
  elseif(ARCHTYPE STREQUAL "x86")
    set(TARGET_TRIPLE "${NUTTX_APPS_DIR}/tools/i486-unknown-nuttx.json")
  elseif(ARCHTYPE STREQUAL "aarch64")
    set(TARGET_TRIPLE "aarch64-unknown-nuttx")
  elseif(ARCHTYPE MATCHES "thumb")
    if(ARCHTYPE MATCHES "thumbv8")
      # Extract just the base architecture type (thumbv8m.main or thumbv8m.base)
      # Handle both thumbv8m and thumbv8.1m variants
      if(ARCHTYPE MATCHES "thumbv8.*\.main")
        set(ARCH_BASE "thumbv8m.main")
      elseif(ARCHTYPE MATCHES "thumbv8m.base")
        set(ARCH_BASE "thumbv8m.base")
      else()
        # Otherwise determine if we should use thumbv8m.main or thumbv8m.base
        # based on CPU type
        if(CPUTYPE MATCHES "cortex-m23")
          set(ARCH_BASE "thumbv8m.base")
        else()
          set(ARCH_BASE "thumbv8m.main")
        endif()
      endif()
      set(TARGET_TRIPLE "${ARCH_BASE}-nuttx-${ABITYPE}")
    else()
      set(TARGET_TRIPLE "${ARCHTYPE}-nuttx-${ABITYPE}")
    endif()
  elseif(ARCHTYPE STREQUAL "riscv32")
    if(CPUTYPE STREQUAL "sifive-e20")
      set(TARGET_TRIPLE "riscv32imc-unknown-nuttx-elf")
    elseif(CPUTYPE STREQUAL "sifive-e31")
      set(TARGET_TRIPLE "riscv32imac-unknown-nuttx-elf")
    elseif(CPUTYPE STREQUAL "sifive-e76")
      set(TARGET_TRIPLE "riscv32imafc-unknown-nuttx-elf")
    else()
      set(TARGET_TRIPLE "riscv32imc-unknown-nuttx-elf")
    endif()
  elseif(ARCHTYPE STREQUAL "riscv64")
    if(CPUTYPE STREQUAL "sifive-s51")
      set(TARGET_TRIPLE "riscv64imac-unknown-nuttx-elf")
    elseif(CPUTYPE STREQUAL "sifive-u54")
      set(TARGET_TRIPLE "riscv64imafdc-unknown-nuttx-elf")
    else()
      set(TARGET_TRIPLE "riscv64imac-unknown-nuttx-elf")
    endif()
  endif()
  set(${OUTPUT}
      ${TARGET_TRIPLE}
      PARENT_SCOPE)
endfunction()

# Build flags for rust_unified_lib, this file will be included only once in the
# NuttX build system, so it's safe to set these variables globally
set(RUST_UNIFIED_CARGO_BUILD_FLAGS "")
if(NOT CONFIG_DEBUG_NOOPT)
  list(APPEND RUST_UNIFIED_CARGO_BUILD_FLAGS "--release")
  set(RUST_UNIFIED_CARGO_PROFILE "release")
else()
  set(RUST_UNIFIED_CARGO_PROFILE "debug")
endif()
nuttx_rust_target_triple(${LLVM_ARCHTYPE} ${LLVM_ABITYPE} ${LLVM_CPUTYPE}
                         RUST_UNIFIED_CARGO_TARGET)

# Handle JSON target: use base name for target directory if needed
if(RUST_UNIFIED_CARGO_TARGET MATCHES ".json$")
  get_filename_component(RUST_UNIFIED_TARGET_BASE ${RUST_UNIFIED_CARGO_TARGET}
                         NAME_WE)
else()
  set(RUST_UNIFIED_TARGET_BASE ${RUST_UNIFIED_CARGO_TARGET})
endif()

list(APPEND RUST_UNIFIED_CARGO_BUILD_FLAGS
     "--target=${RUST_UNIFIED_CARGO_TARGET}")
list(APPEND RUST_UNIFIED_CARGO_BUILD_FLAGS
     "--manifest-path=${CMAKE_BINARY_DIR}/rust_unified_lib/Cargo.toml")

set(RUST_UNIFIED_LIBPATH
    ${CMAKE_BINARY_DIR}/rust_unified_lib/target/${RUST_UNIFIED_TARGET_BASE}/${RUST_UNIFIED_CARGO_PROFILE}/librust_unified_lib.a
)

# Create a global target to collect all Rust crate information Step 1: Generate
# the unified Rust library source
if(NOT TARGET rust_unified_lib)
  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/rust_unified_lib
    DEPENDS ${NUTTX_APPS_DIR}/tools/generate_rust_unified_lib.py
    COMMAND
      python3 ${NUTTX_APPS_DIR}/tools/generate_rust_unified_lib.py
      --crate-name=$<TARGET_PROPERTY:rust_unified_lib,RUST_CRATE_NAMES>
      --crate-path=$<TARGET_PROPERTY:rust_unified_lib,RUST_CRATE_PATHS>
      --output-dir ${CMAKE_BINARY_DIR}/rust_unified_lib
    COMMENT "Generating unified Rust library sources"
    VERBATIM)

  # Step 2: Build the Rust static library

  # Convert NUTTX_EXTRA_FLAGS to a space-separated string
  string(REPLACE ";" " " CFLAGS_STRING "${NUTTX_EXTRA_FLAGS}")

  # Get include directory for toolchain headers
  execute_process(
    COMMAND ${CMAKE_C_COMPILER} -print-file-name=include
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE TOOLCHAIN_INCLUDE_DIR)

  add_custom_command(
    OUTPUT ${RUST_UNIFIED_LIBPATH}
    DEPENDS ${CMAKE_BINARY_DIR}/rust_unified_lib
    COMMAND
      ${CMAKE_COMMAND} -E env
      NUTTX_INCLUDE_DIR=${TOOLCHAIN_INCLUDE_DIR}:${PROJECT_SOURCE_DIR}/include:${CMAKE_BINARY_DIR}/include:${CMAKE_BINARY_DIR}/include/arch
      CC=${CMAKE_C_COMPILER} AR=${CMAKE_AR} CFLAGS=${CFLAGS_STRING}
      NUTTX_APPS_DIR=${NUTTX_APPS_DIR} NUTTX_BUILD_DIR=${CMAKE_BINARY_DIR} cargo
      build ${RUST_UNIFIED_CARGO_BUILD_FLAGS}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/rust_unified_lib
    COMMENT "Building unified Rust static library"
    BYPRODUCTS ${CMAKE_BINARY_DIR}/rust_unified_lib/dummy_product
    VERBATIM)

  # Step 3: Create the target and make it depend on the static library
  add_custom_target(rust_unified_lib DEPENDS ${RUST_UNIFIED_LIBPATH})
endif()

# ~~~
# nuttx_add_rust
#
# Description:
#   Build a Rust crate and add it as a static library to the NuttX build system
#
# Example:
#  nuttx_add_rust(
#    CRATE_NAME
#    hello
#    CRATE_PATH
#    ${CMAKE_CURRENT_SOURCE_DIR}/hello
#  )
# ~~~

function(nuttx_add_rust)

  # parse arguments into variables
  nuttx_parse_function_args(
    FUNC
    nuttx_add_rust
    ONE_VALUE
    CRATE_NAME
    CRATE_PATH
    REQUIRED
    CRATE_NAME
    CRATE_PATH
    ARGN
    ${ARGN})

  # Import the unified Rust static library only once
  if(NOT TARGET rust_unified_lib_static)

    add_custom_target(rust_unified_lib_static)

    add_dependencies(apps rust_unified_lib)

    # Add the Rust library to NuttX build
    nuttx_add_extra_library(${RUST_UNIFIED_LIBPATH})

  endif()

  # Add crate information to the unified target properties Store crate name and
  # path as target properties
  set_property(
    TARGET rust_unified_lib
    APPEND
    PROPERTY RUST_CRATE_NAMES ${CRATE_NAME})
  set_property(
    TARGET rust_unified_lib
    APPEND
    PROPERTY RUST_CRATE_PATHS ${CRATE_PATH})

endfunction()
