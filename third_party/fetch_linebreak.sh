#!/bin/sh
set -eu

script_parent=$(dirname -- "$0")
script_dir=$(CDPATH= cd -- "$script_parent" && pwd)
destination="$script_dir/libunibreak"
revision=28a2756b864c343f438cd22537d49d394d4666a5
remote=https://github.com/adah1972/libunibreak.git

# Development text-mask dependency only. Preserve local modifications and refuse
# an unexpected checkout rather than changing its source underneath a build.
if [ ! -d "$destination/.git" ]; then
  if [ -e "$destination" ]; then
    echo "Existing libunibreak path is not the expected Git checkout" >&2
    exit 1
  fi
  git init "$destination"
  git -C "$destination" remote add origin "$remote"
fi
actual_remote=$(git -C "$destination" remote get-url origin)
if [ "$actual_remote" != "$remote" ]; then
  echo "Unexpected libunibreak origin" >&2
  exit 1
fi
changes=$(git -C "$destination" status --porcelain)
if [ -n "$changes" ]; then
  echo "Refusing to replace modified libunibreak source" >&2
  exit 1
fi
# Three rounds, ten and twenty seconds apart, outlast a short host outage.
if ! git -C "$destination" cat-file -e "$revision^{commit}" 2>/dev/null; then
  fetched=0
  for round in 1 2 3; do
    if git -C "$destination" fetch --depth=1 origin "$revision"; then
      fetched=1
      break
    fi
    if [ "$round" != 3 ]; then
      sleep $((round * 10))
    fi
  done
  if [ "$fetched" != 1 ]; then
    echo "No host served libunibreak $revision" >&2
    exit 1
  fi
fi
git -C "$destination" checkout --detach "$revision"
test "$(git -C "$destination" rev-parse HEAD)" = "$revision"
