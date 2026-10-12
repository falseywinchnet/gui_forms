# Stage T1: anatomy — articulated controls and one schema

Release: 0.2 Verified. Depends on: nothing. Unblocks T2, T3, L, C and,
through them, almost everything else. Status 2026-10-12: not started.
Authority: `planning/FORWARD_PROGRAMME.md` §4 T1.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12: "named control parts or more articulated controls by
design, intrinsically in the framework, expose the capability needed for
theming." And: "addressable anatomy: yes. It's not like nice CSS where you
have operators and a tree." Controls are composed of named, parameterized
parts with states, addressed by name.

The second half is the owner's "one specification structure": "it's
directionally more correct to have dot operators that register interactions
and also that set properties. This is all one language really, it's all one
specification structure that lives inside GUI.Forms." Windows Forms spreads a
form across a layout file, a hook-up file and a handler file; the owner calls
that "both correct and at the same time perhaps a bit sloppy". One registry
describes every control's parts, states, properties and interactions; the
C++ dot API, form documents (FD), theme documents (T3), the C ABI (C) and
the visual tools (D, V) are projections of it.

What goes wrong without it, concretely: PlaySuite's `SettingSlider`
(`~/Developer/playsuite-games/src/settings_sheet.cpp`) subclasses `TrackBar`
and repaints it, and to put its groove and thumb where the pointer mapping
expects them it hard-codes "TrackBar maps the pointer across its width less
ten points at each end" (`constexpr double inset = 10`). Private geometry
leaked into a consumer. PlaySuite also carries about 3,000 literal colours and
does every restyle as a C++ paint override; its own proposal says "theming is
a palette, not skins" and asks for named parts with states.

The design-language view (§8.1): richness fails to repeat when anatomy
cannot be addressed compositionally. Cues bind to (part role, state), so a
cue set on one role reaches every control (the owner's "paint-fill").

## What exists today

- **Property metadata.** `PropertyRegistration` and value-only
  `PropertyDescriptor` snapshots (`experiments/M12P15_PROPERTY_METADATA_CENTER.md`
  and P16–P18): canonical lookup, kinds, categories, defaults, binding,
  serialization visibility, reset, change notification and declared dirty
  effects. It drives `PropertyGrid`
  (`include/gui_forms/controls/panel/property_grid/`) and the inspection
  registries (`include/gui_forms/inspection/`). This is the seed of the one
  schema.
- **Control registry.** `manifests/controls.json` lists the 72 public control
  types (id, type, header, category, purpose, Windows Forms equivalent,
  status, Gallery page, doc page). `tools/control_registry.py` reads it for
  the docs site. It lists no parts or states yet.
- **Theme records.** `include/gui_forms/theme/types/theme_types.hpp`:
  `ControlVisualRole`, seven `ControlSurfaceState`s (normal, hot, pressed,
  pending, invalid, disabled, deactivated) plus selected/focused/defaulted/
  high-contrast flags, `ControlVisualRecipe`, `ControlStateRecipes`,
  `ControlRoleRecipes`, spacing, geometry, typography, motion and structure
  tokens. `src/core/theme/theme/theme.cpp` builds the only theme,
  `windows-professional`.
- **Paint.** Every stock control paints by hand, for example
  `src/controls/range_control/range_control_rendering.cpp`
  (`paint_sunken`, `paint_thumb`, progress stripes). Part geometry is
  computed privately inside each control.
- **Semantics** (stage A) and **stable IDs** (`src/core/control/stable_id/`)
  already exist and must not be disturbed.

## What to deliver

1. Every stock control publishes its parts, with public geometry in the
   control's coordinate space after layout: a track bar's track and thumb
   rectangles; a scroll bar's two arrows, track and thumb; every control's
   focus part; and so on through `manifests/controls.json`.
2. Every part maps to a shared part role from design language §8.1 (region,
   shadow, face, edge, keyline, perimeter, sheen, seam, divider, caption, cap,
   well, track, fill, thumb, actuator, indicator, activity, glyph, icon,
   label, content, caret, disclosure, grip, chip, and the state cues focus,
   default, selection, validation, drop). Composites reuse simpler systems:
   a scroll arrow is a button; a breadcrumb segment is a button in a
   parent-owned overlap (§8.1; CONTROL_DNA §3.6, §4).
3. Every control publishes its states from the set of §8.4 that it actually
   has. A theme can select only published states and can never invent one
   visible to code or accessibility (§8.4 rule 3).
4. The property registry grows into the one schema: parts, states,
   properties and interactions per control type, readable at run time and
   exportable (the manifest and the docs are generated from it, which is also
   a 1.0 condition).
5. Remove the PlaySuite-style workarounds: a consumer reads the thumb and
   track rectangles instead of copying insets.

## How

- Geometry belongs to the engine and the control anatomy, never to a theme
  (owner decision, design language §12.3 "Reshaping"). The control computes
  each part's geometry from its layout and the shape parameters a theme
  selected; the hit region derives from that geometry and may exceed the
  visual but never falls below the target minimums of §7.4. Interactive order
  belongs to the parent region (`planning/AUTHORITATIVE_CONTROL_REGIONS.md`).
- Name parts with stable, versioned identifiers from day one. GTK 3 changed
  its CSS node names until 3.20 and broke third-party themes every release;
  part names and roles are a public contract, additions only within a major
  version.
- Parts are addressed by name, not walked as a tree; there are no combinators
  (T3 relies on this).
- Keep painting where it is for now. T1 publishes anatomy; T2 and T4 move
  paint onto the engine. Do not change any control's look in this stage.
- House style: part identity and part index are distinct types (§1); part
  rectangles are value records with named fields; the registry is an explicit
  registration protocol, opt-in per control type, with no member detection
  choosing behaviour (§7); no lambdas in registration (§2, §10).
- The registry is renderer-neutral: no Skia type in any public header or in
  the schema (`AGENTS.md`).

## Gate

Every control in `manifests/controls.json` lists its parts and states in the
schema; a test reads each part's geometry from a laid-out Gallery; the
PlaySuite workarounds that copied private geometry (the track bar inset) are
no longer needed. Record the coverage table and test run under `results/`.

## Open decisions (ask, do not decide)

1. **Part naming spelling.** Kebab-case CSS-style names (`::part(thumb)`) are
   what T3 will read; the C++ enumerators follow house naming. Confirm the
   public spelling before freezing it, since it becomes a contract.
2. **Application-authored states.** CONTROL_DNA §3.7 allows "application-
   authored finite states declared in the source contract". Whether custom
   controls can publish their own states in this stage, or later, is open.

## Read first

- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §8 (parts, propagation,
  participation, states), §7.4, §12.3
- `planning/CONTROL_DNA_AND_STYLE_ADDRESSABILITY.md` §3, §4, §7
- `planning/AUTHORITATIVE_CONTROL_REGIONS.md`
- `planning/DESIGN_LANGUAGE_SYSTEM_MODEL.md` §1–§2, §5
- `experiments/M12P15_PROPERTY_METADATA_CENTER.md` to `M12P18_*`
- `include/gui_forms/theme/types/theme_types.hpp`
