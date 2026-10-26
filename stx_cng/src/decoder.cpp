// The CNG 1.0 decoder (FORMAT.md sections 6 to 10) and the CngDecoder owner.
#include "container.hpp"
#include "decoder.hpp"
#include "tables.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <functional>

namespace stx_cng::detail {
namespace {

// ---------------------------------------------------------------- rANS streams (section 6)
struct RangeDecoder final {
    std::span<const std::uint8_t> data{};
    std::size_t position = 4;
    std::uint32_t state = 0;
    bool failed = false;
};

void start_range(RangeDecoder& range, std::span<const std::uint8_t> stream) noexcept {
    range.data = stream;
    range.position = 4;
    range.state = 0;
    range.failed = stream.size() < 4;
    if (!range.failed) {
        range.state = read_u32(stream, 0);
    }
}

void renormalize(RangeDecoder& range, std::uint32_t next) noexcept {
    while (next < lower_bound) {
        if (range.position >= range.data.size()) {
            range.failed = true;
            next = lower_bound;
            break;
        }
        next = (next << 8) | range.data[range.position];
        ++range.position;
    }
    range.state = next;
}

std::uint32_t decode_symbol(RangeDecoder& range, const DecodeTable& table) noexcept {
    if (!table.used) {
        range.failed = true;
        return 0;
    }
    std::uint32_t slot = range.state & (total_frequency - 1);
    std::uint32_t symbol = table.slot[slot];
    std::uint32_t next = table.frequency[symbol] * (range.state >> precision) + slot - table.start[symbol];
    renormalize(range, next);
    return symbol;
}

std::uint32_t decode_bits(RangeDecoder& range, std::uint32_t count) noexcept {
    if (count == 0) {
        return 0;
    }
    if (count > maximum_raw_bits) {
        range.failed = true;
        return 0;
    }
    std::uint32_t slot = range.state & (total_frequency - 1);
    std::uint32_t frequency = total_frequency >> count;
    std::uint32_t value = slot / frequency;
    std::uint32_t next = frequency * (range.state >> precision) + slot - value * frequency;
    renormalize(range, next);
    return value;
}

// ---------------------------------------------------------------- distances (section 7.6)
// The last three distances used, most recent first.
struct Repeats final {
    std::array<std::uint32_t, repeats> distance{};
};

void reset_repeats(Repeats& list, std::uint32_t width) noexcept {
    list.distance = {1, width, 2};
}

// Moves `used` to the front; a new distance pushes the last one out.
void use_distance(Repeats& list, std::uint32_t used) noexcept {
    std::uint32_t found = repeats - 1;
    for (std::uint32_t index = 0; index < repeats; ++index) {
        if (list.distance[index] == used) {
            found = index;
            break;
        }
    }
    for (std::uint32_t index = found; index > 0; --index) {
        list.distance[index] = list.distance[index - 1];
    }
    list.distance[0] = used;
}

// ---------------------------------------------------------------- tile state (section 7)
// Activity classes: a pixel's three energies (green |g|, blue max(|g|, |b|),
// red max(|g|, |r|)), each at most 255, are packed sixteen bits apart in one
// 64-bit word. Six of them sum to at most 1,530, so the fields add without
// carries and one word addition sums all three planes; a 2,048-entry table
// maps a field's sum to its class.
constexpr std::uint32_t energy_field_bits = 16;
constexpr std::uint64_t energy_field_mask = (std::uint64_t{1} << energy_field_bits) - 1;
constexpr std::uint32_t class_table_entries = 2048;

constexpr std::array<std::uint8_t, class_table_entries> make_classes() noexcept {
    std::array<std::uint8_t, class_table_entries> table{};
    for (std::uint32_t activity = 0; activity < class_table_entries; ++activity) {
        std::uint32_t level = (activity > 0 ? 1U : 0U) + (activity > 3 ? 1U : 0U) + (activity > 12 ? 1U : 0U)
            + (activity > 48 ? 1U : 0U);
        table[activity] = static_cast<std::uint8_t>(level);
    }
    return table;
}
constexpr std::array<std::uint8_t, class_table_entries> class_of = make_classes();

// The green bucket of the blue and red contexts by green residual + 128:
// 0 for zero, 1 for |g| at most 2, otherwise 2.
constexpr std::array<std::uint8_t, 256> make_green_buckets() noexcept {
    std::array<std::uint8_t, 256> table{};
    for (std::int32_t green = -128; green < 128; ++green) {
        std::uint8_t bucket = green == 0 ? 0 : (green >= -2 && green <= 2 ? 1 : 2);
        table[static_cast<std::size_t>(green + 128)] = bucket;
    }
    return table;
}
constexpr std::array<std::uint8_t, 256> green_bucket_of = make_green_buckets();

// Plane c's residual in a pixel's packed residual word.
std::int32_t lane(std::uint32_t packed, std::uint32_t plane) noexcept {
    std::int32_t residual = static_cast<std::int8_t>(static_cast<std::uint8_t>(packed >> (8 * plane)));
    return residual;
}

std::uint64_t energy_word(std::int32_t green, std::int32_t blue, std::int32_t red) noexcept {
    std::uint64_t green_energy = static_cast<std::uint64_t>(green >= 0 ? green : -green);
    std::uint64_t blue_magnitude = static_cast<std::uint64_t>(blue >= 0 ? blue : -blue);
    std::uint64_t red_magnitude = static_cast<std::uint64_t>(red >= 0 ? red : -red);
    std::uint64_t blue_energy = std::max(blue_magnitude, green_energy);
    std::uint64_t red_energy = std::max(red_magnitude, green_energy);
    std::uint64_t word = green_energy | (blue_energy << energy_field_bits) | (red_energy << (2 * energy_field_bits));
    return word;
}

// The decoding state of one tile. It owns no storage (the storage is the
// caller's TileWorkspace), so its address never escapes the pixel loop and
// the compiler keeps its fields in registers across the byte stores.
struct TileState final {
    const DecodeTables* tables = nullptr;
    std::array<RangeDecoder, maximum_channels> streams{};
    std::array<std::uint32_t, specials> special{};
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t channels = 0;
    std::uint32_t pixels = 0;
    // Per pixel: the plane residuals as bytes, plane c in bits 8c to 8c + 7.
    std::uint32_t* residual = nullptr;
    // Three rows of width + 3 energy words: two zero words before each row and
    // one after, so neighbours outside the tile read zero without tests.
    std::uint64_t* ring = nullptr;
    std::uint64_t* energy_row = nullptr;
    // Per column: U + UL + UR of row y - 1 and UU of row y - 2.
    std::uint64_t* above = nullptr;
    Repeats whole_repeats{};
    std::array<Repeats, maximum_channels> plane_repeats{};
    std::uint32_t whole_left = 0;
    std::uint32_t whole_distance = 0;
    std::array<std::uint32_t, maximum_channels> left{};
    std::array<std::uint32_t, maximum_channels> distance{};
    // Per green value: the last earlier pixel with it, or -1.
    std::array<std::int32_t, 256> last{};
    bool failed = false;
    std::uint8_t* destination = nullptr;
    std::size_t row_bytes = 0;

