# Stage F, F1: the metrics differential's first runs (2026-10-23)

These are the first runs of `tools/font_metrics_differential` for every
planned mapping. The candidates are the bundled faces, plus Arimo, Tinos and
Selawik, which the owner approved for download on 2026-10-23. Each pair is
measured by our own engine on both sides: the reference font's file goes
through the same pinned FreeType and HarfBuzz code as the substitute's (owner
decision 9). This is evidence for the plan, not the stage gate. The gate is
recorded in F4, once the classes and exceptions below are settled.

**Environment.**
- Windows 11 Home 10.0.26200 (Shadow Windows), reference fonts from its own
  `C:\Windows\Fonts`.
- clang 22.1.8 (MSYS2 CLANG64, `cmake/llvm22.cmake`), Release.
- The tool was built from the commit that adds this record (branch
  `claude/f-metrics-harness`), with corpus `forms_latin_v1.txt`.
- Arimo, Tinos and Selawik were read from their verified downloads (see
  `assets/fonts/ATTRIBUTION.md` once F3 lands; their hashes are listed below).

**Command**, one per row:

```text
gui_forms_font_metrics_differential --reference C:/Windows/Fonts/<reference> --candidate <candidate>
    --class <class> --corpus tools/font_metrics_differential/corpus/forms_latin_v1.txt
    --out <run>.json --gdi
```

`--gdi` adds GDI's measurements of both faces to each report. They are
information for stages R and S, which compare against Windows Forms, and are
never part of F's gate.

## The engine's line metrics

The engine takes its vertical metrics from FreeType 2.14.2, pinned and
unmodified apart from its options header. The rule:

- the OS/2 typo metrics when `USE_TYPO_METRICS` (bit 7 of `fsSelection`) is
  set;
- otherwise `hhea`;
- the typo metrics, then the win metrics, only when `hhea`'s ascender and
  descender are both zero.

At a size, FreeType rounds the ascender up, the descender down and the height
to the nearest pixel (`GRID_FIT_METRICS`). The line height is ascent plus
descent plus the remaining gap, never below zero.

Of the faces measured here, only Cousine, Arimo, Portsmouth Mono and Portsmouth
Rapids set `USE_TYPO_METRICS`. Every Microsoft reference font and every other
candidate uses `hhea`.

## Results

The JSON report of each row is in this directory. A reference file appears
only by name, version, size and SHA-256, never by its bytes.

