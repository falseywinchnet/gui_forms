# Native verification of GUI.Forms on a Mac and a Lima Linux VM

You are being asked to run a fixed set of build steps, tests and probes on
two machines and report the results. You do not change any code. This
document is complete: everything you need is here or in the scripts it
names.

## The project

GUI.Forms is a C++20 desktop GUI library: retained controls (buttons, text
boxes, lists, docking panels and so on) laid out and painted by the library
itself on the CPU, with a native host adapter per operating system: AppKit
on macOS, Win32 on Windows and X11 on Linux. The portable core never sees a
platform type; each host translates native windows, input and window
manager behaviour into the same portable events. It is built only with a
pinned LLVM 22.1.x toolchain.

Its owner works remotely and has no local Windows or Linux desktop, only an
Apple silicon Mac. Some host behaviours can only be confirmed on a real
desktop with a real window manager: how floating windows follow their main
window into full screen, minimize and hide with it, how a window manager
stacks them, how a key and an input method reach the library, and how
smoothly a window follows a drag. The repository contains scripts and probe
programs that check each of these without anyone watching and write a JSON
result. Your job is to run them on the owner's M4 Mac and in a Lima Linux
VM on it, and report what they wrote.

## Rules

- Install nothing beyond the packages listed below. If something else
  seems to be needed, stop and report it.
- Do not sign in to anything (no Apple ID, GitHub, package registry or
  other account). The repository is public and cloned over HTTPS.
- Do not change system settings. The one exception: if, and only if, the
  owner approves, grant the Accessibility permission to the terminal
  application you run the scripts from (System Settings, Privacy &
  Security, Accessibility). Without it two macOS checks are skipped and the
  drag probe uses its in-process mode; both are valid results.
- Do not edit, commit or push anything in the repository. If a build or a
  check fails, do not try to fix it: record it and report it.
- While a script runs on the Mac, do not use the mouse or keyboard: the
  probes move the real cursor, press the button on their own windows and
  post keys to them. They touch no other application's windows.
- Run everything from a fresh clone, as below, and keep the console output.

## Mac setup

Needs macOS 14 or later on Apple silicon, the Xcode command-line tools and
Homebrew. If either is missing, stop and report; do not install them
without the owner's approval.

```sh
xcode-select -p                      # prints a path when the tools are installed
brew --version
brew install cmake ninja ccache llvm@22 lima
```

Those five packages are the only ones to install on the Mac (`lima` is for
the Linux VM below).

## Run on the Mac

```sh
cd ~
git clone https://github.com/falseywinchnet/gui_forms.git gui-forms-verification-source
cd gui-forms-verification-source
git checkout main                    # or the branch or commit the owner names
sh tools/native_verification/run_macos.sh ~/gui-forms-verification
```

The first run builds the pinned LLVM runtimes and Skia as well as the
library, so it is long (not yet timed on an M4; expect most of an hour);
later runs reuse them. The script prints where it writes:
`~/gui-forms-verification/results-macos-<time>/` and a matching `.tar.gz`.

If the owner approves the Accessibility permission, grant it to your
terminal application, quit and reopen the terminal, and run the script
again; report both runs.

## Lima setup

Lima runs an Ubuntu 24.04 VM (arm64 on the M4). No desktop environment is
needed: the Linux script starts its own virtual displays and window
managers (Xvfb with Openbox, Mutter and KWin) and two headless Wayland
compositors with XWayland, so every run is unattended and repeatable.

```sh
limactl start --name=gui-forms --cpus=6 --memory=12 --disk=60 template://ubuntu-24.04
limactl shell gui-forms
```

Inside the VM (these are the only packages to install there):

```sh
sudo apt-get update
sudo apt-get install -y ca-certificates curl git python3
curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /usr/share/keyrings/apt-llvm.asc > /dev/null
echo 'deb [signed-by=/usr/share/keyrings/apt-llvm.asc] https://apt.llvm.org/noble/ llvm-toolchain-noble-22 main' \
    | sudo tee /etc/apt/sources.list.d/llvm-22.list
sudo apt-get update
sudo apt-get install -y cmake ninja-build ccache clang-22 generate-ninja pkg-config \
    libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev libatk1.0-dev libatk-bridge2.0-dev \
    xvfb xauth xdg-utils libxtst-dev openbox mutter kwin-x11 kwin-wayland xwayland
```

## Run in the VM

Clone inside the VM's own home directory (Lima mounts the Mac's home
read-only):

```sh
cd ~
git clone https://github.com/falseywinchnet/gui_forms.git gui-forms-verification-source
cd gui-forms-verification-source
git checkout main                    # the same commit as on the Mac
export PATH=/usr/lib/llvm-22/bin:$PATH
sh tools/native_verification/run_linux.sh ~/gui-forms-verification
```

