# Download

GUI.Forms ships three demonstration programs so that you can see the toolkit
before you build it. **No application release exists yet.** The programs are
built from the repository today, and the release pipeline that packages them
is in progress. When it lands, each release on the project's GitHub Releases
page carries one archive per program per platform plus a `SHA256SUMS.txt`
file, and this page will link to the latest one.

The existing `build-<sha>` entries on the Releases page are compiler-cache
publications used by CI and by downstream projects. They are not application
downloads; their notes say so.

## The three programs

GUI.Forms Gallery
: One page per control, grouped by function, with a properties panel that
  mutates the live instance, plus a "Labs" group for rendering and layout
  experiments (vertical-sync surfaces, seam proximity, layered material). An
  About page shows the version, commit, renderer name and the instrumentation
  snapshot. The Gallery is also the toolkit's own test subject: its automation
  verb drives it by stable control ID in CI.

GUI.Forms Explore (preview)
: A file-explorer interface over deterministic in-memory fixtures, built only
  through public GUI.Forms seams. It never reads the real filesystem. It
  exists to exercise panels, trees, details views, breadcrumbs, menus and
  dialogs in one composed window. The preview label is literal: its own
  status file lists which interaction gates are complete and which are not,
  and a download does not claim native accessibility publication is finished.

GUI.Forms Correspondence
: A mail-client mockup over in-memory fixtures: folder tree, message list,
  reading pane, menus, command bar, tooltips, context menus and dialogs. It
  demonstrates the reusable panel controls (`MasterDetailView`,
  `SplitContainer`, `CorrespondenceView`, `TreeView`) composed into a
  three-pane layout. It is a demonstration, not a mail client.

All three programs are built from `demo/` and `file_manager_demoboard/` using
only the public headers, so anything they do is available to your
application.

## Platform notes

Read these before running a download; they apply to every release until
stated otherwise.

macOS
: Apple silicon (arm64) only, macOS 14.0 or newer. Builds are **ad hoc
  signed** (`codesign --sign -`) and **not notarized**. Gatekeeper will refuse
  the first launch; open the program from the Finder context menu with
  "Open", or remove the quarantine attribute with
  `xattr -dr com.apple.quarantine "GUI.Forms Gallery.app"`. There is no
  Developer ID certificate behind these builds.

Windows
: Windows 10 or newer, x64. Builds are **unsigned**. SmartScreen will warn
  that the publisher is unknown; choose "More info" and then "Run anyway" if
  you decide to trust the download. Verify the archive against
  `SHA256SUMS.txt` first. The `fonts/` directory must stay beside the
  executable.

Linux
: Ubuntu 24.04 (x64 and arm64) are the tested targets. The programs need an
  **X11** display; Wayland-only sessions must run them through XWayland. The
  `.deb` and `.tar.gz` archives depend on `libx11`, `libxcursor`,
  `libxfixes` and `libatk-bridge2.0`. No Flatpak, Snap or AppImage is planned
  for the first releases.

## Verify a download

Each release lists the SHA-256 of every archive in `SHA256SUMS.txt`. Check it
before extracting:

```sh
shasum -a 256 -c SHA256SUMS.txt --ignore-missing   # macOS, Linux
certutil -hashfile "GUI.Forms Gallery-windows-x64.zip" SHA256   # Windows
```

## Build it yourself instead

Until releases exist, [Build from source](build-from-source) produces the
Gallery in a few minutes on a machine with the LLVM 22 toolchain, and
`-DGUI_FORMS_BUILD_FILE_MANAGER_DEMOBOARD=ON` adds Explore.
