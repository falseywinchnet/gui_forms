#!/bin/sh
# Native verification on Linux (stage DK decision 50), for the owner's Lima
# VM. Builds GUI.Forms with the X11 host from a clean checkout with the
# documented pinned toolchain, runs the test suite under Xvfb, then runs the
# forensic frame-drag probe and the X11 behaviour checks once per display
# session it can start:
#
#   xvfb-openbox   Xvfb with Openbox (a plain EWMH window manager)
#   xvfb-mutter    Xvfb with Mutter as an X11 window manager
#   xvfb-kwin      Xvfb with KWin as an X11 window manager
#   xwayland-mutter  headless Mutter (Wayland) and its XWayland
#   xwayland-kwin    virtual KWin (Wayland) and its XWayland
#
# A session whose programs are not installed, or that does not start, is
# recorded as skipped with the reason. Results go under one directory with a
# RESULTS.md summary.
#
#   sh tools/native_verification/run_linux.sh [WORK_DIRECTORY]
#
# WORK_DIRECTORY defaults to ~/gui-forms-verification. Nothing is installed;
# the script stops and names what is missing (SIBLING_PROMPT.md lists the
# packages).
set -eu

source_directory=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
work=${1:-"$HOME/gui-forms-verification"}
mkdir -p "$work"
work=$(CDPATH= cd -- "$work" && pwd)
stamp=$(date -u +%Y%m%dT%H%M%SZ)
results="$work/results-linux-$stamp"
build="$work/build-linux"
skia="$work/build-skia-linux"
mkdir -p "$results"
log="$results/run.log"

say() {
    echo "$*" | tee -a "$log"
}

if [ "$(uname -s)" != Linux ]; then
    echo "run_linux.sh runs on Linux" >&2
    exit 2
fi
if [ -n "$(git -C "$source_directory" status --porcelain)" ]; then
    echo "the checkout at $source_directory has local changes; use a clean clone" >&2
    exit 2
fi
if [ -d /usr/lib/llvm-22/bin ]; then
    PATH="/usr/lib/llvm-22/bin:$PATH"
    export PATH
fi
missing=""
for tool in git cmake ninja gn pkg-config python3 clang clang++ Xvfb xvfb-run xauth openbox; do
    command -v "$tool" >/dev/null 2>&1 || missing="$missing $tool"
done
if [ -n "$missing" ]; then
    echo "missing:$missing (see SIBLING_PROMPT.md, Lima setup)" >&2
    exit 2
fi
if ! clang --version | head -1 | grep -q 'version 22\.1\.'; then
    echo "clang is not LLVM 22.1.x (put /usr/lib/llvm-22/bin first on PATH)" >&2
    exit 2
fi
CC=clang
CXX=clang++
export CC CXX

{
    echo "commit $(git -C "$source_directory" rev-parse HEAD)"
    if [ -r /etc/os-release ]; then . /etc/os-release && echo "os $PRETTY_NAME"; fi
    echo "kernel $(uname -r) $(uname -m)"
    clang --version | head -1
    cmake --version | head -1
    for program in openbox mutter kwin_x11 kwin_wayland Xwayland; do
        if command -v "$program" >/dev/null 2>&1; then echo "$program present"; else echo "$program absent"; fi
    done
} > "$results/environment.txt"
say "results: $results"

say "fetching the pinned text and Skia sources"
(cd "$source_directory" && sh third_party/fetch_text_stack.sh && sh third_party/fetch_skia_cpu.sh) >> "$log" 2>&1
if [ ! -d "$skia" ]; then
    say "building the CPU-only Skia (once)"
    (cd "$source_directory" && GUI_FORMS_SYSTEM_BUILD_TOOLS=1 sh third_party/build_skia_cpu_linux.sh "$skia") \
        >> "$log" 2>&1
fi
say "configuring and building"
cmake -S "$source_directory" -B "$build" -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="$source_directory/cmake/llvm22.cmake" \
    -DGUI_FORMS_ENABLE_LINUX_HOST=ON -DGUI_FORMS_SKIA_PREBUILT=ON -DGUI_FORMS_SKIA_OUT="$skia" \
    -DGUI_FORMS_BUILD_TESTS=ON >> "$log" 2>&1
cmake --build "$build" --parallel >> "$log" 2>&1

say "running the test suite under Xvfb"
suite_status=0
GUI_FORMS_FONT_DIR="$source_directory/assets/fonts" xvfb-run -a -s "-screen 0 1280x1024x24 -noreset" \
    ctest --test-dir "$build" --output-on-failure --timeout 300 --output-junit "$results/ctest-junit.xml" \
    > "$results/ctest.txt" 2>&1 || suite_status=$?
