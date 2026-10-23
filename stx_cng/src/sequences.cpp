#include "format.hpp"

#include <algorithm>
#include <cstring>

namespace stx_cng::detail {
namespace {

// Pixel bytes as one little-endian word (alpha zero for RGB), for equality.
template<std::size_t channels>
std::uint32_t pixel_word(std::span<const std::uint8_t> source, std::size_t pixel) {
    std::size_t offset = pixel * channels;
    std::uint32_t value = source[offset];
    value |= static_cast<std::uint32_t>(source[offset + 1]) << 8;
    value |= static_cast<std::uint32_t>(source[offset + 2]) << 16;
    if constexpr (channels == 4) {
        value |= static_cast<std::uint32_t>(source[offset + 3]) << 24;
    }
    return value;
}

// Pixels from `pixel` onward that the gradient prediction reproduces exactly.
// A zero prediction error means the vertical difference (pixel minus the pixel
// above) stays constant along the row; at each row start the reference
// difference is zero. A rejected short run costs at most three comparisons and
// accepted runs are skipped by the caller, so the lookahead is linear overall.
template<std::size_t channels>
std::size_t prediction_length(std::span<const std::uint8_t> source,
                              std::span<const std::uint8_t> previous,
                              std::size_t pixel) {
    std::size_t row_bytes = previous.size();
    std::size_t begin = pixel * channels;
    std::size_t offset = begin;
    while (offset < source.size()) {
        std::size_t column = offset % row_bytes;
        std::size_t row_begin = offset - column;
        const std::uint8_t* above = row_begin == 0
            ? previous.data() : source.data() + row_begin - row_bytes;
        std::array<std::uint8_t, channels> difference{};
        if (column != 0) {
            for (std::size_t channel = 0; channel < channels; ++channel) {
                std::uint8_t left = source[offset - channels + channel];
                std::uint8_t upper_left = above[column - channels + channel];
                difference[channel] = static_cast<std::uint8_t>(left - upper_left);
            }
        }
        std::size_t row_end = row_begin + row_bytes;
        while (offset < row_end) {
            bool equal = true;
            for (std::size_t channel = 0; channel < channels; ++channel) {
                std::uint8_t vertical = static_cast<std::uint8_t>(source[offset + channel] - above[column + channel]);
                equal = equal && vertical == difference[channel];
            }
            if (!equal) {
                std::size_t length = (offset - begin) / channels;
                return length;
            }
            offset += channels;
            column += channels;
        }
    }
    std::size_t length = (offset - begin) / channels;
    return length;
}

// Length of the match between pixels at `earlier` and `pixel`, known to agree
// on their first minimum_run pixels, bounded by the band end.
template<std::size_t channels>
std::size_t match_length(std::span<const std::uint8_t> source, std::size_t count,
                         std::size_t earlier, std::size_t pixel) {
    std::size_t length = minimum_run;
    while (pixel + length < count) {
        std::size_t batch = std::min<std::size_t>(256, count - pixel - length);
        const std::uint8_t* earlier_bytes = source.data() + (earlier + length) * channels;
        const std::uint8_t* current_bytes = source.data() + (pixel + length) * channels;
        if (std::memcmp(earlier_bytes, current_bytes, batch * channels) == 0) {
            length += batch;
            continue;
        }
        // The batch holds a difference, so this scan stops inside it.
        while (pixel_word<channels>(source, earlier + length) == pixel_word<channels>(source, pixel + length)) {
            ++length;
        }
        break;
    }
    return length;
}

std::size_t hash_slot(std::uint32_t first, std::uint32_t second) {
    std::uint64_t mixed = static_cast<std::uint64_t>(first) * 0x9e3779b1U;
    mixed ^= static_cast<std::uint64_t>(second) * 0x85ebca77U;
    std::uint64_t folded = mixed ^ (mixed >> 16) ^ (mixed >> 32) ^ (mixed >> 48);
    std::size_t slot = static_cast<std::size_t>(folded & (index_slots - 1));
    return slot;
}

// Greedy search: at each pixel, the longer of the exact prediction run and the
// hashed earlier match wins (prediction on ties); shorter than minimum_run
// leaves the pixel a literal.
template<std::size_t channels>
std::size_t search(std::span<const std::uint8_t> source, std::span<const std::uint8_t> previous,
                   std::span<std::int32_t> index, std::span<Sequence> output,
                   std::size_t& literal_pixels) {
    std::size_t count = source.size() / channels;
    std::fill(index.begin(), index.end(), -1);
    std::size_t records = 0;
    std::size_t pixel = 0;
    std::size_t literal_begin = 0;
    literal_pixels = 0;
    while (pixel + minimum_run - 1 < count) {
        std::uint32_t first = pixel_word<channels>(source, pixel);
        std::uint32_t second = pixel_word<channels>(source, pixel + 1);
        std::size_t slot = hash_slot(first, second);
        std::int32_t prior = index[slot];
        index[slot] = static_cast<std::int32_t>(pixel);
        std::size_t predicted = prediction_length<channels>(source, previous, pixel);
        std::size_t length = 0;
        std::size_t distance = 0;
        if (prior >= 0 && predicted < count - pixel) {
            std::size_t earlier = static_cast<std::size_t>(prior);
            bool candidate = pixel - earlier <= maximum_distance
                && pixel_word<channels>(source, earlier) == first
                && pixel_word<channels>(source, earlier + 1) == second
                && pixel_word<channels>(source, earlier + 2) == pixel_word<channels>(source, pixel + 2)
                && pixel_word<channels>(source, earlier + 3) == pixel_word<channels>(source, pixel + 3);
            if (candidate) {
                length = match_length<channels>(source, count, earlier, pixel);
                distance = pixel - earlier;
            }
        }
        if (predicted >= minimum_run && predicted >= length) {
            length = predicted;
            distance = 0;
        }
        if (length < minimum_run) {
            ++pixel;
            continue;
        }
        std::size_t literals = pixel - literal_begin;
        output[records] = Sequence{
            .literals = static_cast<std::uint32_t>(literals),
            .length = static_cast<std::uint32_t>(length),
            .distance = static_cast<std::uint32_t>(distance)};
        ++records;
        literal_pixels += literals;
        pixel += length;
        literal_begin = pixel;
    }
    std::size_t tail = count - literal_begin;
    output[records] = Sequence{.literals = static_cast<std::uint32_t>(tail)};
    ++records;
    literal_pixels += tail;
    return records;
}

} // namespace

std::size_t find_sequences(std::span<const std::uint8_t> source,
                           std::span<const std::uint8_t> previous,
                           std::size_t channels,
                           std::span<std::int32_t> index,
                           std::span<Sequence> output,
                           std::size_t& literal_pixels) noexcept {
    std::size_t count = source.size() / channels;
    if (count < minimum_run) {
        output[0] = Sequence{.literals = static_cast<std::uint32_t>(count)};
        literal_pixels = count;
        return 1;
    }
    std::size_t records = 0;
    if (channels == 3) {
        records = search<3>(source, previous, index, output, literal_pixels);
    } else {
        records = search<4>(source, previous, index, output, literal_pixels);
    }
    return records;
}

void pack_sequences(std::span<const Sequence> sequences, std::span<std::uint8_t> raw) noexcept {
    std::size_t count = sequences.size();
    for (std::size_t record = 0; record < count; ++record) {
        std::array<std::uint32_t, 3> fields{sequences[record].literals, sequences[record].length, sequences[record].distance};
        for (std::size_t field = 0; field < 3; ++field) {
            for (std::size_t byte = 0; byte < 4; ++byte) {
                raw[(field * 4 + byte) * count + record] = static_cast<std::uint8_t>(fields[field] >> (8 * byte));
            }
        }
    }
}

void unpack_sequences(std::span<const std::uint8_t> raw, std::span<Sequence> sequences) noexcept {
    std::size_t count = sequences.size();
    for (std::size_t record = 0; record < count; ++record) {
        std::array<std::uint32_t, 3> fields{};
        for (std::size_t field = 0; field < 3; ++field) {
            for (std::size_t byte = 0; byte < 4; ++byte) {
                fields[field] |= static_cast<std::uint32_t>(raw[(field * 4 + byte) * count + record]) << (8 * byte);
            }
        }
        sequences[record] = Sequence{.literals = fields[0], .length = fields[1], .distance = fields[2]};
    }
}

bool valid_sequences(std::span<const Sequence> sequences, std::size_t band_pixels,
                     std::size_t literal_pixels) noexcept {
    if (sequences.empty()) {
        return false;
    }
    std::size_t produced = 0;
    std::size_t consumed = 0;
    std::size_t last = sequences.size() - 1;
    for (std::size_t record = 0; record < sequences.size(); ++record) {
        const Sequence& sequence = sequences[record];
        if (sequence.literals > band_pixels - produced || sequence.literals > literal_pixels - consumed) {
            return false;
        }
        produced += sequence.literals;
        consumed += sequence.literals;
        if (record == last) {
            // The terminal record carries only trailing literals.
            if (sequence.length != 0 || sequence.distance != 0) {
                return false;
            }
            continue;
        }
        if (sequence.length < minimum_run || sequence.length > band_pixels - produced
            || sequence.distance > produced || sequence.distance > maximum_distance) {
            return false;
        }
        produced += sequence.length;
    }
    bool exact = produced == band_pixels && consumed == literal_pixels;
    return exact;
}

} // namespace stx_cng::detail
