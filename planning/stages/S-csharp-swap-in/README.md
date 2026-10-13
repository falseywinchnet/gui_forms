# Stage S: C# sibling — Windows Forms swap-in

Release: 0.6 Bindings. Depends on: R and C (and through them T4 and F).
Status 2026-10-12: not started. A sibling thread dogfoods it only after R, C
and the binding layer exist. Authority: `planning/FORWARD_PROGRAMME.md` §5 S.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12: "this entails setting up a remote instance on our
shadow desktop with Visual Studio where there is tons of room and building a
native C# demonstration program. It also entails building an intermediary
layer between C# and our library. It also entails exporting it as a
framework akin to System.Forms … that can be imported as an exact swap-in for
WinForms. Behaviour will differ slightly, and it's not on us to do things the
way WinForms does them — only to be able to LOOK the same way."

On behaviour: "my stance on not copying behaviour is things like — we have
our own event loop internally that we use, we've made principled choices."
Interview GF009–GF011 says the same: broad familiarity, not strict
compatibility. Incorrect historical ordering is not preserved merely because
Windows Forms exposed it.

On scope: only Windows Forms types and members supported on .NET 8.0 or later
are in scope (owner, 2026-10-13: "no dotnet 2.0 items"; that "also
eliminates .NET 4.5"). Stage W builds the missing controls. The Windows Forms
catalogue counts about 9,850 members GUI.Forms
lacks. The owner: "I do want to try to expand on the number of controls in
Forms, but 9850 does include some massive amount of deprecated redundancy
probably from the .NET 2.0 era." Coverage is driven by real applications,
not the whole catalogue.

The selling point is Windows Forms applications that run on macOS and Linux
by swapping one package reference.

## Prior art and its lesson

- Mono's `System.Windows.Forms` was a cross-platform reimplementation with
  its own X11 driver and Classic and XP-style themes. It died chasing exact
  behaviour across a vast API. Looking the same without copying behaviour,
  driven by a measured corpus, is how S avoids that trap.
- Modern.Forms is Forms-shaped on SkiaSharp but not a drop-in replacement.

## What exists today

- The generated facade (`generated/facade-v1/`) and its tools (see R): it
  already presents `System.Windows.Forms`, `System.Windows.Forms.Primitives`
  and `System.Drawing.Common` over the C ABI for SDRSharp.
- The capture tool (`tools/compat_capture/`) measures exactly which Forms
  types and members a program calls, without executing it.
- `planning/WINFORMS_API_CATALOGUE.md`, `WINFORMS_BEHAVIOR_GAP_CATALOGUE.md`,
  `CONTROL_COMPLETENESS_MATRIX.md`, and the public page
  `docs/user/for-windows-forms-developers.md`.
- ADR-001: no general HWND or message emulation; `WndProc` overrides and raw
  message dispatch are excluded; ActiveX, browser hosting, printing, MDI and
  obsolete families are excluded or packaged separately.

## What to deliver

1. Generated C# bindings over the installed C ABI (stage C), using
   `SafeHandle`-style deterministic wrappers, P/Invoke, explicit pinning or
   copying rules, callback trampolines and thread dispatch (master plan
   §4.10).
2. A `System.Windows.Forms`-shaped package. A project replaces
   `<UseWindowsForms>` with a package reference, targets plain `net10.0`, and
   runs on macOS, Linux and Windows. (A binary swap also works because .NET 5
   and later do not verify strong-name signatures; the loader lab showed the
   mechanism. The source swap is the product.)
3. A corpus of real applications: SDRSharp first, then open-source Windows
   Forms applications you propose and the owner approves, measured with the
   capture tool. Coverage is what they use.
4. A native C# demonstration program built on Shadow Windows in Visual
   Studio.
5. Generated `InitializeComponent` from form documents (FD), so C# users
   still see the familiar file while the document stays authoritative.

## How

- GUI.Forms itself never depends on .NET (`AGENTS.md`; interview GF014–GF016).
  The C# layer is a consumer on the installed SDK, in its own repository.
- Look the same: visual differentials against the same application on real
  Windows Forms on Shadow Windows, using the Windows-familiar themes of T4 and
  the fonts of F.
- Behave principled: GUI.Forms' event loop, ordering (ADR-003) and lifecycle
  (ADR-014). Document each deviation that a corpus application notices.
- Keep the public site's description general ("C# bindings with a Windows
  Forms-shaped API"); the SDRSharp harness stays off it.

## Gate

Each corpus application builds by swapping the package reference and runs on
all three platforms; a visual differential against the same application on
real Windows Forms is within tolerance. Record per application and platform
under `results/`.

## Owner decisions, 2026-10-13

1. **The C# layer lives in its own repository**, consuming the installed
   SDK; the native build and CI never see .NET.
2. **Corpus.** Propose a few open-source Windows Forms applications to join
   SDRSharp, measured with the capture tool, and get the owner's approval of
   the list.

Still open, minor: package identity and naming on NuGet.

## Read first

- [R](../R-sdrsharp-reflection/README.md) and [C](../C-c-abi/README.md) briefs
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF009–GF011, GF014–GF017
- `decisions/ADR-001-*`, `ADR-002-*`, `ADR-003-observable-ordering.md`,
  `ADR-014-lifecycle-contract.md`
- `planning/MASTER_IMPLEMENTATION_PLAN.md` §4.10, M11
- `docs/user/for-windows-forms-developers.md`
