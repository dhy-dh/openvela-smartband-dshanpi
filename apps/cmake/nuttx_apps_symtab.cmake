# ##############################################################################
# cmake/nuttx_apps_symtab.cmake
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

if(NOT TARGET nuttx_apps_mksymtab)
  add_custom_target(nuttx_apps_mksymtab)
endif()

if(CONFIG_EXECFUNCS_HAVE_SYMTAB
   OR CONFIG_NSH_SYMTAB
   OR CONFIG_LIBC_ELF_HAVE_SYMTAB)
  set(SYMTAB_APPS_SOURCE ${CMAKE_BINARY_DIR}/symtab_apps.c)

  add_custom_command(
    OUTPUT ${SYMTAB_APPS_SOURCE} always_rebuild_symtab
    COMMAND ${NUTTX_APPS_DIR}/tools/mksymtab.sh ${CMAKE_BINARY_DIR}/bin >
            ${CMAKE_BINARY_DIR}/symtab_apps.c
    COMMAND ${CMAKE_COMMAND} -E touch always_rebuild_symtab
    COMMAND ${CMAKE_COMMAND} -E remove always_rebuild_symtab
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS nuttx_apps_mksymtab
    VERBATIM
    COMMENT "Mksymtab: generating symbol table for apps")
  add_custom_target(symtab_source_gen ALL DEPENDS ${SYMTAB_APPS_SOURCE})

  add_library(apps_symtab)
  add_dependencies(apps_symtab symtab_source_gen)
  if(NOT CONFIG_TRICORE_TOOLCHAIN_TASKING)
    target_compile_options(
      apps_symtab PRIVATE ${NO_LTO} -fno-builtin
                          -Wno-builtin-declaration-mismatch)
  endif()
  target_sources(apps_symtab PRIVATE ${SYMTAB_APPS_SOURCE})
  nuttx_add_library_internal(apps_symtab)
  set_property(GLOBAL APPEND PROPERTY NUTTX_SYSTEM_LIBRARIES apps_symtab)
endif()
