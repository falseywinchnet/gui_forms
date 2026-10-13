# Generated control indexes

The files in this directory (`by-function.md` and `alphabetical.md`) are rendered from `manifests/controls.json` by `python3 tools/control_registry.py render`; `tools/test_control_registry.py` fails when they are stale. Do not edit them by hand: change the registry and re-run the renderer.

`schema.md` is rendered from `manifests/control-schema.json` by `python3 tools/control_schema.py render`; `tools/test_control_schema.py` fails when it is stale. The manifest itself is written by `gui_forms_gallery_pages_tests` from the run-time registry (set `GUI_FORMS_CONTROL_SCHEMA_WRITE=1`).
