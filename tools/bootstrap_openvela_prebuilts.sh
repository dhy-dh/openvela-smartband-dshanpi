#!/usr/bin/env bash

set -euo pipefail

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_root=$(CDPATH= cd -- "$script_dir/.." && pwd)
prebuilts_root=${OPENVELA_PREBUILTS_DIR:-"$project_root/prebuilts"}
branch=${OPENVELA_PREBUILTS_BRANCH:-dev-ai-contest-2026}

clone_component()
{
  repo=$1
  relative_path=$2
  destination="$prebuilts_root/$relative_path"

  if [ -d "$destination" ] && [ -n "$(find "$destination" -mindepth 1 -maxdepth 1 -print -quit 2>/dev/null)" ]; then
    printf 'KEEP %s\n' "$destination"
    return
  fi

  mkdir -p "$(dirname -- "$destination")"
  git clone --depth 1 --branch "$branch" \
    "https://github.com/open-vela/$repo.git" "$destination"
}

clone_component prebuilts_gcc_linux-x86_64_arm-none-eabi \
  gcc/linux-x86_64/arm-none-eabi
clone_component prebuilts_build-tools_linux-x86_64 \
  build-tools/linux-x86_64
clone_component prebuilts_cmake_linux-x86_64 \
  cmake/linux-x86_64
clone_component prebuilts_tools tools

compiler="$prebuilts_root/gcc/linux-x86_64/arm-none-eabi/bin/arm-none-eabi-gcc"
if [ ! -x "$compiler" ]; then
  printf 'Missing compiler after download: %s\n' "$compiler" >&2
  exit 1
fi

printf 'openvela prebuilts are ready in %s\n' "$prebuilts_root"
