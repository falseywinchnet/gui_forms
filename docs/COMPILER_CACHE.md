# Source consumption and compiler-cache artifacts

Consumers pin this repository's source and build it normally through ccache.
CI checks out the toolkit at `gui_forms/` below the build workspace and builds
under `.build/native-<platform>/gui-forms`. Consumers use the same relative
layout, compiler/runtime, build options and ccache compiler-content check.
The native workflow emits a platform-specific cache archive only after tests
pass. Cache snapshots accelerate compilation; they do not replace consumer tests.

Restore the archive below the consumer workspace, set `CCACHE_DIR` to `.ccache`,
`CCACHE_BASEDIR` to the workspace, `CCACHE_COMPILERCHECK=content`, and all three
CMake C/C++/Objective-C++ compiler launchers to `ccache`. A miss compiles the
source normally. Do not weaken header, compiler or option validation for hits.
Caches are bounded at 500 MB and contain no runtime payload added to the app.

The portable proof in `tools/prove_compiler_cache.py` uses a clean source
snapshot and a new scratch directory. It builds the real retained-control
event test in different producer/consumer locations, requires identical final
executable bytes, and checks implementation/header/compiler-option invalidation.
It measures the fixture only; native application/package evidence is separate.

The owner approved independent source/cache publication on 2026-10-07.
History was extracted from `falseywinchnet/file_manager` at
`0dc39f989f2d077d5485027dba9046f884e43bc7`; its `gui_forms/` subtree became this
repository root. First-party changes follow `planning/PROGRAMMING_HOUSE_STYLE.md`.
Current shared/static target choices are preserved. ThinLTO and static Application
linking remain separate, unmeasured candidates, not consequences of caching.
