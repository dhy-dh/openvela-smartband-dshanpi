############################################################################
# apps/interpreters/wamr/Module.mk
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

WAMR_MODULE_REGISTRY = $(APPDIR)$(DELIM)interpreters$(DELIM)wamr$(DELIM)registry

define WAMR_MODULE_REGISTER
	$(Q) echo Register WAMR Module: $1
	$(Q) echo "$2," > "$(WAMR_MODULE_REGISTRY)$(DELIM)$1.bdat"
	$(Q) echo "bool $2(void);" > "$(WAMR_MODULE_REGISTRY)$(DELIM)$1.pdat"
	$(Q) touch "$(WAMR_MODULE_REGISTRY)$(DELIM).updated"
endef

ifneq ($(WAMR_MODULE_NAME),)
# Create individual targets for each module
# This macro generates a build target for each WAMR module
# $(1) - Module name that will be used to generate:
#   - A .bdat file containing the module registration function name
#   - A .pdat file containing the function prototype
# The target depends on configuration and Makefile changes
define WAMR_MODULE_TARGET
$(WAMR_MODULE_REGISTRY)$(DELIM)$(1).bdat: $(DEPCONFIG) Makefile
	$$(call WAMR_MODULE_REGISTER,$(1),wamr_module_$(1)_register)
endef

# Evaluate the WAMR_MODULE_TARGET macro for each module name
# This creates individual build targets for all modules listed in WAMR_MODULE_NAME
$(foreach mod,$(WAMR_MODULE_NAME),$(eval $(call WAMR_MODULE_TARGET,$(mod))))

# Add all module .bdat files as dependencies of the 'register' target
register:: $(foreach mod,$(WAMR_MODULE_NAME),$(WAMR_MODULE_REGISTRY)$(DELIM)$(mod).bdat)
endif
