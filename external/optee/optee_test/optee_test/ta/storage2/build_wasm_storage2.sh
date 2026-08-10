#
# Copyright (C) 2023 Xiaomi Corporation
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

rm ta_entry.wo storage.wo
rm 9E271E73FBAA7545A77138CAA6F0CCA6
rm 9E271E73FBAA7545A77138CAA6F0CCA6.aot
rm ../../../../optee_nuttx/optee_nuttx/wasm/wasm_ta_framework.wo

make -f Makefile-wasm
../../../../../../prebuilts/clang/linux/wasm/wamrc --target=thumbv8m.main --target-abi=gnueabihf --cpu=cortex-m55 --enable-indirect-mode --disable-llvm-intrinsics -o 9E271E73FBAA7545A77138CAA6F0CCA6.aot 9E271E73FBAA7545A77138CAA6F0CCA6

cp 9E271E73FBAA7545A77138CAA6F0CCA6.aot ../../../../../../vendor/bes/boards/best1600_ep/$1/src/etc/ta/9E271E73FBAA7545A77138CAA6F0CCA6
