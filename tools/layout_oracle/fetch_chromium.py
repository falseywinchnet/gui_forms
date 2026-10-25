"""Fetch, verify and unpack the pinned Chromium headless shell.

    python -B tools/layout_oracle/fetch_chromium.py --platform linux64
        --download <new-or-empty directory> --unpack <new-or-empty directory>
        --approved [--archive <already downloaded zip>]

The package, its size and its SHA-256 come from chromium_pin.json (owner
decision L-Q10). Without --approved the script refuses: getting Chromium is
an owner-approved download. With --archive it verifies a file fetched
earlier instead of downloading. The archive is checked against the pinned
size and hash before anything is unpacked; members that would land outside
the unpack directory are refused. Each directory must be new or empty, and
must not be the repository. The unpacked executable's path is printed on
the last line of standard output.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import stat
import sys
import urllib.request
import zipfile
from pathlib import Path

TOOL_DIRECTORY = Path(__file__).resolve().parent
REPOSITORY = TOOL_DIRECTORY.parent.parent
PIN = TOOL_DIRECTORY / "chromium_pin.json"
CHUNK_BYTES = 1024 * 1024


class FetchError(RuntimeError):
    pass


def require_empty_directory(path: Path) -> None:
    resolved = path.resolve()
    if resolved == REPOSITORY or REPOSITORY in resolved.parents:
        raise FetchError(f"{path} is inside the repository; Chromium never goes there")
    if resolved.exists() and any(resolved.iterdir()):
        raise FetchError(f"{path} must be new or empty")
    resolved.mkdir(parents=True, exist_ok=True)


def sha256_of(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(CHUNK_BYTES), b""):
            digest.update(chunk)
    return digest.hexdigest()


def download(url: str, destination: Path, expected_bytes: int) -> None:
    with urllib.request.urlopen(url) as response, destination.open("wb") as output:
        written = 0
        while True:
            chunk = response.read(CHUNK_BYTES)
            if not chunk:
                break
            written += len(chunk)
            if written > expected_bytes:
                raise FetchError(f"{url} is larger than the pinned {expected_bytes} bytes")
            output.write(chunk)


def verify(archive: Path, package: dict[str, object]) -> None:
    size = archive.stat().st_size
    if size != package["bytes"]:
        raise FetchError(f"{archive} is {size} bytes, not the pinned {package['bytes']}")
    digest = sha256_of(archive)
    if digest != package["sha256"]:
        raise FetchError(f"{archive} has SHA-256 {digest}, not the pinned {package['sha256']}")


def unpack(archive: Path, destination: Path) -> None:
    root = destination.resolve()
    with zipfile.ZipFile(archive) as bundle:
        for member in bundle.infolist():
            target = (root / member.filename).resolve()
            if target != root and root not in target.parents:
                raise FetchError(f"archive member {member.filename!r} leaves the unpack directory")
        for member in bundle.infolist():
            bundle.extract(member, root)
            # zipfile drops Unix modes; restore the executable bits it recorded.
            mode = (member.external_attr >> 16) & 0o777
            if mode & stat.S_IXUSR:
                extracted = root / member.filename
                extracted.chmod(extracted.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)


def main(arguments: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--platform", choices=("linux64", "win64"), required=True)
    parser.add_argument("--download", type=Path)
    parser.add_argument("--archive", type=Path)
    parser.add_argument("--unpack", type=Path, required=True)
    parser.add_argument("--approved", action="store_true")
    options = parser.parse_args(arguments)
    pin = json.loads(PIN.read_text(encoding="utf-8"))
    package = pin["packages"][options.platform]
    try:
        if not options.approved:
            raise FetchError("getting Chromium is an owner-approved download; pass --approved")
        if options.archive is not None:
            archive = options.archive
        else:
            if options.download is None:
                raise FetchError("pass --download or --archive")
            require_empty_directory(options.download)
            archive = options.download / Path(str(package["url"])).name
            download(str(package["url"]), archive, int(package["bytes"]))
        verify(archive, package)
        require_empty_directory(options.unpack)
        unpack(archive, options.unpack)
    except (FetchError, OSError, zipfile.BadZipFile) as error:
        print(f"fetch_chromium: {error}", file=sys.stderr)
        return 1
    executable = (options.unpack / str(package["executable"])).resolve()
    if not executable.is_file():
        print(f"fetch_chromium: {executable} is missing after unpacking", file=sys.stderr)
        return 1
    print(f"verified {pin['version']} {options.platform} sha256 {package['sha256']}")
    print(executable)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
