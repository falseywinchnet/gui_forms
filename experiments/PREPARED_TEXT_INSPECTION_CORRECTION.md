# Prepared-text visual inspection correction

Date: 2026-10-07.

**GIVEN:** investigate the unhandled `draw_prepared_text` switch warning.
**OBSERVED:** enabling the development prepared-text profile adds that private
display operation, but visual inspection did not map it to a public operation.
Inspecting a committed prepared-text chunk threw `logic_error`; the separate
display replay/raster path already handled the operation. Default builds without
prepared text did not contain the missing enumerator. Existing prepared-display
tests exercised recording, replay and retirement but did not call inspection.

**MEASURED locally:** a regression fixture that paints and then inspects a real
prepared-text control failed with `GUI.Forms display operation has no inspection
spelling` before the fix and passes afterward. Prepared text now has an explicit
inspection/JSON spelling and reads font, byte count and opt-in text from the
retained immutable input. Default redaction remains intact. Inspection does not
reshape through the ordinary provider and can diagnose a retained command even
after paint authority is revoked. No private geometry or backend object becomes
public. The new enum member is confined to the existing development profile.

The mapping source compiles with `-Werror=switch` on Clang/GNU so another omitted
operation fails the build. The prepared-display regression is now run in the
Windows development lane as well as macOS/Linux. Source review against
`planning/PROGRAMMING_HOUSE_STYLE.md` covers the added mapping, retained-input
borrow/copy, public spelling, test control, regression assertions and CMake/CI
edits. Checked explicit types, named painting behavior, owner lifetimes,
redaction before copying, and read-only inspection without new worker or renderer
authority. No violations were found in this changed scope; legacy code outside
it is not claimed compliant.
