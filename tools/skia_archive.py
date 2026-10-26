#!/usr/bin/env python3
"""Pack, check, publish, describe and fetch one platform's pinned CPU Skia build (stage SK).

A Skia archive holds what a GUI.Forms build reads from Skia and nothing else:

  skia/include/...           Skia's public headers, whole (GUI.Forms includes
                             include/core and include/effects, which include
                             include/private)
  skia/modules/skcms/...     skcms's headers, which the public headers include
  skia/LICENSE
  out/libskia.a, out/libskcms.a

Its identity is the SHA-256 of the inputs that decide those bytes: the source
pin and patches, the platform's GN arguments and build script, the pinned
compiler, the preprocessor definitions every consumer must share, and the
layout below. The manifest beside each archive lists every file with its
size and digest.

CI builds the archive from the pinned source (third_party/fetch_skia_cpu.sh
and the platform's build script), packs and checks it, and uploads it from
the validating run as the artifact gui-forms-skia-<platform>-<identity>. A
push to main publishes that artifact's bytes in the revision's build-<sha>
release. tools/skia_archive_release.json then pins the release per
platform; the pin follows every identity change, as the Windows
toolchain's pin does.

A consumer, local or CI, fetches the pinned archive: the manifest's identity
must match the current tree and its digest the pin, the archive must match
the manifest, and it is extracted only into a new directory outside the
repository, where every file is checked against the manifest again. Then

  -DGUI_FORMS_SKIA_PREBUILT=ON
  -DGUI_FORMS_SKIA_ROOT=<directory>/skia -DGUI_FORMS_SKIA_OUT=<directory>/out

configures GUI.Forms against it. No local Skia source fetch or build is
involved.

Commands:
  identity --platform P          print the identity of the current inputs
  pack --platform P --skia-out O [--skia-root R] [--destination D]
                                 write the archive and its manifest into D
                                 (the workspace by default)
  probe --platform P [--destination D]
                                 extract D's packed archive into a temporary
                                 directory, then compile, link and run
                                 tools/skia_archive_probe.cpp against it
  publish --platform P           (CI, a push to main) publish this identity's
                                 archive unless the pin already names it
  describe --platform P          print the pinned archive's release, source
                                 address, size and digest (no download)
  fetch --platform P --into D    download the pinned archive, verify it and
                                 extract it into the new directory D

pack, probe, publish and fetch require Python 3.14 (stdlib zstd).
"""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import subprocess
import sys
import tarfile
import tempfile
from typing import Any, BinaryIO, TextIO

SOURCE: Path = Path(__file__).resolve().parents[1]
WORKSPACE: Path = SOURCE.parent
PIN_PATH: Path = Path(__file__).resolve().parent / 'skia_archive_release.json'
PROBE_SOURCE: Path = Path(__file__).resolve().parent / 'skia_archive_probe.cpp'
REPOSITORY: str = 'falseywinchnet/gui_forms'
SCHEMA: int = 1
# Changing what the archive holds or how it is laid out changes the identity.
LAYOUT: int = 1
ZSTD_LEVEL: int = 10
MAXIMUM_MANIFEST_BYTES: int = 4 * 1024 * 1024
LIBRARIES: tuple[str, ...] = ('libskia.a', 'libskcms.a')
# Header trees copied from the Skia source tree, and the files kept from them.
HEADER_TREES: tuple[str, ...] = ('include', 'modules/skcms')
HEADER_SUFFIX: str = '.h'
LICENSE: str = 'LICENSE'
# The top-level directories of an archive.
MEMBER_ROOTS: tuple[str, ...] = ('skia', 'out')

# The inputs that decide each platform's archive bytes, relative to SOURCE.
COMMON_INPUTS: tuple[str, ...] = (
    'third_party/fetch_skia_cpu.sh',
    'third_party/skia_png_only.patch',
    'third_party/skia_windows_mingw.patch',
)
PLATFORM_INPUTS: dict[str, tuple[str, ...]] = {
    'windows-x64': (
        'third_party/skia_cpu_windows_mingw_args.gn',
        'third_party/build_skia_cpu_windows_mingw.sh',
        'tools/windows_toolchain.json',
        'tools/windows_toolchain_release.json',
    ),
}
# Preprocessor definitions every translation unit that includes Skia's
# headers shares with the archive's build. GUI.Forms stores BGRA8
# premultiplied sRGB on every platform (stage SK); all four shifts are
# spelled out rather than left to Skia's platform default.
PLATFORM_DEFINES: dict[str, tuple[str, ...]] = {
    'windows-x64': ('SK_B32_SHIFT=0', 'SK_G32_SHIFT=8', 'SK_R32_SHIFT=16', 'SK_A32_SHIFT=24'),
}


