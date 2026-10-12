# Stage V: WYSIWYG theme designer

Release: 0.7 Designers. Depends on: T4 (a real default theme to edit) and D's
core (document editing, stable-ID lookup). Status 2026-10-12: not started.
Authority: `planning/FORWARD_PROGRAMME.md` §6 V.

Read [the shared brief](../README.md) first.

## Why

The owner's "way 5.5", 2026-10-12: "a WYSIWYG theming tool. In other words,
a user designs animations, controls, etc. all using a reference in something
like [a classic timeline-and-stage animation authoring tool]'s interface, and
we can dogfood-subset Plan Paint editors in different places. We can also
devise ways to default or 'paint-fill' all controls with the same cues, when
cues are established for one button. The design language and ergonomics here
are going to be key."

On per-window custom controls: "custom, specific extra control handling
specific just to one window in your program — that's an ergonomics problem.
Do you specify it in code? Maybe you build it in the designer, give it a
unique label, save the project, open the theme in the WYSIWYG designer, find
your control, initialized with a template, and then adjust it until you get
it behaving how you like it, then save."

In GUI.Forms documents, describe the tool as "a timeline-and-stage animation
tool". Do not name or copy a specific third-party animation product's model.

## What to deliver

1. A designer, written in GUI.Forms, that edits theme documents (T3) on a
   live stage showing real controls, with a timeline for the presentational
   state machines and transitions (T2).
2. Plan Paint's editors reused for gradients, curves and colour (from
   `falseywinchnet/plan-paint`; they are the owner's own and already run on
   GUI.Forms).
3. Cue propagation ("paint-fill"): cues established on one control's parts
   (face, edge, focus …) propagate to every part with the same role (design
   language §8.2). Part roles are shared vocabulary, which is what makes this
   possible.
4. Stable-ID rules: a control given a stable ID in the interface builder (D)
   is found in the theme designer, starts from its type's template, and saves
   as a rule keyed on that ID.
5. Round-trip safety: theme documents edited by hand and in the designer keep
   every untouched byte (T3's guarantee).

## How

- The designer can only produce what the theme language can say. New kinds of
  effect go into the engine as C++ primitives (T2), never as designer-only
  data.
- Geometry and hit regions stay the engine's. The designer offers the
  engine's shape parameters, not free-form reshaping (§12.3).
- The design language's limits apply to what the designer helps produce:
  contrast gate, focus visibility, no colour-only state, reduced motion
  honoured. The designer should check them live.
- Dogfood: the designer runs on the default theme and must not be
  hand-styled.

## Gate

The default theme's button cues are authored in the designer and propagated;
the saved theme passes T3 conformance and T4's reference images. Record under
`results/`.

## Open decisions (ask, do not decide)

1. **ADR-013 and the designer's panels**, as for D.

## Read first

- [T2](../T2-engine-core/README.md), [T3](../T3-theme-language/README.md),
  [T4](../T4-default-theme/README.md), [D](../D-interface-builder/README.md)
  briefs
- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §8.2, §9.6, §1.2
- Plan Paint's editor sources (`editor.cpp`, `color_palette.cpp`)
