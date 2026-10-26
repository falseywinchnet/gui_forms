#!/bin/sh
# Build the CPU Skia archive natively on Windows with the pinned MSYS2 CLANG64
# toolchain (clang, clang++ and llvm-ar on PATH): skia_cpu_windows_mingw_args.gn.
# CI runs it (.github/workflows/native-platform.yml); local trees fetch the
# archive CI publishes instead (tools/skia_archive.py fetch).
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
skia_dir="$script_dir/skia"
output_dir=${1:-"$skia_dir/out/gui_forms-cpu-windows-mingw"}

# fetch-gn and fetch-ninja install gn.exe and ninja.exe on Windows.
if { [ ! -x "$skia_dir/bin/gn" ] && [ ! -x "$skia_dir/bin/gn.exe" ]; } ||
   { [ ! -x "$skia_dir/third_party/ninja/ninja" ] && [ ! -x "$skia_dir/third_party/ninja/ninja.exe" ]; }; then
  echo "Skia is absent. Run $script_dir/fetch_skia_cpu.sh first." >&2
  exit 1
fi
for tool in clang clang++ llvm-ar; do
  if ! command -v "$tool" >/dev/null; then
    echo "$tool is not on PATH; this build needs the MSYS2 CLANG64 toolchain." >&2
    exit 1
  fi
done

mkdir -p "$output_dir"
# gn and ninja are native Windows programs: give them Windows paths.
if command -v cygpath >/dev/null; then
  skia_dir=$(cygpath -m "$skia_dir")
  output_dir=$(cygpath -m "$output_dir")
fi
cp "$script_dir/skia_cpu_windows_mingw_args.gn" "$output_dir/args.gn"
if [ "${CMAKE_CXX_COMPILER_LAUNCHER:-}" = ccache ]; then
  printf '\ncc_wrapper = "ccache"\n' >> "$output_dir/args.gn"
fi
"$skia_dir/bin/gn" gen "$output_dir" --root="$skia_dir"
"$skia_dir/third_party/ninja/ninja" -C "$output_dir" -j "${BUILD_JOBS:-4}" skia
