#!/bin/bash

set -x

top_path=$1
build_config_path=$2
cd ${top_path}
echo "check defconfig:${build_config_path}"
if [[ "$build_config_path" =~ "test" ]]; then
    echo "skip defconfig check for ${build_config_path}"
else
    if ! ./nuttx/tools/refresh.sh --silent ${build_config_path} > .check_defconfig_warning; then
        fail=1
        echo "error: check defconfig warning: ${build_config_path}"
        cat .check_defconfig_warning
        exit 1
    fi
fi
cd -


if [ "$IS_DOWNLOAD_SIGNKEYS" == "false" ];then
    exit 0
fi
