# ##############################################################################
# cmake/nuttx_add_aidl.cmake
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

# "nuttx_aidl_interface" is a source-less target that encapsulates all the AIDL
# compiler options and include path needed by all AIDL.
if(NOT TARGET nuttx_aidl_interface)
  add_custom_target(nuttx_aidl_interface)
endif()

# the visible scope is all the AIDL include search path
function(include_directories_for_all_aidl)
  set_property(
    TARGET nuttx_aidl_interface
    APPEND
    PROPERTY AIDL_INCLUDE_DIRECTORIES ${ARGN})
endfunction()

# all AIDL flags
function(compile_flags_for_all_aidl)
  set_property(
    TARGET nuttx_aidl_interface
    APPEND
    PROPERTY AIDL_COMPILE_FLAGS ${ARGN})
endfunction()

# ~~~
# nuttx_add_aidl
#
# Description:
#   Generate source code files using AIDL and add them to the given target
#
# Example:
#  nuttx_add_aidl(
#    TARGET
#    apps
#    AIDL_INCLUDE_DIR
#    ${CMAKE_CURRENT_LIST_DIR}
#    AIDL_HEADER_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}
#    AIDL_OUT_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}
#    AIDL_FLAGS
#    --lang=cpp
#    AIDLS
#    TEST.aidl
# ~~~

function(nuttx_add_aidl)

  # parse arguments into variables

  nuttx_parse_function_args(
    FUNC
    nuttx_add_aidl
    ONE_VALUE
    TARGET
    AIDL_BASE_DIR
    AIDL_HEADER_DIR
    AIDL_OUT_DIR
    MULTI_VALUE
    AIDLS
    AIDL_INCLUDE_DIR
    AIDL_FLAGS
    REQUIRED
    TARGET
    AIDLS
    AIDL_FLAGS
    ARGN
    ${ARGN})

  # process global aidl options
  get_property(
    aidl_include_path_gobal_scope
    TARGET nuttx_aidl_interface
    PROPERTY AIDL_INCLUDE_DIRECTORIES)
  get_property(
    aidl_flags_gobal_scope
    TARGET nuttx_aidl_interface
    PROPERTY AIDL_COMPILE_FLAGS)

  # add flags in FRONT and add search path BEHIND
  list(PREPEND AIDL_FLAGS ${aidl_flags_gobal_scope})
  list(APPEND AIDL_INCLUDE_DIR ${aidl_include_path_gobal_scope})

  # concat aidl tool command
  string(JOIN " " AIDL_FLAGS_STR ${AIDL_FLAGS})

  set(AIDL_CMD_STR "aidl ${AIDL_FLAGS_STR}")

  if(AIDL_HEADER_DIR)
    string(APPEND AIDL_CMD_STR " -h${AIDL_HEADER_DIR}")
  endif()

  if(AIDL_OUT_DIR)
    string(APPEND AIDL_CMD_STR " -o${AIDL_OUT_DIR}")
  endif()

  if(AIDL_INCLUDE_DIR)
    foreach(_inc_dir ${AIDL_INCLUDE_DIR})
      string(APPEND AIDL_CMD_STR " -I${_inc_dir}")
    endforeach()
  endif()

  foreach(aidl_file ${AIDLS})
    get_filename_component(aidl_file_full ${aidl_file} ABSOLUTE)
    file(RELATIVE_PATH REL_PATH ${AIDL_BASE_DIR} ${aidl_file_full})
    get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
    # the generated source code and AIDL file have the same name
    get_filename_component(FILE_NAME ${aidl_file} NAME_WE)
    separate_arguments(AIDL_CMD UNIX_COMMAND ${AIDL_CMD_STR})
    set(AIDL_OUTPUT_SOURCE ${AIDL_OUT_DIR}/${REL_DIR}/${FILE_NAME}.cpp)
    set(INTER_TARGET ${TARGET}_${FILE_NAME})
    add_custom_command(
      OUTPUT ${AIDL_OUTPUT_SOURCE}
      COMMAND ${AIDL_CMD} ${aidl_file}
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      DEPENDS ${aidl_file_full}
      VERBATIM
      COMMENT "AIDL:Gen and Updating ${AIDL_OUTPUT_SOURCE}")

    add_custom_target(${INTER_TARGET} DEPENDS ${AIDL_OUTPUT_SOURCE})
    add_dependencies(apps_context ${INTER_TARGET})
    add_dependencies(${TARGET} ${INTER_TARGET})
    target_sources(${TARGET} PRIVATE ${AIDL_OUTPUT_SOURCE})
    target_include_directories(${TARGET} PRIVATE ${AIDL_HEADER_DIR})
  endforeach()

endfunction()
