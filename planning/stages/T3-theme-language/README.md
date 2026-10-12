# Stage T3: theme language front end

Release: 0.3 Themeable. Depends on: T1 (part and state names), T2 (the
engine's properties, primitives, inputs and expressions). Unblocks T4, FD, D,
H, V. Status 2026-10-12: not started. Authority:
`planning/FORWARD_PROGRAMME.md` §4 T3; design language §12.1 item 1.

Read [the shared brief](../README.md) first.

## Why

The owner's starting idea, 2026-10-12: theming "both by inline effects as
well as by the use of a theme style sheet. Think WinCustomize-type theming
for Windows, or KDE themes, or GTK themes, but better. Done right … bold,
insane idea: inline CSS subset." The owner liked "the huge variety of styles
KDE had".

The owner called the choice of authoring model "a landmine question if we get
it wrong", and it was worked through with a scenario. The scenario was a play
button that pops when pressed, has a sheen following the cursor, glows with
the audio level, and in one window is circular through its stable ID. The
owner's conclusion: "a CSS-specific only declarative could be the option, CSS
now has state machines". Recorded resolution (design language §12.1 item 1):
themes are written in a CSS-syntax subset addressed to the named anatomy
(`::part()`, `:state()`, stable IDs), not a CSS document tree; engine inputs
are custom properties only the engine sets; the state machine's states,
transitions and timings are declared in the theme and run by the engine; new
kinds of effect are C++ engine primitives, never theme code.

Why CSS syntax fits: states are pseudo-classes (including custom `:state()`),
transitions and keyframes move between them, `calc()`, `clamp()`, `min()`
and `max()` are bounded expressions, custom properties are named inputs, and
`::part()` with `:state()` is exactly the named-parts-and-states model.
Anyone who knows CSS can read a theme, and the visual designer (V) can read
and write all of it.

What history teaches:

- Windows XP visual styles addressed everything by class, part and state
  (`Button.PushButton(Hot)`). That is the lasting idea. Their weakness was
  bitmaps everywhere.
- GTK 2's `gtkrc` used selectors, per-state colours and a C engine with
  parameters, so one engine gave dozens of looks. GTK 3 moved to a CSS subset,
  but its node names kept changing until 3.20 and broke third-party themes
  every release. A style language works only if its part names are a stable,
  versioned contract.
- KDE 3 styles were C++ `QStyle` code, and style, colours, window decoration
  and icons combined freely. QtCurve showed that one engine with a large
  parameter set could imitate most others.

## Scope

- **Selectors**: control type, stable ID (`#id`), class, `::part(name)`,
  `:state(name)` and the standard state pseudo-classes the schema publishes.
  No combinators: no descendant, child or sibling selection.
- **Properties**: the engine's style vocabulary (fills, gradients, borders,
  keylines, radii from the engine's shape parameters, shadows, effect layers,
  text role and colour roles, motion). No layout properties. Layout belongs
  to forms and the engine (stage L); the owner: "dynamic flexing layouts
  should be easier than they are. This is an engine task."
- **Inheritance** only for text and colour roles.
- **Inputs**: engine-set custom properties (`--pointer-x`, `--pointer-y`,
  published application values such as `--level`). A theme reads them and
  never sets them.
- **Expressions**: only the bounded grammar of T2.
- **Transitions and state machines**: CSS transition and keyframe syntax
  mapped onto the engine's state machines and settling primitives
  (`follow()`, `spring()`, `decay()` as engine functions).
- **Resources**: PNG for theme assets (a tiling background, a backplane
  fresco), never control faces (§12.3). No font bytes; a theme selects among
  bundled typography roles. No executable code, filesystem or network access.
- **Engine version**: a theme declares the engine version it needs.

The scenario, in the syntax direction agreed with the owner (illustrative,
not a frozen grammar):

```css
button::part(face) {
  background: linear-gradient(var(--face-light), var(--face-dark));
  transition: scale 90ms cubic-bezier(.3, 1.6, .5, 1);   /* the pop */
}
button:active::part(face) { scale: 0.97; }
button:hover::part(sheen) {
  background: radial-gradient(at var(--pointer-x) var(--pointer-y),
                              rgb(255 255 255 / 25%), transparent 60%);
}
#player-play::part(face) {
  shape: circle;             /* a shape parameter the engine provides */
  --glow: clamp(0, calc(var(--level) * 2), 1);
}
```

Note the last rule. A theme chooses among the engine's shape parameters; it
never sets geometry or hit regions directly (owner decision §12.3: "the engine
must specify geometry, not the theme. Therefore the engine must also specify
hit region"). The scenario used CSS `clip-path` during discussion. The
grammar must express shape as an engine parameter.

## How

- Diagnostics name the file, line and rule. An invalid rule is dropped, not
  fatal; the rest of the theme applies, and omitted parts fall back to the
  built-in recipe.
- A byte-preserving round trip: parsing and re-serializing an untouched
  document returns identical bytes. Keep comments, whitespace and rule order
  in the parsed form. V and D depend on this ("no clobber").
- Treat every theme as hostile input: bounded nesting, token and rule counts,
  string lengths and expression sizes; no recursion depth that input
  controls. Add a fuzz target beside `fuzz/png_registry_fuzz.cpp`.
- House style: the parser fills caller-owned rule tables (§4); token, offset
  and line types are distinct (§1); failure states say what they leave
  behind (§9); no lambdas in the parser (§2, §10).
- Theme collections are immutable data assemblies with a manifest, coverage
  table and content hashes (`planning/interview/GUI_FORMS_RESOURCES_AND_CONFIGURATION.md`
  §2.1, §3). The owner settled RC001: data only.

## Gate

Conformance fixtures for every selector and property form; malformed and
hostile input tests; a byte-preserving round trip of an untouched document.
Record the fixture coverage and fuzz run under `results/`.

## Open decisions (ask, do not decide)

1. **RC003, live switching.** May theme collections change while the program
   runs, or only at the next launch? The core already has atomic theme
   replacement.
2. **RC004, trust.** Do external theme packs need a signature, only a content
   hash, or a conspicuous untrusted state?
3. **The file extension and media type** for theme documents.

## Read first

- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §1.2, §8.2, §9.6, §12.1 item 1,
  §12.3
- `planning/interview/GUI_FORMS_RESOURCES_AND_CONFIGURATION.md` §1–§3, §8
- `planning/CONTROL_DNA_AND_STYLE_ADDRESSABILITY.md` §5, §7 gate 5
- `planning/DESIGN_LANGUAGE_SYSTEM_MODEL.md` (the earlier HTML/CSS framing,
  superseded where it describes a CSS document tree)
- [T1](../T1-anatomy/README.md) and [T2](../T2-engine-core/README.md) briefs
