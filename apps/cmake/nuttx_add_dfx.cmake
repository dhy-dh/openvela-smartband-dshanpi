# ##############################################################################
# cmake/nuttx_add_dfx.cmake
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

set(DFX_DIR ${CMAKE_BINARY_DIR}/dfx)

if(NOT EXISTS {DFX_DIR})
  file(MAKE_DIRECTORY ${DFX_DIR})
endif()

# "nuttx_dfx_interface" is a source-less target that encapsulates all the DFX
# compiler options and include path needed by all DFX.
if(NOT TARGET nuttx_dfx_interface)
  add_custom_target(nuttx_dfx_interface)
endif()

include(nuttx_parse_function_args)

# ~~~
# nuttx_add_dfx_event
#
# Description:
#    Register custom dfx event to dfx framework
#
# Example:
#  nuttx_add_dfx_event(
#    XML
#    ${custom_xml}
#
# ~~~

function(nuttx_add_dfx_event)

  # parse arguments into variables

  nuttx_parse_function_args(
    FUNC
    nuttx_add_dfx_event
    ONE_VALUE
    TARGET
    MULTI_VALUE
    XML_PATHS
    REQUIRED
    XML_PATHS
    ARGN
    ${ARGN})

  set_property(TARGET nuttx_dfx_interface PROPERTY DFX_EVENT_XML_DIR ${DFX_DIR})

  foreach(xmlfile ${XML_PATHS})
    get_filename_component(MOD_NAME ${xmlfile} NAME_WE)
    set(TARGET_XML ${DFX_DIR}/${MOD_NAME}.xml)
    set(DFX_EVENT_TARGET event_${MOD_NAME}_target)
    add_custom_command(
      OUTPUT ${TARGET_XML}
      COMMAND ${CMAKE_COMMAND} -E copy ${xmlfile} ${TARGET_XML}
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
      COMMENT "DFX Framework:Gen and Updating xml file => ${xmlfile}")
    add_custom_target(${DFX_EVENT_TARGET} DEPENDS ${TARGET_XML})
    add_dependencies(apps_context ${DFX_EVENT_TARGET})
  endforeach()

endfunction()