say "test suite exit status $suite_status ($(grep -E 'tests passed|tests failed' "$results/ctest.txt" || echo 'no summary'))"

checks="utility_type skip_taskbar transient_for restack_above_owner_only minimized_state net_wm_state xwayland_position"

# The probes for one session, run inside it. Arguments: session directory.
cat > "$results/session-probes.sh" <<EOF
#!/bin/sh
out=\$1
mkdir -p "\$out"
env | grep -E '^(DISPLAY|WAYLAND_DISPLAY|XDG_SESSION_TYPE)=' > "\$out/display.txt" || true
status=0
"$build/gui_forms_frame_drag_probe" --out "\$out/frame-drag" --input auto > "\$out/frame-drag.txt" 2>&1 || status=\$?
echo "frame-drag \$status" >> "\$out/status.txt"
for check in $checks; do
    status=0
    "$build/gui_forms_x11_behaviour_probe" --check "\$check" --out "\$out/behaviour-\$check.json" \
        >> "\$out/behaviour.txt" 2>&1 || status=\$?
    echo "\$check \$status" >> "\$out/status.txt"
done
EOF
chmod +x "$results/session-probes.sh"

skip_session() {
    mkdir -p "$results/$1"
    printf '{"probe":"session","session":"%s","result":"skipped","reason":"%s"}\n' "$1" "$2" \
        > "$results/$1/session.json"
    say "session $1 skipped: $2"
}

# An X11 window manager on a fresh Xvfb. Arguments: session name, manager
# command.
run_xvfb_session() {
    name=$1
    shift
    say "session $name"
    mkdir -p "$results/$name"
    xvfb-run -a -s "-screen 0 1280x1024x24 -noreset" sh -c '
        out=$0
        probes=$1
        shift
        "$@" > "$out/window-manager.txt" 2>&1 &
        manager=$!
        sleep 3
        if ! kill -0 "$manager" 2>/dev/null; then
            echo "{\"probe\":\"session\",\"result\":\"skipped\",\"reason\":\"the window manager exited at start\"}" > "$out/session.json"
            exit 0
        fi
        sh "$probes" "$out"
        kill "$manager" 2>/dev/null || true
    ' "$results/$name" "$results/session-probes.sh" "$@" >> "$log" 2>&1 || true
}

run_xvfb_session xvfb-openbox openbox
if command -v mutter >/dev/null 2>&1; then
    run_xvfb_session xvfb-mutter mutter --x11 --replace
else
    skip_session xvfb-mutter "mutter is not installed"
fi
if command -v kwin_x11 >/dev/null 2>&1; then
    run_xvfb_session xvfb-kwin kwin_x11 --replace
else
    skip_session xvfb-kwin "kwin_x11 is not installed"
fi

# XWayland under a headless Wayland compositor: the compositor starts
# XWayland and runs the probes as its session client.
if command -v mutter >/dev/null 2>&1 && command -v Xwayland >/dev/null 2>&1; then
    say "session xwayland-mutter"
    mkdir -p "$results/xwayland-mutter"
    XDG_SESSION_TYPE=wayland timeout 300 mutter --wayland --headless --virtual-monitor 1280x1024 \
        -- sh "$results/session-probes.sh" "$results/xwayland-mutter" \
        > "$results/xwayland-mutter/compositor.txt" 2>&1 || true
    [ -f "$results/xwayland-mutter/status.txt" ] ||
        skip_session xwayland-mutter "headless Mutter did not run the probes (see compositor.txt)"
else
    skip_session xwayland-mutter "mutter or Xwayland is not installed"
fi
if command -v kwin_wayland >/dev/null 2>&1 && command -v Xwayland >/dev/null 2>&1; then
    say "session xwayland-kwin"
    mkdir -p "$results/xwayland-kwin"
    XDG_SESSION_TYPE=wayland timeout 300 kwin_wayland --virtual --xwayland --width 1280 --height 1024 \
        --exit-with-session "sh $results/session-probes.sh $results/xwayland-kwin" \
        > "$results/xwayland-kwin/compositor.txt" 2>&1 || true
    [ -f "$results/xwayland-kwin/status.txt" ] ||
        skip_session xwayland-kwin "virtual KWin did not run the probes (see compositor.txt)"
else
    skip_session xwayland-kwin "kwin_wayland or Xwayland is not installed"
fi

python3 "$source_directory/tools/native_verification/summarize.py" "$results" > "$results/RESULTS.md"
say "summary: $results/RESULTS.md"
(cd "$work" && tar -czf "results-linux-$stamp.tar.gz" "results-linux-$stamp")
say "archive: $work/results-linux-$stamp.tar.gz"
