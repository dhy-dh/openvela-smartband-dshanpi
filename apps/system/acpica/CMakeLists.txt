# ##############################################################################
# apps/system/acpica/CMakeLists.txt
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

if(CONFIG_SYSTEM_ACPICA)

  set(ACPICA_DIR ${CMAKE_CURRENT_LIST_DIR}/acpica)

  if(NOT EXISTS ${ACPICA_DIR})
    set(CONFIG_ACPICA_URL https://github.com/acpica/acpica/archive/refs/heads)

    FetchContent_Declare(
      acpica_fetch
      URL ${CONFIG_ACPICA_URL}/master.zip SOURCE_DIR
          ${CMAKE_CURRENT_LIST_DIR}/acpica BINARY_DIR
          ${CMAKE_BINARY_DIR}/apps/system/acpica/acpica
      DOWNLOAD_NO_PROGRESS true
      TIMEOUT 30)

    FetchContent_GetProperties(acpica_fetch)
    if(NOT acpica_fetch_POPULATED)
      FetchContent_Populate(acpica_fetch)
    endif()
  endif()

  set(ACPICA_DIR ${CMAKE_CURRENT_LIST_DIR}/acpica)
  set(ACPICA_SRC ${ACPICA_DIR}/source)

  set(INCDIR ${ACPICA_SRC}/ ${ACPICA_SRC}/compiler/ ${ACPICA_SRC}/include/
             ${ACPICA_SRC}/include/platform/ ${ACPICA_SRC}/tools/acpisrc/)

  set(CFLAGS -D_CYGWIN -D_GNU_SOURCE -DACPI_ASL_COMPILER -DACPI_BIN_APP
             -DACPI_XTRACT_APP -DACPI_SRC_APP)

  set(ASL_COMPILER ${ACPICA_SRC}/compiler)

  find_program(YACC bison REQUIRED)
  find_program(LEX flex REQUIRED)
  find_program(MACROPROC m4 REQUIRED)

  set(YFLAGS -y)
  set(MFLAGS -P -I${ASL_COMPILER})
  set(LFLAGS -i -s)
  set(DLFLAGS -i)

  set(HEADERS
      ${ASL_COMPILER}/*.h ${ACPICA_SRC}/aslcompiler.y.h
      ${ACPICA_SRC}/dtcompilerparser.y.h ${ACPICA_SRC}/dtparser.y.h
      ${ACPICA_SRC}/prparser.y.h)

  set(INTERMEDIATES
      ${ACPICA_SRC}/aslcompiler.y
      ${ACPICA_SRC}/aslcompilerlex.c
      ${ACPICA_SRC}/aslcompilerparse.c
      ${ACPICA_SRC}/dtparserlex.c
      ${ACPICA_SRC}/dtparserparse.c
      ${ACPICA_SRC}/dtcompilerparserlex.c
      ${ACPICA_SRC}/dtcompilerparserparse.c
      ${ACPICA_SRC}/prparserlex.c
      ${ACPICA_SRC}/prparserparse.c)

  set(MISC ${ACPICA_SRC}/aslcompiler.y.h ${ACPICA_SRC}/dtcompilerparser.y.h
           ${ACPICA_SRC}/dtparser.y.h ${ACPICA_SRC}/prparser.y.h)

  set(ASL_PARSER
      ${ASL_COMPILER}/aslcstyle.y
      ${ASL_COMPILER}/aslhelpers.y
      ${ASL_COMPILER}/aslparser.y
      ${ASL_COMPILER}/aslprimaries.y
      ${ASL_COMPILER}/aslresources.y
      ${ASL_COMPILER}/aslrules.y
      ${ASL_COMPILER}/aslsupport.y
      ${ASL_COMPILER}/asltokens.y
      ${ASL_COMPILER}/asltypes.y)

  set(ASL_LEXER ${ASL_COMPILER}/aslcompiler.l ${ASL_COMPILER}/aslsupport.l
                ${ACPICA_SRC}/aslcompiler.y.h)

  function(safe_yacc PREFIX INPUT OUTPUT_C OUTPUT_H)
    set(TEMP_DIR "${CMAKE_BINARY_DIR}/${PREFIX}_temp")
    file(MAKE_DIRECTORY ${TEMP_DIR})

    add_custom_command(
      OUTPUT ${OUTPUT_C} ${OUTPUT_H}
      COMMAND ${YACC} ${YFLAGS} -v -d -p${PREFIX} -o ${TEMP_DIR}/y.tab.c
              ${INPUT}
      COMMAND mv ${TEMP_DIR}/y.tab.c ${OUTPUT_C}
      COMMAND mv ${TEMP_DIR}/y.tab.h ${OUTPUT_H}
      COMMAND sed -i 's|"y.tab.h"|"'${OUTPUT_H}'"|' ${OUTPUT_C}
      COMMAND rm -rf ${TEMP_DIR}
      DEPENDS ${INPUT}
      COMMENT "Generating ${OUTPUT_C} and ${OUTPUT_H} using ${YACC}")
  endfunction()

  # Macro processing for iASL .y files

  add_custom_command(
    OUTPUT ${ACPICA_SRC}/aslcompiler.y
    COMMAND ${MACROPROC} ${MFLAGS} ${ASL_COMPILER}/aslparser.y >
            ${ACPICA_SRC}/aslcompiler.y
    DEPENDS ${ASL_COMPILER}/aslparser.y
    COMMENT "Preprocessing main iASL parser")

  # Define the yacc and lex rules

  safe_yacc(AslCompiler ${ACPICA_SRC}/aslcompiler.y
            ${ACPICA_SRC}/aslcompilerparse.c ${ACPICA_SRC}/aslcompiler.y.h)

  add_custom_command(
    OUTPUT ${ACPICA_SRC}/aslcompilerlex.c
    COMMAND ${LEX} ${LFLAGS} -PAslCompiler -o${ACPICA_SRC}/aslcompilerlex.c
            ${ASL_COMPILER}/aslcompiler.l
    DEPENDS ${ASL_COMPILER}/aslcompiler.l ${ACPICA_SRC}/aslcompiler.y.h
    COMMENT "Generating aslcompilerlex.c")

  safe_yacc(DtCompilerParser ${ASL_COMPILER}/dtcompilerparser.y
            ${ACPICA_SRC}/dtcompilerparserparse.c
            ${ACPICA_SRC}/dtcompilerparser.y.h)

  add_custom_command(
    OUTPUT ${ACPICA_SRC}/dtcompilerparserlex.c
    COMMAND
      ${LEX} ${DLFLAGS} -PDtCompilerParser -o${ACPICA_SRC}/dtcompilerparserlex.c
      ${ASL_COMPILER}/dtcompilerparser.l
    DEPENDS ${ASL_COMPILER}/dtcompilerparser.l
            ${ACPICA_SRC}/dtcompilerparser.y.h
    COMMENT "Generating dtcompilerparserlex.c")

  safe_yacc(DtParser ${ASL_COMPILER}/dtparser.y ${ACPICA_SRC}/dtparserparse.c
            ${ACPICA_SRC}/dtparser.y.h)

  add_custom_command(
    OUTPUT ${ACPICA_SRC}/dtparserlex.c
    COMMAND ${LEX} ${DLFLAGS} -PDtParser -o${ACPICA_SRC}/dtparserlex.c
            ${ASL_COMPILER}/dtparser.l
    DEPENDS ${ASL_COMPILER}/dtparser.l ${ACPICA_SRC}/dtparser.y.h
    COMMENT "Generating dtparserlex.c")

  safe_yacc(PrParser ${ASL_COMPILER}/prparser.y ${ACPICA_SRC}/prparserparse.c
            ${ACPICA_SRC}/prparser.y.h)

  add_custom_command(
    OUTPUT ${ACPICA_SRC}/prparserlex.c
    COMMAND ${LEX} ${LFLAGS} -PPrParser -o${ACPICA_SRC}/prparserlex.c
            ${ASL_COMPILER}/prparser.l
    DEPENDS ${ASL_COMPILER}/prparser.l ${ACPICA_SRC}/prparser.y.h
    COMMENT "Generating prparserlex.c")

  set(SRCS
      ${ACPICA_SRC}/aslcompilerlex.c
      ${ACPICA_SRC}/aslcompilerparse.c
      ${ACPICA_SRC}/dtcompilerparserlex.c
      ${ACPICA_SRC}/dtcompilerparserparse.c
      ${ACPICA_SRC}/dtparserlex.c
      ${ACPICA_SRC}/dtparserparse.c
      ${ACPICA_SRC}/prparserlex.c
      ${ACPICA_SRC}/prparserparse.c
      ${ACPICA_SRC}/os_specific/service_layers/osunixxf.c
      ${ACPICA_SRC}/os_specific/service_layers/osunixdir.c
      ${ACPICA_SRC}/components/debugger/dbfileio.c
      ${ACPICA_SRC}/components/disassembler/dmbuffer.c
      ${ACPICA_SRC}/components/disassembler/dmcstyle.c
      ${ACPICA_SRC}/components/disassembler/dmdeferred.c
      ${ACPICA_SRC}/components/disassembler/dmnames.c
      ${ACPICA_SRC}/components/disassembler/dmopcode.c
      ${ACPICA_SRC}/components/disassembler/dmresrc.c
      ${ACPICA_SRC}/components/disassembler/dmresrcl.c
      ${ACPICA_SRC}/components/disassembler/dmresrcl2.c
      ${ACPICA_SRC}/components/disassembler/dmresrcs.c
      ${ACPICA_SRC}/components/disassembler/dmutils.c
      ${ACPICA_SRC}/components/disassembler/dmwalk.c
      ${ACPICA_SRC}/components/dispatcher/dsargs.c
      ${ACPICA_SRC}/components/dispatcher/dscontrol.c
      ${ACPICA_SRC}/components/dispatcher/dsfield.c
      ${ACPICA_SRC}/components/dispatcher/dsobject.c
      ${ACPICA_SRC}/components/dispatcher/dsopcode.c
      ${ACPICA_SRC}/components/dispatcher/dspkginit.c
      ${ACPICA_SRC}/components/dispatcher/dsutils.c
      ${ACPICA_SRC}/components/dispatcher/dswexec.c
      ${ACPICA_SRC}/components/dispatcher/dswload.c
      ${ACPICA_SRC}/components/dispatcher/dswload2.c
      ${ACPICA_SRC}/components/dispatcher/dswscope.c
      ${ACPICA_SRC}/components/dispatcher/dswstate.c
      ${ACPICA_SRC}/components/executer/exconcat.c
      ${ACPICA_SRC}/components/executer/exconvrt.c
      ${ACPICA_SRC}/components/executer/excreate.c
      ${ACPICA_SRC}/components/executer/exdump.c
      ${ACPICA_SRC}/components/executer/exmisc.c
      ${ACPICA_SRC}/components/executer/exmutex.c
      ${ACPICA_SRC}/components/executer/exnames.c
      ${ACPICA_SRC}/components/executer/exoparg1.c
      ${ACPICA_SRC}/components/executer/exoparg2.c
      ${ACPICA_SRC}/components/executer/exoparg3.c
      ${ACPICA_SRC}/components/executer/exoparg6.c
      ${ACPICA_SRC}/components/executer/exprep.c
      ${ACPICA_SRC}/components/executer/exregion.c
      ${ACPICA_SRC}/components/executer/exresnte.c
      ${ACPICA_SRC}/components/executer/exresolv.c
      ${ACPICA_SRC}/components/executer/exresop.c
      ${ACPICA_SRC}/components/executer/exstore.c
      ${ACPICA_SRC}/components/executer/exstoren.c
      ${ACPICA_SRC}/components/executer/exstorob.c
      ${ACPICA_SRC}/components/executer/exsystem.c
      ${ACPICA_SRC}/components/executer/exutils.c
      ${ACPICA_SRC}/components/namespace/nsaccess.c
      ${ACPICA_SRC}/components/namespace/nsalloc.c
      ${ACPICA_SRC}/components/namespace/nsdump.c
      ${ACPICA_SRC}/components/namespace/nsnames.c
      ${ACPICA_SRC}/components/namespace/nsobject.c
      ${ACPICA_SRC}/components/namespace/nsparse.c
      ${ACPICA_SRC}/components/namespace/nssearch.c
      ${ACPICA_SRC}/components/namespace/nsutils.c
      ${ACPICA_SRC}/components/namespace/nswalk.c
      ${ACPICA_SRC}/components/namespace/nsxfobj.c
      ${ACPICA_SRC}/components/parser/psargs.c
      ${ACPICA_SRC}/components/parser/psloop.c
      ${ACPICA_SRC}/components/parser/psobject.c
      ${ACPICA_SRC}/components/parser/psopcode.c
      ${ACPICA_SRC}/components/parser/psopinfo.c
      ${ACPICA_SRC}/components/parser/psparse.c
      ${ACPICA_SRC}/components/parser/psscope.c
      ${ACPICA_SRC}/components/parser/pstree.c
      ${ACPICA_SRC}/components/parser/psutils.c
      ${ACPICA_SRC}/components/parser/pswalk.c
      ${ACPICA_SRC}/components/tables/tbdata.c
      ${ACPICA_SRC}/components/tables/tbxfroot.c
      ${ACPICA_SRC}/components/tables/tbfadt.c
      ${ACPICA_SRC}/components/tables/tbinstal.c
      ${ACPICA_SRC}/components/tables/tbprint.c
      ${ACPICA_SRC}/components/tables/tbutils.c
      ${ACPICA_SRC}/components/tables/tbxface.c
      ${ACPICA_SRC}/components/tables/tbxfload.c
      ${ACPICA_SRC}/components/utilities/utaddress.c
      ${ACPICA_SRC}/components/utilities/utalloc.c
      ${ACPICA_SRC}/components/utilities/utascii.c
      ${ACPICA_SRC}/components/utilities/utbuffer.c
      ${ACPICA_SRC}/components/utilities/utcache.c
      ${ACPICA_SRC}/components/utilities/utcksum.c
      ${ACPICA_SRC}/components/utilities/utcopy.c
      ${ACPICA_SRC}/components/utilities/utdebug.c
      ${ACPICA_SRC}/components/utilities/utdecode.c
      ${ACPICA_SRC}/components/utilities/utdelete.c
      ${ACPICA_SRC}/components/utilities/uterror.c
      ${ACPICA_SRC}/components/utilities/utexcep.c
      ${ACPICA_SRC}/components/utilities/utglobal.c
      ${ACPICA_SRC}/components/utilities/uthex.c
      ${ACPICA_SRC}/components/utilities/utinit.c
      ${ACPICA_SRC}/components/utilities/utlock.c
      ${ACPICA_SRC}/components/utilities/utmath.c
      ${ACPICA_SRC}/components/utilities/utmisc.c
      ${ACPICA_SRC}/components/utilities/utmutex.c
      ${ACPICA_SRC}/components/utilities/utnonansi.c
      ${ACPICA_SRC}/components/utilities/utobject.c
      ${ACPICA_SRC}/components/utilities/utownerid.c
      ${ACPICA_SRC}/components/utilities/utpredef.c
      ${ACPICA_SRC}/components/utilities/utresdecode.c
      ${ACPICA_SRC}/components/utilities/utresrc.c
      ${ACPICA_SRC}/components/utilities/utstate.c
      ${ACPICA_SRC}/components/utilities/utstrtoul64.c
      ${ACPICA_SRC}/components/utilities/utstrsuppt.c
      ${ACPICA_SRC}/components/utilities/utstring.c
      ${ACPICA_SRC}/components/utilities/utuuid.c
      ${ACPICA_SRC}/components/utilities/utxface.c
      ${ACPICA_SRC}/components/utilities/utxferror.c
      ${ACPICA_SRC}/compiler/aslmapoutput.c
      ${ACPICA_SRC}/compiler/asltransform.c
      ${ACPICA_SRC}/compiler/aslallocate.c
      ${ACPICA_SRC}/compiler/aslanalyze.c
      ${ACPICA_SRC}/compiler/aslascii.c
      ${ACPICA_SRC}/compiler/aslbtypes.c
      ${ACPICA_SRC}/compiler/aslcache.c
      ${ACPICA_SRC}/compiler/aslcodegen.c
      ${ACPICA_SRC}/compiler/aslcompile.c
      ${ACPICA_SRC}/compiler/asldebug.c
      ${ACPICA_SRC}/compiler/aslerror.c
      ${ACPICA_SRC}/compiler/aslexternal.c
      ${ACPICA_SRC}/compiler/aslfiles.c
      ${ACPICA_SRC}/compiler/aslfileio.c
      ${ACPICA_SRC}/compiler/aslfold.c
      ${ACPICA_SRC}/compiler/aslhelp.c
      ${ACPICA_SRC}/compiler/aslhex.c
      ${ACPICA_SRC}/compiler/asllength.c
      ${ACPICA_SRC}/compiler/asllisting.c
      ${ACPICA_SRC}/compiler/asllistsup.c
      ${ACPICA_SRC}/compiler/aslload.c
      ${ACPICA_SRC}/compiler/asllookup.c
      ${ACPICA_SRC}/compiler/aslmain.c
      ${ACPICA_SRC}/compiler/aslmap.c
      ${ACPICA_SRC}/compiler/aslmapenter.c
      ${ACPICA_SRC}/compiler/aslmaputils.c
      ${ACPICA_SRC}/compiler/aslmessages.c
      ${ACPICA_SRC}/compiler/aslmethod.c
      ${ACPICA_SRC}/compiler/aslnamesp.c
      ${ACPICA_SRC}/compiler/asloffset.c
      ${ACPICA_SRC}/compiler/aslopcodes.c
      ${ACPICA_SRC}/compiler/asloperands.c
      ${ACPICA_SRC}/compiler/aslopt.c
      ${ACPICA_SRC}/compiler/asloptions.c
      ${ACPICA_SRC}/compiler/aslparseop.c
      ${ACPICA_SRC}/compiler/aslpredef.c
      ${ACPICA_SRC}/compiler/aslprepkg.c
      ${ACPICA_SRC}/compiler/aslprintf.c
      ${ACPICA_SRC}/compiler/aslprune.c
      ${ACPICA_SRC}/compiler/aslresource.c
      ${ACPICA_SRC}/compiler/aslrestype1.c
      ${ACPICA_SRC}/compiler/aslrestype1i.c
      ${ACPICA_SRC}/compiler/aslrestype2.c
      ${ACPICA_SRC}/compiler/aslrestype2d.c
      ${ACPICA_SRC}/compiler/aslrestype2e.c
      ${ACPICA_SRC}/compiler/aslrestype2q.c
      ${ACPICA_SRC}/compiler/aslrestype2s.c
      ${ACPICA_SRC}/compiler/aslrestype2w.c
      ${ACPICA_SRC}/compiler/aslstartup.c
      ${ACPICA_SRC}/compiler/aslstubs.c
      ${ACPICA_SRC}/compiler/aslpld.c
      ${ACPICA_SRC}/compiler/asltree.c
      ${ACPICA_SRC}/compiler/aslutils.c
      ${ACPICA_SRC}/compiler/asluuid.c
      ${ACPICA_SRC}/compiler/aslwalks.c
      ${ACPICA_SRC}/compiler/aslxref.c
      ${ACPICA_SRC}/compiler/aslxrefout.c
      ${ACPICA_SRC}/compiler/cvcompiler.c
      ${ACPICA_SRC}/compiler/cvdisasm.c
      ${ACPICA_SRC}/compiler/cvparser.c
      ${ACPICA_SRC}/compiler/dtcompile.c
      ${ACPICA_SRC}/compiler/dtexpress.c
      ${ACPICA_SRC}/compiler/dtfield.c
      ${ACPICA_SRC}/compiler/dtio.c
      ${ACPICA_SRC}/compiler/dtsubtable.c
      ${ACPICA_SRC}/compiler/dttable.c
      ${ACPICA_SRC}/compiler/dttable1.c
      ${ACPICA_SRC}/compiler/dttable2.c
      ${ACPICA_SRC}/compiler/dttemplate.c
      ${ACPICA_SRC}/compiler/dtutils.c
      ${ACPICA_SRC}/compiler/prexpress.c
      ${ACPICA_SRC}/compiler/prmacros.c
      ${ACPICA_SRC}/compiler/prscan.c
      ${ACPICA_SRC}/compiler/prutils.c
      ${ACPICA_SRC}/common/acfileio.c
      ${ACPICA_SRC}/common/dmextern.c
      ${ACPICA_SRC}/common/adfile.c
      ${ACPICA_SRC}/common/adisasm.c
      ${ACPICA_SRC}/common/adwalk.c
      ${ACPICA_SRC}/common/ahids.c
      ${ACPICA_SRC}/common/ahpredef.c
      ${ACPICA_SRC}/common/ahtable.c
      ${ACPICA_SRC}/common/ahuuids.c
      ${ACPICA_SRC}/common/cmfsize.c
      ${ACPICA_SRC}/common/dmrestag.c
      ${ACPICA_SRC}/common/dmswitch.c
      ${ACPICA_SRC}/common/dmtable.c
      ${ACPICA_SRC}/common/dmtables.c
      ${ACPICA_SRC}/common/dmtbdump.c
      ${ACPICA_SRC}/common/dmtbdump1.c
      ${ACPICA_SRC}/common/dmtbdump2.c
      ${ACPICA_SRC}/common/dmtbdump3.c
      ${ACPICA_SRC}/common/dmtbinfo.c
      ${ACPICA_SRC}/common/dmtbinfo1.c
      ${ACPICA_SRC}/common/dmtbinfo2.c
      ${ACPICA_SRC}/common/dmtbinfo3.c
      ${ACPICA_SRC}/common/getopt.c
      ${ACPICA_SRC}/tools/acpibin/abcompare.c
      ${ACPICA_SRC}/tools/acpibin/abmain.c
      ${ACPICA_SRC}/tools/acpixtract/acpixtract.c
      ${ACPICA_SRC}/tools/acpixtract/axmain.c
      ${ACPICA_SRC}/tools/acpixtract/axutils.c
      ${ACPICA_SRC}/tools/acpisrc/ascase.c
      ${ACPICA_SRC}/tools/acpisrc/asconvrt.c
      ${ACPICA_SRC}/tools/acpisrc/asfile.c
      ${ACPICA_SRC}/tools/acpisrc/asmain.c
      ${ACPICA_SRC}/tools/acpisrc/asremove.c
      ${ACPICA_SRC}/tools/acpisrc/astable.c
      ${ACPICA_SRC}/tools/acpisrc/asutils.c)

  nuttx_add_library(acpica)

  target_sources(acpica PRIVATE ${SRCS})
  target_include_directories(acpica PRIVATE ${INCDIR})
  target_compile_options(acpica PRIVATE ${CFLAGS})

  nuttx_add_application(
    NAME
    "iasl"
    STACKSIZE
    ${CONFIG_ACPICA_STACKSIZE}
    PRIORITY
    ${CONFIG_ACPICA_PRIORITY}
    INCLUDE_DIRECTORIES
    ${INCDIR}
    COMPILE_FLAGS
    ${CFLAGS}
    SRCS
    ${ACPICA_SRC}/compiler/aslmain.c
    DEPENDS
    acpica)

  nuttx_add_application(
    NAME
    "acpibin"
    STACKSIZE
    ${CONFIG_ACPICA_STACKSIZE}
    PRIORITY
    ${CONFIG_ACPICA_PRIORITY}
    INCLUDE_DIRECTORIES
    ${INCDIR}
    COMPILE_FLAGS
    ${CFLAGS}
    SRCS
    ${ACPICA_SRC}/tools/acpibin/abmain.c
    DEPENDS
    acpica)

  nuttx_add_application(
    NAME
    "acpixtract"
    STACKSIZE
    ${CONFIG_ACPICA_STACKSIZE}
    PRIORITY
    ${CONFIG_ACPICA_PRIORITY}
    INCLUDE_DIRECTORIES
    ${INCDIR}
    COMPILE_FLAGS
    ${CFLAGS}
    SRCS
    ${ACPICA_SRC}/tools/acpixtract/axmain.c
    DEPENDS
    acpica)

  nuttx_add_application(
    NAME
    "acpisrc"
    STACKSIZE
    ${CONFIG_ACPICA_STACKSIZE}
    PRIORITY
    ${CONFIG_ACPICA_PRIORITY}
    INCLUDE_DIRECTORIES
    ${INCDIR}
    COMPILE_FLAGS
    ${CFLAGS}
    SRCS
    ${ACPICA_SRC}/tools/acpisrc/asmain.c
    DEPENDS
    acpica)

endif()