The first run builds Skia as well (not yet timed in a VM; expect more than
an hour). Copy the archive to the Mac when it finishes, from a Mac
terminal:

```sh
limactl copy gui-forms:'~/gui-forms-verification/results-linux-*.tar.gz' ~/
```

(If `limactl copy` rejects the wildcard, run `ls ~/gui-forms-verification`
in the VM and copy the file by name.)

## What each check verifies, and what passes

Every result is a JSON file with `result` (`pass`, `fail`, `skipped`,
`inconclusive`) or, for the drag probe, `verdict` (`keeps_up`,
`falls_behind`, `not_measured`), a `reason` and the raw observations.

The test suite (both machines): the repository's whole ctest suite, the
native host tests included. Pass: every test passes (`ctest.txt`,
`ctest-junit.xml`).

The forensic frame-drag probe (both machines; on Linux once per session):
three synthetic drags of a floating window (slow, medium, fast). It records
every input, frame request and frame the host reported, and computes
latencies, missed refreshes and position error. `keeps_up` passes;
`falls_behind` is a valid measurement, not a broken run; `not_measured`
means the probe could not run (its `error` says why). The `input_path`
field says whether input went through the OS or in-process.

macOS (`behaviour-<check>.json`):

| Check | Passes when |
|---|---|
| `fullscreen_auxiliary` | with the main window in full screen, both floating windows (a tool and a document) are visible on the full-screen space |
| `child_order_out` | hiding the main window hides both floating windows, and showing it brings them back |
| `child_miniaturize` | miniaturizing the main window takes both floating windows off screen, and restoring it brings them back |
| `tool_hides_on_deactivate` | while the application is inactive the floating tool is hidden and the floating document stays visible; the tool returns on reactivation |
| `context_menu_key` | virtual key 0x6E (a PC keyboard's context-menu key) reaches the focused control as the context-menu key (needs Accessibility; skipped without it) |
| `ime_escape` | with an input method composing, Escape clears the composition, commits nothing and is not delivered as a key (needs Accessibility and an input method that is already enabled; skipped otherwise, since enabling one is a settings change) |

Linux, per session (`<session>/behaviour-<check>.json`):

| Check | Passes when |
|---|---|
| `utility_type` | both floating windows have `_NET_WM_WINDOW_TYPE_UTILITY` and the main window does not |
| `skip_taskbar` | both floating windows carry `_NET_WM_STATE_SKIP_TASKBAR`, also after one is hidden and shown again |
| `transient_for` | both floating windows name the main window in `WM_TRANSIENT_FOR` |
| `restack_above_owner_only` | with the main window active the floating windows are above it; with another application's window active they are below that window and still above their owner |
| `minimized_state` | minimizing the main window unmaps both floating windows, and restoring it maps them again |
| `net_wm_state` | full screen appears and disappears in the main window's `_NET_WM_STATE`, and the tool keeps skip-taskbar meanwhile |
| `xwayland_position` | under XWayland, the window lands within 2 units of two requested positions; skipped in the Xvfb sessions |

Sessions: `xvfb-openbox`, `xvfb-mutter`, `xvfb-kwin`, `xwayland-mutter`,
`xwayland-kwin`. A session that cannot start writes `session.json` with
`skipped` and a reason; that is a result too, report it. `inconclusive`
means the check could not establish its precondition (for example, a window
manager that does not minimize); report its reason.

## What to capture

- Both archives: `results-macos-<time>.tar.gz` (one per Mac run) and
  `results-linux-<time>.tar.gz`. They hold every JSON result, `RESULTS.md`,
  `environment.txt`, the test output and `run.log`.
- The exact commands you ran, including anything that failed, with its
  console output.
- Anything a script printed that this document does not explain.

## How to report

Write one Markdown file, `NATIVE_VERIFICATION_REPORT_<yyyy-mm-dd>.md`, and
give it to the owner with the archives attached. In it:

1. The commit checked (`environment.txt` prints it) and both machines'
   environment lines.
2. Whether the Accessibility permission was granted, and for which runs.
3. The `RESULTS.md` tables from each archive, unchanged.
4. Every `fail`, `inconclusive`, `not_measured` and skipped session, each
   with its reason from the JSON.
5. For the drag probe, each run's verdict and its p95 request-to-frame
   latency.
6. Anything that went wrong and was not a check result (a build failure, a
   script error, a hang), with the commands and output.

Do not interpret or round the numbers; copy them. Say plainly what you did
not run and why.
