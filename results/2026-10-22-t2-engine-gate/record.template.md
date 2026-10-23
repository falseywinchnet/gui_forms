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

Feature samples (from `engine_samples::builtin_feature_samples()`):
{{samples}}

{{sample_image_count}} sample images: each sample at scales 1, 1.25, 1.5 and
2, at rest; a pointer sample also at the pointer; a motion sample also 32,
80, 144 and 224 ms after its part turns hot, and once it has settled. A
sample image hashes the part rasters and every command the window gave the
painter (geometry, opacity, colour and paint group), so a scale, a
translation, a cross-fade or a blend shows in it.

{{host_table}}

### Differences

{{differences}}

### The census: each feature and the first sample drawing it

The test requires every value of the engine's vocabulary (fill kinds,
spreads, image modes, procedural patterns, blends, shapes, border styles,
glow placements, keyline edges, presentational properties, curves, easings
and step positions) and the features that are no enumeration in at least
one sample.

{{feature_table}}

## Parts 2 and 4: the window counters

The idle counters cover the two seconds after each window settled; every one
must be 0. The reduced-motion counters cover the ribbon hovered under
reduced motion: no motion clock tick, and the rise jumped to its end.

{{counter_table}}
