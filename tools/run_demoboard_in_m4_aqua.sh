#!/bin/sh
set -eu

# Screen Sharing/Aqua launcher for the deterministic m4build mirror. Running
# the binary through a symlink changes NSBundle discovery and hides bundled
# fonts, so the executable path itself must remain inside the real .app.
if [ "$#" -lt 1 ]; then
    echo "usage: $0 <path-to>/File Manager Demoboard.app/Contents/MacOS/File Manager Demoboard" >&2
    exit 2
fi
demoboard_executable=$1
exec "$demoboard_executable"
