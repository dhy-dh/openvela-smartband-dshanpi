/*
 * Copyright (C) 2024 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

include $(APPDIR)/Make.defs

CXXEXT     := .cpp
CXXFLAGS   += -std=c++17

BIN := $(APPDIR)/staging/libqjsextends.a
OBJDIR := jshelp

QUICKJS_DIR := $(APPDIR)/interpreters/quickjs
QJS_EXTENDS_DIR := $(APPDIR)/frameworks/runtimes/quickapp/qjsextends

# Include files
CXXFLAGS += ${INCDIR_PREFIX}$(QUICKJS_DIR)
CXXFLAGS += ${INCDIR_PREFIX}$(QJS_EXTENDS_DIR)

# Source files
CXXSRCS	+=  $(QJS_EXTENDS_DIR)/jshelp/module_js_impl.cpp
CXXSRCS	+=  $(QJS_EXTENDS_DIR)/jshelp/js_utils.cpp
CXXSRCS	+=  $(QJS_EXTENDS_DIR)/jshelp/log_util.cpp
LDFLAGS  +=-ldl -lpthread
LDFLAGS  += -O3

PROGNAME = qjs_extends_test
PRIORITY = 100
STACKSIZE = 131072
MAINSRC += $(QJS_EXTENDS_DIR)/main/qjs_extends_test.cpp

CXXSRCS += $(QJS_EXTENDS_DIR)/main/test_module.cpp
CXXFLAGS += ${INCDIR_PREFIX}$(QUICKJS_DIR)/quickjs
CXXFLAGS   += -g -Wall -MMD
CXXFLAGS   += -DCONFIG_VERSION=\"2021-03-27\"
CXXFLAGS   += -Wno-unused-label -Wno-array-bounds -Wno-format-truncation

ifneq ($(CONFIG_QJS_EXTENDS_LOG_LEVEL),)
CXXFLAGS += -DQJS_EXTENDS_LOG_LEVEL=$(CONFIG_QJS_EXTENDS_LOG_LEVEL)
endif

$(TEST_OUT)/%.o:$(TEST_OUT)/%.c
	$(CC) -g -I. -o $@ -c $<

clean::
	rm -f $(OBJDIR)/*.o ${OBJDIR}/*.d main/*.o main/*.d

distclean::
	rm -f $(OBJDIR)/*.o ${OBJDIR}/*.d main/*.o main/*.d

include $(APPDIR)/Application.mk
