#!/bin/sh
set -eu
export PATH="/opt/homebrew/opt/llvm@22/bin:/opt/homebrew/bin:/usr/bin:/bin:$PATH"
export GUI_FORMS_LLVM_RUNTIME=/Users/joshuahkuttenkuler/Developer/PlaysuiteDependencies/toolkit-4e126d6/toolchain/llvm-22.1.8-macos14
root="$PWD"
# Restore only dependency checkouts created by this experiment: a mirror sync
# removed their working files while leaving their .git databases. Fetch scripts
# below will reapply the repository's declared patches.
for dependency in third_party/skia third_party/skia/third_party/externals/libpng third_party/skia/third_party/externals/zlib third_party/skia/third_party/externals/freetype third_party/harfbuzz third_party/freetype; do
    if [ -d "$dependency/.git" ]; then
        git -C "$dependency" restore --source=HEAD --worktree -- .
    fi
done
mkdir -p build/full-ci
if [ ! -e build/full-ci/gui_forms ]; then
    ln -s "$root" build/full-ci/gui_forms
fi
# The M4 mirror name includes a hash. Preserve ci_build's exact logical layout
# via a private symlink and override only its workspace location, not options.
python3 -c 'import sys; from pathlib import Path; sys.path.insert(0, str(Path("tools").resolve())); import ci_build; ci_build.ROOT = Path("build/full-ci").resolve(); ci_build.main()' --sanitize --jobs 4
