# ##############################################################################
# apps/math/ruy/CMakeLists.txt
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

if(CONFIG_MATH_RUY)

  # ############################################################################
  # Config and Fetch ruy lib
  # ############################################################################

  set(RUY_DIR ${CMAKE_CURRENT_LIST_DIR}/ruy)

  if(NOT EXISTS ${RUY_DIR})
    set(RUY_URL
        https://github.com/google/ruy/archive/d37128311b445e758136b8602d1bbd2a755e115d.zip
    )
    FetchContent_Declare(
      ruy_fetch
      URL ${RUY_URL} SOURCE_DIR ${RUY_DIR} BINARY_DIR
          ${CMAKE_BINARY_DIR}/apps/math/ruy/ruy
      DOWNLOAD_NO_PROGRESS true
      TIMEOUT 30)

    FetchContent_GetProperties(ruy_fetch)

    if(NOT ruy_fetch_POPULATED)
      FetchContent_Populate(ruy_fetch)
    endif()
  endif()

  # ############################################################################
  # Include Directory
  # ############################################################################

  set(INCDIR ${CMAKE_CURRENT_LIST_DIR}/ruy)

  # ############################################################################
  # Collect source files
  # ############################################################################

  set(RUY_SOURCES
      ${RUY_DIR}/ruy/allocator.cc
      ${RUY_DIR}/ruy/apply_multiplier.cc
      ${RUY_DIR}/ruy/blocking_counter.cc
      ${RUY_DIR}/ruy/block_map.cc
      ${RUY_DIR}/ruy/context.cc
      ${RUY_DIR}/ruy/context_get_ctx.cc
      ${RUY_DIR}/ruy/cpuinfo.cc
      ${RUY_DIR}/ruy/ctx.cc
      ${RUY_DIR}/ruy/denormal.cc
      ${RUY_DIR}/ruy/frontend.cc
      ${RUY_DIR}/ruy/have_built_path_for_avx.cc
      ${RUY_DIR}/ruy/have_built_path_for_avx2_fma.cc
      ${RUY_DIR}/ruy/have_built_path_for_avx512.cc
      ${RUY_DIR}/ruy/kernel_arm32.cc
      ${RUY_DIR}/ruy/kernel_arm64.cc
      ${RUY_DIR}/ruy/kernel_avx.cc
      ${RUY_DIR}/ruy/kernel_avx2_fma.cc
      ${RUY_DIR}/ruy/kernel_avx512.cc
      ${RUY_DIR}/ruy/pack_arm.cc
      ${RUY_DIR}/ruy/pack_avx.cc
      ${RUY_DIR}/ruy/pack_avx2_fma.cc
      ${RUY_DIR}/ruy/pack_avx512.cc
      ${RUY_DIR}/ruy/pmu.cc
      ${RUY_DIR}/ruy/prepacked_cache.cc
      ${RUY_DIR}/ruy/prepare_packed_matrices.cc
      ${RUY_DIR}/ruy/system_aligned_alloc.cc
      ${RUY_DIR}/ruy/thread_pool.cc
      ${RUY_DIR}/ruy/trmul.cc
      ${RUY_DIR}/ruy/tune.cc
      ${RUY_DIR}/ruy/wait.cc)

  # ############################################################################
  # Library Configuration
  # ############################################################################

  nuttx_add_library(ruy STATIC ${RUY_SOURCES})
  target_compile_options(ruy PRIVATE -fomit-frame-pointer)
  target_include_directories(ruy PUBLIC ${INCDIR})

  # Disable specific warnings for ruy library to avoid compilation errors
  target_compile_options(ruy PRIVATE -Wno-shadow -Wno-error=shadow -Wno-error)

endif()
