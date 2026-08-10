# ##############################################################################
# cmake/nuttx_add_jidl.cmake
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
include(nuttx_add_rust)

# ~~~
# nuttx_add_jidl
#
# Description:
#   Generate source code files for JIDL, adding them to the given target and generate feature registry info
#
# Example:
#  nuttx_add_jidl(
#    TARGET
#    libfeature
#    FEATURE_SRCS
#    error_impl.cpp
#    JIDL_OUT_DIR
#    ${CMAKE_CURRENT_BINARY_DIR}
#    JIDLS
#    test.jidl
#    FEATURE_NAMES
#    Error
#    OUT_SRC_EXT
#    cpp)
# ~~~

set(QUICKAPP_FEATURES_OUT_DIR ${CMAKE_BINARY_DIR}/jidl_generated)
set(JIDL_TOOL ${NUTTX_APPS_DIR}/../prebuilts/tools/rust/bin/jidl/jidl_gen_cpp)

if(NOT TARGET remove_jidl_generated)
  file(REMOVE_RECURSE ${QUICKAPP_FEATURES_OUT_DIR})
  add_custom_target(remove_jidl_generated)
endif()

if(NOT EXISTS {QUICKAPP_FEATURES_OUT_DIR})
  file(MAKE_DIRECTORY ${QUICKAPP_FEATURES_OUT_DIR})
endif()

if(NOT EXISTS ${QUICKAPP_FEATURES_OUT_DIR}/features_registry_list.h)
  file(WRITE ${QUICKAPP_FEATURES_OUT_DIR}/features_registry_list.h "")
endif()

if(NOT EXISTS ${QUICKAPP_FEATURES_OUT_DIR}/cfeatures_registry_list.h)
  file(WRITE ${QUICKAPP_FEATURES_OUT_DIR}/cfeatures_registry_list.h "")
endif()

if(NOT EXISTS ${QUICKAPP_FEATURES_OUT_DIR}/features_registry_table.h)
  file(WRITE ${QUICKAPP_FEATURES_OUT_DIR}/features_registry_table.h "")
endif()

