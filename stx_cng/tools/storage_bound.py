# Owned-array maxima of stx_cng's encoder and decoder (src/codec.cpp,
# reserve_encoder and reserve_decoder) over every admitted width and both
# channel counts, at the largest band height the extent allows. Zstandard
# contexts are excluded; tests/limits.cpp bounds them separately.
# Usage: python3 -B tools/storage_bound.py
BUDGET = 1048576
INDEX_BYTES = 65536 * 4
RECORD_BYTES = 12


def arrays(width, channels):
    row = width * channels
    rows = (BUDGET - channels) // row
    height = min(rows, (1 << 26) // width, 16384)
    band = min(rows, height) * row
    pixels = band // channels
    records = pixels // 4 + 1
    packed = ((pixels + 1) // 2) * 2 * channels
    separated = 2 * packed + 4
    # Raw and unpacked sequence records, the band's literals, packed and
    # run-separated literal bytes.
    common = records * RECORD_BYTES * 2 + band + packed + separated
    encoder = INDEX_BYTES + row + common
    # Strided input is copied into a band of its own.
    encoder_strided = encoder + band
    decoder = row + common
    return encoder, encoder_strided, decoder


def main():
    names = ("encoder arrays (tight)", "encoder arrays (strided)", "decoder arrays")
    best = [(0, None), (0, None), (0, None)]
    for channels in (3, 4):
        for width in range(1, 16385):
            sizes = arrays(width, channels)
            for index in range(3):
                if sizes[index] > best[index][0]:
                    best[index] = (sizes[index], (width, channels))
    for index in range(3):
        print(names[index], best[index][0], "bytes at width, channels", best[index][1])


if __name__ == "__main__":
    main()
