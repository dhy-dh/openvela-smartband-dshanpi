# ##############################################################################
# cmake/nuttx_add_erpc.cmake
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

# ~~~
# nuttx_add_erpc
#
# Description:
#   Generate source code files using erpcgen and add them to the given target
#
# Example:
#  nuttx_add_erpc(
#    TARGET
#    apps
#    ERPC_OUT_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}
#    ERPCS
#    TEST.erpc)
# ~~~

function(nuttx_add_erpc)

  # parse arguments into variables

  nuttx_parse_function_args(
    FUNC
    nuttx_add_erpc
    MULTI_VALUE
    TARGET
    ERPC_OUT_DIR
    ERPCS
    ARGN
    ${ARGN})

  foreach(erpc_file ${ERPCS})
    get_filename_component(erpc_name ${erpc_file} NAME_WE)
    set(ERPC_SRC
        ${ERPC_OUT_DIR}/${erpc_name}_server.cpp
        ${ERPC_OUT_DIR}/c_${erpc_name}_server.cpp
        ${ERPC_OUT_DIR}/${erpc_name}_interface.cpp
        ${ERPC_OUT_DIR}/${erpc_name}_client.cpp
        ${ERPC_OUT_DIR}/c_${erpc_name}_client.cpp)
    set(ERPC_TARGET erpc_${erpc_name}_target)
    add_custom_command(
      OUTPUT ${ERPC_SRC}
      COMMAND erpcgen -a -o ${ERPC_OUT_DIR} ${erpc_file}
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      COMMENT "ERPC: generating erpc files for ${erpc_name}.erpc"
      DEPENDS ${erpc_file}
      VERBATIM)

    add_custom_target(${ERPC_TARGET} DEPENDS ${ERPC_SRC})
    target_include_directories(${TARGET} PRIVATE ${ERPC_OUT_DIR})
    target_sources(${TARGET} PRIVATE ${ERPC_SRC})
    add_dependencies(${TARGET} erpc_${erpc_name}_target)
  endforeach()

endfunction()