class SkiaArchiveError(RuntimeError):
    """An archive, manifest or pin that does not match what this tree requires."""


def file_digest(path: Path) -> str:
    stream: BinaryIO
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def archive_name(platform: str) -> str:
    return 'gui-forms-skia-' + platform + '.tar.zst'


def manifest_name(platform: str) -> str:
    return 'gui-forms-skia-' + platform + '.json'


def artifact_name(platform: str, identity_digest: str) -> str:
    return 'gui-forms-skia-' + platform + '-' + identity_digest


def require_platform(platform: str) -> None:
    if platform not in PLATFORM_INPUTS:
        raise SkiaArchiveError('No Skia archive is defined for platform ' + platform)


def write_output(name: str, value: str) -> None:
    """Set a step output when running as a GitHub Actions step; otherwise do nothing."""
    path: str = os.environ.get('GITHUB_OUTPUT', '')
    if not path:
        return
    stream: TextIO
    with Path(path).open('a', encoding='utf-8') as stream:
        stream.write(name + '=' + value + '\n')


def skia_revision(source: Path) -> str:
    text: str = (source / 'third_party/fetch_skia_cpu.sh').read_text(encoding='utf-8')
    found: re.Match[str] | None = re.search(r'^skia_revision=([0-9a-f]{40})\r?$', text, re.MULTILINE)
    if found is None:
        raise SkiaArchiveError('third_party/fetch_skia_cpu.sh names no Skia revision')
    return found.group(1)


def input_digests(platform: str, source: Path) -> dict[str, str]:
    require_platform(platform)
    digests: dict[str, str] = {}
    relative: str
    for relative in COMMON_INPUTS + PLATFORM_INPUTS[platform]:
        # Line endings are normalised so that a checkout's conversion does
        # not change the identity.
        data: bytes = (source / relative).read_bytes().replace(b'\r\n', b'\n')
        digests[relative] = hashlib.sha256(data).hexdigest()
    return digests


def identity_record(platform: str, source: Path) -> dict[str, Any]:
    record: dict[str, Any] = {'schema': SCHEMA, 'layout': LAYOUT, 'platform': platform,
                              'skia_revision': skia_revision(source),
                              'inputs': input_digests(platform, source),
                              'defines': list(PLATFORM_DEFINES.get(platform, ()))}
    return record


def identity(platform: str, source: Path = SOURCE) -> str:
    text: str = json.dumps(identity_record(platform, source), sort_keys=True, separators=(',', ':'))
    return hashlib.sha256(text.encode('utf-8')).hexdigest()


def member_path(member: tuple[str, Path]) -> str:
    return member[0]


def archive_members(skia_root: Path, skia_out: Path) -> list[tuple[str, Path]]:
    """Every (archive path, source file) the archive holds, sorted by archive path."""
    members: list[tuple[str, Path]] = []
    tree: str
    for tree in HEADER_TREES:
        base: Path = skia_root / tree
        if not base.is_dir():
            raise SkiaArchiveError('Skia source tree has no ' + tree + ': ' + str(base))
        path: Path
        for path in base.rglob('*' + HEADER_SUFFIX):
            if path.is_file():
                members.append(('skia/' + path.relative_to(skia_root).as_posix(), path))
    license_path: Path = skia_root / LICENSE
    if not license_path.is_file():
        raise SkiaArchiveError('Skia source tree has no ' + LICENSE)
    members.append(('skia/' + LICENSE, license_path))
    library: str
    for library in LIBRARIES:
        built: Path = skia_out / library
        if not built.is_file() or built.stat().st_size == 0:
            raise SkiaArchiveError('Missing Skia library ' + str(built))
        members.append(('out/' + library, built))
    members.sort(key=member_path)
    return members


def normalized(info: tarfile.TarInfo) -> tarfile.TarInfo:
    """The same member header from every producer: no owner, no time, read-only mode."""
    info.uid = 0
    info.gid = 0
    info.uname = ''
    info.gname = ''
    info.mtime = 0
    info.mode = 0o644
    return info


