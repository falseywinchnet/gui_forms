# Stage F, F4: the font substitution gate (2026-10-23)

This is the record of stage F's gate. Every substitution in the final table,
`assets/fonts/font-substitution.json`, is measured against its Windows
reference face. The measurement is by our own engine on both sides (decision
9), under its class's tolerance (decision 8) and the named exceptions
(decisions 16 and 17). The measurement runs on every CI host, not only
Windows (owner decision F4-1, stage F decision 18).

## How the gate runs on hosts without the reference faces

The Microsoft reference faces cannot be committed or downloaded. Each one was
measured once, on Windows, into a **metric snapshot** in
`tools/font_metrics_differential/reference_snapshots/`. A snapshot holds
measurements only, never font bytes:

- each code point's advance in font units;
- the line metrics at each size;
- the corpus line widths and line breaks at each size;
- the reference file's name, version, byte count and SHA-256.

The ctest `gui_forms_font_metrics_gate_tests` (area `render`) runs on every
host. For each snapshot, it:

1. checks that the snapshot was measured with this corpus;
2. resolves the snapshot's request through the substitution table, which must
   answer `substitution` with the face the snapshot names, in the requested
   style;
3. measures that bundled candidate with the bundled FreeType and HarfBuzz;
4. requires the candidate's measurement digest (the SHA-256 of its
   measurement text) to equal the digest recorded with the snapshot. Every
   host must therefore measure the candidate exactly as Windows did;
5. compares candidate and snapshot under the class's criteria. Each failing
   criterion must have a named exception in the table and stay within its
   limit. A named exception whose criterion passes fails the gate as stale;
6. on Windows only, measures the installed reference again when its SHA-256
   matches the snapshot. That measurement must reproduce the snapshot
   exactly.

The test also fails when a substitution has no snapshot, or when a named
exception has no snapshot to exercise it.

Because of step 4, a pass on any host means that host measured every
candidate exactly as recorded. The numbers in the results table therefore
hold on every host that passes.

## Tolerances (decision 8)

| Class | Gated criteria | Recorded only |
|---|---|---|
| `metric_compatible` | every advance identical in font units (319 code points); every paragraph line-break set identical (15 paragraph sets × 6 sizes = 90); line height within 1 px at every size | mean line width |
| `nearest` | mean corpus line width within ±5% of the reference at every size | advances, line breaks, line height |

- Sizes: 11, 12, 13, 16, 22 and 24 px.
- Wrap widths: 20, 35 and 60 em.
- Corpus: `tools/font_metrics_differential/corpus/forms_latin_v1.txt`, with
  content digest `52838e1be0e7f570ff7feb550dcd5c761faaa20efe58259e8cf20349c647c04c`.
  This is the SHA-256 of its parsed entries joined with LF, so a checkout's
  line endings do not change it.

## Every mapping's class

| Request | Bundled family | Kind | Class | Measured as |
|---|---|---|---|---|
| Calibri | Carlito | substitution | metric_compatible | Calibri |
| Arial | Arimo | substitution | metric_compatible | Arial |
| Times New Roman | Tinos | substitution | metric_compatible | Times New Roman |
| Courier New | Cousine | substitution | metric_compatible | Courier New |
| Microsoft Sans Serif | Arimo | substitution | nearest | Microsoft Sans Serif |
| Tahoma | Selawik | substitution | nearest | Tahoma |
| Segoe UI | Selawik | substitution | nearest | Segoe UI |
| Segoe UI Semibold | Selawik Semibold | substitution | nearest | Segoe UI Semibold |
| Consolas | Portsmouth Mono | substitution | nearest | Consolas |
| Helvetica | Arimo | alias | metric_compatible | Arial |
| Times | Tinos | alias | metric_compatible | Times New Roman |
| Courier | Cousine | alias | metric_compatible | Courier New |
| MS Sans Serif | Arimo | alias | nearest | Microsoft Sans Serif |
| MS Shell Dlg | Arimo | alias | nearest | Microsoft Sans Serif |
| MS Shell Dlg 2 | Selawik | alias | nearest | Tahoma |
| Segoe UI Light | Selawik | alias | nearest | Segoe UI |
| Segoe UI Semilight | Selawik | alias | nearest | Segoe UI |
| sans-serif | Carlito | generic | none | not measured |
| serif | Tinos | generic | none | not measured |
| monospace | Cousine | generic | none | not measured |

