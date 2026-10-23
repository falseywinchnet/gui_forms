# T2 engine gate record

Filled by `python tools/engine_gate.py compare` in the `engine-gate` job of
`native.yml`. See `README.md` in this directory for how the evidence is made
and for the tests behind parts 3 and 4 of the gate.

- **Commit:** `{{commit}}`
- **CI run:** {{run}}
- **Verdict:** {{verdict}}

## Part 1: reference images per host

Primitives (from `builtin_primitive_registry()`): {{primitives}}

{{image_count}} images: each primitive at scales 1, 1.25, 1.5 and 2, at rest,
hot and pressed.

{{host_table}}

### Differences

{{differences}}

## Parts 2 and 4: the window counters

The idle counters cover the two seconds after each window settled; every one
must be 0. The reduced-motion counters cover the ribbon hovered under
reduced motion: no motion clock tick, and the rise jumped to its end.

{{counter_table}}