    std::uint64_t* ring_row(std::uint32_t slot) const noexcept {
        std::uint64_t* row = ring + static_cast<std::size_t>(slot) * (width + 3) + 2;
        return row;
    }

    // Rows y - 1 and y - 2 occupy the other two ring slots; before row 2 those
    // slots still hold zeros.
    void prepare_row(std::uint32_t y) noexcept {
        energy_row = ring_row(y % 3);
        const std::uint64_t* previous = ring_row((y + 2) % 3);
        const std::uint64_t* earlier = ring_row((y + 1) % 3);
        for (std::uint32_t x = 0; x < width; ++x) {
            const std::uint64_t* column = previous + x;
            above[x] = column[-1] + column[0] + column[1] + earlier[x];
        }
    }

    std::uint32_t symbol(std::uint32_t plane, std::uint32_t context) noexcept {
        const DecodeTable& table = (*tables)[context];
        std::uint32_t value = decode_symbol(streams[plane], table);
        if (value == table.escape) {
            value = decode_bits(streams[plane], raw_bits(context));
        }
        return value;
    }

    // Reads a match's length and distance from plane `plane`'s stream at
    // pixel i; returns the distance (0 on failure) and sets `length`.
    std::uint32_t match(std::uint32_t plane, std::uint32_t code, Repeats& list, std::uint32_t i, std::uint32_t& length) noexcept {
        if (code >= length_codes) {
            failed = true;
            return 0;
        }
        RangeDecoder& range = streams[plane];
        length = minimum_length + unprefix(code, decode_bits(range, code_bits(code)));
        std::uint32_t distance_symbol = symbol(plane, distance_context);
        std::uint32_t used = 0;
        if (distance_symbol < repeats) {
            used = list.distance[distance_symbol];
        } else if (distance_symbol < repeats + specials) {
            used = special[distance_symbol - repeats];
        } else {
            std::uint32_t distance_code = distance_symbol - repeats - specials;
            if (distance_code >= length_codes) {
                failed = true;
                return 0;
            }
            used = 1 + unprefix(distance_code, decode_bits(range, code_bits(distance_code)));
        }
        if (used < 1 || used > i || length > pixels - i) {
            failed = true;
            return 0;
        }
        use_distance(list, used);
        return used;
    }

