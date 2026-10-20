#!/bin/sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
# HarfBuzz 14.6.0 (latest stable release verified 2026-10-06).
harfbuzz_revision=c7a7457b7385f33178e8cf87615ca077a810bbe7
freetype_revision=f4205da14867c5387cd6a329b90ee10a6df6eeff

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

fetch_revision "$script_dir/harfbuzz" \
  https://github.com/harfbuzz/harfbuzz.git "$harfbuzz_revision"
fetch_revision "$script_dir/freetype" \
  https://gitlab.freedesktop.org/freetype/freetype.git "$freetype_revision" \
  https://github.com/freetype/freetype.git

test "$(git -C "$script_dir/harfbuzz" rev-parse HEAD)" = "$harfbuzz_revision"
test "$(git -C "$script_dir/freetype" rev-parse HEAD)" = "$freetype_revision"