| Run | Class | Advances identical (missing in candidate) | Mean width deviation | Line breaks identical | Line height, reference / candidate (px at 11, 12, 13, 16, 22, 24) | Ascent at 12 px, ref / cand | Missing clusters (cand, 12 px) | Gate |
|---|---|---|---|---|---|---|---|---|
| arial-arimo-bold | metric_compatible | 318/319 | -0.10% | 90/90 | 13/13, 14/14, 15/15, 19/19, 25/25, 28/28 | 11 / 11 | 0 | not passed (advances) |
| arial-arimo-bolditalic | metric_compatible | 319/319 | -0.09% | 86/90 | 13/13, 14/14, 15/15, 19/19, 25/25, 28/28 | 11 / 11 | 0 | not passed (line breaks) |
| arial-arimo-italic | metric_compatible | 319/319 | +0.00% | 90/90 | 13/13, 14/14, 15/15, 19/19, 25/25, 28/28 | 11 / 11 | 0 | passed |
| arial-arimo-regular | metric_compatible | 319/319 | +0.00% | 90/90 | 13/13, 14/14, 15/15, 19/19, 25/25, 28/28 | 11 / 11 | 0 | passed |
| calibri-carlito-bold | metric_compatible | 318/318 (1) | -0.01% | 90/90 | 13/14, 15/16, 16/17, 20/21, 27/27, 29/30 | 9 / 12 | 0 | passed |
| calibri-carlito-bolditalic | metric_compatible | 318/318 (1) | -0.00% to +0.00% | 88/90 | 13/14, 15/16, 16/17, 20/21, 27/27, 29/30 | 9 / 12 | 0 | not passed (line breaks) |
| calibri-carlito-italic | metric_compatible | 318/318 (1) | +0.00% | 90/90 | 13/14, 15/16, 16/17, 20/21, 27/27, 29/30 | 9 / 12 | 0 | passed |
| calibri-carlito-regular | metric_compatible | 318/318 (1) | +0.00% | 90/90 | 13/14, 15/16, 16/17, 20/21, 27/27, 29/30 | 9 / 12 | 0 | passed |
| consolas-portsmouthmono-regular | nearest | 0/313 (6) | +4.50% to +4.74% | 12/90 | 13/15, 14/16, 15/17, 19/21, 26/28, 28/31 | 9 / 12 | 4 | passed |
| couriernew-cousine-bold | metric_compatible | 319/319 | +0.00% | 90/90 | 14/14, 14/14, 15/15, 19/19, 26/26, 28/28 | 10 / 10 | 0 | passed |
| couriernew-cousine-bolditalic | metric_compatible | 319/319 | +0.00% | 90/90 | 14/14, 14/14, 15/15, 19/19, 26/26, 28/28 | 10 / 10 | 0 | passed |
| couriernew-cousine-italic | metric_compatible | 319/319 | +0.00% | 90/90 | 14/14, 14/14, 15/15, 19/19, 26/26, 28/28 | 10 / 10 | 0 | passed |
| couriernew-cousine-regular | metric_compatible | 319/319 | +0.00% | 90/90 | 14/14, 14/14, 15/15, 19/19, 26/26, 28/28 | 10 / 10 | 0 | passed |
| microsoftsansserif-arimo-regular | nearest | 284/319 | -0.28% to -0.25% | 90/90 | 14/13, 15/14, 15/15, 19/19, 26/25, 29/28 | 12 / 11 | 0 | passed |
| segoeui-selawik-bold | metric_compatible | 289/291 (28) | +0.34% to +0.34% | 83/90 | 15/14, 17/15, 19/16, 23/20, 30/27, 33/30 | 13 / 12 | 0 | not passed (advances, line breaks, line height) |
| segoeui-selawik-regular | metric_compatible | 290/291 (28) | +0.39% | 90/90 | 15/14, 17/15, 19/16, 23/20, 30/27, 33/30 | 13 / 12 | 0 | not passed (advances, line height) |
| segoeui-selawik-semibold | metric_compatible | 3/291 (28) | +2.29% to +2.32% | 60/90 | 15/14, 17/15, 19/16, 23/20, 30/27, 33/30 | 13 / 12 | 0 | not passed (advances, line breaks, line height) |
| tahoma-arimo-bold | nearest | 0/319 | -6.04% to -5.95% | 0/90 | 14/13, 15/14, 16/15, 21/19, 28/25, 30/28 | 12 / 11 | 0 | not passed (line width) |
| tahoma-arimo-regular | nearest | 0/319 | +1.44% to +1.55% | 79/90 | 14/13, 15/14, 16/15, 21/19, 28/25, 30/28 | 12 / 11 | 0 | passed |
| tahoma-carlito-regular | nearest | 0/318 (1) | -8.84% to -8.79% | 0/90 | 14/14, 15/16, 16/17, 21/21, 28/27, 30/30 | 12 / 12 | 0 | not passed (line width) |
| tahoma-selawik-bold | nearest | 0/291 (28) | -7.05% to -6.99% | 0/90 | 14/14, 15/15, 16/16, 21/20, 28/27, 30/30 | 12 / 12 | 0 | not passed (line width) |
| tahoma-selawik-regular | nearest | 0/291 (28) | -1.01% to -0.97% | 71/90 | 14/14, 15/15, 16/16, 21/20, 28/27, 30/30 | 12 / 12 | 0 | passed |
| timesnewroman-tinos-bold | metric_compatible | 319/319 | +0.00% | 90/90 | 13/13, 14/14, 15/15, 19/19, 25/25, 28/28 | 11 / 11 | 0 | passed |
| timesnewroman-tinos-bolditalic | metric_compatible | 319/319 | +0.00% | 90/90 | 13/13, 14/14, 15/15, 19/19, 25/25, 28/28 | 11 / 11 | 0 | passed |
| timesnewroman-tinos-italic | metric_compatible | 319/319 | +0.00% | 90/90 | 13/13, 14/14, 15/15, 19/19, 25/25, 28/28 | 11 / 11 | 0 | passed |
| timesnewroman-tinos-regular | metric_compatible | 319/319 | +0.00% | 90/90 | 13/13, 14/14, 15/15, 19/19, 25/25, 28/28 | 11 / 11 | 0 | passed |

## Findings