    bool streams_failed() const noexcept {
        bool any = failed;
        for (std::uint32_t plane = 0; plane < channels; ++plane) {
            any = any || streams[plane].failed;
        }
        return any;
    }

    // A blue, red or alpha symbol: a literal, or the start of that plane's
    // own match.
    bool plane_symbol(std::uint32_t plane, std::uint32_t context, std::uint32_t i, std::uint32_t pixel_class, std::int32_t green,
                      std::int32_t& out) noexcept {
        std::uint32_t value = symbol(plane, context);
        if (value >= plane_length0) {
            std::uint32_t length = 0;
            distance[plane] = match(plane, value - plane_length0, plane_repeats[plane], i, length);
            if (failed) {
                return false;
            }
            left[plane] = length - 1;
            out = lane(residual[i - distance[plane]], plane);
            return true;
        }
        std::int32_t literal = unzig(value);
        out = plane != alpha_plane && pixel_class >= gate_class ? wrap(literal + green) : literal;
        return true;
    }

    // Stores pixel i from its predictions and residuals: samples, residuals,
    // energies and the cache entry. Stream failures are checked once per row:
    // every decoded symbol is bounded by its table, and match extents are
    // checked where they are read.
    template<std::uint32_t Channels>
    void finish(std::uint32_t x, std::uint32_t i, std::uint8_t* row, const std::array<std::int32_t, 4>& predicted,
                const std::array<std::int32_t, 4>& residuals) noexcept {
        std::uint32_t packed = 0;
        for (std::uint32_t plane = 0; plane < Channels; ++plane) {
            row[static_cast<std::size_t>(x) * Channels + plane] = static_cast<std::uint8_t>(predicted[plane] + residuals[plane]);
            packed |= (static_cast<std::uint32_t>(residuals[plane]) & 255U) << (8 * plane);
        }
        residual[i] = packed;
        energy_row[x] = energy_word(residuals[green_plane], residuals[blue_plane], residuals[red_plane]);
        std::uint32_t green = static_cast<std::uint32_t>(predicted[green_plane] + residuals[green_plane]) & 255U;
        last[green] = static_cast<std::int32_t>(i);
    }

