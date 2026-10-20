# Default theme rendering reference

Date: 2026-10-12. The exact constructions the owner approved for the
default theme, written for the engine (T2) and theme (T4) implementers.
`planning/DESIGN_LANGUAGE_SPECIFICATION.md` §3.1, §3.5, §11 and §12.4–§12.5
are authoritative; this file restates them with the working numbers of the
approved studies. Plan Paint (`falseywinchnet/plan-paint` at `6b901fc`) is the
source of truth for every Plan Paint value: read the source, sample the
screenshot pixels, never approximate.

## 1. Push buttons: Plan Paint's action plate ("slight pill")

Plan Paint has no raised buttons. Nothing is lifted by a Windows-style bevel.
A push button, command tile, chip or actuator is Plan Paint's
`action_material` (`src/forms/ribbon.cpp`):

| Element | Value |
|---|---|
| face | vertical gradient `#D2E4FB` → `#A3C1EC` |
| border | 1 px `#6384B2` (hot `#355E96`) |
| inner top and left line | white (studies: 90 % white) |
| inner bottom and right line | `#315686` at about 53 % |
| contact shadow | 1 px below, `#294B77` at about 33 % |
| corner radius | 3 logical px (gold plate 2) |
| ink | `#153765` |
| hover | brighter face, darker border; does not move |
| pressed | darker face; inner lines inverted (dark top/left, light bottom/right); contact shadow removed |
| active tool | gold `#FFEDBA` → `#FFCF66`, border `#C49332`, radius 2, flat |
| default button | a second dark perimeter, never colour, glow or pulse (M105) |
| disabled | contrast reduced, faint border, paler face; still visible (M104) |

## 2. Ribbon buttons: the cloth bulge

The owner's words: "a slight 3D pill effect on the ribbon buttons", "a faint
pill/bubble". Then the analogy that fixed it: "pretend I have my finger stuck
into a sheet of cloth from behind it." The bulge is the ribbon plane itself
swelling. **It has no edge of any kind: no border, line or shadow. "If there's
an edge, then it fails automatically."**

Field, in the button's box of width w and height h, with points measured
from the top-left:

```text
field(x, y; cx, cy, rx, ry):
    dx = (x − cx·w) / (rx·w)
    dy = (y − cy·h) / (ry·h)
    t  = sqrt(dx² + dy²)
    return 0                 if t ≥ 1
    return 1 − t^q           otherwise, q = 2.2

light = field(x, y; 0.42, 0.38, 0.48, 0.46)   lit slope, toward the upper-left light
shade = field(x, y; 0.60, 0.64, 0.50, 0.48)   shaded slope, away from the light
crown = field(x, y; 0.50, 0.50, 0.50, 0.50)   faint crown
```

- **Why `1 − t^q`.** Each component holds near its peak across the bulge and
  falls off over a narrow band at the rim, then melts into the plane before
  the part's bounds. The owner asked for a "tighter exponent drop-off" and
  then rejected two looser versions. Tighter means a larger q. A curve
  concentrated at the centre, such as `(1 − t)^p`, reads looser and is wrong.
- **Direction.** "Deepest at the lower right corner, lightening towards the
  other three." The bulge is not a top-to-bottom taper.
- **Smooth form** (accepted as an alternative to flat, §12.5): composite
  white at peak 0.55 × light, blue-grey `rgb(55, 90, 150)` at peak
  0.22 × shade, and white at 0.12 × crown over the ribbon plane. The studies
  sample each radial at 0, 20, 40, 55, 70, 80, 90 and 100 % of t.
- **Pressed** is a dent: swap the centres (light at 0.60, 0.64; shade at
  0.40, 0.36) and use shade colour for the near slope.
- **Active tool** is the same bulge in gold: light `rgb(255, 250, 225)` at
  0.70, shade `rgb(190, 130, 25)` at 0.30, crown `rgb(255, 214, 110)` at
  0.45.
