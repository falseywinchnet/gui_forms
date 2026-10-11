#!/usr/bin/env python3
"""Extract (and optionally install) each packaged program, launch it, and verify its close.

Reads packages-<platform>.json written by tools/package_programs.py. For every
packaged program the archive is extracted to a scratch directory and the
executable is launched with the program's automation flag and extra smoke
arguments, with GUI_FORMS_FONT_DIR unset so the bundled fonts are the ones
exercised. With --install the installer is also applied on this machine and
the installed copy is launched the same way (macOS `installer -pkg`, Windows
`setup.exe /S` then the quiet uninstaller, Linux `apt-get install ./x.deb`).

Smoke modes (packaging/programs.json → smoke.mode; "{scratch}" in
smoke.extra_arguments expands to a per-launch temporary directory):
  survive  the program must still be running after smoke.seconds, then it is
           asked to quit: macOS `osascript ... quit`, Windows the automation
           probe's `close` when gui_forms_windows_probe.exe is available (else
           taskkill without /F, which posts WM_CLOSE), Linux `xdotool windowquit`
           when xdotool sees the display. Every host prints a
           {"window":...,"host":...} receipt on stdout at close; when the quit
           was graceful the exit code must be 0 and, if smoke.expect_receipt,
           that line must be present. If no graceful channel exists the process
           is terminated by signal after surviving and the result is recorded
           with graceful=false (a liveness check, not a clean-close check).
  exit     the program exits by itself (a future scheduled-close verb); exit 0
           within the timeout with the receipt.

Results are written to smoke-<platform>.json beside the packages receipt and
the exit status is non-zero when any launch fails.
"""
from __future__ import annotations

import argparse
import json
import os
import platform as host_platform
import shutil
import subprocess
import sys
import tarfile
import tempfile
import time
import zipfile
from dataclasses import asdict, dataclass, field
from pathlib import Path
from typing import Any

ROOT: Path = Path(__file__).resolve().parents[1]
PLATFORMS: tuple[str, ...] = ("macos-arm64", "windows-x64", "linux-x64", "linux-arm64")
PROBE_NAME: str = "gui_forms_windows_probe.exe"


@dataclass
class LaunchResult:
    program: str
    source: str
    executable: str
    arguments: list[str]
    passed: bool
    graceful: bool
    exit_code: int | None
    receipt_found: bool
    renderer_name: str | None
    survived_seconds: float
    detail: str
    stdout_tail: list[str] = field(default_factory=list)
    stderr_tail: list[str] = field(default_factory=list)


def detect_platform() -> str:
    system = host_platform.system()
    arm = host_platform.machine().lower() in ("arm64", "aarch64")
    if system == "Darwin":
        return "macos-arm64"
    if system == "Windows":
        return "windows-x64"
    return "linux-arm64" if arm else "linux-x64"


def clean_environment(platform: str) -> dict[str, str]:
    environment = os.environ.copy()
    for variable in ("GUI_FORMS_FONT_DIR", "LD_LIBRARY_PATH", "DYLD_LIBRARY_PATH", "DYLD_FALLBACK_LIBRARY_PATH"):
        environment.pop(variable, None)
    if platform.startswith("windows"):
        environment["PATH"] = environment.get("SystemRoot", "C:\\Windows") + "\\System32"
    return environment


def extract_archive(archive: Path, destination: Path, platform: str) -> None:
    destination.mkdir(parents=True, exist_ok=True)
    if archive.name.endswith(".tar.gz"):
        with tarfile.open(archive, "r:gz") as tar:
            tar.extractall(destination, filter="data")
        return
    if platform.startswith("macos") and shutil.which("ditto"):
        # ditto preserves the bundle's extended attributes and code signature.
        subprocess.run(["ditto", "-x", "-k", str(archive), str(destination)], check=True)
        return
    with zipfile.ZipFile(archive) as compressed:
        compressed.extractall(destination)
        if not platform.startswith("windows"):
            for member in compressed.infolist():
                mode = (member.external_attr >> 16) & 0o777
                if mode:
                    (destination / member.filename).chmod(mode)