    // A cache hit: green from its residual, the other samples copied from
    // pixel `source` (the last earlier pixel with this green value) and their
    // residuals derived from them.
    template<std::uint32_t Channels>
    void finish_cached(std::uint32_t x, std::uint32_t i, std::uint8_t* row, const std::array<std::int32_t, 4>& predicted,
                       std::int32_t green_residual, std::uint32_t source) noexcept {
        const std::uint8_t* from = destination + static_cast<std::size_t>(source / width) * row_bytes
            + static_cast<std::size_t>(source % width) * Channels;
        std::array<std::int32_t, 4> residuals{0, green_residual, 0, 0};
        std::uint32_t green = static_cast<std::uint32_t>(predicted[green_plane] + green_residual) & 255U;
        std::uint32_t packed = 0;
        for (std::uint32_t plane = 0; plane < Channels; ++plane) {
            std::int32_t sample = plane == green_plane ? static_cast<std::int32_t>(green) : from[plane];
            if (plane != green_plane) {
                residuals[plane] = wrap(sample - predicted[plane]);
            }
            row[static_cast<std::size_t>(x) * Channels + plane] = static_cast<std::uint8_t>(sample);
            packed |= (static_cast<std::uint32_t>(residuals[plane]) & 255U) << (8 * plane);
        }
        residual[i] = packed;
        energy_row[x] = energy_word(residuals[green_plane], residuals[blue_plane], residuals[red_plane]);
        last[green] = static_cast<std::int32_t>(i);
    }

    // Copies every plane's residual from pixel i - from_distance.
    template<std::uint32_t Channels>
    void copy_residuals(std::uint32_t i, std::uint32_t from_distance, std::array<std::int32_t, 4>& residuals) const noexcept {
        std::uint32_t copied = residual[i - from_distance];
        for (std::uint32_t plane = 0; plane < Channels; ++plane) {
            residuals[plane] = lane(copied, plane);
        }
    }

