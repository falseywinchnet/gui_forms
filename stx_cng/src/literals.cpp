#include "format.hpp"

#include <algorithm>
#include <cstring>

namespace stx_cng::detail {
namespace {

// Signed residual in [-128, 127] for any integer, modulo 256.
int wrap(int value) {
    unsigned reduced = static_cast<unsigned>(value + 128) & 255U;
    int result = static_cast<int>(reduced) - 128;
    return result;
}

// floor((second + parity + 1) / 3) for a numerator in [-127, 129]: adding
// 129 = 43 * 3 makes it nonnegative, so truncating division floors it.
int third_of(int second, int difference) {
    int result = (second + (difference & 1) + 130) / 3 - 43;
    return result;
}

// Reversible colour transform of three signed residuals (FORMAT.md). C++20
// right shift of a negative value is floor division by two.
void forward_colour(std::array<int, 4>& values) {
    int difference = wrap(values[0] - values[1]);
    int second = wrap(values[2] - values[1] - (difference >> 1));
    int centre = wrap(values[1] + (difference >> 1));
    int brightness = wrap(centre + third_of(second, difference));
    values[0] = brightness;
    values[1] = difference;
    values[2] = second;
}

void inverse_colour(std::array<int, 4>& values) {
    int centre = wrap(values[0] - third_of(values[2], values[1]));
    int green = wrap(centre - (values[1] >> 1));
    int red = wrap(green + values[1]);
    int blue = wrap(green + values[2] + (values[1] >> 1));
    values[0] = red;
    values[1] = green;
    values[2] = blue;
}

unsigned zigzag(int value) {
    unsigned mapped = value < 0 ? static_cast<unsigned>(-2 * value - 1) : static_cast<unsigned>(2 * value);
    return mapped;
}

int unzigzag(unsigned mapped) {
    int magnitude = static_cast<int>(mapped / 2);
    int value = (mapped & 1U) != 0 ? -magnitude - 1 : magnitude;
    return value;
}

template<std::size_t channels>
void collect(std::span<const std::uint8_t> source, std::span<const std::uint8_t> previous,
             std::span<const Sequence> sequences, std::span<std::uint8_t> samples,
             std::span<std::uint8_t> packed) {
    std::fill(packed.begin(), packed.end(), std::uint8_t{0});
    std::size_t literal_count = samples.size() / channels;
    std::size_t groups = (literal_count + 1) / 2;
    std::size_t row_bytes = previous.size();
    std::size_t position = 0;
    std::size_t written = 0;
    for (const Sequence& sequence : sequences) {
        std::size_t remaining = static_cast<std::size_t>(sequence.literals) * channels;
        while (remaining != 0) {
            std::size_t column = position % row_bytes;
            std::size_t count = std::min(remaining, row_bytes - column);
            const std::uint8_t* above = position < row_bytes
                ? previous.data() : source.data() + position - column - row_bytes;
            // Running left-minus-upper-left difference: prediction error is
            // (sample - above) - (left - upper_left).
            std::array<int, channels> left_difference{};
            if (column != 0) {
                for (std::size_t channel = 0; channel < channels; ++channel) {
                    left_difference[channel] = source[position - channels + channel] - above[column - channels + channel];
                }
            }
            for (std::size_t offset = 0; offset < count; offset += channels) {
                std::array<int, 4> values{};
                for (std::size_t channel = 0; channel < channels; ++channel) {
                    std::uint8_t sample = source[position + offset + channel];
                    int vertical = sample - above[column + offset + channel];
                    values[channel] = wrap(vertical - left_difference[channel]);
                    left_difference[channel] = vertical;
                    samples[written * channels + channel] = sample;
                }
                forward_colour(values);
                unsigned shift = static_cast<unsigned>((written & 1U) * 4);
                for (std::size_t channel = 0; channel < channels; ++channel) {
                    unsigned mapped = zigzag(values[channel]);
                    packed[(channel * 2) * groups + written / 2] |= static_cast<std::uint8_t>((mapped & 15U) << shift);
                    packed[(channel * 2 + 1) * groups + written / 2] |= static_cast<std::uint8_t>((mapped >> 4) << shift);
                }
                ++written;
            }
            position += count;
            remaining -= count;
        }
        position += static_cast<std::size_t>(sequence.length) * channels;
    }
}

template<std::size_t channels>
void unpack(std::span<const std::uint8_t> packed, std::span<std::uint8_t> residuals) {
    std::size_t count = residuals.size() / channels;
    std::size_t groups = (count + 1) / 2;
    for (std::size_t literal = 0; literal < count; ++literal) {
        unsigned shift = static_cast<unsigned>((literal & 1U) * 4);
        std::array<int, 4> values{};
        for (std::size_t channel = 0; channel < channels; ++channel) {
            unsigned low = (packed[(channel * 2) * groups + literal / 2] >> shift) & 15U;
            unsigned high = (packed[(channel * 2 + 1) * groups + literal / 2] >> shift) & 15U;
            values[channel] = unzigzag(low | (high << 4));
        }
        inverse_colour(values);
        for (std::size_t channel = 0; channel < channels; ++channel) {
            residuals[literal * channels + channel] = static_cast<std::uint8_t>(values[channel]);
        }
    }
}

void write_descriptor(std::span<std::uint8_t> storage, std::size_t& position, std::size_t value) {
    while (value >= 128) {
        --position;
        storage[position] = static_cast<std::uint8_t>((value & 127U) | 128U);
        value >>= 7;
    }
    --position;
    storage[position] = static_cast<std::uint8_t>(value);
}

// A descriptor read backward from `position`, never below `floor`: at most
// five seven-bit groups, least significant first, without a redundant zero
// final group. False when malformed.
bool read_descriptor(std::span<const std::uint8_t> raw, std::size_t& position,
                     std::size_t floor, std::uint64_t& value) {
    value = 0;
    for (unsigned shift = 0; shift <= 28; shift += 7) {
        if (position <= floor) {
            return false;
        }
        --position;
        unsigned byte = raw[position];
        value |= static_cast<std::uint64_t>(byte & 127U) << shift;
        if (byte < 128) {
            bool canonical = shift == 0 || byte != 0;
            return canonical;
        }
    }
    return false;
}

} // namespace

std::size_t packed_bytes(std::size_t literals, std::size_t channels) noexcept {
    std::size_t result = ((literals + 1) / 2) * 2 * channels;
    return result;
}

void collect_literals(std::span<const std::uint8_t> source, std::span<const std::uint8_t> previous,
                      std::size_t channels, std::span<const Sequence> sequences,
                      std::span<std::uint8_t> samples, std::span<std::uint8_t> packed) noexcept {
    if (channels == 3) {
        collect<3>(source, previous, sequences, samples, packed);
    } else {
        collect<4>(source, previous, sequences, samples, packed);
    }
}

std::size_t separate_runs(std::span<const std::uint8_t> packed, std::span<std::uint8_t> separated) noexcept {
    // Literal bytes fill the first packed.size() bytes; descriptors grow
    // backward from the end of the remaining packed.size() + 4. A segment of
    // n packed bytes adds at most n descriptor bytes (a stretch: one varint of
    // 2(n-1); a run, n >= 4: a varint of 2(n-1)+1 and its value), so the
    // descriptors never reach the literal region.
    std::span<std::uint8_t> descriptors = separated.subspan(packed.size());
    std::size_t descriptor_position = descriptors.size();
    std::size_t literal_count = 0;
    std::size_t consumed = 0;
    std::size_t position = 0;
    while (packed.size() - position >= minimum_run) {
        std::uint32_t word = 0;
        std::memcpy(&word, packed.data() + position, sizeof(word));
        // Adjacent octets are equal exactly when this is zero, in any byte order.
        if (((word ^ (word >> 8)) & 0x00ffffffU) != 0) {
            ++position;
            continue;
        }
        std::size_t begin = position;
        std::uint8_t value = packed[position];
        position += minimum_run;
        while (position < packed.size() && packed[position] == value) {
            ++position;
        }
        std::size_t literal_size = begin - consumed;
        if (literal_size != 0) {
            write_descriptor(descriptors, descriptor_position, 2 * (literal_size - 1));
            std::memcpy(separated.data() + literal_count, packed.data() + consumed, literal_size);
            literal_count += literal_size;
        }
        std::size_t run_size = position - begin;
        write_descriptor(descriptors, descriptor_position, 2 * (run_size - 1) + 1);
        --descriptor_position;
        descriptors[descriptor_position] = value;
        consumed = position;
    }
    if (consumed < packed.size()) {
        std::size_t literal_size = packed.size() - consumed;
        write_descriptor(descriptors, descriptor_position, 2 * (literal_size - 1));
        std::memcpy(separated.data() + literal_count, packed.data() + consumed, literal_size);
        literal_count += literal_size;
    }
    // Move the descriptors beside the literals; the ranges may overlap.
    std::size_t descriptor_count = descriptors.size() - descriptor_position;
    std::memmove(separated.data() + literal_count, descriptors.data() + descriptor_position, descriptor_count);
    std::size_t result = literal_count + descriptor_count;
    return result;
}

bool restore_runs(std::span<const std::uint8_t> separated, std::span<std::uint8_t> packed) noexcept {
    std::size_t back = separated.size();
    std::size_t front = 0;
    std::size_t written = 0;
    // Every descriptor emits at least one byte, so this loop is bounded by
    // packed.size() iterations.
    while (written < packed.size()) {
        std::uint64_t code = 0;
        if (!read_descriptor(separated, back, front, code)) {
            return false;
        }
        std::uint64_t length = (code >> 1) + 1;
        if (length > packed.size() - written) {
            return false;
        }
        std::size_t count = static_cast<std::size_t>(length);
        if ((code & 1U) != 0) {
            if (count < minimum_run || back <= front) {
                return false;
            }
            --back;
            std::fill_n(packed.data() + written, count, separated[back]);
        } else {
            if (count > back - front) {
                return false;
            }
            std::memcpy(packed.data() + written, separated.data() + front, count);
            front += count;
        }
        written += count;
    }
    bool exact = front == back;
    return exact;
}

bool unpack_literals(std::span<const std::uint8_t> packed, std::size_t channels,
                     std::span<std::uint8_t> residuals) noexcept {
    std::size_t count = residuals.size() / channels;
    std::size_t groups = (count + 1) / 2;
    if (count % 2 != 0) {
        // The odd final literal uses only the low nibble of each plane's last
        // byte; the encoder zeroes the high nibble.
        for (std::size_t plane = 0; plane < 2 * channels; ++plane) {
            if ((packed[plane * groups + groups - 1] >> 4) != 0) {
                return false;
            }
        }
    }
    if (channels == 3) {
        unpack<3>(packed, residuals);
    } else {
        unpack<4>(packed, residuals);
    }
    return true;
}

} // namespace stx_cng::detail
