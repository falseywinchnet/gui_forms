"""Rebuild the 2026-10-22 intake corpus by SHA-256 from local files.

usage: find_corpus.py <inputs.tsv> <out dir> <search root>...
Every file under the roots whose SHA-256 matches a row of inputs.tsv is
copied to <out dir>/<category>/<name>; rows not found are listed.
"""
import hashlib
import pathlib
import shutil
import sys


def main() -> int:
    inputs = pathlib.Path(sys.argv[1])
    out = pathlib.Path(sys.argv[2])
    roots = [pathlib.Path(root) for root in sys.argv[3:]]
    wanted: dict[str, list[tuple[str, str]]] = {}
    for line in inputs.read_text(encoding='utf-8').splitlines():
        fields = line.split('\t')
        if len(fields) < 4:
            continue
        wanted.setdefault(fields[3], []).append((fields[0], fields[1]))
    sizes = set()
    for line in inputs.read_text(encoding='utf-8').splitlines():
        fields = line.split('\t')
        if len(fields) >= 4:
            sizes.add(int(fields[2]))
    found: set[str] = set()
    for root in roots:
        for path in root.rglob('*'):
            try:
                if not path.is_file() or path.stat().st_size not in sizes:
                    continue
                digest = hashlib.sha256(path.read_bytes()).hexdigest()
            except OSError:
                continue
            if digest in wanted and digest not in found:
                found.add(digest)
                for category, name in wanted[digest]:
                    target = out / category / (digest[:12] + '_' + name)
                    target.parent.mkdir(parents=True, exist_ok=True)
                    shutil.copyfile(path, target)
    missing = [(rows, digest) for digest, rows in wanted.items() if digest not in found]
    print(f'{len(found)} of {len(wanted)} distinct inputs found')
    for rows, digest in missing:
        print('missing', digest, rows)
    return 0


if __name__ == '__main__':
    sys.exit(main())
