#!/usr/bin/env python3
"""Archive the installed GUI.Forms SDK for one platform.

Installs the configured build directory into a staging prefix named after the
archive's top directory, adds what a consumer needs beside the installed
package, checks the shape, and writes one deterministic archive into --out:

  gui-forms-sdk-<version>-<platform>.tar.gz   macOS and Linux
  gui-forms-sdk-<version>-<platform>.zip      Windows

  gui-forms-sdk-<version>-<platform>/
    include/gui_forms/            public headers
    lib/                          libraries (Windows DLLs are in bin/)
    lib/cmake/GUIForms/           GUIFormsConfig.cmake, the version file, the exported targets
    cmake/llvm22.cmake            the toolchain file every consumer passes
    share/GUIForms/fonts/         the bundled fonts, with a native host
    share/licenses/GUIForms/      LICENSE, THIRD_PARTY_NOTICES.md and the third-party licences
    llvm-runtime/                 macOS only: the LLVM 22 libc++ runtime for GUI_FORMS_LLVM_RUNTIME

tools/publish_tested_release.py expects one archive per platform under
sdk_archive_name() and lists it in SHA256SUMS.txt. --check reads an existing
archive and checks its shape without installing anything.
"""
from __future__ import annotations

import argparse
import gzip
import os
import shutil
import subprocess
import sys
import tarfile
import tempfile
import zipfile
from pathlib import Path

ROOT: Path = Path(__file__).resolve().parents[1]
TOOLCHAIN_FILE: Path = ROOT / "cmake/llvm22.cmake"
PLATFORMS: tuple[str, ...] = ("macos-arm64", "windows-x64", "linux-x64", "linux-arm64")
ZIP_PLATFORMS: tuple[str, ...] = ("windows-x64",)
MACOS_PLATFORM: str = "macos-arm64"
RUNTIME_DIRECTORY: str = "llvm-runtime"
# Files every SDK archive carries, relative to its top directory. The package
# configuration is what find_package(GUIForms CONFIG) reads; the toolchain
# file appears twice: where the package installs it and where the source
# tree keeps it, so the documented -DCMAKE_TOOLCHAIN_FILE=<root>/cmake/llvm22.cmake
# works for an archive as for a checkout.
REQUIRED_FILES: tuple[str, ...] = (
    "lib/cmake/GUIForms/GUIFormsConfig.cmake",
    "lib/cmake/GUIForms/GUIFormsConfigVersion.cmake",
    "lib/cmake/GUIForms/GUIFormsTargets.cmake",
    "lib/cmake/GUIForms/llvm22.cmake",
    "cmake/llvm22.cmake",
    "share/licenses/GUIForms/LICENSE",
    "share/licenses/GUIForms/THIRD_PARTY_NOTICES.md",
)
REQUIRED_DIRECTORY_PREFIXES: tuple[str, ...] = ("include/gui_forms/",)
# The macOS runtime prefix tools/build_macos_runtimes.py installs: three
# shared libraries, the configured libc++ headers and their licences.
MACOS_RUNTIME_FILES: tuple[str, ...] = (
    RUNTIME_DIRECTORY + "/lib/libc++.1.dylib",
    RUNTIME_DIRECTORY + "/include/c++/v1/__config_site",
    RUNTIME_DIRECTORY + "/share/licenses/llvm-runtimes/libcxx-LICENSE.TXT",
)
# Every member of an archive gets this time stamp, owner and group, so two
# packagings of the same install are byte-identical. 1980-01-01 is the
# earliest time a zip entry can record.
ARCHIVE_EPOCH: int = 315532800
ZIP_DATE_TIME: tuple[int, int, int, int, int, int] = (1980, 1, 1, 0, 0, 0)


def sdk_directory_name(version: str, platform: str) -> str:
    """The archive's single top directory, and the staged install prefix."""
    return "gui-forms-sdk-" + version + "-" + platform


def sdk_archive_name(version: str, platform: str) -> str:
    """The release asset name of one platform's SDK archive."""
    if platform not in PLATFORMS:
        raise ValueError("unknown platform " + platform)
    suffix: str = ".zip" if platform in ZIP_PLATFORMS else ".tar.gz"
    return sdk_directory_name(version, platform) + suffix