1. **Exact clones.**
   - Cousine matches Courier New, and Tinos matches Times New Roman, in all
     four styles: advances, line breaks and line heights.
   - Arimo matches Arial in regular and italic.
   - Their line metrics agree although the tables differ. Cousine and Arimo
     set `USE_TYPO_METRICS`, and their typo metrics equal the references'
     `hhea`.
2. **Near-exact clones** (each one is a property of the face pair):
   - Arimo Bold's µ (U+00B5) is 1229 units against Arial Bold's 1180. This is
     the only differing advance in 1,276 compared across Arimo's four styles.
   - Arimo Bold Italic breaks 4 of 90 paragraph sets differently from Arial
     Bold Italic, and Carlito Bold Italic breaks 2 of 90 differently from
     Calibri Bold Italic. Both have identical advances and mean width
     deviations under 0.1%, so the difference is in kerning.
   - Under decision 8's strict rules, these three runs do not pass.
3. **Carlito's line height is 1 px above Calibri's at five of six sizes.**
   That is within the 1 px tolerance. The cause is the face pair:
   - Calibri's `hhea` is 1536/-512/452.
   - Carlito's `hhea` is 1950/-550/0, which is Calibri's win metrics. Carlito's
     typo metrics equal Calibri's.
   - Neither face sets `USE_TYPO_METRICS`, so the engine reads `hhea` for both.
     The total heights are equal in font units (2500), but the rounding
     differs.
   - The ascent differs more: 9 px for Calibri against 12 px for Carlito at
     12 px, so Carlito's baseline sits 3 px lower in its line box. The
     ascent is recorded here and is not gated.
4. **Selawik is not metric-compatible with Segoe UI as a family.**
   - Selawik Regular matches 290 of 291 compared advances (U+013E differs),
     and its width deviation is +0.39%.
   - Selawik Semibold matches 3 of 291, at +2.3%.
   - Selawik Bold matches 289 of 291, at +0.34%.
   - All three lack 28 of the compared Latin-1 and Latin Extended-A code
     points; Selawik 1.01 has about 350 glyphs.
   - Selawik's `hhea` (2027/-431/0) is shorter than Segoe UI's (2210/-514/0),
     so every line is 1 to 3 px shorter.
   - As a `nearest` mapping it passes at every size (widths within 5%).
5. **Tahoma.**
   - Regular: Selawik is nearer (-1.0%, line heights equal at 4 of 6 sizes)
     than Arimo (+1.5%, 1 to 3 px shorter). By decision 5, Tahoma Regular
     maps to Selawik.
   - Bold: neither candidate is within 5%. Arimo Bold is -6.0% and Selawik
     Bold is -7.0%, because Tahoma Bold is unusually wide.
   - Carlito (-8.8%) is not a candidate.
6. **Microsoft Sans Serif** maps to Arimo within tolerance: -0.27%, every
   line break identical, line height 1 px shorter at 4 of 6 sizes.
7. **Portsmouth Mono against Consolas** passes as `nearest`.
   - Widths run +4.5% to +4.7%.
   - Lines are 2 to 3 px taller.
   - It lacks 6 of the compared code points, which fall back to another face.

## Engine-internal observation

This is not about the table choice. The engine has two line-height consumers:

- `HarfBuzzFontEngine::primary_line_metrics`, used by prepared text, adds the
  rounded line gap: the height less the ascent and descent.
- `ResolvedTextLayout`, used by `TextBox`, always reports a zero line gap,
  because `shape` sets its height to ascent plus descent.

The two agree whenever that gap rounds to zero, which holds for every bundled
role face (Portsmouth Rapids, Carlito, Cousine) and for Arimo, Tinos and
Selawik. They disagree for a face with a large `hhea` line gap, such as
Calibri (3 px at 12 px) or Consolas (1 px) loaded from host bytes. Nothing in
this record suggests changing which table the engine reads.

## GDI, for stages R and S

Every report carries GDI's heights and extents for both faces. In every run,
`GetFontData` confirmed that GDI measured the exact bytes of both files.

Two points for R and S:

- GDI's hinted integer advances differ from the engine's unhinted ones by up
  to 8.7%: Calibri at 12 px renders 8.7% narrower in the engine than in GDI.
- GDI rounds the win metrics, so its line heights differ from the engine's by
  up to 2 px.

## Faces measured

