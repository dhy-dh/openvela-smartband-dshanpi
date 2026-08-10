# ##############################################################################
# frameworks/runtimes/wasm/lwac/CMakeLists.txt
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

if(CONFIG_WASM_LWAC)
  nuttx_add_application(
    NAME
    lwac
    SRCS
    lwac.c
    lwac_module_registry.c
    loader.c
    INCLUDE_DIRECTORIES
    ${CMAKE_BINARY_DIR}/wamrmod
    STACKSIZE
    ${CONFIG_WASM_LWAC_STACKSIZE}
    PRIORITY
    ${CONFIG_WASM_LWAC_PRIORITY})

  # Add WASM test modules
  add_subdirectory(tests/test_wasm_modules)

  # Add LWAC test application
  if(CONFIG_WASM_LWAC_TEST)
    nuttx_add_application(
      NAME
      lwac_test
      SRCS
      tests/lwac_test.c
      STACKSIZE
      ${CONFIG_WASM_LWAC_TEST_STACKSIZE}
      PRIORITY
      ${CONFIG_WASM_LWAC_TEST_PRIORITY})
  endif()
endif()
