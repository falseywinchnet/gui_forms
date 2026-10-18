#!/bin/sh
# Native verification on macOS (stage DK decision 50). Builds GUI.Forms from
# a clean checkout with the documented pinned toolchain, runs the test
# suite, the forensic frame-drag probe and the AppKit behaviour checks, and
# collects every result under one directory with a RESULTS.md summary.
#
#   sh tools/native_verification/run_macos.sh [WORK_DIRECTORY]
#
# WORK_DIRECTORY defaults to ~/gui-forms-verification. The checkout is the
# repository this script is in; it must have no local changes. Nothing is
# installed: the script stops and names what is missing. The only network
# use is the documented build itself (the LLVM runtime and Skia sources it
# fetches by pinned digest). tools/native_verification/SIBLING_PROMPT.md is
# the procedure around it.
set -eu

source_directory=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
work=${1:-"$HOME/gui-forms-verification"}
mkdir -p "$work"
work=$(CDPATH= cd -- "$work" && pwd)
stamp=$(date -u +%Y%m%dT%H%M%SZ)
results="$work/results-macos-$stamp"
build="$work/build-macos"
mkdir -p "$results"
log="$results/run.log"

say() {
    echo "$*" | tee -a "$log"
}

if [ "$(uname -s)" != Darwin ] || [ "$(uname -m)" != arm64 ]; then
    echo "run_macos.sh runs on an Apple silicon Mac" >&2
    exit 2
fi
if [ -n "$(git -C "$source_directory" status --porcelain)" ]; then
    echo "the checkout at $source_directory has local changes; use a clean clone" >&2
    exit 2
fi
missing=""
for tool in git cmake ninja python3 brew; do
    command -v "$tool" >/dev/null 2>&1 || missing="$missing $tool"
done
if [ -n "$missing" ]; then
    echo "missing:$missing (see SIBLING_PROMPT.md, Mac setup)" >&2
    exit 2
fi
llvm_prefix=$(brew --prefix llvm@22 2>/dev/null || true)
if [ -z "$llvm_prefix" ] || [ ! -x "$llvm_prefix/bin/clang" ]; then
    echo "missing: llvm@22 from Homebrew (see SIBLING_PROMPT.md, Mac setup)" >&2
    exit 2
fi
PATH="$llvm_prefix/bin:$PATH"
CC=clang
CXX=clang++
OBJCXX=clang++
export PATH CC CXX OBJCXX

{
    echo "commit $(git -C "$source_directory" rev-parse HEAD)"
    echo "macOS $(sw_vers -productVersion) ($(sw_vers -buildVersion))"
    echo "hardware $(sysctl -n machdep.cpu.brand_string 2>/dev/null || echo unknown)"
    clang --version | head -1
    cmake --version | head -1
} > "$results/environment.txt"
say "results: $results"

# The documented macOS 14 runtimes (docs/user/install.md), built once.
runtime="$work/toolchain/llvm-22.1.8-macos14"
if [ ! -d "$runtime" ]; then
    say "building the pinned LLVM runtimes (once)"
    python3 "$source_directory/tools/build_macos_runtimes.py" --work "$work/llvm-runtimes" \
        --prefix "$runtime" --jobs 4 >> "$log" 2>&1
fi
GUI_FORMS_LLVM_RUNTIME=$runtime
export GUI_FORMS_LLVM_RUNTIME

say "fetching the pinned text sources"
(cd "$source_directory" && sh third_party/fetch_text_stack.sh) >> "$log" 2>&1
say "configuring and building (the first build also builds the CPU-only Skia)"
cmake -S "$source_directory" -B "$build" -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="$source_directory/cmake/llvm22.cmake" \
    -DGUI_FORMS_BUILD_TESTS=ON >> "$log" 2>&1
cmake --build "$build" --parallel >> "$log" 2>&1

say "running the test suite"
suite_status=0
GUI_FORMS_FONT_DIR="$source_directory/assets/fonts" ctest --test-dir "$build" --output-on-failure \
    --timeout 300 --output-junit "$results/ctest-junit.xml" > "$results/ctest.txt" 2>&1 || suite_status=$?
say "test suite exit status $suite_status ($(grep -E 'tests passed|tests failed' "$results/ctest.txt" || echo 'no summary'))"

say "running the forensic frame-drag probe"
probe_status=0
"$build/gui_forms_frame_drag_probe" --out "$results/frame-drag" --input auto > "$results/frame-drag.txt" 2>&1 ||
    probe_status=$?
say "frame-drag probe exit status $probe_status"

for check in fullscreen_auxiliary child_order_out child_miniaturize tool_hides_on_deactivate \
    context_menu_key ime_escape; do
    status=0
    "$build/gui_forms_macos_behaviour_probe" --check "$check" --out "$results/behaviour-$check.json" \
        >> "$results/behaviour.txt" 2>&1 || status=$?
    say "behaviour $check exit status $status"
done

python3 "$source_directory/tools/native_verification/summarize.py" "$results" > "$results/RESULTS.md"
say "summary: $results/RESULTS.md"
(cd "$work" && tar -czf "results-macos-$stamp.tar.gz" "results-macos-$stamp")
say "archive: $work/results-macos-$stamp.tar.gz"
