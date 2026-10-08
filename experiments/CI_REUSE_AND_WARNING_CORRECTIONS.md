# CI reuse and warning corrections

Date: 2026-10-07.

**GIVEN:** reduce repeated compilation and validation after pushes/merges, and
fix the CoreGraphics mixed-enum and stb_vorbis pointer-comparison warnings.

**OBSERVED baseline:** the old workflow ran the cold/relocated/invalidation
experiment before restoring the native object cache on every platform and every
push. Previous receipts recorded roughly 7–11 minutes of deliberate recompilation
per platform. PR-scoped caches were not directly visible to main, and Windows
fetched its development text dependencies again. Ordinary native compilation
could hit ccache while still paying all that other work.

The new workflow reuses cache-mechanism receipts and, for identical complete trees
and build environments, successful native validation. `docs/COMPILER_CACHE.md`
describes the matching/trust rules, provenance, force controls and fallbacks.
No ccache compiler/header validation or test requirement for changed code is
weakened. Native platforms remain independent matrix jobs.

**OBSERVED warning causes and corrections:**

- CoreGraphics declares the alpha and byte-order flags in different enum types.
  Explicitly convert each to the API's 32-bit mask representation before OR;
  pixel format, alpha and byte-order bits are unchanged.
- stb_vorbis's `set_file_offset` formed `stream_start + loc` before validating
  `loc`, and attempted to detect wraparound with a comparison the compiler can
  eliminate under valid-pointer arithmetic rules. The generated source projection
  compares the integer offset with the buffer length first. Exact EOF remains
  rejected as upstream intended; the pointer is formed only in the valid branch.
  The fetched upstream source and its SHA-256 check are unchanged. The one-line
  projection is visible in `cmake/Audio.cmake`; it is not a blanket warning waiver.

**MEASURED locally:** the native clipboard-image and audio fixtures pass. The new
Vorbis boundary fixture covers first/last bytes, exact EOF, empty input and the
maximum unsigned offset; existing malformed-stream, quota, cancellation and PCM
fixtures pass. Rebuilding both affected translation units emits neither warning.

House-style review covers the explicit mask value, Vorbis test seam and boundary
assertions, CMake source projection, CI identity/provenance/reuse helpers, Python
rejection/wait/restore fixtures and workflow. Review checks named behavior,
explicit types, ownership of extracted files, bounds before pointer formation,
immutable artifact identities, acyclic waits, and no partial validation receipts.
The source projection preserves upstream C spelling; legacy/vendor files outside
the changed scope are not claimed compliant.
