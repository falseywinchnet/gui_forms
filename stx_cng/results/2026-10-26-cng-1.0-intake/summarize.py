"""Class summary of bench.tsv, with the 2026-10-22 CNG 0.1 sizes beside it.

usage: summarize.py <bench.tsv> <2026-10-22 bench.tsv> <threads>
Bytes are sums over each class; speeds are medians of per-image ratios.
"""
import csv
import statistics
import sys

ORDER = ['icon16', 'icon24', 'icon32', 'icon48', 'icon64', 'icon96', 'icon256', 'gallery', 'screenshot', 'text_ui',
         'photo_crop', 'photo_full']


def rows_of(path: str) -> list[dict[str, str]]:
    with open(path, encoding='utf-8', newline='') as handle:
        reader = csv.DictReader(handle, delimiter='\t')
        rows = [row for row in reader if row.get('width') not in (None, '', 'not_png')]
    return rows


def main() -> int:
    rows = rows_of(sys.argv[1])
    old = rows_of(sys.argv[2])
    threads = sys.argv[3]
    old_bytes: dict[tuple[str, str, str, str], int] = {}
    for row in old:
        old_bytes[(row['category'], row['name'], row['width'], row['height'])] = int(row['cng_bytes'])
    print(f'| class | n | CNG/PNG6 bytes | CNG/PNG9 bytes | CNG 1.0/0.1 bytes | decode PNG/CNG, 1 thread '
          f'| decode PNG/CNG, {threads} threads | CNG {threads} threads/1 |')
    print('|---|---:|---:|---:|---:|---:|---:|---:|')
    total = {'png6': 0, 'png9': 0, 'cng': 0, 'old_cng': 0, 'old_new': 0}
    for category in ORDER:
        members = [row for row in rows if row['category'] == category]
        if not members:
            continue
        png6 = sum(int(row['png6_bytes']) for row in members)
        png9 = sum(int(row['png9_bytes']) for row in members)
        cng = sum(int(row['cng_bytes']) for row in members)
        matched_old = 0
        matched_new = 0
        for row in members:
            name = row['name'].split('_', 1)[1]
            key = (category, name, row['width'], row['height'])
            if key in old_bytes:
                matched_old += old_bytes[key]
                matched_new += int(row['cng_bytes'])
        one = statistics.median(float(row['png_decode_us']) / float(row['cng_decode_us_t1']) for row in members)
        many = statistics.median(float(row['png_decode_us']) / float(row[f'cng_decode_us_t{threads}']) for row in members)
        scaling = statistics.median(float(row['cng_decode_us_t1']) / float(row[f'cng_decode_us_t{threads}']) for row in members)
        versus_old = f'{matched_new / matched_old:.3f}' if matched_old else 'n/a'
        print(f'| {category} | {len(members)} | {cng / png6:.3f} | {cng / png9:.3f} | {versus_old} | {one:.2f}x | {many:.2f}x '
              f'| {scaling:.2f}x |')
        total['png6'] += png6
        total['png9'] += png9
        total['cng'] += cng
        total['old_cng'] += matched_old
        total['old_new'] += matched_new
    count = len(rows)
    print(f'| all | {count} | {total["cng"] / total["png6"]:.3f} | {total["cng"] / total["png9"]:.3f} | '
          f'{total["old_new"] / total["old_cng"]:.3f} | | | |')
    png_time = sum(float(row['png_decode_us']) for row in rows)
    one_time = sum(float(row['cng_decode_us_t1']) for row in rows)
    many_time = sum(float(row[f'cng_decode_us_t{threads}']) for row in rows)
    print()
    print(f'Summed decode time: PNG {png_time / 1000:.1f} ms, CNG 1 thread {one_time / 1000:.1f} ms, '
          f'CNG {threads} threads {many_time / 1000:.1f} ms.')
    largest_encoder = max(int(row['cng_encoder_workspace']) for row in rows)
    largest_decoder = max(int(row['cng_decoder_workspace']) for row in rows)
    print(f'Largest workspaces: encoder {largest_encoder:,} B, decoder {largest_decoder:,} B.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
