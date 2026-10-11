# GUI.Forms library atlas inputs

This directory holds the **inputs** and the machine-readable **manifest** of
the generated declaration atlas. The atlas pages themselves are MyST Markdown
under `docs/controls/atlas/` and are part of the Sphinx site
(`docs/controls/atlas/index.md`).

| Path | Role |
| --- | --- |
| `manual.json` | Hand-reviewed type narratives, selected method explanations, visual-classification overrides, and capture paths. Edited by people. |
| `captures/` | Source of truth for control captures. A PNG is admitted only after the native surface was viewed through Screen Sharing. Referenced captures are copied into `docs/controls/atlas/captures/` by the generator so the Sphinx tree is self-contained. |
| `manifest.json` | Generated page inventory (schema 2): one record per type and enum with its page name, header, hierarchy, visual flag, review status, and search text. |
| `MIGRATION_FRONTIER.md` | The per-type source isolation bundle record. |

The older iframe navigator (`index.html`, `manifest.js`, `assets/`), the 617
generated HTML pages (`pages/`) and their Markdown mirrors (`markdown/`) were
removed on 2026-10-11; the Sphinx atlas supersedes them.

## Regenerate

```sh
python3 tools/generate_library_docs.py
```

Parses every `class`, `struct`, and `enum class` below `include/gui_forms/`
and `src/`, locates definitions below `src/`, merges `manual.json`, writes
`docs/controls/atlas/` and `manifest.json`, and prints one summary line with
the counts of types lacking hand-reviewed narratives, visual types with
captures pending, and grouped source migrations pending. These are reported,
not hidden: pages label each narrative as **hand-reviewed** or
**declaration-derived** and each visual type as captured or **capture
pending**.

## Check (CI)

```sh
python3 tools/generate_library_docs.py --check
```

Regenerates into a temporary directory and fails with the list of differing
paths when the committed `docs/controls/atlas/` or `manifest.json` is stale.

## Strict mode

```sh
python3 tools/generate_library_docs.py --strict
```

Restores the earlier hard gates: every type needs a review in `manual.json`,
every visual type needs a capture, every reviewed type with a `methods` object
must explain every discovered method, and no grouped source migration may
remain. The repository does not currently pass strict mode; the index page
reports the exact counts.

## `manual.json` schema

```json
{
  "schema": 1,
  "types": {
    "TypeName": {
      "name": "TypeName",
      "declaration_header": "include/gui_forms/.../type_name.hpp",
      "status": "OBSERVED: ...",
      "summary": "One-paragraph narrative.",
      "visual": true,
      "capture": "captures/some_capture.png",
      "capture_note": "What the crop shows.",
      "method_scope": "public",
      "methods": {"method_name": "One sentence."}
    }
  }
}
```

`name` defaults to the key and `declaration_header` is required only when the
same type name is declared in more than one header. A review without a
`methods` object accepts the generator's declaration-derived method
explanations; a review with one may omit methods (they are then labelled
declaration-derived on the page) but may not name methods that no longer
exist. A review that does not resolve to exactly one declaration is an error.

## Per-type source layout

New and migrated visual types use one directory per type, mirrored between the
public and implementation trees. A derived type nests below its reusable base
when that relationship is stable and informative. Compatibility family headers
remain thin umbrellas during migration; the atlas "Source layout" line reports
`isolated per type`, `header-only or distributed`, `grouped migration
pending`, or `demonstration composition` for each type.
