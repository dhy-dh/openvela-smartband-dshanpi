# ##############################################################################
# apps/examples/sotest/main/CMakeLists.txt
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

if(CONFIG_EXAMPLES_SOTEST)

  if(CONFIG_EXAMPLES_SOTEST_BUILTINFS)
    add_custom_target(
      sotest_romfs
      COMMAND genromfs -f sotest_romfs.img -d ${CMAKE_BINARY_DIR}/bin
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS nuttx_apps_mksymtab)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/sotest_romfs.c
      COMMAND xxd -i sotest_romfs.img >
              ${CMAKE_CURRENT_BINARY_DIR}/sotest_romfs.c
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS sotest_romfs)

    add_library(sotest_romfs_lib)
    target_sources(sotest_romfs_lib
                   PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/sotest_romfs.c)
    nuttx_add_library_internal(sotest_romfs_lib)
    set_property(GLOBAL APPEND PROPERTY NUTTX_SYSTEM_LIBRARIES sotest_romfs_lib)

  endif()

  nuttx_add_application(NAME sotest SRCS sotest_main.c)

  set(SYMTAB_APPS_SOURCE ${CMAKE_BINARY_DIR}/sotest_symtab_apps.c)

  add_custom_command(
    OUTPUT ${SYMTAB_APPS_SOURCE} always_rebuild_symtab
    COMMAND ${NUTTX_APPS_DIR}/tools/mksymtab.sh ${CMAKE_BINARY_DIR}/bin g_sot >
            ${CMAKE_BINARY_DIR}/sotest_symtab_apps.c
    COMMAND ${CMAKE_COMMAND} -E touch always_rebuild_symtab
    COMMAND ${CMAKE_COMMAND} -E remove always_rebuild_symtab
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS nuttx_apps_mksymtab
    VERBATIM
    COMMENT "Mksymtab: generating symbol table for sotest")
  add_custom_target(sotest_symtab_source_gen ALL DEPENDS ${SYMTAB_APPS_SOURCE})

  add_library(sotest_symtab)
  add_dependencies(sotest_symtab sotest_symtab_source_gen)
  target_compile_options(sotest_symtab PRIVATE ${NO_LTO} -fno-builtin)
  target_sources(sotest_symtab PRIVATE ${SYMTAB_APPS_SOURCE})
  nuttx_add_library_internal(sotest_symtab)
  set_property(GLOBAL APPEND PROPERTY NUTTX_SYSTEM_LIBRARIES sotest_symtab)

endif()
