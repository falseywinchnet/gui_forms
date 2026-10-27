"""The SDK archive has one name and one shape per platform, and the release expects it."""
from __future__ import annotations

import contextlib
import io
import os
import tarfile
import tempfile
import unittest
import zipfile
from pathlib import Path
from typing import Any

import package_programs as pp
import package_sdk as sdk
import publish_tested_release as publish

VERSION: str = "0.2.1"


def write_file(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def make_install(top: Path) -> None:
    """A staged install with what cmake --install leaves, before add_consumer_files."""
    write_file(top / "include/gui_forms/application.hpp", "#pragma once\n")
    write_file(top / "lib/cmake/GUIForms/GUIFormsConfig.cmake", "# config\n")
    write_file(top / "lib/cmake/GUIForms/GUIFormsConfigVersion.cmake", "# version\n")
    write_file(top / "lib/cmake/GUIForms/GUIFormsTargets.cmake", "# targets\n")
    (top / "lib/cmake/GUIForms/llvm22.cmake").write_bytes(sdk.TOOLCHAIN_FILE.read_bytes())
    write_file(top / "share/licenses/GUIForms/LICENSE", "MIT License\n")
    write_file(top / "share/licenses/GUIForms/THIRD_PARTY_NOTICES.md", pp.FREETYPE_CREDIT + "\n")


def make_runtime(prefix: Path) -> None:
    """A macOS LLVM runtime prefix with the files package_sdk checks."""
    write_file(prefix / "lib/libc++.1.dylib", "dylib\n")
    write_file(prefix / "include/c++/v1/__config_site", "// configured\n")
    write_file(prefix / "include/c++/v1/vector", "// vector\n")
    write_file(prefix / "share/licenses/llvm-runtimes/libcxx-LICENSE.TXT", "Apache-2.0 WITH LLVM-exception\n")


def make_sdk(root: Path, platform: str) -> Path:
    """A complete staged SDK for platform under root; its top directory."""
    top: Path = root / sdk.sdk_directory_name(VERSION, platform)
    make_install(top)
    runtime: Path | None = None
    if platform == sdk.MACOS_PLATFORM:
        runtime = root / "runtime"
        make_runtime(runtime)
    sdk.add_consumer_files(top, platform, runtime)
    return top


class NameTests(unittest.TestCase):
    def test_archive_names_per_platform(self) -> None:
        self.assertEqual(sdk.sdk_archive_name(VERSION, "macos-arm64"), "gui-forms-sdk-0.2.1-macos-arm64.tar.gz")
        self.assertEqual(sdk.sdk_archive_name(VERSION, "windows-x64"), "gui-forms-sdk-0.2.1-windows-x64.zip")
        self.assertEqual(sdk.sdk_archive_name(VERSION, "linux-x64"), "gui-forms-sdk-0.2.1-linux-x64.tar.gz")
        self.assertEqual(sdk.sdk_archive_name(VERSION, "linux-arm64"), "gui-forms-sdk-0.2.1-linux-arm64.tar.gz")

    def test_platforms_match_the_program_packager(self) -> None:
        self.assertEqual(sdk.PLATFORMS, pp.PLATFORMS)

    def test_unknown_platform_is_rejected(self) -> None:
        with self.assertRaises(ValueError):
            sdk.sdk_archive_name(VERSION, "windows-arm64")


class ArchiveShapeTests(unittest.TestCase):
    def setUp(self) -> None:
        self.directory: tempfile.TemporaryDirectory[str] = tempfile.TemporaryDirectory()
        self.root: Path = Path(self.directory.name)

    def tearDown(self) -> None:
        self.directory.cleanup()

    def archive(self, platform: str, folder: str = "dist") -> Path:
        top: Path = make_sdk(self.root / ("stage-" + folder), platform)
        out: Path = self.root / folder
        out.mkdir(exist_ok=True)
        path: Path = out / sdk.sdk_archive_name(VERSION, platform)
        sdk.write_archive(top, path)
        return path

    def test_every_platform_archive_has_the_shape(self) -> None:
        platform: str
        for platform in sdk.PLATFORMS:
            with self.subTest(platform=platform):
                path: Path = self.archive(platform)
                sdk.check_archive(path, VERSION, platform)
                names: list[str] = sdk.archive_file_names(path)
                top: str = sdk.sdk_directory_name(VERSION, platform) + "/"
                self.assertIn(top + "cmake/llvm22.cmake", names)
                self.assertIn(top + "lib/cmake/GUIForms/GUIFormsConfig.cmake", names)
                self.assertIn(top + "share/licenses/GUIForms/THIRD_PARTY_NOTICES.md", names)
                self.assertEqual(platform == "macos-arm64", top + "llvm-runtime/lib/libc++.1.dylib" in names)
                self.assertEqual(platform == "windows-x64", zipfile.is_zipfile(path))

    def test_the_zip_and_tar_carry_the_same_files(self) -> None:
        zip_names: list[str] = sdk.archive_file_names(self.archive("windows-x64"))
        tar_names: list[str] = sdk.archive_file_names(self.archive("linux-x64"))
        self.assertEqual(sorted(name.split("/", 1)[1] for name in zip_names),
                         sorted(name.split("/", 1)[1] for name in tar_names))

    def test_archives_are_reproducible(self) -> None:
        platform: str
        for platform in ("windows-x64", "linux-x64"):
            with self.subTest(platform=platform):
                first: Path = self.archive(platform, "first")
                second: Path = self.archive(platform, "second")
                self.assertEqual(first.read_bytes(), second.read_bytes())

    def test_tar_members_have_no_owner_or_time(self) -> None:
        with tarfile.open(self.archive("linux-x64"), "r:gz") as tar:
            member: tarfile.TarInfo
            for member in tar.getmembers():
                self.assertEqual((member.uid, member.gid, member.uname, member.gname), (0, 0, "", ""))
                self.assertEqual(member.mtime, sdk.ARCHIVE_EPOCH)

    def test_tar_keeps_library_links(self) -> None:
        top: Path = make_sdk(self.root / "stage", "linux-x64")
        write_file(top / "lib/libgui_forms_threading.so.0", "library\n")
        try:
            os.symlink("libgui_forms_threading.so.0", top / "lib/libgui_forms_threading.so")
        except OSError:
            self.skipTest("this host cannot create symbolic links")
        path: Path = self.root / sdk.sdk_archive_name(VERSION, "linux-x64")
        sdk.write_archive(top, path)
        with tarfile.open(path, "r:gz") as tar:
            link: tarfile.TarInfo = tar.getmember(top.name + "/lib/libgui_forms_threading.so")
            self.assertTrue(link.issym())
            self.assertEqual(link.linkname, "libgui_forms_threading.so.0")

    def test_a_missing_package_file_is_rejected(self) -> None:
        top: Path = make_sdk(self.root / "stage", "linux-x64")
        (top / "lib/cmake/GUIForms/GUIFormsConfigVersion.cmake").unlink()
        path: Path = self.root / sdk.sdk_archive_name(VERSION, "linux-x64")
        sdk.write_archive(top, path)
        with self.assertRaisesRegex(ValueError, "GUIFormsConfigVersion.cmake"):
            sdk.check_archive(path, VERSION, "linux-x64")

    def test_missing_headers_are_rejected(self) -> None:
        top: Path = make_sdk(self.root / "stage", "windows-x64")
        (top / "include/gui_forms/application.hpp").unlink()
        path: Path = self.root / sdk.sdk_archive_name(VERSION, "windows-x64")
        sdk.write_archive(top, path)
        with self.assertRaisesRegex(ValueError, "include/gui_forms/"):
            sdk.check_archive(path, VERSION, "windows-x64")

    def test_a_member_outside_the_top_directory_is_rejected(self) -> None:
        path: Path = self.archive("windows-x64")
        with zipfile.ZipFile(path, "a") as compressed:
            compressed.writestr("stray.txt", "outside\n")
        with self.assertRaisesRegex(ValueError, "outside the top directory"):
            sdk.check_archive(path, VERSION, "windows-x64")

    def test_another_version_or_platform_is_rejected(self) -> None:
        path: Path = self.archive("linux-x64")
        with self.assertRaisesRegex(ValueError, "is not named"):
            sdk.check_archive(path, "0.2.2", "linux-x64")
        with self.assertRaisesRegex(ValueError, "is not named"):
            sdk.check_archive(path, VERSION, "linux-arm64")

    def test_check_command_reads_an_archive(self) -> None:
        path: Path = self.archive("linux-arm64")
        with contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(sdk.main(["--platform", "linux-arm64", "--version", VERSION, "--check", str(path)]), 0)


class StagingTests(unittest.TestCase):
    def setUp(self) -> None:
        self.directory: tempfile.TemporaryDirectory[str] = tempfile.TemporaryDirectory()
        self.root: Path = Path(self.directory.name)

    def tearDown(self) -> None:
        self.directory.cleanup()

    def test_macos_needs_the_runtime(self) -> None:
        top: Path = self.root / sdk.sdk_directory_name(VERSION, "macos-arm64")
        make_install(top)
        with self.assertRaisesRegex(RuntimeError, "--runtime"):
            sdk.add_consumer_files(top, "macos-arm64", None)

    def test_macos_runtime_is_copied_whole(self) -> None:
        top: Path = make_sdk(self.root, "macos-arm64")
        self.assertTrue((top / "llvm-runtime/include/c++/v1/vector").is_file())
        self.assertEqual((top / "cmake/llvm22.cmake").read_bytes(), sdk.TOOLCHAIN_FILE.read_bytes())

    def test_an_installed_toolchain_that_differs_is_rejected(self) -> None:
        top: Path = self.root / sdk.sdk_directory_name(VERSION, "linux-x64")
        make_install(top)
        write_file(top / "lib/cmake/GUIForms/llvm22.cmake", "# another toolchain\n")
        with self.assertRaisesRegex(RuntimeError, "differs"):
            sdk.add_consumer_files(top, "linux-x64", None)

    def test_staged_notices_are_checked(self) -> None:
        top: Path = make_sdk(self.root, "linux-x64")
        with contextlib.redirect_stdout(io.StringIO()):
            sdk.check_staged_tree(top, VERSION, "linux-x64")
        write_file(top / "share/licenses/GUIForms/extra.txt", "GNU GENERAL PUBLIC LICENSE\n")
        with self.assertRaisesRegex(RuntimeError, "General Public License"):
            sdk.check_staged_tree(top, VERSION, "linux-x64")


class ReleaseTests(unittest.TestCase):
    def receipts(self) -> dict[str, dict[str, Any]]:
        program: dict[str, Any] = {"id": "gallery", "display_name": "GUI.Forms Gallery", "status": "packaged",
                                   "artifacts": {"archive": "gallery.zip"}}
        return {"windows-x64": {"programs": [program]}, "linux-x64": {"programs": []},
                "macos-arm64": {"programs": [program]}}

    def test_the_release_expects_one_sdk_per_platform(self) -> None:
        self.assertEqual(publish.expected_sdk_archives(self.receipts(), VERSION), [
            "gui-forms-sdk-0.2.1-linux-x64.tar.gz",
            "gui-forms-sdk-0.2.1-macos-arm64.tar.gz",
            "gui-forms-sdk-0.2.1-windows-x64.zip",
        ])

    def test_the_release_notes_list_the_sdk(self) -> None:
        notes: str = publish.release_notes(VERSION, "0" * 40, "malkuth-software/gui_forms", self.receipts(), [])
        self.assertIn("## SDK", notes)
        self.assertIn("| Windows x64 | `gui-forms-sdk-0.2.1-windows-x64.zip` |", notes)
        self.assertIn("`gui-forms-sdk-0.2.1-linux-x64.tar.gz`", notes)


if __name__ == "__main__":
    unittest.main()
