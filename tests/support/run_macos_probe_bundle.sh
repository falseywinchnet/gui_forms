#!/bin/sh
# Launches a probe application bundle through LaunchServices, as a user's
# launch arrives, so that it is activated as an ordinary application is
# (macOS 14 and later grant [NSApp activate] only when the active
# application yields; a process started from a terminal is not given that).
#
#   sh run_macos_probe_bundle.sh <bundle.app> <output.jsonl>
#
# The bundle writes its lines to <output.jsonl> (--output) and this script
# prints them. It succeeds only when the bundle's last line is
# "macos compositor probe finished".
bundle=$1
output=$2
if [ -z "$bundle" ] || [ -z "$output" ]; then
  echo "usage: run_macos_probe_bundle.sh <bundle.app> <output.jsonl>" >&2
  exit 2
fi
rm -f "$output"
# -W waits for the bundle to quit; -n starts a new instance even if one runs.
/usr/bin/open -W -n "$bundle" --args --output "$output"
launched=$?
if [ ! -f "$output" ]; then
  echo "the probe bundle wrote no output (open exited $launched)" >&2
  exit 1
fi
cat "$output"
last=$(tail -n 1 "$output")
if [ "$last" != "macos compositor probe finished" ]; then
  echo "$last" >&2
  exit 1
fi
exit 0