    // Section 7.7 for pixel (x, y). Edge pixels (the top row and the left
    // column) apply the boundary rules of section 7.1.
    template<bool Edge, std::uint32_t Channels>
    bool pixel(std::uint32_t x, std::uint32_t y, std::uint8_t* row, const std::uint8_t* above_samples) noexcept {
        std::uint32_t i = y * width + x;
        std::array<std::int32_t, 4> predicted{0, 0, 0, 0};
        bool alpha_quiet = true;
        for (std::uint32_t plane = 0; plane < Channels; ++plane) {
            std::int32_t up = 0;
            std::int32_t left_sample = 0;
            std::int32_t up_left = 0;
            if constexpr (Edge) {
                up = above_samples != nullptr ? above_samples[static_cast<std::size_t>(x) * Channels + plane] : 0;
                left_sample = x >= 1 ? row[static_cast<std::size_t>(x - 1) * Channels + plane] : up;
                up_left = above_samples != nullptr && x >= 1 ? above_samples[static_cast<std::size_t>(x - 1) * Channels + plane] : up;
            } else {
                up = above_samples[static_cast<std::size_t>(x) * Channels + plane];
                left_sample = row[static_cast<std::size_t>(x - 1) * Channels + plane];
                up_left = above_samples[static_cast<std::size_t>(x - 1) * Channels + plane];
            }
            std::int32_t high = std::max(left_sample, up);
            std::int32_t low = std::min(left_sample, up);
            predicted[plane] = up_left >= high ? low : (up_left <= low ? high : left_sample + up - up_left);
            if (plane == alpha_plane) {
                alpha_quiet = left_sample == up && up == up_left;
            }
        }
        std::array<std::int32_t, 4> residuals{0, 0, 0, 0};
        if (whole_left != 0) {
            copy_residuals<Channels>(i, whole_distance, residuals);
            --whole_left;
            finish<Channels>(x, i, row, predicted, residuals);
            return true;
        }
        const std::uint64_t* left_energy = energy_row + x;
        std::uint64_t activity = above[x] + left_energy[-1] + left_energy[-2];
        std::int32_t green = 0;
        if (left[green_plane] != 0) {
            green = lane(residual[i - distance[green_plane]], green_plane);
            --left[green_plane];
        } else {
            std::uint32_t value = symbol(green_plane, class_of[activity & energy_field_mask]);
            if (value < whole_length0) {
                green = unzig(value);
            } else if (value < cache0) {
                std::uint32_t length = 0;
                whole_distance = match(green_plane, value - whole_length0, whole_repeats, i, length);
                if (failed) {
                    return false;
                }
                whole_left = length - 1;
                copy_residuals<Channels>(i, whole_distance, residuals);
                finish<Channels>(x, i, row, predicted, residuals);
                return true;
            } else if (value < green_only0) {
                green = unzig(value - cache0);
                std::uint32_t green_sample = static_cast<std::uint32_t>(predicted[green_plane] + green) & 255U;
                std::int32_t source = last[green_sample];
                if (source < 0) {
                    failed = true;
                    return false;
                }
                finish_cached<Channels>(x, i, row, predicted, green, static_cast<std::uint32_t>(source));
                return true;
            } else {
                std::uint32_t length = 0;
                distance[green_plane] = match(green_plane, value - green_only0, plane_repeats[green_plane], i, length);
                if (failed) {
                    return false;
                }
                left[green_plane] = length - 1;
                green = lane(residual[i - distance[green_plane]], green_plane);
            }
        }
        residuals[green_plane] = green;
        // A residual lies in [-128, 127].
        std::uint32_t bucket = green_bucket_of[static_cast<std::uint32_t>(green + 128)];
        if (left[blue_plane] != 0) {
            residuals[blue_plane] = lane(residual[i - distance[blue_plane]], blue_plane);
            --left[blue_plane];
        } else {
            std::uint32_t blue_class = class_of[(activity >> energy_field_bits) & energy_field_mask];
            std::uint32_t context = blue_context0 + 3 * blue_class + bucket;
            if (!plane_symbol(blue_plane, context, i, blue_class, green, residuals[blue_plane])) {
                return false;
            }
        }
        if (left[red_plane] != 0) {
            residuals[red_plane] = lane(residual[i - distance[red_plane]], red_plane);
            --left[red_plane];
        } else {
            std::uint32_t red_class = class_of[(activity >> (2 * energy_field_bits)) & energy_field_mask];
            std::uint32_t context = red_context0 + 3 * red_class + bucket;
            if (!plane_symbol(red_plane, context, i, red_class, green, residuals[red_plane])) {
                return false;
            }
        }
        if constexpr (Channels == 4) {
            if (left[alpha_plane] != 0) {
                residuals[alpha_plane] = lane(residual[i - distance[alpha_plane]], alpha_plane);
                --left[alpha_plane];
            } else {
                std::uint32_t context = alpha_context0 + (alpha_quiet ? 0U : 1U);
                if (!plane_symbol(alpha_plane, context, i, 0, green, residuals[alpha_plane])) {
                    return false;
                }
            }
        }
        finish<Channels>(x, i, row, predicted, residuals);
        return true;
    }

