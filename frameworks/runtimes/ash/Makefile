#
# Copyright (C) 2025 Xiaomi Corporation
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

ifeq ($(CONFIG_LIBASH),y)
CXXEXT     := .cpp
CXXFLAGS   += -std=c++17

# workaround for gcc-13 warning
GCC_VERSION := $(shell gcc -dumpversion)
ifeq ($(shell expr $(GCC_VERSION) \>= 13), 1)
  CFLAGS += --param=min-pagesize=0
  CXXFLAGS += --param=min-pagesize=0
endif

CXXFLAGS += ${INCDIR_PREFIX}$(CURDIR)/include
CXXFLAGS += ${INCDIR_PREFIX}$(CURDIR)/c/include
CXXFLAGS += ${INCDIR_PREFIX}$(APPDIR)/system/zlib/zlib
CXXFLAGS += ${INCDIR_PREFIX}$(APPDIR)/system/zlib/zlib/contrib

CXXSRCS += ${CURDIR}/src/bundle/bundle.cpp \
           ${CURDIR}/src/crash/crash.cpp \
           ${CURDIR}/src/device_info/device_info.cpp \
           ${CURDIR}/src/fds/scoped_fd.cpp \
           ${CURDIR}/src/file/file.cpp \
           ${CURDIR}/src/file/file_path.cpp \
           ${CURDIR}/src/logging/logging.cpp \
           ${CURDIR}/src/memory/global_variable.cpp \
           ${CURDIR}/src/memory/lifecycle_watcher.cpp \
           ${CURDIR}/src/memory/thread_local.cpp \
           ${CURDIR}/src/memory/variable_segment.cpp \
           ${CURDIR}/src/message_loop/message_loop.cpp \
           ${CURDIR}/src/message_loop/message_pump_android.cpp \
           ${CURDIR}/src/message_loop/message_pump.cpp \
           ${CURDIR}/src/message_loop/message_pump_impl.cpp \
           ${CURDIR}/src/message_loop/message_pump_uv.cpp \
           ${CURDIR}/src/message_loop/message_queue.cpp \
           ${CURDIR}/src/message_loop/message_queue_runner.cpp \
           ${CURDIR}/src/stream/input_stream.cpp \
           ${CURDIR}/src/stream/file_input_stream.cpp \
           ${CURDIR}/src/stream/raw_input_stream.cpp \
           ${CURDIR}/src/strings/number_string_conversions.cpp \
           ${CURDIR}/src/strings/string_converter.cpp \
           ${CURDIR}/src/threading/thread.cpp \
           ${CURDIR}/src/time/duration.cpp \
           ${CURDIR}/src/time/time.cpp \
           ${CURDIR}/src/timer/timer.cpp \
           ${CURDIR}/src/trace_event/trace_event.cpp \
           ${CURDIR}/src/zip/in_zip.cpp \
           ${CURDIR}/src/zip/out_zip.cpp \
           ${CURDIR}/src/zip/zip_file.cpp

CXXSRCS += ${CURDIR}/c/src/message_loop/cmessage_loop.cpp \
           ${CURDIR}/c/src/task_runner/ctask_runner.cpp

endif

ifeq ($(CONFIG_ASH_UNIT_TEST), y)
  PROGNAME += ash_unit_test
  PRIORITY += 100
  STACKSIZE += 16384
  MAINSRC += $(APPDIR)/frameworks/runtimes/ash/tests/ash_unit_test.cpp
endif

include $(APPDIR)/Application.mk
