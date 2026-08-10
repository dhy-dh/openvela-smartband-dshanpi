############################################################################
# apps/system/acpica/Makefile
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

include $(APPDIR)/Make.defs

# Download and unpack tarball if no git repo found

ifeq ($(wildcard acpica/.git),)
acpica-master.zip:
	$(call DOWNLOAD,https://github.com/acpica/acpica/archive/refs/heads/,master.zip,acpica-master.zip)
	$(Q) unzip -o acpica-master.zip
	mv acpica-master acpica

context:: acpica-master.zip
endif

ACPICA_SRC = acpica/source

distclean::
ifeq ($(wildcard acpica/.git),)
	$(call DELDIR, acpica)
	$(call DELFILE, acpica-master.zip)
endif

CFLAGS += ${INCDIR_PREFIX}$(ACPICA_SRC)/compiler/
CFLAGS += ${INCDIR_PREFIX}$(ACPICA_SRC)/include/
CFLAGS += ${INCDIR_PREFIX}$(ACPICA_SRC)/include/platform/
CFLAGS += ${INCDIR_PREFIX}$(ACPICA_SRC)/
CFLAGS += ${INCDIR_PREFIX}$(ACPICA_SRC)/tools/acpisrc/

CFLAGS += -D_CYGWIN -D_GNU_SOURCE -DACPI_ASL_COMPILER
CFLAGS += -DACPI_BIN_APP -DACPI_XTRACT_APP -DACPI_SRC_APP

ASL_COMPILER = $(ACPICA_SRC)/compiler

YACC      = bison
YFLAGS   += -y
MACROPROC = m4
MFLAGS    = -P -I$(ASL_COMPILER)
LEX       = flex
LFLAGS   += -i -s
DLFLAGS  += -i

HEADERS = \
  $(wildcard $(ASL_COMPILER)/*.h) \
  $(ACPICA_SRC)/aslcompiler.y.h \
  $(ACPICA_SRC)/dtcompilerparser.y.h \
  $(ACPICA_SRC)/dtparser.y.h \
  $(ACPICA_SRC)/prparser.y.h

INTERMEDIATES = \
  $(ACPICA_SRC)/aslcompiler.y \
  $(ACPICA_SRC)/aslcompilerlex.c \
  $(ACPICA_SRC)/aslcompilerparse.c \
  $(ACPICA_SRC)/dtparserlex.c \
  $(ACPICA_SRC)/dtparserparse.c \
  $(ACPICA_SRC)/dtcompilerparserlex.c \
  $(ACPICA_SRC)/dtcompilerparserparse.c \
  $(ACPICA_SRC)/prparserlex.c \
  $(ACPICA_SRC)/prparserparse.c

MISC = \
  $(ACPICA_SRC)/aslcompiler.y.h \
  $(ACPICA_SRC)/dtcompilerparser.y.h \
  $(ACPICA_SRC)/dtparser.y.h \
  $(ACPICA_SRC)/prparser.y.h

ASL_PARSER = \
  $(ASL_COMPILER)/aslcstyle.y \
  $(ASL_COMPILER)/aslhelpers.y \
  $(ASL_COMPILER)/aslparser.y \
  $(ASL_COMPILER)/aslprimaries.y \
  $(ASL_COMPILER)/aslresources.y \
  $(ASL_COMPILER)/aslrules.y \
  $(ASL_COMPILER)/aslsupport.y \
  $(ASL_COMPILER)/asltokens.y \
  $(ASL_COMPILER)/asltypes.y

ASL_LEXER = \
  $(ASL_COMPILER)/aslcompiler.l \
  $(ASL_COMPILER)/aslsupport.l \
  $(ACPICA_SRC)/aslcompiler.y.h

#
# Function to safely execute yacc:
# Generate the output files to a temporary directory,
# move the file to $(ACPICA_SRC), and discard the directory.
#
safe_yacc = \
  _d=`mktemp -d $(ACPICA_SRC)/$(1).XXXXXX` && \
  cd $$_d && \
  $(YACC) $(YFLAGS) -v -d -p$(1) "$(abspath $(2))" && \
  cd - > /dev/null; \
  mv $$_d/y.tab$(suffix $(3)) $(3); \
  _r=$$?; \
  rm -fr $$_d; \
  exit $$_r

#
# Macro processing for iASL .y files
#

$(ACPICA_SRC)/aslcompiler.y : $(ASL_PARSER)
	$(ECHO_BEGIN)"MACROPROC: Preprocessing $(ASL_COMPILER)/aslparser.y -> $@"
	$(Q) $(MACROPROC) $(MFLAGS) $(ASL_COMPILER)/aslparser.y > $@
	$(ECHO_END)

#
# Parser and Lexer - intermediate C files
#

$(ACPICA_SRC)/aslcompilerlex.c : $(ASL_LEXER)
	$(ECHO_BEGIN) "LEX: Processing $<"
	$(Q) $(LEX) $(LFLAGS) -PAslCompiler -o$@ $(ASL_COMPILER)/aslcompiler.l
	$(ECHO_END)

$(ACPICA_SRC)/aslcompilerparse.c $(ACPICA_SRC)/aslcompiler.y.h : $(ACPICA_SRC)/aslcompiler.y
	$(ECHO_BEGIN) "YACC: Processing $<"
	$(Q) $(call safe_yacc,AslCompiler,$<,$@)
	$(ECHO_END)

$(ACPICA_SRC)/dtcompilerparserlex.c : $(ASL_COMPILER)/dtcompilerparser.l $(ACPICA_SRC)/dtcompilerparser.y.h
	$(ECHO_BEGIN) "LEX: Processing $<"
	$(Q) $(LEX) $(DLFLAGS) -PDtCompilerParser -o$@ $<
	$(ECHO_END)

$(ACPICA_SRC)/dtcompilerparserparse.c $(ACPICA_SRC)/dtcompilerparser.y.h : $(ASL_COMPILER)/dtcompilerparser.y
	$(ECHO_BEGIN) "YACC: Processing $<"
	$(Q) $(call safe_yacc,DtCompilerParser,$<,$@)
	$(ECHO_END)

$(ACPICA_SRC)/dtparserlex.c : $(ASL_COMPILER)/dtparser.l $(ACPICA_SRC)/dtparser.y.h
	$(ECHO_BEGIN) "LEX: Processing $<"
	$(Q) $(LEX) $(DLFLAGS) -PDtParser -o$@ $<
	$(ECHO_END)

$(ACPICA_SRC)/dtparserparse.c $(ACPICA_SRC)/dtparser.y.h : $(ASL_COMPILER)/dtparser.y
	$(ECHO_BEGIN) "YACC: Processing $<"
	$(Q) $(call safe_yacc,DtParser,$<,$@)
	$(ECHO_END)

$(ACPICA_SRC)/prparserlex.c : $(ASL_COMPILER)/prparser.l $(ACPICA_SRC)/prparser.y.h
	$(ECHO_BEGIN) "LEX: Processing $<"
	$(Q) $(LEX) $(LFLAGS) -PPrParser -o$@ $<
	$(ECHO_END)

$(ACPICA_SRC)/prparserparse.c $(ACPICA_SRC)/prparser.y.h : $(ASL_COMPILER)/prparser.y
	$(ECHO_BEGIN) "YACC: Processing $<"
	$(Q) $(call safe_yacc,PrParser,$<,$@)
	$(ECHO_END)

depend:: $(ACPICA_SRC)/aslcompiler.y.h $(ACPICA_SRC)/dtcompilerparser.y.h \
         $(ACPICA_SRC)/dtparser.y.h $(ASL_COMPILER)/prparser.l $(ACPICA_SRC)/prparser.y.h

CSRCS += $(ACPICA_SRC)/aslcompilerlex.c
CSRCS += $(ACPICA_SRC)/aslcompilerparse.c
CSRCS += $(ACPICA_SRC)/dtcompilerparserlex.c
CSRCS += $(ACPICA_SRC)/dtcompilerparserparse.c
CSRCS += $(ACPICA_SRC)/dtparserlex.c
CSRCS += $(ACPICA_SRC)/dtparserparse.c
CSRCS += $(ACPICA_SRC)/prparserlex.c
CSRCS += $(ACPICA_SRC)/prparserparse.c

CSRCS += $(ACPICA_SRC)/os_specific/service_layers/osunixxf.c
CSRCS += $(ACPICA_SRC)/os_specific/service_layers/osunixdir.c

CSRCS += $(ACPICA_SRC)/components/debugger/dbfileio.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmbuffer.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmcstyle.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmdeferred.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmnames.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmopcode.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmresrc.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmresrcl.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmresrcl2.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmresrcs.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmutils.c
CSRCS += $(ACPICA_SRC)/components/disassembler/dmwalk.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dsargs.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dscontrol.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dsfield.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dsobject.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dsopcode.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dspkginit.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dsutils.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dswexec.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dswload.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dswload2.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dswscope.c
CSRCS += $(ACPICA_SRC)/components/dispatcher/dswstate.c
CSRCS += $(ACPICA_SRC)/components/executer/exconcat.c
CSRCS += $(ACPICA_SRC)/components/executer/exconvrt.c
CSRCS += $(ACPICA_SRC)/components/executer/excreate.c
CSRCS += $(ACPICA_SRC)/components/executer/exdump.c
CSRCS += $(ACPICA_SRC)/components/executer/exmisc.c
CSRCS += $(ACPICA_SRC)/components/executer/exmutex.c
CSRCS += $(ACPICA_SRC)/components/executer/exnames.c
CSRCS += $(ACPICA_SRC)/components/executer/exoparg1.c
CSRCS += $(ACPICA_SRC)/components/executer/exoparg2.c
CSRCS += $(ACPICA_SRC)/components/executer/exoparg3.c
CSRCS += $(ACPICA_SRC)/components/executer/exoparg6.c
CSRCS += $(ACPICA_SRC)/components/executer/exprep.c
CSRCS += $(ACPICA_SRC)/components/executer/exregion.c
CSRCS += $(ACPICA_SRC)/components/executer/exresnte.c
CSRCS += $(ACPICA_SRC)/components/executer/exresolv.c
CSRCS += $(ACPICA_SRC)/components/executer/exresop.c
CSRCS += $(ACPICA_SRC)/components/executer/exstore.c
CSRCS += $(ACPICA_SRC)/components/executer/exstoren.c
CSRCS += $(ACPICA_SRC)/components/executer/exstorob.c
CSRCS += $(ACPICA_SRC)/components/executer/exsystem.c
CSRCS += $(ACPICA_SRC)/components/executer/exutils.c
CSRCS += $(ACPICA_SRC)/components/namespace/nsaccess.c
CSRCS += $(ACPICA_SRC)/components/namespace/nsalloc.c
CSRCS += $(ACPICA_SRC)/components/namespace/nsdump.c
CSRCS += $(ACPICA_SRC)/components/namespace/nsnames.c
CSRCS += $(ACPICA_SRC)/components/namespace/nsobject.c
CSRCS += $(ACPICA_SRC)/components/namespace/nsparse.c
CSRCS += $(ACPICA_SRC)/components/namespace/nssearch.c
CSRCS += $(ACPICA_SRC)/components/namespace/nsutils.c
CSRCS += $(ACPICA_SRC)/components/namespace/nswalk.c
CSRCS += $(ACPICA_SRC)/components/namespace/nsxfobj.c
CSRCS += $(ACPICA_SRC)/components/parser/psargs.c
CSRCS += $(ACPICA_SRC)/components/parser/psloop.c
CSRCS += $(ACPICA_SRC)/components/parser/psobject.c
CSRCS += $(ACPICA_SRC)/components/parser/psopcode.c
CSRCS += $(ACPICA_SRC)/components/parser/psopinfo.c
CSRCS += $(ACPICA_SRC)/components/parser/psparse.c
CSRCS += $(ACPICA_SRC)/components/parser/psscope.c
CSRCS += $(ACPICA_SRC)/components/parser/pstree.c
CSRCS += $(ACPICA_SRC)/components/parser/psutils.c
CSRCS += $(ACPICA_SRC)/components/parser/pswalk.c
CSRCS += $(ACPICA_SRC)/components/tables/tbdata.c
CSRCS += $(ACPICA_SRC)/components/tables/tbxfroot.c
CSRCS += $(ACPICA_SRC)/components/tables/tbfadt.c
CSRCS += $(ACPICA_SRC)/components/tables/tbinstal.c
CSRCS += $(ACPICA_SRC)/components/tables/tbprint.c
CSRCS += $(ACPICA_SRC)/components/tables/tbutils.c
CSRCS += $(ACPICA_SRC)/components/tables/tbxface.c
CSRCS += $(ACPICA_SRC)/components/tables/tbxfload.c
CSRCS += $(ACPICA_SRC)/components/utilities/utaddress.c
CSRCS += $(ACPICA_SRC)/components/utilities/utalloc.c
CSRCS += $(ACPICA_SRC)/components/utilities/utascii.c
CSRCS += $(ACPICA_SRC)/components/utilities/utbuffer.c
CSRCS += $(ACPICA_SRC)/components/utilities/utcache.c
CSRCS += $(ACPICA_SRC)/components/utilities/utcksum.c
CSRCS += $(ACPICA_SRC)/components/utilities/utcopy.c
CSRCS += $(ACPICA_SRC)/components/utilities/utdebug.c
CSRCS += $(ACPICA_SRC)/components/utilities/utdecode.c
CSRCS += $(ACPICA_SRC)/components/utilities/utdelete.c
CSRCS += $(ACPICA_SRC)/components/utilities/uterror.c
CSRCS += $(ACPICA_SRC)/components/utilities/utexcep.c
CSRCS += $(ACPICA_SRC)/components/utilities/utglobal.c
CSRCS += $(ACPICA_SRC)/components/utilities/uthex.c
CSRCS += $(ACPICA_SRC)/components/utilities/utinit.c
CSRCS += $(ACPICA_SRC)/components/utilities/utlock.c
CSRCS += $(ACPICA_SRC)/components/utilities/utmath.c
CSRCS += $(ACPICA_SRC)/components/utilities/utmisc.c
CSRCS += $(ACPICA_SRC)/components/utilities/utmutex.c
CSRCS += $(ACPICA_SRC)/components/utilities/utnonansi.c
CSRCS += $(ACPICA_SRC)/components/utilities/utobject.c
CSRCS += $(ACPICA_SRC)/components/utilities/utownerid.c
CSRCS += $(ACPICA_SRC)/components/utilities/utpredef.c
CSRCS += $(ACPICA_SRC)/components/utilities/utresdecode.c
CSRCS += $(ACPICA_SRC)/components/utilities/utresrc.c
CSRCS += $(ACPICA_SRC)/components/utilities/utstate.c
CSRCS += $(ACPICA_SRC)/components/utilities/utstrtoul64.c
CSRCS += $(ACPICA_SRC)/components/utilities/utstrsuppt.c
CSRCS += $(ACPICA_SRC)/components/utilities/utstring.c
CSRCS += $(ACPICA_SRC)/components/utilities/utuuid.c
CSRCS += $(ACPICA_SRC)/components/utilities/utxface.c
CSRCS += $(ACPICA_SRC)/components/utilities/utxferror.c

CSRCS += $(ACPICA_SRC)/compiler/aslmapoutput.c
CSRCS += $(ACPICA_SRC)/compiler/asltransform.c
CSRCS += $(ACPICA_SRC)/compiler/aslallocate.c
CSRCS += $(ACPICA_SRC)/compiler/aslanalyze.c
CSRCS += $(ACPICA_SRC)/compiler/aslascii.c
CSRCS += $(ACPICA_SRC)/compiler/aslbtypes.c
CSRCS += $(ACPICA_SRC)/compiler/aslcache.c
CSRCS += $(ACPICA_SRC)/compiler/aslcodegen.c
CSRCS += $(ACPICA_SRC)/compiler/aslcompile.c
CSRCS += $(ACPICA_SRC)/compiler/asldebug.c
CSRCS += $(ACPICA_SRC)/compiler/aslerror.c
CSRCS += $(ACPICA_SRC)/compiler/aslexternal.c
CSRCS += $(ACPICA_SRC)/compiler/aslfiles.c
CSRCS += $(ACPICA_SRC)/compiler/aslfileio.c
CSRCS += $(ACPICA_SRC)/compiler/aslfold.c
CSRCS += $(ACPICA_SRC)/compiler/aslhelp.c
CSRCS += $(ACPICA_SRC)/compiler/aslhex.c
CSRCS += $(ACPICA_SRC)/compiler/asllength.c
CSRCS += $(ACPICA_SRC)/compiler/asllisting.c
CSRCS += $(ACPICA_SRC)/compiler/asllistsup.c
CSRCS += $(ACPICA_SRC)/compiler/aslload.c
CSRCS += $(ACPICA_SRC)/compiler/asllookup.c
CSRCS += $(ACPICA_SRC)/compiler/aslmain.c
CSRCS += $(ACPICA_SRC)/compiler/aslmap.c
CSRCS += $(ACPICA_SRC)/compiler/aslmapenter.c
CSRCS += $(ACPICA_SRC)/compiler/aslmaputils.c
CSRCS += $(ACPICA_SRC)/compiler/aslmessages.c
CSRCS += $(ACPICA_SRC)/compiler/aslmethod.c
CSRCS += $(ACPICA_SRC)/compiler/aslnamesp.c
CSRCS += $(ACPICA_SRC)/compiler/asloffset.c
CSRCS += $(ACPICA_SRC)/compiler/aslopcodes.c
CSRCS += $(ACPICA_SRC)/compiler/asloperands.c
CSRCS += $(ACPICA_SRC)/compiler/aslopt.c
CSRCS += $(ACPICA_SRC)/compiler/asloptions.c
CSRCS += $(ACPICA_SRC)/compiler/aslparseop.c
CSRCS += $(ACPICA_SRC)/compiler/aslpredef.c
CSRCS += $(ACPICA_SRC)/compiler/aslprepkg.c
CSRCS += $(ACPICA_SRC)/compiler/aslprintf.c
CSRCS += $(ACPICA_SRC)/compiler/aslprune.c
CSRCS += $(ACPICA_SRC)/compiler/aslresource.c
CSRCS += $(ACPICA_SRC)/compiler/aslrestype1.c
CSRCS += $(ACPICA_SRC)/compiler/aslrestype1i.c
CSRCS += $(ACPICA_SRC)/compiler/aslrestype2.c
CSRCS += $(ACPICA_SRC)/compiler/aslrestype2d.c
CSRCS += $(ACPICA_SRC)/compiler/aslrestype2e.c
CSRCS += $(ACPICA_SRC)/compiler/aslrestype2q.c
CSRCS += $(ACPICA_SRC)/compiler/aslrestype2s.c
CSRCS += $(ACPICA_SRC)/compiler/aslrestype2w.c
CSRCS += $(ACPICA_SRC)/compiler/aslstartup.c
CSRCS += $(ACPICA_SRC)/compiler/aslstubs.c
CSRCS += $(ACPICA_SRC)/compiler/aslpld.c
CSRCS += $(ACPICA_SRC)/compiler/asltree.c
CSRCS += $(ACPICA_SRC)/compiler/aslutils.c
CSRCS += $(ACPICA_SRC)/compiler/asluuid.c
CSRCS += $(ACPICA_SRC)/compiler/aslwalks.c
CSRCS += $(ACPICA_SRC)/compiler/aslxref.c
CSRCS += $(ACPICA_SRC)/compiler/aslxrefout.c
CSRCS += $(ACPICA_SRC)/compiler/cvcompiler.c
CSRCS += $(ACPICA_SRC)/compiler/cvdisasm.c
CSRCS += $(ACPICA_SRC)/compiler/cvparser.c
CSRCS += $(ACPICA_SRC)/compiler/dtcompile.c
CSRCS += $(ACPICA_SRC)/compiler/dtexpress.c
CSRCS += $(ACPICA_SRC)/compiler/dtfield.c
CSRCS += $(ACPICA_SRC)/compiler/dtio.c
CSRCS += $(ACPICA_SRC)/compiler/dtsubtable.c
CSRCS += $(ACPICA_SRC)/compiler/dttable.c
CSRCS += $(ACPICA_SRC)/compiler/dttable1.c
CSRCS += $(ACPICA_SRC)/compiler/dttable2.c
CSRCS += $(ACPICA_SRC)/compiler/dttemplate.c
CSRCS += $(ACPICA_SRC)/compiler/dtutils.c
CSRCS += $(ACPICA_SRC)/compiler/prexpress.c
CSRCS += $(ACPICA_SRC)/compiler/prmacros.c
CSRCS += $(ACPICA_SRC)/compiler/prscan.c
CSRCS += $(ACPICA_SRC)/compiler/prutils.c

CSRCS += $(ACPICA_SRC)/common/acfileio.c
CSRCS += $(ACPICA_SRC)/common/dmextern.c
CSRCS += $(ACPICA_SRC)/common/adfile.c
CSRCS += $(ACPICA_SRC)/common/adisasm.c
CSRCS += $(ACPICA_SRC)/common/adwalk.c
CSRCS += $(ACPICA_SRC)/common/ahids.c
CSRCS += $(ACPICA_SRC)/common/ahpredef.c
CSRCS += $(ACPICA_SRC)/common/ahtable.c
CSRCS += $(ACPICA_SRC)/common/ahuuids.c
CSRCS += $(ACPICA_SRC)/common/cmfsize.c
CSRCS += $(ACPICA_SRC)/common/dmrestag.c
CSRCS += $(ACPICA_SRC)/common/dmswitch.c
CSRCS += $(ACPICA_SRC)/common/dmtable.c
CSRCS += $(ACPICA_SRC)/common/dmtables.c
CSRCS += $(ACPICA_SRC)/common/dmtbdump.c
CSRCS += $(ACPICA_SRC)/common/dmtbdump1.c
CSRCS += $(ACPICA_SRC)/common/dmtbdump2.c
CSRCS += $(ACPICA_SRC)/common/dmtbdump3.c
CSRCS += $(ACPICA_SRC)/common/dmtbinfo.c
CSRCS += $(ACPICA_SRC)/common/dmtbinfo1.c
CSRCS += $(ACPICA_SRC)/common/dmtbinfo2.c
CSRCS += $(ACPICA_SRC)/common/dmtbinfo3.c
CSRCS += $(ACPICA_SRC)/common/getopt.c

CSRCS += $(ACPICA_SRC)/tools/acpibin/abcompare.c
CSRCS += $(ACPICA_SRC)/tools/acpibin/abmain.c
CSRCS += $(ACPICA_SRC)/tools/acpixtract/acpixtract.c
CSRCS += $(ACPICA_SRC)/tools/acpixtract/axmain.c
CSRCS += $(ACPICA_SRC)/tools/acpixtract/axutils.c
CSRCS += $(ACPICA_SRC)/tools/acpisrc/ascase.c
CSRCS += $(ACPICA_SRC)/tools/acpisrc/asconvrt.c
CSRCS += $(ACPICA_SRC)/tools/acpisrc/asfile.c
CSRCS += $(ACPICA_SRC)/tools/acpisrc/asmain.c
CSRCS += $(ACPICA_SRC)/tools/acpisrc/asremove.c
CSRCS += $(ACPICA_SRC)/tools/acpisrc/astable.c
CSRCS += $(ACPICA_SRC)/tools/acpisrc/asutils.c

# iasl command

PROGNAME  += iasl
PRIORITY  += $(CONFIG_ACPICA_PRIORITY)
STACKSIZE += $(CONFIG_ACPICA_STACKSIZE)
MAINSRC   += $(ACPICA_SRC)/compiler/aslmain.c

# acpibin command

PROGNAME  += acpibin
PRIORITY  += $(CONFIG_ACPICA_PRIORITY)
STACKSIZE += $(CONFIG_ACPICA_STACKSIZE)
MAINSRC   += $(ACPICA_SRC)/tools/acpibin/abmain.c

# acpixtract command

PROGNAME  += acpixtract
PRIORITY  += $(CONFIG_ACPICA_PRIORITY)
STACKSIZE += $(CONFIG_ACPICA_STACKSIZE)
MAINSRC   += $(ACPICA_SRC)/tools/acpixtract/axmain.c

# acpisrc command

PROGNAME  += acpisrc
PRIORITY  += $(CONFIG_ACPICA_PRIORITY)
STACKSIZE += $(CONFIG_ACPICA_STACKSIZE)
MAINSRC   += $(ACPICA_SRC)/tools/acpisrc/asmain.c

include $(APPDIR)/Application.mk

