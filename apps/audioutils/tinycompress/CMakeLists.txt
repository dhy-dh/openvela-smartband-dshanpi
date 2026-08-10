# ##############################################################################
# apps/audioutils/tinycompress/CMakeLists.txt
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

if(CONFIG_AUDIOUTILS_TINYCOMPRESS)
  target_sources(apps PRIVATE src/lib/compress_hw.c src/lib/compress.c)

  if(CONFIG_AUDIOUTILS_TINYCOMPRESS_CPLAY)
    nuttx_add_application(
      NAME
      cplay
      SRCS
      ${CMAKE_CURRENT_LIST_DIR}/src/utils/cplay.c
      ${CMAKE_CURRENT_LIST_DIR}/src/utils/wave.c
      STACKSIZE
      ${CONFIG_DEFAULT_TASK_STACKSIZE}
      PRIORITY
      SCHED_PRIORITY_DEFAULT)
  endif()

  if(CONFIG_AUDIOUTILS_TINYCOMPRESS_CRECORD)
    nuttx_add_application(
      NAME
      crecord
      SRCS
      ${CMAKE_CURRENT_LIST_DIR}/src/utils/crecord.c
      STACKSIZE
      ${CONFIG_DEFAULT_TASK_STACKSIZE}
      PRIORITY
      SCHED_PRIORITY_DEFAULT)
  endif()

  set_property(
    TARGET nuttx
    APPEND
    PROPERTY NUTTX_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()