def required_members(platform: str) -> tuple[str, ...]:
    """The files one platform's archive must hold, relative to its top directory."""
    if platform == MACOS_PLATFORM:
        return REQUIRED_FILES + MACOS_RUNTIME_FILES
    return REQUIRED_FILES


def check_member_names(names: list[str], version: str, platform: str) -> None:
    """Raise ValueError unless names (archive members, files only) have the SDK's shape.

    Every member lies under the one top directory, no member escapes it, and
    the required files and directories are present.
    """
    top: str = sdk_directory_name(version, platform) + "/"
    relative: set[str] = set()
    name: str
    for name in names:
        if not name.startswith(top):
            raise ValueError(f"{name} is outside the top directory {top}")
        inner: str = name[len(top):]
        if inner.startswith("/") or ".." in inner.split("/"):
            raise ValueError(f"{name} escapes the top directory")
        relative.add(inner)
    missing: list[str] = []
    required: str
    for required in required_members(platform):
        if required not in relative:
            missing.append(required)
    prefix: str
    for prefix in REQUIRED_DIRECTORY_PREFIXES:
        if not any(entry.startswith(prefix) for entry in relative):
            missing.append(prefix)
    if missing:
        raise ValueError("the SDK archive lacks " + ", ".join(missing))


def archive_file_names(archive: Path) -> list[str]:
    """The names of the regular files and links in a .zip or .tar.gz archive."""
    names: list[str] = []
    if archive.name.endswith(".zip"):
        with zipfile.ZipFile(archive) as compressed:
            info: zipfile.ZipInfo
            for info in compressed.infolist():
                if not info.is_dir():
                    names.append(info.filename)
        return names
    with tarfile.open(archive, "r:gz") as tar:
        member: tarfile.TarInfo
        for member in tar.getmembers():
            if not member.isdir():
                names.append(member.name)
    return names


def check_archive(archive: Path, version: str, platform: str) -> None:
    """Raise ValueError unless archive is this platform's SDK archive, by name and shape."""
    expected: str = sdk_archive_name(version, platform)
    if archive.name != expected:
        raise ValueError(f"{archive.name} is not named {expected}")
    check_member_names(archive_file_names(archive), version, platform)


def stage_tree(top: Path) -> list[Path]:
    """Every file, link and directory under top, parents before children, in name order."""
    entries: list[Path] = []
    path: Path
    for path in sorted(top.rglob("*"), key=Path.as_posix):
        entries.append(path)
    return entries


def write_tar_gz(top: Path, archive: Path) -> None:
    """Write top into archive as a gzip tar rooted at top's name, links kept as links."""
    with archive.open("wb") as raw:
        with gzip.GzipFile(filename="", mode="wb", fileobj=raw, mtime=0, compresslevel=9) as compressed:
            with tarfile.open(fileobj=compressed, mode="w", format=tarfile.PAX_FORMAT) as tar:
                paths: list[Path] = [top] + stage_tree(top)
                path: Path
                for path in paths:
                    name: str = top.name
                    if path != top:
                        name = top.name + "/" + path.relative_to(top).as_posix()
                    info: tarfile.TarInfo = tar.gettarinfo(str(path), arcname=name)
                    info.mtime = ARCHIVE_EPOCH
                    info.uid = 0
                    info.gid = 0
                    info.uname = ""
                    info.gname = ""
                    if info.isfile():
                        with path.open("rb") as stream:
                            tar.addfile(info, stream)
                    else:
                        tar.addfile(info)


