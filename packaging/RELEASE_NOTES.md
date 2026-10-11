<!-- Appended verbatim after the generated header by tools/publish_tested_release.py.
     Keep "Breaking changes" separate from "Other changes"; group "Other changes" by
     control or area; attach one screenshot for anything visual. Rewrite this file for
     each release series; the generated header carries version, commit and platforms. -->

## Focus

First downloadable builds of the GUI.Forms demonstration programs: the control
Gallery, and the fixture-backed Explore preview. These are proving-slice
programs, not products; they exist so the toolkit can be inspected without a
compiler.

## Breaking changes

- None. This is the first published series.

## Other changes

### Gallery
- Packaged as a signed-ad-hoc macOS bundle and a portable Windows build with
  an optional per-user installer.

### Explore (preview)
- The `file_manager_demoboard` ships as "GUI.Forms Explore (preview)"; its
  STATUS.md state (D0-D6 measured partial, D7/D8/D10 not started) applies.

### Release pipeline
- Builds, tests, packages and smoke-launches each program on macOS, Windows
  and Linux runners before anything is uploaded.

## Known limits

- macOS: the bundle is ad hoc signed (`codesign --sign -`), not Developer ID
  signed or notarized. Gatekeeper asks for confirmation on first launch;
  right-click, Open, or allow it in System Settings, Privacy & Security.
- Windows: the executable and installer are unsigned; SmartScreen shows
  "Windows protected your PC" until the publisher reputation exists. Choose
  More info, Run anyway.
- Linux: X11 is required (XWayland works); the DEB declares X11, Xcursor and
  ATK dependencies. Linux program mains are still being added, so a release
  may carry fewer Linux programs than macOS or Windows ones.
- Fonts ship with each program; no host fonts are read.