function(nuttx_add_jidl)

  # parse arguments into variables

  nuttx_parse_function_args(
    FUNC
    nuttx_add_jidl
    ONE_VALUE
    TARGET
    JIDL_OUT_DIR
    OUT_SRC_EXT
    JIDL_FLAGS
    MULTI_VALUE
    FEATURE_SRCS
    JIDLS
    JIDL_OUT_NAMES
    FEATURE_NAMES
    REQUIRED
    TARGET
    FEATURE_SRCS
    FEATURE_NAMES
    JIDLS
    ARGN
    ${ARGN})

  if(NOT OUT_SRC_EXT)
    set(OUT_SRC_EXT "cpp")
  endif()

  if(NOT JIDL_OUT_DIR)
    set(JIDL_OUT_DIR ${QUICKAPP_FEATURES_OUT_DIR}/jidl)
  endif()

  target_include_directories(
    ${TARGET} PRIVATE ${NUTTX_APPS_DIR}/frameworks/runtimes/feature/include)

  list(LENGTH JIDLS len)
  math(EXPR len "${len} - 1")

  if(NOT JIDL_OUT_NAMES)
    foreach(index RANGE ${len})
      list(GET JIDLS ${index} JIDL_PATH)
      get_filename_component(JIDL_OUT_NAME ${JIDL_PATH} NAME_WE)
      list(APPEND JIDL_OUT_NAMES ${JIDL_OUT_NAME})
    endforeach()
  endif()

  foreach(index RANGE ${len})
    list(GET JIDLS ${index} JIDL_PATH)
    list(GET JIDL_OUT_NAMES ${index} JIDL_OUT_NAME)
    get_filename_component(JIDL_NAME ${JIDL_PATH} NAME_WE)

    set(JIDL_SRC ${JIDL_OUT_DIR}/${JIDL_OUT_NAME}.${OUT_SRC_EXT})
    set(JIDL_HEADER ${JIDL_OUT_DIR}/${JIDL_OUT_NAME}.h)
    file(WRITE ${JIDL_SRC} )
    file(WRITE ${JIDL_HEADER} )

    set(JIDL_TARGET jidl_${JIDL_NAME}_target)
    add_custom_target(
      ${JIDL_TARGET}
      COMMAND
        ${JIDL_TOOL} ${JIDL_PATH} ${JIDL_FLAGS} --out-dir ${JIDL_OUT_DIR}
        --header ${JIDL_OUT_NAME}.h --source ${JIDL_OUT_NAME}.${OUT_SRC_EXT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      COMMENT "JIDL: generating glue files for ${JIDL_NAME}.jidl")

    add_dependencies(${TARGET} ${JIDL_TARGET})
    target_sources(${TARGET} PRIVATE ${JIDL_SRC})
  endforeach()

  target_include_directories(${TARGET} PRIVATE ${JIDL_OUT_DIR})
  target_include_directories(${TARGET} PRIVATE ${QUICKAPP_FEATURES_OUT_DIR})

  set(FEATURE_REGISTRY_TABLE
      ${QUICKAPP_FEATURES_OUT_DIR}/features_registry_table.h)
  foreach(feature_name ${FEATURE_NAMES})
    set(FEATURE_PDAT ${QUICKAPP_FEATURES_OUT_DIR}/feature_${feature_name}.pdat)
    if(OUT_SRC_EXT STREQUAL "cpp")
      set(QUICKAPP_FEATURE_LIST
          ${QUICKAPP_FEATURES_OUT_DIR}/features_registry_list.h)
    else()
      set(QUICKAPP_FEATURE_LIST
          ${QUICKAPP_FEATURES_OUT_DIR}/cfeatures_registry_list.h)
    endif()
    add_custom_command(
      OUTPUT ${FEATURE_PDAT}
      COMMAND
        echo
        "bool jse_${feature_name}_initFeature(FeatureRegistryHandle handle); "
        >> ${QUICKAPP_FEATURE_LIST}
      COMMAND echo "jse_${feature_name}_initFeature," >>
              ${FEATURE_REGISTRY_TABLE}
      COMMAND touch ${FEATURE_PDAT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      VERBATIM
      COMMENT "generate quickapp feature registry info: ${feature_name}")
    add_custom_target(quickapp_feature_${feature_name} DEPENDS ${FEATURE_PDAT})
    add_dependencies(apps_context quickapp_feature_${feature_name})
  endforeach()

  foreach(feature_src ${FEATURE_SRCS})
    target_sources(${TARGET} PRIVATE ${feature_src})
  endforeach()

endfunction()

function(nuttx_add_jidl_rust)

  # parse arguments into variables
  nuttx_parse_function_args(
    FUNC
    nuttx_add_jidl_rust
    ONE_VALUE
    TARGET
    JIDL_OUT_DIR
    JIDL_FLAGS
    MULTI_VALUE
    FEATURE_CRATE
    JIDLS
    FEATURE_NAMES
    REQUIRED
    TARGET
    FEATURE_CRATE
    FEATURE_NAMES
    JIDLS
    ARGN
    ${ARGN})

  if(NOT JIDL_OUT_DIR)
    set(JIDL_OUT_DIR ${QUICKAPP_FEATURES_OUT_DIR}/jidl)
  endif()

  target_include_directories(
    ${TARGET} PRIVATE ${NUTTX_APPS_DIR}/frameworks/runtimes/feature/include)

  foreach(JIDL_PATH ${JIDLS})
    get_filename_component(JIDL_NAME ${JIDL_PATH} NAME_WE)
    set(JIDL_SRC ${JIDL_OUT_DIR}/${JIDL_NAME}.c)
    set(JIDL_HEADER ${JIDL_OUT_DIR}/${JIDL_NAME}.h)
    file(WRITE ${JIDL_SRC} )
    file(WRITE ${JIDL_HEADER} )

    set(JIDL_TARGET jidl_rust_${JIDL_NAME}_target)
    # TODO: add rust flags to the jidl tool
    add_custom_target(
      ${JIDL_TARGET}
      COMMAND ${JIDL_TOOL} ${JIDL_PATH} ${JIDL_FLAGS} --out-dir ${JIDL_OUT_DIR}
              --header ${JIDL_NAME}.h --source ${JIDL_NAME}.c
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      COMMENT "JIDL: generating glue files for ${JIDL_NAME}.jidl")

    add_dependencies(${TARGET} ${JIDL_TARGET})
    target_sources(${TARGET} PRIVATE ${JIDL_SRC})
  endforeach()

  foreach(CRATE_PATH ${FEATURE_CRATE})
    get_filename_component(CRATE_NAME ${CRATE_PATH} NAME)
    nuttx_add_rust(CRATE_NAME ${CRATE_NAME} CRATE_PATH ${CRATE_PATH})
  endforeach(CRATE_PATH ${FEATURE_CRATE})

  target_include_directories(${TARGET} PRIVATE ${JIDL_OUT_DIR})
  target_include_directories(${TARGET} PRIVATE ${QUICKAPP_FEATURES_OUT_DIR})

  set(FEATURE_REGISTRY_TABLE
      ${QUICKAPP_FEATURES_OUT_DIR}/features_registry_table.h)
  foreach(feature_name ${FEATURE_NAMES})
    set(FEATURE_PDAT ${QUICKAPP_FEATURES_OUT_DIR}/feature_${feature_name}.pdat)
    set(QUICKAPP_FEATURE_LIST
        ${QUICKAPP_FEATURES_OUT_DIR}/cfeatures_registry_list.h)
    add_custom_command(
      OUTPUT ${FEATURE_PDAT}
      COMMAND
        echo
        "bool jse_${feature_name}_initFeature(FeatureRegistryHandle handle); "
        >> ${QUICKAPP_FEATURE_LIST}
      COMMAND echo "jse_${feature_name}_initFeature," >>
              ${FEATURE_REGISTRY_TABLE}
      COMMAND touch ${FEATURE_PDAT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      VERBATIM
      COMMENT "generate quickapp feature registry info: ${feature_name}")
    add_custom_target(quickapp_feature_${feature_name} DEPENDS ${FEATURE_PDAT})
    add_dependencies(apps_context quickapp_feature_${feature_name})
  endforeach()
endfunction()
