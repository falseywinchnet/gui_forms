# First GUI.Forms source review

Generated on 2026-10-26 from GUI.Forms main revision `0067c679` with:

```sh
python3 -B code_integrity_checker/check.py include src \
  --json code_integrity_checker/reports/gui_forms-2026-10-26.json
```

The report covers 963 first-party C/C++ files and contains 44 individual
review candidates: 21 narrow size conversions (I05), 14 raw extent products
at storage sinks (I06), and 9 empty-buffer pointer endpoints (I04). Every
record identifies the exact source line, house-style section and corrective
review question. The `house_style_status` field says the required invariant is
not evident to this local source check; some instances may satisfy a wider
contract, which requires inspection before editing.

As a calibration check, the checker found the former count-before-construction
pattern in the preserved `SkTBlockList.h` reference and the one-byte-late
poison boundary in the preserved `SkBlockAllocator.h` reference. It reported
neither pattern in the corresponding corrected candidate headers. The checker
does not claim that those are its only possible findings or that the current
GUI.Forms report proves the absence of defects.

When using this report to guide an implementation pass, read the finding's
source and its caller contract. Prefer making the invariant clear in the code:
validate shape before storage work, establish bounds in Release, and avoid
forming addresses from potentially empty storage. A human review remains
necessary for candidates whose bounds are established outside the local
function. Rerun the tool after changes; this file is a snapshot, not a live
baseline or a list of accepted exceptions.
