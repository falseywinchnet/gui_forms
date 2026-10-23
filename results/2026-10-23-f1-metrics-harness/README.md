# Stage F, F1: first run of the metrics differential (2026-10-23)

This is the first run of `tools/font_metrics_differential` against the real
Windows reference fonts, over the faces bundled today. It is evidence for the
plan, not the stage gate. The gate is recorded in F4, once the approved faces
(Arimo, Tinos, Selawik) are added and the line-height question below is
decided.

**Environment.** Windows 11 Home 10.0.26200 (Shadow Windows) with its own
`C:\Windows\Fonts`; clang 22.1.8 (MSYS2 CLANG64, `cmake/llvm22.cmake`),
Release. The tool was built from the commit that adds this record (branch
`claude/f-metrics-harness`), with corpus `forms_latin_v1.txt`.

**Command**, one per row (`W` is `C:/Windows/Fonts`, `F` is `assets/fonts`):

```text
gui_forms_font_metrics_differential --reference W/<reference> --candidate F/<candidate>
    --class <class> --corpus tools/font_metrics_differential/corpus/forms_latin_v1.txt
    --out <run>.json --gdi
```

The `preview-*` rows compare two Microsoft fonts, Arial and Segoe UI, as
stand-ins for Arimo and Selawik, which are metric-compatible with them and
are not downloaded yet. They preview owner decisions 4 and 5. They are not
mappings.

## Results

The JSON report of each row is in this directory. Each report records the
reference file's name, version, size and SHA-256, never its bytes.

| Run | Class | Advances identical (missing in candidate) | Mean width deviation over sizes | Line breaks identical | Line height, engine vs GDI reference, worst (px) | Line height, GDI vs GDI, worst (px) | Gate as accepted |
|---|---|---|---|---|---|---|---|
| calibri-carlito-regular | metric_compatible | 318/318 (1) | 0.00% | 90/90 | 2 | 0 | not passed (line height) |
| calibri-carlito-bold | metric_compatible | 318/318 (1) | -0.01% | 90/90 | 2 | 0 | not passed (line height) |
| calibri-carlito-italic | metric_compatible | 318/318 (1) | 0.00% | 90/90 | 2 | 0 | not passed (line height) |
| calibri-carlito-bolditalic | metric_compatible | 318/318 (1) | 0.00% | 88/90 | 2 | 0 | not passed (line breaks, line height) |
| couriernew-cousine-regular | metric_compatible | 319/319 | 0.00% | 90/90 | 2 | 2 | not passed (line height) |
| couriernew-cousine-bold | metric_compatible | 319/319 | 0.00% | 90/90 | 2 | 2 | not passed (line height) |
| couriernew-cousine-italic | metric_compatible | 319/319 | 0.00% | 90/90 | 2 | 3 | not passed (line height) |
| couriernew-cousine-bolditalic | metric_compatible | 319/319 | 0.00% | 90/90 | 2 | 2 | not passed (line height) |
| consolas-portsmouthmono-regular | nearest | 0/313 (6) | +4.50% to +4.74% | 12/90 | 3 | 5 | passed |
| tahoma-carlito-regular | nearest | 0/318 (1) | -8.84% to -8.79% | 0/90 | 2 | 1 | not passed (line width) |
| preview-tahoma-arial-regular | nearest | 0/319 | +1.44% to +1.55% | 79/90 | 2 | 2 | passed |
| preview-tahoma-segoeui-regular | nearest | 0/319 | -1.39% to -1.35% | 71/90 | 4 | 3 | passed |
| preview-microsoftsansserif-arial-regular | nearest | 284/319 | -0.28% to -0.25% | 90/90 | 1 | 2 | passed |

In every run, `GetFontData` confirmed that GDI measured the exact bytes of
both files.

## Findings

1. **Widths are exact for the metric-compatible faces.**
   - Carlito matches Calibri, and Cousine matches Courier New, on every
     compared advance in all four styles.
   - String widths and line breaks are identical, with one exception: bold
     italic Carlito breaks 2 of 90 paragraph sets differently from Calibri.
     The advances there are identical and the mean width differs by less
     than 0.01%, so the difference is in kerning.
2. **The accepted line-height criterion fails for reasons that are not the
   face's.**
   - The engine's line box rounds FreeType's `hhea` ascender up and its
     descender down: Carlito at 12 px gives 12 + 4 = 16 px.
   - GDI rounds the win metrics: Calibri and Carlito both give 11 + 3 = 14 px.
   - So the candidate's engine height sits 1 to 2 px above GDI's reference
     height for every face. Compared GDI to GDI, Carlito equals Calibri at
     every size (0 px).
   - Cousine differs from Courier New by up to 2 px (3 px in italic),
     largest at the small sizes. Courier New's own hinting raises its
     small-size heights: 14 px at 11 px, against 12 px for Cousine with the
     same `hhea`.
   - This is a decision for the coordinator and the owner. The tool records
     both comparisons.
3. **The engine's widths sit up to 8.7% from GDI's.** The engine shapes with
   unhinted advances (ADR-004), while GDI uses hinted integer advances. For
   Calibri at 12 px, the engine is 8.7% narrower than GDI on the corpus mean.
   This is a property of the engine, not of any face. It matters to the
   stages that compare against Windows Forms (R, S).
4. **Microsoft Sans Serif is a near twin of Arial.** The mean width deviation
   is -0.27%, and the line breaks are identical on all 90 sets. This
   supports decision 4 (Microsoft Sans Serif maps to Arimo).
5. **Tahoma is roughly equidistant from Arial and Segoe UI.**
   - Arial runs +1.5% and Segoe UI -1.4%. Arial matches 79 of 90 line-break
     sets and Segoe UI 71.
   - Both pass the 5% bound. Decision 5 picks between Arimo and Selawik by
     measuring them once they are downloaded.
   - Carlito is not a Tahoma substitute (-8.8%).
6. **Portsmouth Mono is wider and taller than Consolas.**
   - Every advance is 4.5% to 4.7% wider, inside the 5% bound but close to it.
   - Its line height is 2 to 3 px taller in the engine.
   - It lacks 6 of the compared code points, which fall back to another face.
