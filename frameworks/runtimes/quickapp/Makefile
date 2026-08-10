#
# Copyright (C) 2020 Xiaomi Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

include $(APPDIR)/Make.defs

-include $(APPDIR)/frameworks/runtimes/quickapp/src/Makefile
-include $(APPDIR)/frameworks/runtimes/quickapp/inspector/Makefile

PRIORITY  = $(CONFIG_QUICKAPP_PRIORITY)
STACKSIZE = $(CONFIG_QUICKAPP_STACKSIZE)
# Export 执行文件, 需要保持 PROGNAME 与 MAINSRC 顺序匹配.
ifeq ($(CONFIG_QUICKAPP_VAPP),y)
# Use prebuilt vapp library (source not available in open source release)
PROGNAME += vapp
endif

ifeq ($(CONFIG_QUICKAPP_VAPP_XMS),y)
#MAINSRC  += src/quick_application.cpp
PROGNAME += vappxms
endif

ifeq ($(CONFIG_QUICKAPP_AIOTJSC),y)
#MAINSRC  += src/ajs_compile.cpp
#PROGNAME += aiotjsc
endif

ifeq ($(CONFIG_QUICKAPP_VAPPTEST),y)
PROGNAME += vapptest
PROGNAME += vapp_promise_test
endif

ifeq ($(CONFIG_QUICKAPP_LMK_TEST),y)
PROGNAME += lmk_test
endif

ifeq ($(CONFIG_QUICKAPP_DEBUG),y)
CFLAGS   += -DMQTT_USE_MBEDTLS
CXXFLAGS += -DMQTT_USE_MBEDTLS
endif

ASRCS := $(wildcard $(ASRCS))
CSRCS := $(wildcard $(CSRCS))
CXXSRCS := $(wildcard $(CXXSRCS))
MAINSRC := $(wildcard $(MAINSRC))
ifeq ($(CONFIG_QUICKAPP_VAPP),y)
# Add vapp sources after wildcard (wildcard fails with relative/VPATH paths)
# Use CURDIR-relative paths to match Application.mk SUFFIX pattern
endif
NOEXPORTSRCS = $(ASRCS)$(CSRCS)$(CXXSRCS)$(MAINSRC)

ifneq ($(NOEXPORTSRCS),)
BIN := $(APPDIR)/staging/libframework.a
endif

EXPORT_FILES := Kconfig

-include $(APPDIR)/frameworks/runtimes/quickapp/proprietary/Makefile

include $(APPDIR)/Application.mk