An alias shares its target's measurement and class. A generic name is not a
claim about any face, so it has no class.

## Named exceptions (decisions 16 and 17)

| Request, style | Criterion | Limit | Measured | Note |
|---|---|---|---|---|
| Calibri bold italic → Carlito Bold Italic | line_breaks | 2 sets | 2 of 90 | kerning |
| Arial bold → Arimo Bold | advances | 1 code point | 1 of 319 (U+00B5, 1229 against 1180 units) | |
| Arial bold italic → Arimo Bold Italic | line_breaks | 4 sets | 4 of 90 | |
| Tahoma bold → Selawik Bold | line_width | 0.071 | 0.0705 (Selawik Bold's lines 7.05% shorter at the worst size; the bound is 5%) | |

The limits live in the table, next to the substitution they qualify. The
generator rejects an unknown style or criterion, a limit that is not a
nonnegative number, and a style named twice for one criterion. The gate
rejects an exception that is no longer needed.

## Results

Each row is one snapshot. These are the values every host reproduces: each
criterion's largest difference over the six sizes. The unit is code points,
line-break sets, pixels, or the fraction of the reference's mean line width.
Values marked "recorded" are not gated for that class.

| Request | Style | Reference file | Candidate | Class | Advances differing | Line-break sets differing | Line height, largest difference (px) | Mean line width, largest deviation | Result |
|---|---|---|---|---|---|---|---|---|---|
| Arial | bold | arialbd.ttf | Arimo-Bold.ttf | metric_compatible | **1** (named exception) | 0 | 0 | 0.10% (recorded) | passed |
| Arial | bold_italic | arialbi.ttf | Arimo-BoldItalic.ttf | metric_compatible | 0 | **4** (named exception) | 0 | 0.09% (recorded) | passed |
| Arial | italic | ariali.ttf | Arimo-Italic.ttf | metric_compatible | 0 | 0 | 0 | 0.00% (recorded) | passed |
| Arial | regular | arial.ttf | Arimo-Regular.ttf | metric_compatible | 0 | 0 | 0 | 0.00% (recorded) | passed |
| Calibri | bold | calibrib.ttf | Carlito-Bold.ttf | metric_compatible | 0 | 0 | 1 | 0.01% (recorded) | passed |
| Calibri | bold_italic | calibriz.ttf | Carlito-BoldItalic.ttf | metric_compatible | 0 | **2** (named exception) | 1 | 0.00% (recorded) | passed |
| Calibri | italic | calibrii.ttf | Carlito-Italic.ttf | metric_compatible | 0 | 0 | 1 | 0.00% (recorded) | passed |
| Calibri | regular | calibri.ttf | Carlito-Regular.ttf | metric_compatible | 0 | 0 | 1 | 0.00% (recorded) | passed |
| Consolas | regular | consola.ttf | PortsmouthMono.ttf | nearest | 313 (recorded) | 78 (recorded) | 3 (recorded) | 4.74% | passed |
| Courier New | bold | courbd.ttf | Cousine-Bold.ttf | metric_compatible | 0 | 0 | 0 | 0.00% (recorded) | passed |
| Courier New | bold_italic | courbi.ttf | Cousine-BoldItalic.ttf | metric_compatible | 0 | 0 | 0 | 0.00% (recorded) | passed |
| Courier New | italic | couri.ttf | Cousine-Italic.ttf | metric_compatible | 0 | 0 | 0 | 0.00% (recorded) | passed |
| Courier New | regular | cour.ttf | Cousine-Regular.ttf | metric_compatible | 0 | 0 | 0 | 0.00% (recorded) | passed |
| Microsoft Sans Serif | regular | micross.ttf | Arimo-Regular.ttf | nearest | 35 (recorded) | 0 (recorded) | 1 (recorded) | 0.28% | passed |
| Segoe UI | bold | segoeuib.ttf | Selawik-Bold.ttf | nearest | 2 (recorded) | 7 (recorded) | 3 (recorded) | 0.34% | passed |
| Segoe UI | regular | segoeui.ttf | Selawik-Regular.ttf | nearest | 1 (recorded) | 0 (recorded) | 3 (recorded) | 0.39% | passed |
| Segoe UI Semibold | regular | seguisb.ttf | Selawik-Semibold.ttf | nearest | 288 (recorded) | 30 (recorded) | 3 (recorded) | 2.32% | passed |
| Tahoma | bold | tahomabd.ttf | Selawik-Bold.ttf | nearest | 291 (recorded) | 90 (recorded) | 1 (recorded) | **7.05%** (named exception) | passed |
| Tahoma | regular | tahoma.ttf | Selawik-Regular.ttf | nearest | 291 (recorded) | 19 (recorded) | 1 (recorded) | 1.01% | passed |
| Times New Roman | bold | timesbd.ttf | Tinos-Bold.ttf | metric_compatible | 0 | 0 | 0 | 0.00% (recorded) | passed |
| Times New Roman | bold_italic | timesbi.ttf | Tinos-BoldItalic.ttf | metric_compatible | 0 | 0 | 0 | 0.00% (recorded) | passed |
| Times New Roman | italic | timesi.ttf | Tinos-Italic.ttf | metric_compatible | 0 | 0 | 0 | 0.00% (recorded) | passed |
| Times New Roman | regular | times.ttf | Tinos-Regular.ttf | metric_compatible | 0 | 0 | 0 | 0.00% (recorded) | passed |

These are not gated: styles a candidate family lacks resolve to its nearest
face (decision 7). Those are Segoe UI and Tahoma italic, Segoe UI Semibold
bold and italic, Consolas bold and italic, and Microsoft Sans Serif's styles,
for which Windows has only a regular face. The table reports such a
resolution as `face_style` differing from `requested_style`.

## Reference faces

Measured in place from `C:\Windows\Fonts`. The faces were never copied, and
none of their data is in the repository (`assets/fonts/ATTRIBUTION.md`,
"Metric snapshots").

| Reference file | Version | Bytes | SHA-256 |
|---|---|---|---|
| arial.ttf | Version 7.06 | 1045720 | `b3658eadae55e682b5f69eb64c439c1ecc8f196c0bb8d4756d145d13bc86476a` |
| arialbd.ttf | Version 7.06 | 989780 | `e8f4e3baf6cc35fed6fcce3a540e8b39e8f6cda1d22a28f2ec8f526fef7a43f5` |
| arialbi.ttf | Version 7.06 | 724872 | `267a411525980d648f10a7222a6ff1ebe4d4feeefbf9b3100cae2b5cfab5594a` |
| ariali.ttf | Version 7.06 | 721584 | `86b32db9a06f9694e2a3760c42e5117bcdc5cc1255bb5186ca8ce0305e22f288` |
| calibri.ttf | Version 6.27 | 1650632 | `57e93ff143572bcc092c1f56202d43ddc07bb119aa104c6d7af163470fb551f6` |
| calibrib.ttf | Version 6.27 | 1615616 | `0a8296747536ead27d5f48f67e911cc3b207742b3f704e66995251d12fd422e5` |
| calibrii.ttf | Version 6.27 | 1202512 | `7f150e9eabe5db3e1b0f9469dca5c96176bc2fbb306dc9d48b7029dc69c52763` |
| calibriz.ttf | Version 6.27 | 1163904 | `760539bfedbe24e7853ca8a621d9cb1f3de52036c116e81aae36b33c4ab60a7c` |
| consola.ttf | Version 7.01 | 453088 | `cf00b507b3286870cc5064ebd0633c303f70b491a4af25eec2d32df413db0179` |
| cour.ttf | Version 6.95 | 817164 | `e4a66a0259718d0c8f4b6e14319b4803ced45afbddf3abba8b8c3e2006856206` |
| courbd.ttf | Version 6.95 | 818292 | `11e629cef937cd08f942aa1bd994a71302dbe2819c72c6d4c183a623acbb421a` |
| courbi.ttf | Version 6.95 | 593192 | `2d61f452de49b57d9ef14ce8896bc1cd2ed8285c2e9fb2e73e78a4b23f91d308` |
| couri.ttf | Version 6.95 | 682968 | `d82eeeb458f661d867f916d691fc6d3c53f8735fe4d5b04b54a5757321ee03e0` |
| micross.ttf | Version 7.04 | 857472 | `89b42a12ea0379133fb2f4a1d1bd53058fb61e2343c1d509452d5761acc85b7a` |
| segoeui.ttf | Version 5.72 | 960076 | `704eada12961a27fd271f7d3b09e2066520e758390c1776744eff430fee30f6e` |
| segoeuib.ttf | Version 5.72 | 947364 | `b169e636719880976a97e94e3b1cb3dbe5f4a38eabc037bf9f2a9165154c3ad2` |
| seguisb.ttf | Version 5.72 | 974760 | `6c3fd894b19950b10c34bf64069ef821eaf2f58f959146306fee5b367472e501` |
| tahoma.ttf | Version 7.05 | 919260 | `9af03d4ad44a3b413d92f7de48b94aa7cc8a1471a75d498406eae837f62ee1d1` |
| tahomabd.ttf | Version 7.05 | 849284 | `b16ed1a15e0cabea8d41d9f8899c253cd61ac466af5e3980d02bd0cf311c2267` |
| times.ttf | Version 7.12 | 1190736 | `931c5de5c70401d9324d5014c123802b4fb753000360ceb2f56c589403cd58c5` |
| timesbd.ttf | Version 7.12 | 1170096 | `54fbe2c70af7c85a97bed0573227e3ccc4b2486012e3ca2a40c6bc77065846f5` |
| timesbi.ttf | Version 7.12 | 904204 | `3a29d114cb5229e8dbda5bef6c69be4a210a13b7277de4d66e9fc86963226f6c` |
| timesi.ttf | Version 7.12 | 962864 | `e7f7a88b65188328aea58670f955022e23c712f55679524729da1e4e03c49d88` |

## Licences of the bundled candidates

| Family | Files | Licence | Licence file |
|---|---|---|---|
| Carlito | Regular, Bold, Italic, Bold Italic | SIL OFL 1.1 | `assets/fonts/OFL-Carlito.txt` |
| Arimo 1.33 | Regular, Bold, Italic, Bold Italic | SIL OFL 1.1 | `assets/fonts/OFL-Arimo.txt` |
| Tinos 1.340 | Regular, Bold, Italic, Bold Italic | SIL OFL 1.1 | `assets/fonts/OFL-Tinos.txt` |
| Cousine | Regular, Bold, Italic, Bold Italic | SIL OFL 1.1 | `assets/fonts/OFL-Cousine.txt` |
| Selawik 1.01 | Regular, Semibold, Bold | SIL OFL 1.1 (Reserved Font Name "Selawik") | `assets/fonts/OFL-Selawik.txt` |
| Portsmouth Mono | Regular | MIT (the repository owner's own face) | `assets/fonts/LICENSE-PortsmouthRapids.txt` |

Sources, byte counts and hashes of each file are in
`assets/fonts/ATTRIBUTION.md`. The raster module ships the faces, the
licences, the table and the attribution in its `fonts` directory.

## Environment and commands

The snapshots were measured on Windows 11 Home 10.0.26200 with clang 22.1.8
(MSYS2 CLANG64, `cmake/llvm22.cmake`), FreeType 2.14.2 and HarfBuzz 14.6.0
(both pinned), in a Release build. One command per row:

```text
gui_forms_font_metrics_differential --reference C:/Windows/Fonts/<reference>
    --candidate assets/fonts/<candidate>
    --corpus tools/font_metrics_differential/corpus/forms_latin_v1.txt
    --request-family "<request>" --request-style <style>
    --snapshot tools/font_metrics_differential/reference_snapshots/<request>-<style>.txt
```

The tool reads each snapshot back before writing it, and requires the read to
reproduce the measurement.

On the same machine, the gate passed: 23 snapshots, 4 named exceptions held,
and 23 installed references measured again, reproducing their snapshots
exactly. Its output is `gate-windows-local.txt`. `gui_forms_font_metrics_gate_tests`,
`gui_forms_font_metrics_differential_tests` and
`gui_drawing_font_substitution_tests` also passed under AddressSanitizer and
UndefinedBehaviorSanitizer.

## Cross-host record

The `native` job of `.github/workflows/native.yml` runs the gate on macOS 15
(arm64), Ubuntu 24.04 (x64 and arm64) and Windows Server 2022 (x64). The
stage gate is green when all four pass on the commit that adds this record.
0.2 may then be declared.
