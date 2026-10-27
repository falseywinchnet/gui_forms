"""The Skia archive holds the pinned headers and libraries, verified before use, and nothing else."""
from __future__ import annotations
import json
from pathlib import Path
import shutil
import tarfile
import tempfile
from typing import Any
import unittest
from unittest.mock import patch

import skia_archive

PLATFORM: str = 'windows-x64'
PRODUCER: dict[str, str] = {'run_id': '1', 'revision': 'r', 'runner': 'windows-2022', 'runner_image_version': ''}


def write_file(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)


def make_source(source: Path) -> None:
    """A repository holding the identity inputs of PLATFORM."""
    write_file(source / 'third_party/fetch_skia_cpu.sh',
               b'#!/bin/sh\nskia_revision=2a9b593bab4b2fd019fa494c8d401ff1fab0b883\n')
    relative: str
    for relative in skia_archive.COMMON_INPUTS[1:] + skia_archive.PLATFORM_INPUTS[PLATFORM]:
        write_file(skia_archive.input_path(relative, source), ('fixture ' + relative + '\n').encode('utf-8'))


def make_skia(skia_root: Path, skia_out: Path) -> None:
    write_file(skia_root / 'include/core/SkCanvas.h', b'// canvas\n')
    write_file(skia_root / 'include/private/base/SkAPI.h', b'// api\n')
    write_file(skia_root / 'include/core/README.md', b'not a header\n')
    write_file(skia_root / 'modules/skcms/skcms.h', b'// skcms\n')
    write_file(skia_root / 'modules/skcms/src/skcms_internals.h', b'// internals\n')
    write_file(skia_root / 'modules/skcms/skcms.cc', b'// source, not packed\n')
    write_file(skia_root / 'src/core/SkCanvas.cpp', b'// source, not packed\n')
    write_file(skia_root / 'LICENSE', b'BSD\n')
    write_file(skia_out / 'libskia.a', b'!<arch>\nskia')
    write_file(skia_out / 'libskcms.a', b'!<arch>\nskcms')
    write_file(skia_out / 'obj/src/core/SkCanvas.o', b'object, not packed')


class Fixture(unittest.TestCase):
    def setUp(self) -> None:
        self.scratch: Path = Path(tempfile.mkdtemp())
        self.source: Path = self.scratch / 'gui_forms'
        make_source(self.source)
        self.skia_root: Path = self.source / 'third_party/skia'
        self.skia_out: Path = self.scratch / '.build/native-windows-x64/skia'
        make_skia(self.skia_root, self.skia_out)
        self.destination: Path = self.scratch / 'packed'
        self.destination.mkdir()

    def tearDown(self) -> None:
        shutil.rmtree(self.scratch)

    def packed(self) -> tuple[Path, dict[str, Any]]:
        manifest: dict[str, Any] = skia_archive.pack(PLATFORM, self.skia_root, self.skia_out, self.destination,
                                                     PRODUCER, self.source)
        return self.destination / skia_archive.archive_name(PLATFORM), manifest


class IdentityTests(Fixture):
    def test_line_endings_do_not_change_the_identity(self) -> None:
        before: str = skia_archive.identity(PLATFORM, self.source)
        script: Path = self.source / 'third_party/fetch_skia_cpu.sh'
        script.write_bytes(script.read_bytes().replace(b'\n', b'\r\n'))
        self.assertEqual(skia_archive.identity(PLATFORM, self.source), before)

    def test_every_input_changes_the_identity(self) -> None:
        before: str = skia_archive.identity(PLATFORM, self.source)
        relative: str
        for relative in skia_archive.COMMON_INPUTS + skia_archive.PLATFORM_INPUTS[PLATFORM]:
            path: Path = skia_archive.input_path(relative, self.source)
            original: bytes = path.read_bytes()
            path.write_bytes(original + b'# changed\n')
            self.assertNotEqual(skia_archive.identity(PLATFORM, self.source), before, relative)
            path.write_bytes(original)
        self.assertEqual(skia_archive.identity(PLATFORM, self.source), before)

    def test_unknown_platform_is_refused(self) -> None:
        with self.assertRaises(skia_archive.SkiaArchiveError):
            skia_archive.identity('beos-x86', self.source)


