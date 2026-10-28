# Private third-party dependencies

The shaping stack is fetched by `fetch_text_stack.sh`: HarfBuzz 14.6.0 at
`c7a7457b7385f33178e8cf87615ca077a810bbe7` and FreeType 2.14.2 at
`f4205da14867c5387cd6a329b90ee10a6df6eeff`. HarfBuzz's optional raster,
vector, GPU, subset and utility targets remain disabled. The upgrade and
compiler-warning investigation are recorded in
[`HARFBUZZ_14_6_INTAKE_2026-10-06.md`](HARFBUZZ_14_6_INTAKE_2026-10-06.md).

The text engine uses libunibreak 8.0, pinned to
`28a2756b864c343f438cd22537d49d394d4666a5` by `fetch_linebreak.sh`. Its private
line-break subset implements Unicode 17 UAX #14. It was admitted for the
logical text-mask profile only (2026-10-01). The owner widened the admission
to the text engine on 2026-10-26 ("libunibreak is fine, can admit it"), so it
is built wherever HarfBuzz text is (`GUI_FORMS_ENABLE_HARFBUZZ_TEXT`) and
fetched wherever `fetch_text_stack.sh` runs. It is still excluded from builds
without HarfBuzz text and from SDK exports. See
[`LIBUNIBREAK_INTAKE_2026-10-01.md`](LIBUNIBREAK_INTAKE_2026-10-01.md) for the
local conformance reproduction, caller-buffer boundary and license requirements.

Unicode grapheme property tables and conformance cases are pinned, generated,
and attributed under [`unicode/`](unicode/README.md). They are checked-in
generated data so renderer-free builds stay offline.

The CPU renderer is not here: it is `stx_skia/` at the repository root, a
pruned, converted CPU derivative of Skia `chrome/m152`
(`2a9b593bab4b2fd019fa494c8d401ff1fab0b883`) that builds with the tree; its
README and PROVENANCE.md record what it keeps and why. No Skia type may leave
`src/render/skia/`.

SheenBidi 2.9.0 is vendored under `sheenbidi/` for Unicode bidirectional ordering in the private text adapter. Its Headers and Source are unchanged from the pinned commit in PROVENANCE.md; Apache License 2.0 is included. Linux accessibility dynamically links the system ATK/AT-SPI bridge (LGPL 2.1 or later); it does not use GTK widgets.
