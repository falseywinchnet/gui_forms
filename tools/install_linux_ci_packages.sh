#!/bin/sh
# Install the Ubuntu 24.04 packages of one CI profile, reusing downloaded
# packages kept in an Actions cache.
#
#   sh tools/install_linux_ci_packages.sh <profile> <archive-directory>
#
# profile: toolkit (native and sanitizer jobs) or programs (release.yml).
# archive-directory: where apt keeps the downloaded .deb files. The workflow
# restores it from an Actions cache before this runs and, on main, saves it
# afterwards; the cache key is a hash of this file, so a changed package set
# starts a new cache.
#
# Why: the runners' Ubuntu mirror (azure.archive.ubuntu.com) sometimes serves
# at 40-260 kB/s, and the 30 MB of Ubuntu packages then took 7-11 minutes
# (run 39806663239). apt.llvm.org stays fast, and its 140 MB of LLVM packages
# are left out of the kept archives so the cache stays small. apt still reads
# the current package lists: a package whose version moved on is downloaded
# again, and everything installed is exactly what apt would install without
# the cache.
set -eu

profile=$1
archives=$2

# xcompmgr: a compositing manager for stage T5's X11 compositor probe and
# gate under Xvfb (owner decision T5-Q12).
toolkit_packages="cmake ninja-build ccache clang-22 generate-ninja pkg-config \
libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libatk1.0-dev libatk-bridge2.0-dev \
xvfb xauth xcompmgr xdg-utils dbus at-spi2-core python3-gi gir1.2-atspi-2.0"
case "$profile" in
  toolkit) packages=$toolkit_packages ;;
  programs) packages="$toolkit_packages xdotool patchelf dpkg-dev" ;;
  *) echo "unknown package profile: $profile (expected toolkit or programs)" >&2; exit 2 ;;
esac

# Ubuntu 24.04's default Clang 18 rejects the existing in-class friend
# exception specifications. Use the LLVM 22 release series.
curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key -o "${RUNNER_TEMP:-/tmp}/llvm-key.asc"
sudo install -m 0644 "${RUNNER_TEMP:-/tmp}/llvm-key.asc" /usr/share/keyrings/apt-llvm.asc
echo 'deb [signed-by=/usr/share/keyrings/apt-llvm.asc] https://apt.llvm.org/noble/ llvm-toolchain-noble-22 main' \
  | sudo tee /etc/apt/sources.list.d/llvm-22.list
sudo apt-get update

mkdir -p "$archives/partial"
kept_before=$(find "$archives" -maxdepth 1 -name '*.deb' | wc -l)
started=$(date +%s)
# shellcheck disable=SC2086 # the package list is split into words on purpose
sudo apt-get install -y -o Dir::Cache::Archives="$archives" $packages
finished=$(date +%s)
echo "installed the $profile packages in $((finished - started)) s; $kept_before kept packages were available"

# apt.llvm.org versions carry "~++" (snapshot builds); those downloads are
# fast and large, so they are not kept. The rest are made readable by the
# runner user, which saves the cache.
sudo find "$archives" -maxdepth 1 -name '*~++*.deb' -delete
sudo rm -f "$archives/lock"
sudo chown -R "$(id -u):$(id -g)" "$archives"
echo "$(find "$archives" -maxdepth 1 -name '*.deb' | wc -l) packages kept for the next run"

echo /usr/lib/llvm-22/bin >> "$GITHUB_PATH"