class PackTests(Fixture):
    def test_archive_holds_headers_license_and_libraries_only(self) -> None:
        archive: Path
        manifest: dict[str, Any]
        archive, manifest = self.packed()
        names: list[str] = []
        with tarfile.open(archive, 'r:zst') as payload:
            member: tarfile.TarInfo
            for member in payload.getmembers():
                names.append(member.name)
                self.assertEqual((member.uid, member.gid, member.mtime, member.mode), (0, 0, 0, 0o644))
        self.assertEqual(names, ['out/libskcms.a', 'out/libskia.a', 'skia/LICENSE',
                                 'skia/include/core/SkCanvas.h', 'skia/include/private/base/SkAPI.h',
                                 'skia/modules/skcms/skcms.h', 'skia/modules/skcms/src/skcms_internals.h'])
        listed: list[str] = []
        entry: dict[str, Any]
        for entry in manifest['files']:
            listed.append(entry['path'])
        self.assertEqual(listed, names)
        self.assertEqual(manifest['identity'], skia_archive.identity(PLATFORM, self.source))
        self.assertEqual(manifest['defines'], list(skia_archive.PLATFORM_DEFINES[PLATFORM]))
        self.assertEqual(manifest['archive_sha256'], skia_archive.file_digest(archive))

    def test_packing_twice_gives_the_same_bytes(self) -> None:
        first: dict[str, Any] = self.packed()[1]
        second: dict[str, Any] = self.packed()[1]
        self.assertEqual(first['archive_sha256'], second['archive_sha256'])

    def test_missing_library_is_refused(self) -> None:
        (self.skia_out / 'libskcms.a').unlink()
        with self.assertRaises(skia_archive.SkiaArchiveError):
            self.packed()


class ExtractTests(Fixture):
    def test_round_trip_reproduces_every_file(self) -> None:
        archive: Path
        manifest: dict[str, Any]
        archive, manifest = self.packed()
        target: Path = skia_archive.extract(archive, manifest, self.scratch / 'fetched', self.source)
        self.assertEqual((target / 'out/libskia.a').read_bytes(), b'!<arch>\nskia')
        self.assertEqual((target / 'skia/modules/skcms/skcms.h').read_bytes(), b'// skcms\n')
        self.assertTrue((target / 'manifest.json').is_file())

    def test_inside_the_repository_is_refused(self) -> None:
        archive: Path
        manifest: dict[str, Any]
        archive, manifest = self.packed()
        with self.assertRaises(skia_archive.SkiaArchiveError):
            skia_archive.extract(archive, manifest, self.source / 'third_party/skia-prebuilt', self.source)
        self.assertFalse((self.source / 'third_party/skia-prebuilt').exists())

    def test_non_empty_directory_is_refused(self) -> None:
        archive: Path
        manifest: dict[str, Any]
        archive, manifest = self.packed()
        occupied: Path = self.scratch / 'occupied'
        write_file(occupied / 'keep.txt', b'user file')
        with self.assertRaises(skia_archive.SkiaArchiveError):
            skia_archive.extract(archive, manifest, occupied, self.source)
        self.assertEqual(sorted(path.name for path in occupied.iterdir()), ['keep.txt'])

    def test_changed_archive_is_refused_before_extraction(self) -> None:
        archive: Path
        manifest: dict[str, Any]
        archive, manifest = self.packed()
        archive.write_bytes(archive.read_bytes() + b'\0')
        with self.assertRaises(skia_archive.SkiaArchiveError):
            skia_archive.extract(archive, manifest, self.scratch / 'fetched', self.source)
        self.assertFalse((self.scratch / 'fetched').exists())

    def test_unlisted_member_is_refused(self) -> None:
        archive: Path
        manifest: dict[str, Any]
        archive, manifest = self.packed()
        with tarfile.open(archive, 'w:zst') as output:
            output.add(self.skia_out / 'libskia.a', arcname='out/libskia.a')
            output.add(self.skia_root / 'LICENSE', arcname='../escape')
        manifest['archive_sha256'] = skia_archive.file_digest(archive)
        manifest['archive_bytes'] = archive.stat().st_size
        with self.assertRaises(skia_archive.SkiaArchiveError):
            skia_archive.extract(archive, manifest, self.scratch / 'fetched', self.source)
        self.assertFalse((self.scratch / 'escape').exists())

    def test_manifest_of_another_identity_is_refused(self) -> None:
        manifest: dict[str, Any] = self.packed()[1]
        with self.assertRaises(skia_archive.SkiaArchiveError):
            skia_archive.check_manifest(manifest, PLATFORM, '0' * 64)
        skia_archive.check_manifest(manifest, PLATFORM, skia_archive.identity(PLATFORM, self.source))


