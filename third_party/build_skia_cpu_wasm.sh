#!/bin/sh
# Builds the private CPU-only Skia archive for the Emscripten web host from the
# same arguments as every native host (skia_cpu_args.gn). Needs emsdk on PATH
# (EMSDK set by emsdk_env.sh). Objects are built with -pthread and SIMD128 so
# they link into the threaded web build; libpng's setjmp uses WebAssembly
# exception handling, as the program's C++ exceptions do. Skia's wasm config
# turns its vector types scalar (SKVX_DISABLE_SIMD) and its raster pipeline
# chooses its one-pixel scalar path. Here SkVx keeps its SIMD128 path and the
# raster pipeline is Skia's SSE4.1 one, whose intrinsics Emscripten lowers to
# SIMD128 (skia_wasm_sse.patch keeps skcms, which stays portable, from
# including the AVX headers Emscripten cannot offer).
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
extra_cflags = [ "-msimd128", "-sSUPPORT_LONGJMP=wasm", "-USKVX_DISABLE_SIMD",
                 "-msse4.1", "-DSKRP_CPU_SSE41", "-DSKCMS_PORTABLE" ]
ARGS
# Skia's wasm toolchain first runs bin/activate-emsdk, which installs its own
# pinned emsdk under third_party/externals/emsdk. skia_emsdk_dir names the
# emsdk this build uses, so that step is answered by a stub that does nothing.
stub="$skia_dir/third_party/externals/emsdk"
if [ ! -f "$stub/emsdk.py" ]; then
    mkdir -p "$stub"
    printf 'import sys\nsys.exit(0)\n' > "$stub/emsdk.py"
fi
if git -C "$skia_dir" apply --check "$script_dir/skia_wasm_sse.patch" 2>/dev/null; then
    git -C "$skia_dir" apply "$script_dir/skia_wasm_sse.patch"
elif ! git -C "$skia_dir" apply --reverse --check "$script_dir/skia_wasm_sse.patch" 2>/dev/null; then
    echo "Skia wasm SSE source patch does not apply cleanly." >&2
    exit 1
fi
if [ "${GUI_FORMS_SKIA_DEBUG:-0}" = 1 ]; then
    # Skia's assertions (SkASSERT) in the same official build.
    printf 'extra_cflags += [ "-USK_RELEASE", "-DSK_DEBUG", "-g" ]\n' >> "$output_dir/args.gn"
fi
gn=${GN:-"$skia_dir/bin/gn"}
"$gn" gen "$output_dir" --root="$skia_dir"
ninja -C "$output_dir" -j "${BUILD_JOBS:-8}" skia
