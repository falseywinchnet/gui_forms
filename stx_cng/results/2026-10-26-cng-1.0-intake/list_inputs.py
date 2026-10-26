"""Writes inputs.tsv for the corpus and prints the 2026-10-22 inputs it lacks.

usage: list_inputs.py <corpus dir> <2026-10-22 inputs.tsv> <output inputs.tsv>
"""
import hashlib
import pathlib
import sys


def main() -> int:
    corpus = pathlib.Path(sys.argv[1])
    found: dict[str, tuple[str, str, int]] = {}
    for path in sorted(corpus.glob('*/*')):
        data = path.read_bytes()
        digest = hashlib.sha256(data).hexdigest()
        name = path.name.split('_', 1)[1]
        found[digest] = (path.parent.name, name, len(data))
    with open(sys.argv[3], 'w', encoding='utf-8', newline='\n') as out:
        out.write('category\tname\tbytes\tsha256\n')
        for digest, (category, name, size) in sorted(found.items(), key=lambda item: (item[1][0], item[1][1], item[0])):
            out.write(f'{category}\t{name}\t{size}\t{digest}\n')
    missing = 0
    for line in open(sys.argv[2], encoding='utf-8'):
        fields = line.rstrip('\n').split('\t')
        if len(fields) == 4 and fields[3] not in found:
            missing += 1
            print(f'missing\t{fields[0]}\t{fields[1]}\t{fields[2]}\t{fields[3]}')
    print(f'{len(found)} found, {missing} missing')
    return 0


if __name__ == '__main__':
    sys.exit(main())