def write_zip(top: Path, archive: Path) -> None:
    """Write top's files into archive as a deflated zip rooted at top's name."""
    with zipfile.ZipFile(archive, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as compressed:
        path: Path
        for path in stage_tree(top):
            if path.is_symlink():
                raise ValueError(f"{path} is a link, which a Windows SDK zip does not carry")
            if not path.is_file():
                continue
            info: zipfile.ZipInfo = zipfile.ZipInfo(top.name + "/" + path.relative_to(top).as_posix(), ZIP_DATE_TIME)
            info.compress_type = zipfile.ZIP_DEFLATED
            info.external_attr = (0o100644 | (path.stat().st_mode & 0o111)) << 16
            with path.open("rb") as stream:
                compressed.writestr(info, stream.read(), compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)


def write_archive(top: Path, archive: Path) -> None:
    """Write the staged top directory into archive; the suffix chooses the format."""
    archive.unlink(missing_ok=True)
    if archive.name.endswith(".zip"):
        write_zip(top, archive)
    else:
        write_tar_gz(top, archive)


def add_consumer_files(top: Path, platform: str, runtime: Path | None) -> None:
    """Add the top-level toolchain file and, on macOS, the LLVM runtime to a staged install.

    The installed lib/cmake/GUIForms/llvm22.cmake must equal the source tree's;
    the runtime prefix is copied whole, its links kept as links.
    """
    installed: Path = top / "lib/cmake/GUIForms/llvm22.cmake"
    if not installed.is_file() or installed.read_bytes() != TOOLCHAIN_FILE.read_bytes():
        raise RuntimeError(f"{installed} is missing or differs from {TOOLCHAIN_FILE}")
    toolchain: Path = top / "cmake/llvm22.cmake"
    toolchain.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(TOOLCHAIN_FILE, toolchain)
    if platform != MACOS_PLATFORM:
        return
    if runtime is None or not (runtime / "include/c++/v1/__config_site").is_file():
        raise RuntimeError("the macOS SDK needs --runtime, the LLVM runtime prefix (GUI_FORMS_LLVM_RUNTIME)")
    shutil.copytree(runtime, top / RUNTIME_DIRECTORY, symlinks=True)


def check_staged_tree(top: Path, version: str, platform: str) -> None:
    """Raise unless the staged tree has the SDK's shape and its notices pass."""
    names: list[str] = []
    path: Path
    for path in stage_tree(top):
        if path.is_file() or path.is_symlink():
            names.append(top.name + "/" + path.relative_to(top).as_posix())
    check_member_names(names, version, platform)
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from ci_build import check_installed_notices, check_windows_sdk_imports
    check_installed_notices(top)
    if platform == "windows-x64":
        check_windows_sdk_imports(top)


def package(build_dir: Path, version: str, platform: str, out: Path, runtime: Path | None) -> Path:
    """Install build_dir, stage the SDK, check it and write its archive into out; the archive."""
    out.mkdir(parents=True, exist_ok=True)
    archive: Path = out / sdk_archive_name(version, platform)
    with tempfile.TemporaryDirectory(prefix="gui-forms-sdk-") as directory:
        top: Path = Path(directory) / sdk_directory_name(version, platform)
        subprocess.run(["cmake", "--install", str(build_dir), "--prefix", str(top)], check=True)
        add_consumer_files(top, platform, runtime)
        check_staged_tree(top, version, platform)
        write_archive(top, archive)
    check_archive(archive, version, platform)
    print(f"{archive.name}: {archive.stat().st_size} bytes", flush=True)
    return archive


def main(argv: list[str] | None = None) -> int:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--platform", choices=PLATFORMS, required=True)
    parser.add_argument("--version", required=True)
    parser.add_argument("--build-dir", type=Path, help="the configured, built tree to install")
    parser.add_argument("--out", type=Path, default=Path("dist"))
    parser.add_argument("--runtime", type=Path, default=None,
                        help="macOS: the LLVM runtime prefix (default: $GUI_FORMS_LLVM_RUNTIME)")
    parser.add_argument("--check", type=Path, default=None,
                        help="check this existing archive's name and shape; install nothing")
    args: argparse.Namespace = parser.parse_args(argv)
    if args.check is not None:
        check_archive(args.check, args.version, args.platform)
        print(f"{args.check.name}: SDK shape checked", flush=True)
        return 0
    if args.build_dir is None:
        parser.error("--build-dir is required unless --check is given")
    runtime: Path | None = args.runtime
    if runtime is None and os.environ.get("GUI_FORMS_LLVM_RUNTIME"):
        runtime = Path(os.environ["GUI_FORMS_LLVM_RUNTIME"])
    package(args.build_dir, args.version, args.platform, args.out, runtime)
    return 0


if __name__ == "__main__":
    sys.exit(main())
