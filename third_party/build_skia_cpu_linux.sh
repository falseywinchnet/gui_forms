#!/bin/sh
set -eu
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
skia_dir="$script_dir/skia"
output_dir=${1:-"$skia_dir/out/gui_forms-linux-cpu"}
case $(uname -m) in
  aarch64) target_cpu=arm64 ;;
  x86_64) target_cpu=x64 ;;
  *) echo "Unsupported Linux build architecture" >&2; exit 1 ;;
esac
command -v gn >/dev/null
command -v ninja >/dev/null
mkdir -p "$output_dir"
sed -e "s/target_cpu = .*/target_cpu = \"$target_cpu\"/" \
    "$script_dir/skia_cpu_args.gn" > "$output_dir/args.gn"
cat >> "$output_dir/args.gn" <<'ARGS'
target_os = "linux"
cc = "clang"
cxx = "clang++"
extra_cflags = [ "-fPIC" ]
ARGS
if [ "${CMAKE_CXX_COMPILER_LAUNCHER:-}" = ccache ]; then
    printf '\ncc_wrapper = "ccache"\n' >> "$output_dir/args.gn"
fi
gn gen "$output_dir" --root="$skia_dir"
ninja -C "$output_dir" -j "${BUILD_JOBS:-2}" skia