def pack(platform: str, skia_root: Path, skia_out: Path, destination: Path, producer: dict[str, str],
         source: Path = SOURCE) -> dict[str, Any]:
    """Write destination/archive_name(platform) and return its manifest, which the caller writes."""
    members: list[tuple[str, Path]] = archive_members(skia_root, skia_out)
    archive: Path = destination / archive_name(platform)
    files: list[dict[str, Any]] = []
    output: tarfile.TarFile
    with tarfile.open(archive, 'w:zst', level=ZSTD_LEVEL) as output:
        name: str
        path: Path
        for name, path in members:
            info: tarfile.TarInfo = normalized(output.gettarinfo(str(path), arcname=name))
            stream: BinaryIO
            with path.open('rb') as stream:
                output.addfile(info, stream)
            files.append({'path': name, 'bytes': info.size, 'sha256': file_digest(path)})
    record: dict[str, Any] = identity_record(platform, source)
    manifest: dict[str, Any] = {
        'schema': SCHEMA, 'layout': LAYOUT, 'platform': platform, 'identity': identity(platform, source),
        'skia_revision': record['skia_revision'], 'inputs': record['inputs'], 'defines': record['defines'],
        'archive': archive.name, 'archive_sha256': file_digest(archive), 'archive_bytes': archive.stat().st_size,
        'compression': 'zstd level ' + str(ZSTD_LEVEL), 'files': files, 'producer': producer,
        'configure': ['-DGUI_FORMS_SKIA_PREBUILT=ON', '-DGUI_FORMS_SKIA_ROOT=<directory>/skia',
                      '-DGUI_FORMS_SKIA_OUT=<directory>/out']}
    return manifest


def check_manifest(manifest: dict[str, Any], platform: str, expected_identity: str) -> None:
    if manifest.get('schema') != SCHEMA or manifest.get('layout') != LAYOUT or manifest.get('platform') != platform:
        raise SkiaArchiveError('Unsupported Skia archive manifest for ' + platform)
    if manifest.get('identity') != expected_identity:
        raise SkiaArchiveError('Skia archive manifest has identity ' + str(manifest.get('identity'))
                               + '; this tree requires ' + expected_identity)


def verify_archive(archive: Path, manifest: dict[str, Any]) -> None:
    if archive.stat().st_size != manifest.get('archive_bytes'):
        raise SkiaArchiveError('Skia archive size differs from its manifest')
    if file_digest(archive) != manifest.get('archive_sha256'):
        raise SkiaArchiveError('Skia archive digest differs from its manifest')


def fresh_directory(directory: Path, source: Path = SOURCE) -> Path:
    """`directory`, resolved, once it is known to be absent or empty and outside the repository."""
    resolved: Path = directory.resolve()
    repository: Path = source.resolve()
    if resolved == repository or repository in resolved.parents:
        raise SkiaArchiveError('Refusing to extract Skia inside the repository: ' + str(resolved))
    if resolved.exists():
        if not resolved.is_dir() or any(resolved.iterdir()):
            raise SkiaArchiveError('Refusing to extract Skia into the existing, non-empty ' + str(resolved))
    return resolved


def check_members(members: list[tarfile.TarInfo], expected: dict[str, dict[str, Any]]) -> None:
    """Every member is a regular file the manifest lists, below skia/ or out/; none is missing."""
    member: tarfile.TarInfo
    for member in members:
        name: PurePosixPath = PurePosixPath(member.name)
        if (not name.parts or name.is_absolute() or '..' in name.parts or name.parts[0] not in MEMBER_ROOTS
                or not member.isfile() or member.name not in expected):
            raise SkiaArchiveError('Unexpected Skia archive member: ' + member.name)
    if len(members) != len(expected):
        raise SkiaArchiveError('Skia archive holds ' + str(len(members)) + ' files; its manifest lists '
                               + str(len(expected)))


