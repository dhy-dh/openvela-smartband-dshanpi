# ##############################################################################
# apps/mlearning/nnlib-hifi4/CMakeLists.txt
#
# Cadence HiFi 4 Neural Network Library
#
# ##############################################################################

if(CONFIG_XTENSA_HIFI)

  # ############################################################################
  # Collect nnlib-hifi4 source files
  # ############################################################################

  set(NNLIB_ROOT ${CMAKE_CURRENT_LIST_DIR}/nnlib-hifi4/xa_nnlib)

  # Collect all C source files from algo directory
  file(GLOB_RECURSE NNLIB_SRCS ${NNLIB_ROOT}/algo/kernels/**/*.c
       ${NNLIB_ROOT}/algo/common/*.c ${NNLIB_ROOT}/algo/layers/*.c
       ${NNLIB_ROOT}/algo/ndsp/*.c)

  # Exclude NCHW pool files, use NHWC format
  list(FILTER NNLIB_SRCS EXCLUDE REGEX ".*/xa_nn_avgpool_16\\.c$")
  list(FILTER NNLIB_SRCS EXCLUDE REGEX ".*/xa_nn_avgpool_f32\\.c$")
  list(FILTER NNLIB_SRCS EXCLUDE REGEX ".*/xa_nn_maxpool_16\\.c$")
  list(FILTER NNLIB_SRCS EXCLUDE REGEX ".*/xa_nn_maxpool_f32\\.c$")
  list(FILTER NNLIB_SRCS EXCLUDE REGEX ".*/xa_nn_maxpool_asym8\\.c$")

  # ############################################################################
  # Compiler flags for nnlib-hifi4
  # ############################################################################

  set(NNLIB_FLAGS
      -DHIFI4
      -Dhifi4
      -DHIFI_NNLIB_OPT
      -DNNLIB_V2
      -DTFLITE_SINGLE_ROUNDING=0
      -DMODEL_INT16=1
      -O3
      -fno-strict-aliasing
      -Wno-unused-variable
      -Wno-unused-function
      -Wno-sign-compare
      -Wno-implicit-function-declaration
      -Wno-macro-redefined
      -Wno-undef
      -Wno-shadow)

  # ############################################################################
  # Include directories
  # ############################################################################

  set(NNLIB_INCDIR
      ${NNLIB_ROOT}/include
      ${NNLIB_ROOT}/include/nnlib
      ${NNLIB_ROOT}/algo/common
      ${NNLIB_ROOT}/algo/common/include
      ${NNLIB_ROOT}/algo/kernels
      ${NNLIB_ROOT}/algo/layers
      ${NNLIB_ROOT}/algo/ndsp
      ${NNLIB_ROOT}/algo/ndsp/hifi4/include)

  # ############################################################################
  # Library configuration
  # ############################################################################

  nuttx_add_library(xa_nnlib STATIC)
  target_compile_options(xa_nnlib PRIVATE ${NNLIB_FLAGS})

  # Treat warnings as warnings (not errors) for nnlib-hifi4 third-party library
  target_compile_options(xa_nnlib PRIVATE -Wno-error)

  target_sources(xa_nnlib PRIVATE ${NNLIB_SRCS})
  target_include_directories(xa_nnlib PUBLIC ${NNLIB_INCDIR})

endif()
