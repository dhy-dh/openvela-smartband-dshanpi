
TOP=$(cd $(dirname $0) && cd ../../../../../ && pwd)

mkdir -p $1/prebuilts/emulator
cd $1
cp -r $TOP/prebuilts/emulator/linux-x86_64 $TOP/prebuilts/emulator/tools ./prebuilts/emulator
ln -s ./prebuilts/emulator/tools/emulator.sh ./emulator.sh