class PinTests(Fixture):
    def write_pin(self, pins: dict[str, Any]) -> Path:
        path: Path = self.scratch / 'skia_archive_release.json'
        path.write_text(json.dumps({'notes': [], 'platforms': pins}), encoding='utf-8')
        return path

    def test_unpinned_platform_reads_as_none(self) -> None:
        with patch.object(skia_archive, 'PIN_PATH', self.write_pin({})):
            self.assertIsNone(skia_archive.read_pin(PLATFORM))
            with self.assertRaises(skia_archive.SkiaArchiveError):
                skia_archive.required_pin(PLATFORM)

    def test_stale_pin_is_refused(self) -> None:
        pin: dict[str, Any] = {'identity': '0' * 64, 'archive_sha256': '1' * 64, 'tag': 'build-abc'}
        with patch.object(skia_archive, 'PIN_PATH', self.write_pin({PLATFORM: pin})):
            with self.assertRaises(skia_archive.SkiaArchiveError):
                skia_archive.required_pin(PLATFORM)

    def test_publish_skips_an_identity_already_pinned(self) -> None:
        pin: dict[str, Any] = {'identity': skia_archive.identity(PLATFORM), 'archive_sha256': '1' * 64,
                               'tag': 'build-abc'}
        with patch.object(skia_archive, 'PIN_PATH', self.write_pin({PLATFORM: pin})):
            with patch.object(skia_archive.subprocess, 'run') as run:
                with patch.object(skia_archive.subprocess, 'check_output') as check_output:
                    skia_archive.publish_command('owner/repository', PLATFORM, self.destination)
        run.assert_not_called()
        check_output.assert_not_called()

    def plan_outputs(self, pins: dict[str, Any], artifact: int | None) -> dict[str, str]:
        outputs: Path = self.scratch / 'outputs.txt'
        outputs.write_text('', encoding='utf-8')
        with patch.object(skia_archive, 'PIN_PATH', self.write_pin(pins)):
            with patch.dict(skia_archive.os.environ, {'GITHUB_OUTPUT': str(outputs)}):
                with patch.object(skia_archive, 'artifact_run', return_value=artifact):
                    skia_archive.plan_command('owner/repository', PLATFORM)
        values: dict[str, str] = {}
        line: str
        for line in outputs.read_text(encoding='utf-8').splitlines():
            key: str
            value: str
            key, value = line.split('=', 1)
            values[key] = value
        return values

    def test_plan_builds_only_when_no_archive_of_the_identity_exists(self) -> None:
        current: str = skia_archive.identity(PLATFORM)
        pinned: dict[str, Any] = {PLATFORM: {'identity': current, 'archive_sha256': '1' * 64, 'tag': 'build-abc'}}
        stale: dict[str, Any] = {PLATFORM: {'identity': '0' * 64, 'archive_sha256': '1' * 64, 'tag': 'build-abc'}}
        self.assertEqual(self.plan_outputs(pinned, None), {'identity': current, 'source': 'release', 'build': 'false'})
        self.assertEqual(self.plan_outputs(stale, 7)['source'], 'artifact')
        self.assertEqual(self.plan_outputs({}, None)['build'], 'true')

    def test_local_fetch_without_a_pin_downloads_nothing(self) -> None:
        with patch.object(skia_archive, 'PIN_PATH', self.write_pin({})):
            with patch.object(skia_archive.subprocess, 'run') as run:
                with self.assertRaises(skia_archive.SkiaArchiveError):
                    skia_archive.fetch_command('owner/repository', PLATFORM, self.scratch / 'fetched', False)
        run.assert_not_called()
        self.assertFalse((self.scratch / 'fetched').exists())

    def test_ci_fetch_falls_back_to_the_identity_artifact(self) -> None:
        manifest: dict[str, Any] = self.packed()[1]
        (self.destination / skia_archive.manifest_name(PLATFORM)).write_text(json.dumps(manifest), encoding='utf-8')
        packed: Path = self.destination
        requested: list[tuple[int, str]] = []

        def copy_artifact(repository: str, run: int, name: str, directory: Path) -> None:
            requested.append((run, name))
            shutil.copytree(packed, directory, dirs_exist_ok=True)

        with patch.object(skia_archive, 'PIN_PATH', self.write_pin({})):
            with patch.object(skia_archive, 'identity', return_value=manifest['identity']):
                with patch.object(skia_archive, 'artifact_run', return_value=7):
                    with patch.object(skia_archive, 'download_artifact', side_effect=copy_artifact):
                        target: Path = skia_archive.fetch_command('owner/repository', PLATFORM,
                                                                  self.scratch / 'fetched', True)
        self.assertEqual(requested, [(7, skia_archive.artifact_name(PLATFORM, manifest['identity']))])
        self.assertEqual((target / 'out/libskcms.a').read_bytes(), b'!<arch>\nskcms')

    def test_checked_in_pin_file_parses(self) -> None:
        pins: dict[str, Any] = json.loads(skia_archive.PIN_PATH.read_text(encoding='utf-8'))
        self.assertIsInstance(pins['platforms'], dict)


if __name__ == '__main__':
    unittest.main()
