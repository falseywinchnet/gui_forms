#!/bin/sh
# Fetches the official Zstandard 1.5.7 release tarball into
# stx_cng/third_party/ and verifies its SHA-256 against the pin (the hash the
# release publishes beside the tarball). CMake verifies it again before
# extracting lib/ and the licence. A failed fetch leaves no tarball behind.
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
destination="$script_dir/../third_party"
name=zstd-1.5.7.tar.gz
url=https://github.com/facebook/zstd/releases/download/v1.5.7/$name
pinned=eb33e51f49a15e023950cd7825ca74a4a2b43db8354825ac24fc1b7ee09e6fa3

digest() {
  if command -v sha256sum >/dev/null 2>&1; then
    sha256sum "$1" | cut -d ' ' -f 1
  else
    shasum -a 256 "$1" | cut -d ' ' -f 1
  fi
}

mkdir -p "$destination"
if [ -f "$destination/$name" ] && [ "$(digest "$destination/$name")" = "$pinned" ]; then
  exit 0
fi
partial="$destination/$name.partial"
rm -f "$partial"
# Three rounds, ten and twenty seconds apart, outlast a short host outage.
for round in 1 2 3; do
  if curl -fsSL --retry 2 -o "$partial" "$url"; then
    break
  fi
  rm -f "$partial"
  if [ "$round" = 3 ]; then
    echo "No host served $url" >&2
    exit 1
  fi
  sleep $((round * 10))
done
actual=$(digest "$partial")
if [ "$actual" != "$pinned" ]; then
  rm -f "$partial"
  echo "$name hash $actual is not the pinned $pinned" >&2
  exit 1
fi
mv "$partial" "$destination/$name"
