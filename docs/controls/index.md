# Controls

GUI.Forms ships its controls as stock C++ types under
`include/gui_forms/controls/`. Each has a stable identity, retained state,
synchronous setters, typed change events, a default semantic adapter for
accessibility, and a page in this section. The set is organised the way
Forms developers already think about it: by what the control is *for*.

Every control is described from one registry, `manifests/controls.json`,
which records its header, type name, function category, Forms counterpart (or
"GUI.Forms-only"), status and gallery page. The by-function and alphabetical
indexes below are generated from that registry, so they cannot drift from the
headers; a test fails when a public control type is missing from the
registry or listed twice. The Gallery program builds its navigation from the
same file.

## Status labels

stable
: Construction, behaviour, event order, rendering, input, accessibility and
  disposal are tested on every CI platform, and the type is unlikely to
  change shape before 1.0.

provisional
: Implemented and demonstrated, but the API or visual recipe may still change;
  expect to rebuild and possibly adjust call sites.

lab
: Exists to exercise the engine (rendering experiments, measurement views).
  Usable, but not a product control and not a compatibility promise.

## Where to look

- **By function** answers "which control lets the user set a value / pick
  from a list / see progress?". It is the page to read first.
- **Alphabetical** is the lookup table when you know the name.
- **Atlas** is the generated per-type reference: every public type and
  enumeration with its declaration, methods and reviewed narrative, produced
  by `tools/generate_library_docs.py` from the headers.

```{toctree}
:maxdepth: 1

generated/by-function
generated/alphabetical
atlas/index
```

## Conventions shared by every control

- Construct with `make_control<T>(StableId, ...)`; the parent owns the result
  once you `add_child` it.
- Setting an equal value does nothing; state events fire for every actual
  change; input events (`clicked`, `scroll`, `item_activated`) fire for user
  input only and are never synthesized by setters.
- Collection controls (`ListBox`, `ComboBox`, `CommandBar`, `ChoiceGroup`,
  `ExpandableSections`) take item records with unique IDs; replacing the
  list reuses controls with the same ID and keeps your subscriptions.
- Appearance comes from named roles resolved by the host theme. Named parts
  (`part("item", index)`, `header`, `body`) expose the retained sub-controls
  for custom recipes without subclassing.
- Accessibility metadata is optional; the default adapter publishes a
  sensible role and name, and authored labels enrich it.
