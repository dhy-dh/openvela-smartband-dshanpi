############################################################################
# apps/tools/Wasm.mk
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


ifeq ($(CONFIG_INTERPRETERS_WAMR_BUILD_MODULES_FOR_NUTTX),y)
include $(APPDIR)$(DELIM)tools$(DELIM)WASI-SDK.defs
include $(APPDIR)$(DELIM)interpreters$(DELIM)wamr$(DELIM)Toolchain.defs

# WASMLD - WebAssembly Linking and Optimization Function
#
# This macro handles the compilation, linking, and optimization of WebAssembly modules.
# It processes a single WebAssembly object file and performs the following steps:
#
# 1. Extracts metadata from the filename (using % as delimiter):
#    - INITIAL_MEMORY: WebAssembly memory size
#    - STACKSIZE: Stack size for the module
#    - PROGNAME: Program name for the output file
#    - WAMRMODE: WAMR execution mode (INT, AOT, XIP, JIT)
#
# 2. Loads any additional linker flags from the .ldflags file
#
# 3. Compiles and links the WebAssembly module using WCC
#    - Generates a memory map file
#    - Outputs the WASM file to the binary directory
#    - Reports error if linking fails
#
# 4. Optimizes the WebAssembly module using wasm-opt
#    - Applies size optimization (-Oz)
#    - Reports error if optimization fails
#
# 5. Processes the resulting WASM file:
#    - Strips constant sections if needed
#    - Performs AOT compilation based on the specified WAMR mode
#
# Parameters:
#   $(1): Path to the WebAssembly object file to process

define WASMLD
	$(eval INITIAL_MEMORY=$(shell echo $(notdir $(1)) | cut -d'%' -f2)) \
	$(eval STACKSIZE=$(shell echo $(notdir $(1)) | cut -d'%' -f3)) \
	$(eval PROGNAME=$(shell echo $(notdir $(1)) | cut -d'%' -f1)) \
	$(eval WAMRMODE=$(shell echo $(notdir $(1)) | cut -d'%' -f5)) \
	$(eval WLDFLAGS=$(shell cat $(APPDIR)$(DELIM)wasm$(DELIM)$(PROGNAME).ldflags)) \
	$(eval RETVAL=$(shell $(WCC) $(1) $(WBIN) $(WCFLAGS) $(WLDFLAGS) $(WCC_COMPILER_RT_LIB) \
		-Wl,--Map=$(APPDIR)$(DELIM)wasm$(DELIM)$(PROGNAME).map \
		-o $(BINDIR)$(DELIM)wasm$(DELIM)$(PROGNAME).wasm || echo 1;)) \
	$(if $(RETVAL), \
		$(error wasm build failed for $(PROGNAME).wasm) \
	) \
	$(eval RETVAL=$(shell $(WASI_SDK_PATH)$(DELIM)wasm-opt $(WOPTFLAGS) -Oz -o $(BINDIR)$(DELIM)wasm$(DELIM)$(PROGNAME).wasm \
		$(BINDIR)$(DELIM)wasm$(DELIM)$(PROGNAME).wasm || echo 1;)) \
	$(if $(RETVAL), \
		$(error wasm opt failed for $(PROGNAME).wasm) \
	) \
	$(call STRIP_CONSTANT_SECTION,$(PROGNAME).wasm) \
	$(call WAMR_AOT_COMPILE,$(PROGNAME),$(BINDIR)$(DELIM)wasm,$(WAMRMODE))
endef

# If called from $(APPDIR)/Makefile,
# Provide LINK_WASM, but only execute it when file wasm/*.wo exists

ifeq ($(CURDIR),$(APPDIR))

# LINK_WASM - Links WebAssembly object files into WASM modules
#
# This macro processes all WebAssembly object files (*.wo) found in the wasm
# directory and links them into WebAssembly modules. It is designed to be called
# from the top-level application Makefile.
#
# The macro:
# 1. Checks for WebAssembly object files in the wasm directory
# 2. For each found *.wo file, calls the WASMLD function to:
#    - Extract memory size, stack size, program name and WAMR mode from filename
#    - Link the WebAssembly module
#    - Optimize the resulting WASM file
#    - Strip constant sections and perform AOT compilation if configured
#
# Usage: $(call LINK_WASM) in the application Makefile's build targets

define LINK_WASM
	$(if $(wildcard $(APPDIR)$(DELIM)wasm$(DELIM)*), \
	  $(foreach bin,$(wildcard $(APPDIR)$(DELIM)wasm$(DELIM)*.wo), \
	    $(call WASMLD,$(bin)) \
	   ) \
	 )
endef

endif # CURDIR

# Default values for WASM_BUILD, it's a three state variable:
#   y - build wasm module only
#   n - don't build wasm module, default
#   both - build wasm module and native module

ifneq ($(WASM_BUILD),n)

WASM_INITIAL_MEMORY ?= 65536
STACKSIZE           ?= $(CONFIG_DEFAULT_TASK_STACKSIZE)
PRIORITY            ?= SCHED_PRIORITY_DEFAULT

# Wamr mode:
# INT: Interpreter (Default)
# AOT: Ahead-of-Time
# XIP: Execution In Place
# JIT: Just In Time

WAMR_MODE ?= INT

# Targets follow

.PRECIOUS: $(WBIN)

WSRCS := $(MAINSRC) $(CSRCS)
WOBJS := $(WSRCS:=$(SUFFIX).wo)

all:: $(WBIN)

$(BINDIR)/wasm:
	$(Q) mkdir -p $(BINDIR)/wasm

depend:: $(BINDIR)/wasm

install::
	@:

$(WOBJS): %.c$(SUFFIX).wo : %.c
	$(Q) $(WCC) $(WCFLAGS) -c $^ -o $@

$(WBIN): $(WOBJS)
	$(shell mkdir -p $(APPDIR)/wasm)
	$(Q) flock $(WBIN).lock -c '$(WAR) $@ $(filter-out $(MAINSRC:=$(SUFFIX).wo),$^)'
	$(foreach main,$(MAINSRC), \
	  $(eval progname=$(strip $(PROGNAME_$(main:=$(SUFFIX)$(OBJEXT))))) \
	  $(eval dstname=$(shell echo $(main:=$(SUFFIX).wo) | sed -e 's/\//_/g')) \
	  $(shell cp -rf $(strip $(main:=$(SUFFIX).wo)) \
	    $(strip $(APPDIR)/wasm/$(progname)%$(WASM_INITIAL_MEMORY)%$(STACKSIZE)%$(PRIORITY)%$(WAMR_MODE)%$(dstname)) \
	   ) \
	  $(shell echo $(WLDFLAGS) > $(APPDIR)/wasm/$(progname).ldflags) \
	 )

clean::
	$(call DELFILE, $(WOBJS))
	$(call DELFILE, $(WBIN))

endif # WASM_BUILD

endif