| File | Family | Version | Bytes | SHA-256 |
|---|---|---|---|---|
| arialbd.ttf | Arial | Version 7.06 | 989780 | `e8f4e3baf6cc35fed6fcce3a540e8b39e8f6cda1d22a28f2ec8f526fef7a43f5` |
| Arimo-Bold.ttf | Arimo | Version 1.33; ttfautohint (v1.8.4.16-eb64) | 485872 | `d7a8b187cf8444d4cfee102e8eae9e3043682fd5106d5d33ed677fe268a0e2ba` |
| arialbi.ttf | Arial | Version 7.06 | 724872 | `267a411525980d648f10a7222a6ff1ebe4d4feeefbf9b3100cae2b5cfab5594a` |
| Arimo-BoldItalic.ttf | Arimo | Version 1.33; ttfautohint (v1.8.4.16-eb64) | 508508 | `77c967d49d4ef4a63c39fbcd1e27892102e503e35a20983cb058430c4f268b34` |
| ariali.ttf | Arial | Version 7.06 | 721584 | `86b32db9a06f9694e2a3760c42e5117bcdc5cc1255bb5186ca8ce0305e22f288` |
| Arimo-Italic.ttf | Arimo | Version 1.33; ttfautohint (v1.8.4.16-eb64) | 502332 | `ffdf690e4357a392e4edcc3df0b3adb0302920f75e54f2e6eec355afbe7edcba` |
| arial.ttf | Arial | Version 7.06 | 1045720 | `b3658eadae55e682b5f69eb64c439c1ecc8f196c0bb8d4756d145d13bc86476a` |
| Arimo-Regular.ttf | Arimo | Version 1.33; ttfautohint (v1.8.4.16-eb64) | 478712 | `41b22bc8f0b51f932825d37bc55b5eb6ba67dfe599a626e4aff2b43b624f9f8c` |
| calibrib.ttf | Calibri | Version 6.27 | 1615616 | `0a8296747536ead27d5f48f67e911cc3b207742b3f704e66995251d12fd422e5` |
| Carlito-Bold.ttf | Carlito | Version 1.104 | 682468 | `bb5d20f79b82599ec72983597437373a80f2d2085fa91fc144fd74e876a594db` |
| calibriz.ttf | Calibri | Version 6.27 | 1163904 | `760539bfedbe24e7853ca8a621d9cb1f3de52036c116e81aae36b33c4ab60a7c` |
| Carlito-BoldItalic.ttf | Carlito | Version 1.104 | 808508 | `b32928186c119599e03ca6a1ffc680fdcb7fac95772f4b95d989cf6cd3861517` |
| calibrii.ttf | Calibri | Version 6.27 | 1202512 | `7f150e9eabe5db3e1b0f9469dca5c96176bc2fbb306dc9d48b7029dc69c52763` |
| Carlito-Italic.ttf | Carlito | Version 1.104 | 615236 | `0b019225e58d702bfedcbd35c21696769f8ee115cb6343f84c2f240312450d1c` |
| calibri.ttf | Calibri | Version 6.27 | 1650632 | `57e93ff143572bcc092c1f56202d43ddc07bb119aa104c6d7af163470fb551f6` |
| Carlito-Regular.ttf | Carlito | Version 1.104 | 628032 | `f6418f708baede9789daef5d458c0f53d2a888af9820e8062934e504fedc6595` |
| consola.ttf | Consolas | Version 7.01 | 453088 | `cf00b507b3286870cc5064ebd0633c303f70b491a4af25eec2d32df413db0179` |
| PortsmouthMono.ttf | Portsmouth Mono | Version 1.0 | 164552 | `6129c819de5046e4ec97c7ee8d915bf195d106b98680d9b17901741a1d7314ad` |
| courbd.ttf | Courier New | Version 6.95 | 818292 | `11e629cef937cd08f942aa1bd994a71302dbe2819c72c6d4c183a623acbb421a` |
| Cousine-Bold.ttf | Cousine | Version 1.241; ttfautohint (v1.8.4.16-eb64) | 298048 | `331215ec6445f41e98d8971251bb6237e722907f4101faae990583accfe79545` |
| courbi.ttf | Courier New | Version 6.95 | 593192 | `2d61f452de49b57d9ef14ce8896bc1cd2ed8285c2e9fb2e73e78a4b23f91d308` |
| Cousine-BoldItalic.ttf | Cousine | Version 1.241; ttfautohint (v1.8.4.16-eb64) | 308292 | `7f804549c941ceaac0a9635389ed09c4bbc86e4bfe37589191dacd1b5b0e26a5` |
| couri.ttf | Courier New | Version 6.95 | 682968 | `d82eeeb458f661d867f916d691fc6d3c53f8735fe4d5b04b54a5757321ee03e0` |
| Cousine-Italic.ttf | Cousine | Version 1.241; ttfautohint (v1.8.4.16-eb64) | 306880 | `afb869eee8643d09915c3286edb61dad66278ceb1511c79b474ce21548421bcd` |
| cour.ttf | Courier New | Version 6.95 | 817164 | `e4a66a0259718d0c8f4b6e14319b4803ced45afbddf3abba8b8c3e2006856206` |
| Cousine-Regular.ttf | Cousine | Version 1.241; ttfautohint (v1.8.4.16-eb64) | 296856 | `5a57f0184000371cb22fe3fcea4c500354cab1a69efaf7beaf3e6eca6ecfefea` |
| micross.ttf | Microsoft Sans Serif | Version 7.04 | 857472 | `89b42a12ea0379133fb2f4a1d1bd53058fb61e2343c1d509452d5761acc85b7a` |
| segoeuib.ttf | Segoe UI | Version 5.72 | 947364 | `b169e636719880976a97e94e3b1cb3dbe5f4a38eabc037bf9f2a9165154c3ad2` |
| selawkb.ttf | Selawik | Version 1.01 | 44068 | `f0db5e174a90e0956ad7d2844bdca1d5e6da92ec65b2c04e57ba9b180668c904` |
| segoeui.ttf | Segoe UI | Version 5.72 | 960076 | `704eada12961a27fd271f7d3b09e2066520e758390c1776744eff430fee30f6e` |
| selawk.ttf | Selawik | Version 1.01 | 44224 | `e9d98518d8ac2817782a9a382430463a2e0793ea68350b695bb727d9a830ee1c` |
| seguisb.ttf | Segoe UI Semibold | Version 5.72 | 974760 | `6c3fd894b19950b10c34bf64069ef821eaf2f58f959146306fee5b367472e501` |
| selawksb.ttf | Selawik Semibold | Version 1.01 | 44192 | `0a9e9d0549a10f24bef9b3a29e06fe6e0b5c21e7a784c503b048a307841a7783` |
| tahomabd.ttf | Tahoma | Version 7.05 | 849284 | `b16ed1a15e0cabea8d41d9f8899c253cd61ac466af5e3980d02bd0cf311c2267` |
| tahoma.ttf | Tahoma | Version 7.05 | 919260 | `9af03d4ad44a3b413d92f7de48b94aa7cc8a1471a75d498406eae837f62ee1d1` |
| timesbd.ttf | Times New Roman | Version 7.12 | 1170096 | `54fbe2c70af7c85a97bed0573227e3ccc4b2486012e3ca2a40c6bc77065846f5` |
| Tinos-Bold.ttf | Tinos | Version 1.340; ttfautohint (v1.8.4.16-eb64) | 597880 | `393269dbab8899f938db19783eca5eac92eb431f7ae0ab45b8349ca895f1a06b` |
| timesbi.ttf | Times New Roman | Version 7.12 | 904204 | `3a29d114cb5229e8dbda5bef6c69be4a210a13b7277de4d66e9fc86963226f6c` |
| Tinos-BoldItalic.ttf | Tinos | Version 1.340; ttfautohint (v1.8.4.16-eb64) | 578976 | `a5de79f0fe863ea0954757acb3d47b3ccd0a930ce3dd5b97230cd3866790a06e` |
| timesi.ttf | Times New Roman | Version 7.12 | 962864 | `e7f7a88b65188328aea58670f955022e23c712f55679524729da1e4e03c49d88` |
| Tinos-Italic.ttf | Tinos | Version 1.340; ttfautohint (v1.8.4.16-eb64) | 565016 | `5942266ed398b155d7dc23e36833e7ec6be988f2439bdbeb8ef1bede808eaa91` |
| times.ttf | Times New Roman | Version 7.12 | 1190736 | `931c5de5c70401d9324d5014c123802b4fb753000360ceb2f56c589403cd58c5` |
| Tinos-Regular.ttf | Tinos | Version 1.340; ttfautohint (v1.8.4.16-eb64) | 521588 | `60a0e8ef0c04dd5dd69ffe91025fa2ae5836cbd35600a82ba031977557e2cb61` |
