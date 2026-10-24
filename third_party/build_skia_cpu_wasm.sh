#!/bin/sh
# Builds the private CPU-only Skia archive for the Emscripten web host from the
# same arguments as every native host (skia_cpu_args.gn). Needs emsdk on PATH
# (EMSDK set by emsdk_env.sh). Objects are built with -pthread and SIMD128 so
# they link into the threaded web build; libpng's setjmp uses WebAssembly
# exception handling, as the program's C++ exceptions do.
set -eu
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
skia_dir="$script_dir/skia"
output_dir=${1:-"$skia_dir/out/gui_forms-wasm-cpu"}
: "${EMSDK:?source emsdk_env.sh first}"
mkdir -p "$output_dir"
sed -e '/^target_cpu = /d' "$script_dir/skia_cpu_args.gn" > "$output_dir/args.gn"
cat >> "$output_dir/args.gn" <<ARGS
target_cpu = "wasm"
skia_emsdk_dir = "$EMSDK"
skia_enable_fontmgr_empty = true
extra_cflags = [ "-msimd128", "-sSUPPORT_LONGJMP=wasm" ]
ARGS
# Skia's wasm toolchain first runs bin/activate-emsdk, which installs its own
# pinned emsdk under third_party/externals/emsdk. skia_emsdk_dir names the
# emsdk this build uses, so that step is answered by a stub that does nothing.
stub="$skia_dir/third_party/externals/emsdk"
if [ ! -f "$stub/emsdk.py" ]; then
    mkdir -p "$stub"
    printf 'import sys\nsys.exit(0)\n' > "$stub/emsdk.py"
fi
gn=${GN:-"$skia_dir/bin/gn"}
"$gn" gen "$output_dir" --root="$skia_dir"
ninja -C "$output_dir" -j "${BUILD_JOBS:-8}" skia