- **Hover is the rise** (owner, T2 interview G4-1, 2026-10-19: "when a
  mouse isnt hovering over, the button goes away- the bulge flattens").
  - A ribbon button is flat at rest.
  - When it is hot, its amplitude rises from 0 to 1 by the exponential
    approach (T2 D3-1), and falls back when it is no longer hot.
  - There is no brightness or saturation lift. The studies' brightness
    1.03–1.04 and saturation 1.15 are superseded.

## 3. Ribbon buttons: the dithered bulge

The owner then asked to "take the pill effect and subsample it as a lattice of
dots", and rejected the first try: "this dither reads as just two colours".
The accepted form ("this is quite the style"):

```text
lattice:   one 1-logical-px dot at every odd (x, y), i.e. every 2 px, both axes
threshold: Bayer 4×4 = [0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5]
           threshold(x, y) = (bayer[((y >> 1) & 3)·4 + ((x >> 1) & 3)] + 0.5) / 16
tone:      a·(light·lit_gain + crown·crown_gain − shade·shade_gain), amplitude a in [0, 1] (§2 hover)
quantize(v): s = clamp(v, 0, 0.999)·3; step = floor(s); if s − step > threshold: step += 1
dot colour:
    tone ≥ 0: step = quantize(tone);  step > 0 → lit[step − 1],    else no dot
    tone < 0: step = quantize(−tone); step > 0 → shaded[step − 1], else no dot
```

Between dots, and wherever the tone rounds to zero, the ribbon plane shows.
No edge. Pressed uses the dent centres of §2.

**Rescaled quantization** (owner, T2 interview G1-3, 2026-10-19). The light
default's gains never reach the third shaded step, so each side of the tone
is rescaled to the part's own range:

```text
tone₁(x, y) = the tone at amplitude 1
P = max(tone₁) over the part's dot positions;  N = max(−tone₁) over the same
tone ≥ 0: step = quantize(a·tone₁ / P)
tone < 0: step = quantize(a·(−tone₁) / N)
a side whose extreme is 0 draws no dots
```

- P and N are taken at full amplitude, not the current one, so the rise
  still grows the dither. Dividing by the current extreme would show the
  whole pattern at any non-zero amplitude.
- P and N depend only on the part's size, the gains, the pressed state and
  the lattice, so the engine computes them once per part geometry.
- With this rule the gains set the balance between the fields, no longer the
  absolute tone.

| Palette | Light default | Dark variant |
|---|---|---|
| lit (3 steps) | `rgb(236,244,254)`, `rgb(246,250,255)`, `rgb(255,255,255)` | `rgb(52,66,56)`, `rgb(78,98,82)`, `rgb(118,150,108)` |
| shaded (3 steps) | `rgb(176,198,232)`, `rgb(132,162,212)`, `rgb(88,122,186)` | `rgb(14,18,15)`, `rgb(8,10,9)`, `rgb(0,0,0)` |
| gains lit / shade / crown | 1.0 / 0.85 / 0.25 | 0.9 / 1.0 / 0.2 |
| gold lit | `rgb(255,234,176)`, `rgb(255,244,212)`, `rgb(255,252,240)` | `rgb(150,90,120)`, `rgb(190,120,150)`, `rgb(225,160,190)` |
| gold body | `rgb(250,205,104)`, `rgb(255,220,140)`, `rgb(255,232,178)` | `rgb(110,58,84)`, `rgb(140,72,104)`, `rgb(170,92,128)` |
| gold shaded | `rgb(232,172,62)`, `rgb(206,142,40)`, `rgb(170,112,24)` | `rgb(70,30,50)`, `rgb(52,20,36)`, `rgb(34,10,22)` |

Gold (active tool): for tone ≥ 0, a lit step uses `gold lit`; otherwise the
crown quantized alone picks `gold body` (no dot when it rounds to zero). For
tone < 0, a shaded step uses `gold shaded`; otherwise `gold body[0]`.

**Fractional display scales** (owner, T2 interview G3-3, 2026-10-19). A
dithered part is drawn at its nearest integer scale:

```text
k = max(1, floor(s + 0.5))
lattice:  period 2k device px from the part's device origin, both axes; dot n sits at device (2n + 1)·k
dot:      k × k device px
sample:   the fields at logical (2n + 1)·k / s across the part's logical box
threshold: indexed by the lattice index n (equal to x >> 1 at integer scales)
```

- At integer scales this is the studies' rule exactly: dots at odd logical
  positions, every 2 logical px.
- At fractional scales the lattice stays perfectly even on screen. Its
  logical period becomes 2k/s: 1.6 logical px at 1.25× and 2.67 at 1.5×.

Flat, smooth bulge and dithered bulge are three engine renderings of one part
role. **The default ships the dithered bulge** (owner decision, 2026-10-13;
design language §12.6).

## 4. Tabs: Plan Paint's tab stack, exactly

Not a curve, highlight or bubble on the selected tab. That misreading upset
the owner. Copy `make_tab_theme` in Plan Paint's `src/forms/ribbon.cpp`:

| Element | Value |
|---|---|
| rail | flat `#93ADD0` (Plan Paint's rail strip; the spec's §11 item 4 table gives the rail as `#9BB5D8`, 27 px; take both from the source) |
| tab face | `#9BB6DA` at rest, `#ACC6E7` hot, `#E8F2FF` selected |
| face wash | a top wash fading out by 72 % of the tab height (studies: `rgba(34,70,116,.15)` at rest, `rgba(255,255,255,.57)` selected) |
| top line | white (studies: 49 % at rest, 100 % selected) |
| right keylines | double: `#D1E0F5` inside `#496C9F` |
| cast | every tab casts a hard 2 px `#496C9F` edge and a soft shadow offset (3, 1) blur 2 at about 22 % to its right |
| stacking | tabs abut; each tab overlaps the next, so earlier tabs sit above later ones |
| selected | slightly larger (studies: +1 px font, +3 px height), bold ink `#173F79`, runs into the ribbon plane with no bottom edge |
| corner radius | 4 logical px, top corners only |
| File tab | solid blue gradient `#528BE0` → `#173F88`, white text; carries identity with the rail and command plane |

## 5. Command plane and stock

- Command plane: deepens from near-white to royale blue toward its foot (the
  approved Pearl Ribbon study moved toward Plan Paint's royale blue). Plan
  Paint's plane is `#E8F2FF`, `#CADEFB`, `#A5C4F1`, `#769FDF` with a white top
  line and a shade line beneath.
- Group dividers: thin etched pairs fading at both ends, the group label
  beneath in `#485B70` (Plan Paint `paint_separator`).
- Side planes, captions and dialog stock: near-white `#F4F4F5`. The owner's
  word was "ivory", meaning a near-white in the manner of Windows' `#F0F0F0`,
  never a cream. Warm stock beside blue chrome is rejected.
- No fresco identity band in the default (§12.4).
- Recess is the only deep depth: entry wells (inner top shade, outer bottom
  highlight, thin border; Plan Paint gallery well `#F8FBFF` → `#FFFFFF`,
  border `#94ACC8`) and the work pane (canvas recessed in a mid-grey felt
  surround, sheet shadow (1.5, 2.5) blur 6 at about 29 %). Side panels are
  flat, never raised.
- Status strip: flat and pale, with small soft chips; no sunken cells.
- Icons: high-colour material icons in every theme, including the
  Windows-familiar modern flat one; monochrome glyph sets are "too trivial"
  and "lack character" (C152).

## 6. Dark variant

Derived by rule from the light theme, whole windows, following the host's
dark preference and overridable by the user. Reference: Plan Paint's
"Phosphor" pair, black `#000000` with soft green `#82B361`, complement
`#B36182` for the File tab and the active tool (`src/color_palette.cpp`). The
light stays upper left and inset stays inset. The studies' dark tokens are in
`theme_studies_source.html` under `.t-dark`; they are sketches, not tokens.

## 7. The studies file

`theme_studies_source.html` is the source of the published studies
(https://claude.ai/artifact/VLga27SkyE1rUCPPGdJmPb) with the embedded fonts
removed (the `/*FONTS*/` placeholder). Open it in a browser to see the
default, the dark variant, and the modern flat Windows-familiar comparison.
Its CSS values are browser sketches of shape, light and palette, not engine
pixels and not tokens. Tokens descend from the role model (spec §5.4) and are
revisited after a native specimen sheet (§12.3).
