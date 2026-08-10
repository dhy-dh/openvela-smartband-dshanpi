#!/bin/bash
set -x
./nuttx/tools/buildinfo.sh >./vendor/allwinnertech/boards/r528/r528s3-evb4/src/etc/build.prop
