#!/bin/sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
skia_dir="$script_dir/skia"
output_dir=${1:-"$skia_dir/out/gui_forms-cpu-release"}

if [ ! -x "$skia_dir/bin/gn" ] || [ ! -x "$skia_dir/third_party/ninja/ninja" ]; then
  echo "Skia is absent. Run $script_dir/fetch_skia_cpu.sh first." >&2
  exit 1
fi

mkdir -p "$output_dir"
cp "$script_dir/skia_cpu_args.gn" "$output_dir/args.gn"
if [ "$(uname -s)" = Darwin ]; then
  : "${GUI_FORMS_LLVM_RUNTIME:?Set GUI_FORMS_LLVM_RUNTIME to the macOS 14 LLVM runtimes}"
  sdk_path=$(xcrun --show-sdk-path)
  cat >> "$output_dir/args.gn" <<EOF
cc = "clang"
cxx = "clang++"
extra_asmflags = [ "-target", "arm64-apple-macos14.0" ]
extra_cflags = [ "-target", "arm64-apple-macos14.0", "-isysroot", "$sdk_path" ]
extra_cflags_cc = [ "-stdlib=libc++", "-nostdinc++", "-isystem", "$GUI_FORMS_LLVM_RUNTIME/include/c++/v1" ]
EOF
fi
if [ "${CMAKE_CXX_COMPILER_LAUNCHER:-}" = ccache ]; then
  echo 'cc_wrapper = "ccache"' >> "$output_dir/args.gn"
fi
"$skia_dir/bin/gn" gen "$output_dir" --root="$skia_dir"
"$skia_dir/third_party/ninja/ninja" -C "$output_dir" -j "${BUILD_JOBS:-4}" skia