def receipt_from_output(lines: list[str]) -> dict[str, Any] | None:
    for line in reversed(lines):
        line = line.strip()
        if line.startswith("{") and '"window"' in line:
            try:
                return json.loads(line)
            except json.JSONDecodeError:
                continue
    return None


class Smoke:
    def __init__(self, platform: str, build_dir: Path | None, timeout: float, dry_run: bool, log=print) -> None:
        self.platform = platform
        self.build_dir = build_dir
        self.timeout = timeout
        self.dry_run = dry_run
        self.log = log

    def probe(self) -> Path | None:
        if self.build_dir is None:
            return None
        for candidate in (self.build_dir / PROBE_NAME, *sorted(self.build_dir.rglob(PROBE_NAME))):
            if candidate.is_file():
                return candidate
        return None

    def request_quit(self, process: subprocess.Popen[str], record: dict[str, Any],
                     bundle_path: Path | None = None) -> tuple[bool, str]:
        """Ask the program to close through a host-visible channel; return (graceful, how)."""
        if self.platform.startswith("macos"):
            # Only a bundle LaunchServices already sees as running may be told to quit:
            # `tell application ... to quit` launches a second instance otherwise.
            identifier = record.get("bundle_identifier")
            running = False
            if identifier and shutil.which("lsappinfo"):
                found = subprocess.run(["lsappinfo", "find", f"bundleid={identifier}"], capture_output=True, text=True, timeout=30)
                running = found.returncode == 0 and found.stdout.strip() != ""
            if identifier and running and shutil.which("osascript"):
                result = subprocess.run(["osascript", "-e", f'tell application id "{identifier}" to quit'],
                                        capture_output=True, text=True, timeout=30)
                if result.returncode == 0:
                    return True, "osascript quit by bundle id"
                self.log("    osascript quit failed: " + result.stderr.strip())
            elif identifier:
                self.log(f"    {identifier} ({bundle_path}) is not registered with LaunchServices; no AppleScript quit")
        elif self.platform.startswith("windows"):
            probe = self.probe()
            if probe is not None and record.get("automation_flag"):
                result = subprocess.run([str(probe), "close"], capture_output=True, text=True, timeout=30)
                if result.returncode == 0:
                    return True, "automation probe close"
                self.log("    probe close failed: " + result.stderr.strip())
            result = subprocess.run(["taskkill", "/PID", str(process.pid)], capture_output=True, text=True, timeout=30)
            if result.returncode == 0:
                return True, "taskkill WM_CLOSE"
        else:
            xdotool = shutil.which("xdotool")
            if xdotool and os.environ.get("DISPLAY"):
                found = subprocess.run([xdotool, "search", "--pid", str(process.pid)], capture_output=True, text=True, timeout=30)
                windows = [line for line in found.stdout.split() if line.strip()]
                if windows:
                    for window in windows:
                        subprocess.run([xdotool, "windowquit", window], capture_output=True, text=True, timeout=30)
                    return True, "xdotool windowquit"
        process.terminate()
        return False, "terminated by signal (no graceful close channel on this runner)"

    def launch(self, record: dict[str, Any], executable: Path, source: str) -> LaunchResult:
        smoke = record["smoke"]
        arguments: list[str] = []
        if record.get("automation_flag"):
            arguments.append(record["automation_flag"])
        # "{scratch}" in an extra argument names a per-launch temporary directory,
        # so a self-exiting program can be asked for --capture <png> there.
        scratch_root = tempfile.mkdtemp(prefix="gui-forms-smoke-args-")
        arguments.extend(argument.replace("{scratch}", scratch_root) for argument in smoke.get("extra_arguments", []))
        command = [str(executable), *arguments]
        if self.platform.startswith("linux") and not os.environ.get("DISPLAY") and shutil.which("xvfb-run"):
            command = ["xvfb-run", "-a", "-s", "-screen 0 1280x1024x24 -noreset", *command]
        self.log(f"    launch ({source}): {' '.join(command)}")
        if self.dry_run:
            return LaunchResult(record["id"], source, str(executable), arguments, True, True, None, False, None, 0.0, "dry run")
        if not executable.is_file():
            return LaunchResult(record["id"], source, str(executable), arguments, False, False, None, False, None, 0.0,
                                "executable missing after extraction/installation")
        environment = clean_environment(self.platform)
        with tempfile.TemporaryDirectory(prefix="gui-forms-smoke-") as scratch:
            out_path, err_path = Path(scratch) / "stdout.txt", Path(scratch) / "stderr.txt"
            with out_path.open("w", encoding="utf-8") as out, err_path.open("w", encoding="utf-8") as err:
                started = time.monotonic()
                process = subprocess.Popen(command, stdout=out, stderr=err, env=environment, cwd=str(executable.parent))
                survive = float(smoke["seconds"])
                exited_early = False
                while time.monotonic() - started < survive:
                    if process.poll() is not None:
                        exited_early = True
                        break
                    time.sleep(0.2)
                survived = time.monotonic() - started
                graceful = True
                how = "exited on its own"
                if not exited_early:
                    if smoke["mode"] == "exit":
                        process.terminate()
                        graceful, how = False, "did not exit by itself within smoke.seconds"
                    else:
                        bundle_path = next((parent for parent in executable.parents if parent.suffix == ".app"), None)
                        graceful, how = self.request_quit(process, record, bundle_path)
                try:
                    exit_code: int | None = process.wait(timeout=self.timeout)
                except subprocess.TimeoutExpired:
                    process.kill()
                    exit_code = process.wait(timeout=10)
                    graceful, how = False, how + "; killed after the close timeout"
            stdout_lines = out_path.read_text(encoding="utf-8", errors="replace").splitlines()
            stderr_lines = err_path.read_text(encoding="utf-8", errors="replace").splitlines()
        receipt = receipt_from_output(stdout_lines)
        renderer = None
        presented: int | None = None
        if receipt and isinstance(receipt.get("window"), dict):
            renderer = receipt["window"].get("renderer_name")
            frames = receipt["window"].get("frames_presented")
            presented = frames if isinstance(frames, int) else None
        passed = False
        detail = how
        if exited_early:
            if exit_code == 0 and (receipt is not None or not smoke["expect_receipt"]):
                passed = True
                detail = f"exited cleanly after {survived:.1f}s"
            else:
                detail = f"exited early after {survived:.1f}s with code {exit_code}" + ("" if receipt else ", no close receipt")
        elif graceful:
            if exit_code == 0 and (receipt is not None or not smoke["expect_receipt"]):
                passed = True
            else:
                detail = f"{how}: exit code {exit_code}" + ("" if receipt else ", no close receipt")
        else:
            # Survived the liveness period; only termination by our signal is acceptable.
            passed = smoke["mode"] == "survive" and "killed" not in how
            detail = how + f" after surviving {survived:.1f}s"
        if passed and presented == 0:
            passed = False
            detail += "; the close receipt reports no presented frame"
        return LaunchResult(record["id"], source, str(executable), arguments, passed, graceful and passed, exit_code,
                            receipt is not None, renderer, round(survived, 2), detail, stdout_lines[-20:], stderr_lines[-20:])

    # -- install paths ------------------------------------------------------------

    def install_and_locate(self, record: dict[str, Any], installer: Path) -> Path:
        install = record["install"]
        if self.platform.startswith("macos"):
            self.log(f"    sudo installer -pkg {installer.name} -target /")
            if not self.dry_run:
                subprocess.run(["sudo", "/usr/sbin/installer", "-pkg", str(installer), "-target", "/"], check=True)
            return Path("/Applications") / install["macos_app"] / record["executable"]
        if self.platform.startswith("windows"):
            self.log(f"    {installer.name} /S")
            if not self.dry_run:
                subprocess.run([str(installer), "/S"], check=True, timeout=600)
            root = os.path.expandvars(install["windows_dir"].replace("$LOCALAPPDATA", "%LOCALAPPDATA%"))
            return Path(root) / record["executable"]
        self.log(f"    sudo apt-get install -y ./{installer.name}")
        if not self.dry_run:
            # apt treats a bare relative path as a package name; give it an absolute path.
            subprocess.run(["sudo", "apt-get", "install", "-y", str(installer.resolve())], check=True)
        return Path(install["linux_dir"]) / record["executable"]

    def uninstall(self, record: dict[str, Any]) -> None:
        if not self.platform.startswith("windows"):
            return
        root = os.path.expandvars(record["install"]["windows_dir"].replace("$LOCALAPPDATA", "%LOCALAPPDATA%"))
        uninstaller = Path(root) / "Uninstall.exe"
        self.log(f"    {uninstaller} /S")
        if not self.dry_run and uninstaller.is_file():
            subprocess.run([str(uninstaller), "/S", f"_?={root}"], check=False, timeout=600)
            leftovers = [path for path in Path(root).rglob("*") if path.is_file() and path.name != "Uninstall.exe"] if Path(root).exists() else []
            if leftovers:
                raise RuntimeError("uninstaller left files behind: " + ", ".join(str(path) for path in leftovers[:5]))


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--platform", choices=PLATFORMS, default=None)
    parser.add_argument("--dist", type=Path, required=True, help="directory holding the artifacts and packages-<platform>.json")
    parser.add_argument("--build-dir", type=Path, default=None, help="where gui_forms_windows_probe.exe may be found")
    parser.add_argument("--install", action="store_true", help="also apply the installer and launch the installed copy")
    parser.add_argument("--timeout", type=float, default=90.0, help="seconds to wait for the close after it is requested")
    parser.add_argument("--receipt", type=Path, default=None, help="default <dist>/smoke-<platform>.json")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args(argv)
    platform = args.platform or detect_platform()
    packages = json.loads((args.dist / f"packages-{platform}.json").read_text(encoding="utf-8"))
    if packages.get("platform") != platform:
        raise SystemExit(f"packages receipt is for {packages.get('platform')}, not {platform}")
    smoke = Smoke(platform, args.build_dir, args.timeout, args.dry_run)
    results: list[LaunchResult] = []
    print(f"GUI.Forms package smoke: platform {platform}, version {packages['version']}" + (" [dry run]" if args.dry_run else ""))
    with tempfile.TemporaryDirectory(prefix="gui-forms-packages-") as scratch:
        for record in packages["programs"]:
            if record["status"] != "packaged":
                print(f"- {record['display_name']}: skipped ({record['reason']})")
                continue
            print(f"- {record['display_name']}")
            archive = args.dist / record["artifacts"]["archive"]
            extracted = Path(scratch) / record["id"]
            print(f"    extract {archive.name} -> {extracted}")
            if not args.dry_run:
                extract_archive(archive, extracted, platform)
            folder = record["install"]["macos_app"] if platform.startswith("macos") else record["install"]["folder"]
            results.append(smoke.launch(record, extracted / folder / record["executable"], "archive"))
            print(f"    {'PASS' if results[-1].passed else 'FAIL'}: {results[-1].detail}")
            installer_name = record["artifacts"].get("installer")
            if args.install and installer_name:
                installed = smoke.install_and_locate(record, args.dist / installer_name)
                results.append(smoke.launch(record, installed, "installer"))
                print(f"    {'PASS' if results[-1].passed else 'FAIL'}: {results[-1].detail}")
                smoke.uninstall(record)
    receipt = {"schema": 1, "platform": platform, "version": packages["version"], "revision": packages.get("revision"),
               "passed": all(result.passed for result in results), "launches": [asdict(result) for result in results]}
    if not args.dry_run:
        receipt_path = args.receipt or (args.dist / f"smoke-{platform}.json")
        receipt_path.write_text(json.dumps(receipt, indent=2) + "\n", encoding="utf-8")
        print(f"wrote {receipt_path}")
    failures = [result for result in results if not result.passed]
    for failure in failures:
        print(f"FAILED {failure.program} ({failure.source}): {failure.detail}")
        for line in failure.stderr_tail:
            print("    stderr: " + line)
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
