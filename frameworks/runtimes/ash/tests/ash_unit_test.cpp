#include <gtest/gtest.h>
#include "bundle/bundle_test.cpp"
#include "device_info/device_info_test.cpp"
#include "fds/scoped_fd_test.cpp"
#include "file/file_path_test.cpp"
#include "file/file_test.cpp"
#include "functional/closure_test.cpp"
#include "functional/fn_once_test.cpp"
#include "geometry/size_test.cpp"
#include "macros/compiler_macros_test.cpp"
#include "macros/disallow_copy_test.cpp"
#include "memory/lazy_instance_test.cpp"
#include "memory/raw_ptr_test.cpp"
#include "memory/variable_segment_test.cpp"
#include "memory/weak_ptr_test.cpp"
#include "strings/number_string_conversions_test.cpp"
#include "strings/string_converter_test.cpp"
#include "time/duration_test.cpp"
#include "time/time_test.cpp"
#include "utils/scoped_setter_test.cpp"
#include "utils/value_or_function_test.cpp"
#include "zip/in_zip_test.cpp"
#include "zip/out_zip_test.cpp"

extern "C" int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
