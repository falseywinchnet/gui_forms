#!/bin/sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
skia_dir="$script_dir/skia"
skia_revision=2a9b593bab4b2fd019fa494c8d401ff1fab0b883
libpng_revision=d5515b5b8be3901aac04e5bd8bd5c89f287bcd33
zlib_revision=646b7f569718921d7d4b5b8e22572ff6c76f2596
freetype_revision=264b5fbf5b912b39f98d038bf75d39be0a73f21b

# fetch_revision destination upstream revision [mirror]
# A pin is a commit identity, so every host holding it yields the same tree;
# the checks at the end verify each checkout against its pin. The mirror (an
# official GitHub copy, nearest to CI) is tried before the upstream host, which
# stays recorded as origin. Three rounds, ten and twenty seconds apart, outlast
# a short host outage; a failed fetch leaves the checkout as it was.
fetch_revision() {
  destination=$1
  upstream=$2
  revision=$3
  mirror=${4:-}
  if [ ! -d "$destination/.git" ]; then
    git init "$destination"
    git -C "$destination" remote add origin "$upstream"
  fi
  if ! git -C "$destination" cat-file -e "$revision^{commit}" 2>/dev/null; then
    fetched=0
    for round in 1 2 3; do
      for remote in $mirror "$upstream"; do
        if git -C "$destination" fetch --depth=1 "$remote" "$revision"; then
          fetched=1
          break 2
        fi
      done
      if [ "$round" != 3 ]; then
        sleep $((round * 10))
      fi
    done
    if [ "$fetched" != 1 ]; then
      echo "No host served $revision for $destination" >&2
      exit 1
    fi
  fi
  git -C "$destination" checkout --detach "$revision"
}

fetch_revision "$skia_dir" https://skia.googlesource.com/skia.git "$skia_revision" \
  https://github.com/google/skia.git
if git -C "$skia_dir" apply --check "$script_dir/skia_png_only.patch" 2>/dev/null; then
  git -C "$skia_dir" apply "$script_dir/skia_png_only.patch"
elif ! git -C "$skia_dir" apply --reverse --check "$script_dir/skia_png_only.patch" 2>/dev/null; then
  echo "Skia PNG-only source patch does not apply cleanly." >&2
  exit 1
fi
if git -C "$skia_dir" apply --check "$script_dir/skia_windows_mingw.patch" 2>/dev/null; then
  git -C "$skia_dir" apply "$script_dir/skia_windows_mingw.patch"
elif ! git -C "$skia_dir" apply --reverse --check "$script_dir/skia_windows_mingw.patch" 2>/dev/null; then
  echo "Skia Windows MinGW source patch does not apply cleanly." >&2
  exit 1
fi
if [ "${GUI_FORMS_SYSTEM_BUILD_TOOLS:-0}" = 1 ]; then
  command -v gn >/dev/null
  command -v ninja >/dev/null
else
  "$skia_dir/bin/fetch-gn"
  "$skia_dir/bin/fetch-ninja"
fi
mkdir -p "$skia_dir/third_party/externals"
fetch_revision "$skia_dir/third_party/externals/libpng" \
  https://skia.googlesource.com/third_party/libpng.git "$libpng_revision" \
  https://github.com/pnggroup/libpng.git
# Chromium's zlib fork has no GitHub copy; it comes from its upstream only.
fetch_revision "$skia_dir/third_party/externals/zlib" \
  https://chromium.googlesource.com/chromium/src/third_party/zlib "$zlib_revision"
fetch_revision "$skia_dir/third_party/externals/freetype" \
  https://chromium.googlesource.com/chromium/src/third_party/freetype2.git "$freetype_revision" \
  https://github.com/freetype/freetype.git

test "$(git -C "$skia_dir" rev-parse HEAD)" = "$skia_revision"
test "$(git -C "$skia_dir/third_party/externals/libpng" rev-parse HEAD)" = "$libpng_revision"
test "$(git -C "$skia_dir/third_party/externals/zlib" rev-parse HEAD)" = "$zlib_revision"
test "$(git -C "$skia_dir/third_party/externals/freetype" rev-parse HEAD)" = "$freetype_revision"
