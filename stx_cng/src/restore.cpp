#include "format.hpp"

#include <algorithm>
#include <cstring>

namespace stx_cng::detail {
namespace {

// Per-byte addition and subtraction modulo 256 in one 32-bit word. The low
// seven bits of each byte cannot carry into the next byte; the XOR restores
// each byte's high bit and discards its carry or borrow.
std::uint32_t add_bytes(std::uint32_t left, std::uint32_t right) {
    constexpr std::uint32_t low = 0x7f7f7f7fU;
    constexpr std::uint32_t high = 0x80808080U;
    std::uint32_t sum = (left & low) + (right & low);
    std::uint32_t result = sum ^ ((left ^ right) & high);
    return result;
}

std::uint32_t subtract_bytes(std::uint32_t left, std::uint32_t right) {
    constexpr std::uint32_t low = 0x7f7f7f7fU;
    constexpr std::uint32_t high = 0x80808080U;
    std::uint32_t difference = (left | high) - (right & low);
    std::uint32_t result = difference ^ (~(left ^ right) & high);
    return result;
}

std::uint32_t load_word(const std::uint8_t* source) {
    std::uint32_t word = 0;
    std::memcpy(&word, source, sizeof(word));
    return word;
}

void store_word(std::uint8_t* destination, std::uint32_t word) {
    std::memcpy(destination, &word, sizeof(word));
}

// The row above `position` in the band: the saved previous row for the
// band's first row, otherwise the output row before it.
const std::uint8_t* row_above(std::span<const std::uint8_t> previous, std::span<std::uint8_t> output,
                              std::size_t position, std::size_t column) {
    std::size_t row_bytes = previous.size();
    const std::uint8_t* above = position < row_bytes
        ? previous.data() : output.data() + position - column - row_bytes;
    return above;
}

// Gradient-reconstructs `count` bytes (within one row) at `position` from
// residual literals. The running sum is the vertical difference, so the
// prior row is read without a horizontal recurrence through the output.
template<std::size_t channels>
void residual_span(std::span<const std::uint8_t> literals, std::size_t consumed,
                   std::span<const std::uint8_t> previous, std::span<std::uint8_t> output,
                   std::size_t position, std::size_t count) {
    std::size_t column = position % previous.size();
    const std::uint8_t* above = row_above(previous, output, position, column);
    if constexpr (channels == 4) {
        std::uint32_t sum = 0;
        if (column != 0) {
            sum = subtract_bytes(load_word(output.data() + position - 4), load_word(above + column - 4));
        }
        for (std::size_t offset = 0; offset < count; offset += 4) {
            sum = add_bytes(sum, load_word(literals.data() + consumed + offset));
            store_word(output.data() + position + offset, add_bytes(sum, load_word(above + column + offset)));
        }
    } else {
        std::array<unsigned, channels> sum{};
        if (column != 0) {
            for (std::size_t channel = 0; channel < channels; ++channel) {
                sum[channel] = static_cast<unsigned>(output[position - channels + channel]) - above[column - channels + channel];
            }
        }
        for (std::size_t offset = 0; offset < count; offset += channels) {
            for (std::size_t channel = 0; channel < channels; ++channel) {
                sum[channel] += literals[consumed + offset + channel];
                unsigned value = sum[channel] + above[column + offset + channel];
                output[position + offset + channel] = static_cast<std::uint8_t>(value);
            }
        }
    }
}

// Exact prediction for `count` bytes within one row: each pixel is the pixel
// above plus the constant left-minus-upper-left difference at the span start.
template<std::size_t channels>
void prediction_span(std::span<const std::uint8_t> previous, std::span<std::uint8_t> output,
                     std::size_t position, std::size_t count) {
    std::size_t column = position % previous.size();
    const std::uint8_t* above = row_above(previous, output, position, column);
    if (column == 0) {
        std::memcpy(output.data() + position, above, count);
        return;
    }
    if constexpr (channels == 4) {
        std::uint32_t delta = subtract_bytes(load_word(output.data() + position - 4), load_word(above + column - 4));
        for (std::size_t offset = 0; offset < count; offset += 4) {
            store_word(output.data() + position + offset, add_bytes(load_word(above + column + offset), delta));
        }
    } else {
        std::array<int, channels> delta{};
        for (std::size_t channel = 0; channel < channels; ++channel) {
            delta[channel] = output[position - channels + channel] - above[column - channels + channel];
        }
        for (std::size_t offset = 0; offset < count; offset += channels) {
            for (std::size_t channel = 0; channel < channels; ++channel) {
                int value = above[column + offset + channel] + delta[channel];
                output[position + offset + channel] = static_cast<std::uint8_t>(value);
            }
        }
    }
}

template<std::size_t channels>
void restore(std::span<const Sequence> sequences, std::span<const std::uint8_t> literals,
             LiteralCoding coding, std::span<const std::uint8_t> previous, std::span<std::uint8_t> output) {
    std::size_t row_bytes = previous.size();
    std::size_t position = 0;
    std::size_t consumed = 0;
    for (const Sequence& sequence : sequences) {
        std::size_t literal_bytes = static_cast<std::size_t>(sequence.literals) * channels;
        if (coding == LiteralCoding::stored) {
            if (literal_bytes != 0) {
                std::memcpy(output.data() + position, literals.data() + consumed, literal_bytes);
            }
            position += literal_bytes;
            consumed += literal_bytes;
        } else {
            while (literal_bytes != 0) {
                std::size_t count = std::min(literal_bytes, row_bytes - position % row_bytes);
                residual_span<channels>(literals, consumed, previous, output, position, count);
                position += count;
                consumed += count;
                literal_bytes -= count;
            }
        }
        std::size_t length = static_cast<std::size_t>(sequence.length) * channels;
        std::size_t distance = static_cast<std::size_t>(sequence.distance) * channels;
        if (length != 0 && distance == 0) {
            while (length != 0) {
                std::size_t count = std::min(length, row_bytes - position % row_bytes);
                prediction_span<channels>(previous, output, position, count);
                position += count;
                length -= count;
            }
            continue;
        }
        // Copies grow from the reconstructed prefix: each memcpy is disjoint,
        // including distance-one repeats, while the available prefix doubles.
        std::size_t copied = 0;
        while (copied < length) {
            std::size_t count = std::min(length - copied, distance + copied);
            std::memcpy(output.data() + position + copied, output.data() + position - distance, count);
            copied += count;
        }
        position += length;
    }
}

} // namespace

void restore_band(std::span<const Sequence> sequences, std::span<const std::uint8_t> literals,
                  LiteralCoding coding, std::span<const std::uint8_t> previous,
                  std::size_t channels, std::span<std::uint8_t> output) noexcept {
    if (channels == 3) {
        restore<3>(sequences, literals, coding, previous, output);
    } else {
        restore<4>(sequences, literals, coding, previous, output);
    }
}

} // namespace stx_cng::detail
