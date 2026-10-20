# GUI.Forms Deflate provenance

Owner authorization: after accepting the phase-1 decisions and requesting the
alternatives/leak-evidence screen, the owner authorized taking suitable material
for both PNG and deflate and rewriting it in the complete house style. This
supersedes the shortlist exclusion as a source-use restriction; references are
not thereby certified leak-free. The PNG decision now calls for an owned codec.

## Phase 2 source map

| File / material | Provenance and changes |
|---|---|
| `inflate.cpp`: canonical tree construction and symbol decoding | Derived from Mark Adler's `contrib/puff/puff.c`, version 2.3, in upstream zlib commit `51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf`. This is an altered C++ implementation, not original puff. Count/offset construction and canonical bit traversal retain the algorithm; resumable code state, fixed storage, explicit results, checked quotas and error handling replace puff's whole-buffer/longjmp interface. |
| `inflate.cpp`: block parser and RFC tables | RFC 1951 fields and length/distance tables, informed by the same puff reference and pinned upstream `inflate.c`/`inftrees.c`. Conservatively covered by the retained puff notice. No Chromium optimizations, Boost, miniz, zlib-ng or libdeflate source was copied into this phase. |
| `inflate.cpp`: zlib header/trailer, history ownership, quotas, streaming orchestration | Newly written GUI.Forms implementation against RFC 1950/1951 and the approved study. No heap allocation, setjmp/longjmp or exceptions. Scalar Adler arithmetic here is the wrapper baseline; consolidation/acceleration follows in phase 4. |
| `include/gui_forms/compression/inflate.hpp` | New native API and explicit owner/state layout. Not the eventual C ABI. |
| Tests and fuzz harness | Newly authored test logic; fixture provenance below. The test-only zlib oracle is external, pinned by the source digests in `tests/compression/oracle_sha256.json`; it is never linked into production. |

The unchanged distribution notice is in `LICENSE-puff.txt` and is installed
with the package. Do not remove it during later cleanup. The project licence
continues to apply to newly authored project material; derivation is disclosed.

## PNG and later phases

For the later owned PNG codec, use libspng's progressive/bounded interface and
LodePNG's self-contained PNG/filter implementation as design references,
alongside PNG's specification. Adapt the existing first-party PNG writer to
owned deflate after phase 3. Record exact source pins, per-file notices and
altered-source markings before copying PNG code. No Boost or PNG library is
introduced as a runtime dependency by this phase.

House-style conformance improves reviewability; leak freedom, bounds and
format conformance still require the accepted executable gates. No reference's
security history transfers automatically to a rewritten implementation.