def extract(archive: Path, manifest: dict[str, Any], directory: Path, source: Path = SOURCE) -> Path:
    """Verify the archive, extract it into the new `directory`, then check every file.

    A failure before extraction leaves nothing behind. A failure after it
    leaves the extracted files, which the caller must not use: the
    directory then has no manifest.json.
    """
    verify_archive(archive, manifest)
    target: Path = fresh_directory(directory, source)
    expected: dict[str, dict[str, Any]] = {}
    entry: dict[str, Any]
    for entry in manifest['files']:
        expected[entry['path']] = entry
    payload: tarfile.TarFile
    with tarfile.open(archive, 'r:zst') as payload:
        check_members(payload.getmembers(), expected)
        target.mkdir(parents=True, exist_ok=True)
        payload.extractall(target, filter='data')
    path: str
    for path in expected:
        extracted: Path = target / path
        if extracted.stat().st_size != expected[path]['bytes'] or file_digest(extracted) != expected[path]['sha256']:
            raise SkiaArchiveError('Extracted ' + path + ' differs from the manifest')
    (target / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
    return target


def read_manifest(path: Path) -> dict[str, Any]:
    if path.stat().st_size > MAXIMUM_MANIFEST_BYTES:
        raise SkiaArchiveError('Skia archive manifest exceeds its bound: ' + str(path))
    manifest: dict[str, Any] = json.loads(path.read_text(encoding='utf-8'))
    return manifest


def read_pin(platform: str) -> dict[str, Any] | None:
    """The pin of `platform` in tools/skia_archive_release.json, or None when none is published."""
    pins: dict[str, Any] = json.loads(PIN_PATH.read_text(encoding='utf-8'))
    pin: Any = pins.get('platforms', {}).get(platform)
    if not isinstance(pin, dict) or not pin.get('tag'):
        return None
    return pin


def required_pin(platform: str) -> dict[str, Any]:
    pin: dict[str, Any] | None = read_pin(platform)
    if pin is None:
        raise SkiaArchiveError('tools/skia_archive_release.json pins no published Skia archive for ' + platform)
    current: str = identity(platform)
    if pin.get('identity') != current:
        raise SkiaArchiveError('The pin names Skia identity ' + str(pin.get('identity')) + '; this tree gives '
                               + current + '. A push to main publishes the new archive; the pin then follows it.')
    return pin


def release_asset(repository: str, tag: str, name: str) -> dict[str, Any]:
    """The release asset's metadata (name, size, address); nothing is downloaded."""
    release: dict[str, Any] = json.loads(subprocess.check_output(
        ['gh', 'api', 'repos/' + repository + '/releases/tags/' + tag], text=True))
    asset: dict[str, Any]
    for asset in release['assets']:
        if asset['name'] == name:
            return asset
    raise SkiaArchiveError('Release ' + tag + ' holds no ' + name)


def describe_command(repository: str, platform: str) -> None:
    pin: dict[str, Any] = required_pin(platform)
    asset: dict[str, Any] = release_asset(repository, pin['tag'], archive_name(platform))
    print('Skia archive for ' + platform)
    print('  release:  ' + pin['tag'] + ' (' + repository + ')')
    print('  source:   ' + str(asset['browser_download_url']))
    print('  bytes:    ' + str(asset['size']))
    print('  sha256:   ' + str(pin.get('archive_sha256')) + ' (pinned)')
    print('  identity: ' + str(pin.get('identity')))


def fetch_command(repository: str, platform: str, directory: Path) -> Path:
    target: Path = fresh_directory(directory)
    pin: dict[str, Any] = required_pin(platform)
    tag: str = pin['tag']
    asset: dict[str, Any] = release_asset(repository, tag, archive_name(platform))
    print('Fetching ' + str(asset['browser_download_url']) + ' (' + str(asset['size']) + ' bytes)', flush=True)
    manifest: dict[str, Any]
    scratch: str
    with tempfile.TemporaryDirectory() as scratch:
        name: str
        for name in (manifest_name(platform), archive_name(platform)):
            subprocess.run(['gh', 'release', 'download', tag, '--repo', repository, '--pattern', name,
                            '--dir', scratch], check=True)
        manifest = read_manifest(Path(scratch) / manifest_name(platform))
        check_manifest(manifest, platform, identity(platform))
        if manifest.get('archive_sha256') != pin.get('archive_sha256'):
            raise SkiaArchiveError('Release ' + tag + ' holds archive ' + str(manifest.get('archive_sha256'))
                                   + ', not the pinned ' + str(pin.get('archive_sha256')))
        extract(Path(scratch) / archive_name(platform), manifest, target)
    print('Skia ' + manifest['skia_revision'] + ' for ' + platform + ' is in ' + str(target), flush=True)
    return target


def producer_record() -> dict[str, str]:
    record: dict[str, str] = {'run_id': os.environ.get('GITHUB_RUN_ID', ''),
                              'revision': os.environ.get('GITHUB_SHA', ''),
                              'runner': os.environ.get('GUI_FORMS_RUNNER', ''),
                              'runner_image_version': os.environ.get('ImageVersion', '')}
    return record


def pack_command(platform: str, skia_root: Path, skia_out: Path, destination: Path) -> None:
    destination.mkdir(parents=True, exist_ok=True)
    manifest: dict[str, Any] = pack(platform, skia_root, skia_out, destination, producer_record())
    (destination / manifest_name(platform)).write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
    write_output('artifact-name', artifact_name(platform, manifest['identity']))
    print('Packed Skia ' + manifest['skia_revision'] + ' for ' + platform + ': ' + str(manifest['archive_bytes'])
          + ' bytes, ' + str(len(manifest['files'])) + ' files, sha256 ' + manifest['archive_sha256']
          + ', identity ' + manifest['identity'], flush=True)


def probe_command(platform: str, destination: Path) -> None:
    """Compile, link and run the probe against the packed archive, extracted afresh."""
    archive: Path = destination / archive_name(platform)
    manifest: dict[str, Any] = read_manifest(destination / manifest_name(platform))
    check_manifest(manifest, platform, identity(platform))
    compiler: str = os.environ.get('CXX', 'clang++')
    scratch: str
    with tempfile.TemporaryDirectory() as scratch:
        extracted: Path = extract(archive, manifest, Path(scratch) / 'archive')
        program: Path = Path(scratch) / 'skia_archive_probe.exe'
        command: list[str] = [compiler, '-std=c++20', '-O1', '-I', str(extracted / 'skia')]
        definition: str
        for definition in manifest['defines']:
            command.append('-D' + definition)
        command.extend([str(PROBE_SOURCE), str(extracted / 'out/libskia.a'), str(extracted / 'out/libskcms.a'),
                        '-o', str(program)])
        print(' '.join(command), flush=True)
        subprocess.run(command, check=True)
        subprocess.run([str(program)], check=True)


def find_artifact(repository: str, name: str, directory: Path) -> bool:
    """Download the newest unexpired artifact `name` uploaded by a run of this repository into `directory`."""
    listing: dict[str, Any] = json.loads(subprocess.check_output(
        ['gh', 'api', 'repos/' + repository + '/actions/artifacts?per_page=30&name=' + name], text=True))
    artifact: dict[str, Any]
    for artifact in listing['artifacts']:
        origin: dict[str, Any] = artifact['workflow_run']
        if artifact['expired'] or origin['head_repository_id'] != origin['repository_id']:
            continue
        subprocess.run(['gh', 'run', 'download', str(origin['id']), '--repo', repository,
                        '--name', name, '--dir', str(directory)], check=True)
        return True
    return False


def publish_command(repository: str, platform: str, destination: Path) -> None:
    """Publish this identity's archive in the pushed revision's build release, unless already pinned.

    The archive is the one this job packed, else the validating run's
    artifact. When neither exists (the validating run reused an older
    validation), nothing is published and the job warns: the pin stays
    behind and consumers keep refusing the stale pin.
    """
    current: str = identity(platform)
    pin: dict[str, Any] | None = read_pin(platform)
    if pin is not None and pin.get('identity') == current:
        print('Skia identity ' + current + ' is already published in ' + str(pin['tag']), flush=True)
        return
    archive: Path = destination / archive_name(platform)
    if not archive.is_file():
        name: str = artifact_name(platform, current)
        if not find_artifact(repository, name, destination):
            print('::warning::No packed Skia archive or artifact ' + name + ' exists; nothing published', flush=True)
            return
    manifest_path: Path = destination / manifest_name(platform)
    manifest: dict[str, Any] = read_manifest(manifest_path)
    check_manifest(manifest, platform, current)
    verify_archive(archive, manifest)
    import publish_cache
    revision: str = os.environ['GITHUB_SHA']
    path: Path
    for path in (manifest_path, archive):
        publish_cache.publish(repository, revision, path)
    print('Published Skia archive ' + manifest['archive_sha256'] + ' (' + str(manifest['archive_bytes'])
          + ' bytes) for ' + platform + ' in build-' + revision + '; pin it in tools/skia_archive_release.json',
          flush=True)


def main() -> None:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('command', choices=('identity', 'pack', 'probe', 'publish', 'describe', 'fetch'))
    parser.add_argument('--platform', required=True)
    parser.add_argument('--skia-root', type=Path, default=SOURCE / 'third_party/skia')
    parser.add_argument('--skia-out', type=Path)
    parser.add_argument('--destination', type=Path, default=WORKSPACE)
    parser.add_argument('--into', type=Path)
    parser.add_argument('--repository', default=os.environ.get('GITHUB_REPOSITORY', REPOSITORY))
    args: argparse.Namespace = parser.parse_args()
    try:
        require_platform(args.platform)
        if args.command == 'identity':
            print(identity(args.platform))
        elif args.command == 'pack':
            if args.skia_out is None:
                raise SkiaArchiveError('pack requires --skia-out')
            pack_command(args.platform, args.skia_root.resolve(), args.skia_out.resolve(), args.destination.resolve())
        elif args.command == 'probe':
            probe_command(args.platform, args.destination.resolve())
        elif args.command == 'publish':
            publish_command(args.repository, args.platform, args.destination.resolve())
        elif args.command == 'describe':
            describe_command(args.repository, args.platform)
        else:
            if args.into is None:
                raise SkiaArchiveError('fetch requires --into')
            fetch_command(args.repository, args.platform, args.into)
    except SkiaArchiveError as error:
        print('skia_archive: ' + str(error), file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    main()
