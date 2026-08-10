
TOP=$1
RELEASE_ID=$2
board=$3
config=$4
base_config=$5

buildtool="$TOP/${RELEASE_ID}/images/${board}/buildtool"

if [[ "$config"X == "smartspeaker-knsh"X ]]; then
    cd $TOP/nuttx
    make export
    cd $TOP/apps
    ./tools/mkimport.sh -z -x ../nuttx/nuttx-export-*.tar.gz
    make import -j10

    mkdir -p $buildtool/apps $buildtool/nuttx $buildtool/vendor/qemu/boards/smartspeaker/
    cp -r $TOP/vendor/qemu/boards/smartspeaker/prebuilts $buildtool/vendor/qemu/boards/smartspeaker/
    cp -r $TOP/apps/bin $buildtool/apps

    vela_resource_bin_file="$TOP/nuttx/vela_resource.bin"
    if [ -f $vela_resource_bin_file ]; then
        cp $vela_resource_bin_file $buildtool/nuttx
    fi
fi
if [[ "$board"X == "sim"X ]] && [[ "$config"X == "ap"X ]]; then
    mkdir -p ${TOP}/${RELEASE_ID}/images/${board}/vendor/sim/boards/common/resource
    cp -ap vendor/allwinnertech/lichee/board/common/data/res/app ${TOP}/${RELEASE_ID}/images/${board}/vendor/sim/boards/common/resource/app
    cp -ap vendor/allwinnertech/lichee/board/common/data/res/font ${TOP}/${RELEASE_ID}/images/${board}/vendor/sim/boards/common/resource/font
fi