    // Every counter is zero and every stream ended exactly (section 6, End).
    bool complete() const noexcept {
        bool counters = whole_left == 0 && left[0] == 0 && left[1] == 0 && left[2] == 0 && left[3] == 0;
        bool ended = counters && !failed;
        for (std::uint32_t plane = 0; plane < channels; ++plane) {
            const RangeDecoder& range = streams[plane];
            ended = ended && !range.failed && range.position == range.data.size() && range.state == lower_bound;
        }
        return ended;
    }
};

template<std::uint32_t Channels>
bool decode_row(TileState& state, std::uint32_t y, std::uint8_t* row, const std::uint8_t* above_samples) noexcept {
    state.prepare_row(y);
    if (y == 0) {
        for (std::uint32_t x = 0; x < state.width; ++x) {
            if (!state.pixel<true, Channels>(x, y, row, above_samples)) {
                return false;
            }
        }
        bool row_ok = !state.streams_failed();
        return row_ok;
    }
    if (!state.pixel<true, Channels>(0, y, row, above_samples)) {
        return false;
    }
    for (std::uint32_t x = 1; x < state.width; ++x) {
        if (!state.pixel<false, Channels>(x, y, row, above_samples)) {
            return false;
        }
    }
    bool row_ok = !state.streams_failed();
    return row_ok;
}

bool read_leb128(std::span<const std::uint8_t> data, std::size_t& position, std::uint64_t& value) noexcept {
    value = 0;
    std::uint32_t shift = 0;
    while (true) {
        // At most five bytes.
        if (position >= data.size() || shift > 28) {
            return false;
        }
        std::uint8_t byte = data[position];
        ++position;
        value |= static_cast<std::uint64_t>(byte & 0x7FU) << shift;
        shift += 7;
        if ((byte & 0x80U) == 0) {
            return true;
        }
    }
}

// The plane streams of a payload: the lengths of the first C - 1 streams as
// LEB128, then green, blue, red, alpha (section 6). Indexed by plane.
bool split_streams(std::span<const std::uint8_t> payload, std::uint32_t channels,
                   std::array<std::span<const std::uint8_t>, maximum_channels>& streams) noexcept {
    std::array<std::uint64_t, maximum_channels> lengths{0, 0, 0, 0};
    std::size_t position = 0;
    std::uint64_t listed = 0;
    for (std::uint32_t stream = 0; stream + 1 < channels; ++stream) {
        if (!read_leb128(payload, position, lengths[stream])) {
            return false;
        }
        listed += lengths[stream];
    }
    if (listed > payload.size() - position) {
        return false;
    }
    lengths[channels - 1] = payload.size() - position - listed;
    for (std::uint32_t stream = 0; stream < channels; ++stream) {
        std::size_t length = static_cast<std::size_t>(lengths[stream]);
        streams[stream_planes[stream]] = payload.subspan(position, length);
        position += length;
    }
    return true;
}

} // namespace

bool reserve_tile(TileWorkspace& workspace, Shape tile) noexcept {
    std::size_t pixels = static_cast<std::size_t>(tile.width) * tile.height;
    std::size_t ring = 3 * (static_cast<std::size_t>(tile.width) + 3);
    bool reserved = grow(workspace.residuals, pixels) && grow(workspace.energy, ring) && grow(workspace.above, tile.width);
    return reserved;
}

bool decode_tile(const DecodeTables& tables, std::span<const std::uint8_t> payload, Shape tile, TileWorkspace& workspace,
                 std::uint8_t* destination, std::size_t row_bytes) noexcept {
    std::array<std::span<const std::uint8_t>, maximum_channels> streams{};
    if (!split_streams(payload, tile.channels, streams)) {
        return false;
    }
    TileState state;
    state.tables = &tables;
    for (std::uint32_t plane = 0; plane < tile.channels; ++plane) {
        start_range(state.streams[plane], streams[plane]);
    }
    state.width = tile.width;
    state.height = tile.height;
    state.channels = tile.channels;
    state.pixels = tile.width * tile.height;
    state.special = {1, tile.width, tile.width - 1, tile.width + 1, 2, 2 * tile.width};
    std::fill_n(workspace.energy.begin(), 3 * (static_cast<std::size_t>(tile.width) + 3), std::uint64_t{0});
    state.residual = workspace.residuals.data();
    state.ring = workspace.energy.data();
    state.above = workspace.above.data();
    reset_repeats(state.whole_repeats, tile.width);
    for (Repeats& list : state.plane_repeats) {
        reset_repeats(list, tile.width);
    }
    state.last.fill(-1);
    state.destination = destination;
    state.row_bytes = row_bytes;
    for (std::uint32_t y = 0; y < tile.height; ++y) {
        std::uint8_t* row = destination + static_cast<std::size_t>(y) * row_bytes;
        const std::uint8_t* above_samples = y >= 1 ? row - row_bytes : nullptr;
        bool row_ok = tile.channels == 4 ? decode_row<4>(state, y, row, above_samples) : decode_row<3>(state, y, row, above_samples);
        if (!row_ok) {
            return false;
        }
    }
    bool decoded = state.complete();
    return decoded;
}

} // namespace stx_cng::detail
