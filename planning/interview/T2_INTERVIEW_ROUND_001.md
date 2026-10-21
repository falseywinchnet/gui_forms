# T2 engine core interview, round 001

Date: 2026-10-19

Status: **answered.** All 26 questions.

The questions come from the T2 study, prepared by worker 2 on 2026-10-18
(`D:\gui_forms_workers\T2_STUDY.md`, section 5). They were extended by:
- G4, the study's taste panel TT6;
- three vocabulary gaps the T1 recheck handed to T2 (H1 to H3, from
  `results/2026-10-19-t1-recheck/README.md`);
- Z1, which accepts the phase plan.

The owner answered on a local HTML page, `interview_round_001_source.html`
beside the T2 brief. It holds the source with the embedded fonts removed; the
`/*FONTS*/` placeholder marks where they went. Each question explained its
options and showed sketches drawn from the rendering reference's formulas.
The answers were saved to `t2-interview-answers.json` and are transcribed
below. Each answer names the option number. This record is not rewritten:
later answers and corrections are added as dated notes.

## A. Scope and order

- **A1. Which phase comes first?** 1: kernels and the composite (E1, E2),
  then motion (E3).
- **A2. What the identity gate covers.** 2: each primitive's raster bytes,
  plus the composite into the window through each host's painter (the
  ADR-004 precedent).
- **A3. The paused PRs.** 1: #159 (plan) and #161 (E1 code) stay drafts and
  are revised to these answers.

## B. Atoms (§12.7)

- **B1. Engine inputs.** 1: the brief's set:
  - `pointer_x` and `pointer_y`, published only while the part is hot;
  - `time_in_state`, `display_scale` and `text_scale`;
  - published application values (named, numeric, read-only).
- **B2. Pointer units.** 1: part-local logical units. (The owner first chose
  2, normalized, then changed to 1.)
- **B3. Ceremonial surface.** 1: an application-set flag on a surface lifts
  the 10 % ambient cap for that surface only.
- **B4. Density.** 2: left until stage L or T4 needs it.

## C. Opacity and the docking tiles

- **C1. Where part opacity comes from.** 1: an engine presentational
  property driven by transitions. The control passes only its states and
  part keys, and no new atom is added.
- **C2. The tile fade before T4.** 1: yes, drawn by the built-in rules once
  transitions exist (E7).

## D. Motion

- **D1. Motion a control computes itself.** 1: motion values the control
  owns, on one clock per window. They replace the stamp's and the flyout's
  own timers.
- **D2. Reduced motion.** 1: §9.5 governs:
  - transitions jump to their end;
  - ambient effects stop;
  - the activity object holds a static pose beside its stage text.

  `MotionPolicy::reduced` moves to this in T4.
- **D3. The rise easing.** 1: exponential approach, a fixed fraction of the
  remaining gap per about 75 ms.
- **D4. Occluded or minimized windows.** 1: every transition jumps to its
  end and nothing is scheduled.
- **D5. Presentational scale.** 1: allowed as a capability only:
  - drawn about the part's centre, inside its granted overflow;
  - the hit region and layout are unchanged;
  - the default theme does not use it.

## E. Overlay shadow and content colour

- **E1. The overlay shadow.** 1: an engine primitive with a first-party
  blur, identical on every host (E9). Today's values, outsets and
  high-contrast rule are kept.
- **E2. Content colour.** 1: drawn exactly as given; a theme styles only
  what surrounds it.

## F. Budgets

- **F1. Per-part cost.** 1: at most 512 declared operations per device pixel
  for a part's stack, and 4 million per frame for an animated part. Beyond
  that, the part falls back to the built-in recipe.
- **F2. Cached part rasters.** 1: at most 8 MB per window, evicting the least
  recently painted parts first.

## G. Rendering reference findings

- **G1. The third shaded dither step.** **3: rescale quantization to the
  field's actual tone range** (not the recommendation). Every step is used
  whatever the gains.
- **G2. The shade field at the edge.** 1: keep the formula as the approved
  studies drew it, and correct §3.5's wording to match.
- **G3. Dither at fractional display scales.** **3: draw dithered parts at
  the nearest integer scale** (not the recommendation).
- **G4. Hover on the smooth bulge.** **1: no hover change** (not the
  recommendation). The owner's note, verbatim:

  > when a mouse isnt hovering over, the button goes away- the bulge
  > flattens.

  Hover is therefore the rise itself. A ribbon button is flat at rest, and
  its bulge rises when hot (D3); there is no brightness or saturation lift.
  This agrees with the ribbon record (RB README: "at rest the dithering sinks
  into the flat command plane, and under the pointer it rises to form the
  button").

## H. Vocabulary gaps handed to T2

- **H1. An image part role.** 1: admitted. A picture that is content (the
  docking stamp's snapshot, future thumbnails) is drawn exactly as given;
  themes never tint or resize it.
- **H2. Grip dots across a box.** 1: a grip primitive fills its part's box
  with the dot pattern, as one part.
- **H3. Part identity across frames.** 1: the control's stable ID, the
  declared part, and the item's stable ID. Items without one fall back to
  their index, and that fallback is reported.

## Z. The plan

- **Z1.** 1: phases E1 to E11 are accepted as the T2 plan, revised by these
  answers.

## Consequences recorded elsewhere

- **The T2 brief** lists these answers as GIVEN decisions.
- **The rendering reference** (`T4-default-theme/RENDERING_REFERENCE.md`)
  takes G1, G3 and G4:
  - **Quantization:** the rescaled rule is stated with its normalization.
    Each side is normalized by the part's full-amplitude extreme, so that
    the rise (D3) still grows the dither instead of popping it.
  - **Lattice:** the integer-scale rule is stated.
  - **Hover:** it is the rise, not a lift.
- **The design language** records the round as §12.12 and corrects §3.5's
  "melt back into the plane before the part's bounds" for the shade field
  (G2).
- **The E1 code (#161)** changes for G1 and G3, and gains the bulge
  amplitude; its reference images are regenerated on every CI platform.

## Note, 2026-10-19: what disabled means for content

Building E2a (#190) raised whether a disabled `image` part or a disabled
content colour may dim, as the built-in recipe did at 45 %. E2-1 and H1-1
already said no. The owner, verbatim:

> disabled - usually- is a change in font color or control highlighting.
> greyed out, you could say.

- **Disabled greys the chrome:** label and caption text, the edge, the
  well, and the hot and selected plates.
- **Content stays exact:** an `image` part and content colour are drawn
  exactly as given.
- **Icons are chrome**, so they keep their disabled treatment.

#190 removed the 45 % from images and content colour and tests both halves
(`gui_forms_windows_content_pixels_tests`). The design language §8.5 says so.
