// The CNG 1.0 reference encoder (FORMAT.md section 11) and the CngEncoder
// owner. It makes the decisions of the format's Python reference operation
// for operation, and writes byte-identical files given the same Zstandard:
//   1. every tile (the 128 x 32 segments, then the mosaic) is parsed greedily
//      under the older shared-class contexts, and the symbol counts give the
//      first code lengths;
//   2. twice, every tile is planned under the current code lengths (each
//      plane parsed alone, whole-pixel matches and cache hits where they beat
//      that route, then per-plane matches) and its plane streams emitted; the
//      emitted symbols' counts give the next code lengths;
//   3. the last emission's counts give the escaped per-image tables, and every
//      plane stream is rANS coded.
// Code lengths come from a generated table (code_lengths.hpp), and the parse
// prices in doubles in the reference's order of operations, so it makes the
// same decisions on every platform (the build turns floating-point
// contraction off for this reason).
#include "stx_cng/codec.hpp"

#include "code_lengths.hpp"
#include "format.hpp"
#include "interface.hpp"
#include "storage.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <thread>
#include <vector>

#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>

namespace stx_cng {
namespace detail {
namespace {

constexpr double unseen_length = 14.0;   // code length of a symbol a table does not list
constexpr double infinite_cost = std::numeric_limits<double>::infinity();
constexpr std::uint32_t chain_positions = 16;   // match candidates per 4-gram
constexpr std::uint32_t sufficient_length = 64;   // a match this long ends the search at a position
constexpr std::array<std::uint32_t, 9> trial_lengths{2, 4, 6, 8, 12, 16, 24, 32, 48};
constexpr std::uint32_t new_minimum_length = 4;   // shortest match at a new distance
constexpr std::uint32_t maximum_length = 16384;
constexpr std::uint32_t maximum_new_distance = 16384;   // largest distance a distance code expresses
constexpr std::uint64_t escape_maximum = 2;   // symbols used at most this often are escaped
// Candidates at one position: three repeats, six specials, the chain.
constexpr std::size_t maximum_candidates = repeats + specials + chain_positions;
constexpr std::size_t symbol_entries = static_cast<std::size_t>(contexts) * green_symbols;

// ---------------------------------------------------------------- tiles
// One tile's samples, interleaved, rows without padding.
struct Tile final {
    Shape shape{};
    std::vector<std::uint8_t> samples;
};

std::uint32_t tile_pixels(const Tile& tile) noexcept {
    std::uint32_t pixels = tile.shape.width * tile.shape.height;
    return pixels;
}

// The input image: samples `row_bytes` apart.
struct Source final {
    std::span<const std::uint8_t> samples{};
    std::size_t row_bytes = 0;
    Shape shape{};
};

void cut_segment(const Source& source, const SegmentRectangle& rectangle, Tile& tile) {
    tile.shape = rectangle.shape;
    std::size_t row = static_cast<std::size_t>(tile.shape.width) * tile.shape.channels;
    tile.samples.resize(row * tile.shape.height);
    for (std::uint32_t y = 0; y < tile.shape.height; ++y) {
        std::size_t from = static_cast<std::size_t>(rectangle.y + y) * source.row_bytes
            + static_cast<std::size_t>(rectangle.x) * tile.shape.channels;
        std::copy_n(source.samples.begin() + static_cast<std::ptrdiff_t>(from), row,
                    tile.samples.begin() + static_cast<std::ptrdiff_t>(y * row));
    }
}

struct MosaicWorkspace final {
    std::vector<std::uint64_t> sums;
    std::vector<std::uint64_t> counts;
};

// The mosaic: the rounded mean colour of every 16 x 16 cell over the pixels
// inside the image (section 9).
void block_means(const Source& source, MosaicWorkspace& workspace, Tile& tile) {
    Shape image = source.shape;
    Shape mosaic = mosaic_shape(image);
    std::size_t channels = image.channels;
    tile.shape = mosaic;
    tile.samples.assign(static_cast<std::size_t>(mosaic.width) * mosaic.height * channels, 0);
    workspace.sums.resize(static_cast<std::size_t>(mosaic.width) * channels);
    workspace.counts.resize(mosaic.width);
    for (std::uint32_t cell_y = 0; cell_y < mosaic.height; ++cell_y) {
        std::fill(workspace.sums.begin(), workspace.sums.end(), 0);
        std::fill(workspace.counts.begin(), workspace.counts.end(), 0);
        std::uint32_t y_end = std::min(image.height, (cell_y + 1) * cell);
        for (std::uint32_t y = cell_y * cell; y < y_end; ++y) {
            const std::uint8_t* row = source.samples.data() + static_cast<std::size_t>(y) * source.row_bytes;
            for (std::uint32_t x = 0; x < image.width; ++x) {
                std::size_t cell_x = x / cell;
                for (std::size_t channel = 0; channel < channels; ++channel) {
                    workspace.sums[cell_x * channels + channel] += row[static_cast<std::size_t>(x) * channels + channel];
                }
                ++workspace.counts[cell_x];
            }
        }
        for (std::size_t cell_x = 0; cell_x < mosaic.width; ++cell_x) {
            std::uint64_t count = workspace.counts[cell_x];
            for (std::size_t channel = 0; channel < channels; ++channel) {
                std::uint64_t mean = (workspace.sums[cell_x * channels + channel] + count / 2) / count;
                std::size_t at = (static_cast<std::size_t>(cell_y) * mosaic.width + cell_x) * channels + channel;
                tile.samples[at] = static_cast<std::uint8_t>(mean);
            }
        }
    }
}

// ---------------------------------------------------------------- residuals and activity
// MED residuals of every plane under the boundary rules of section 7.1, and
// the alpha context (L, U and UL alpha samples equal).
struct Residuals final {
    std::array<std::vector<std::int32_t>, maximum_channels> plane;
    std::vector<std::uint8_t> quiet;
};

void residuals_of(const Tile& tile, Residuals& out) {
    std::uint32_t width = tile.shape.width;
    std::uint32_t height = tile.shape.height;
    std::uint32_t channels = tile.shape.channels;
    std::uint32_t pixels = tile_pixels(tile);
    for (std::uint32_t plane = 0; plane < channels; ++plane) {
        out.plane[plane].resize(pixels);
    }
    out.quiet.assign(pixels, 1);
    for (std::uint32_t y = 0; y < height; ++y) {
        for (std::uint32_t x = 0; x < width; ++x) {
            std::size_t i = static_cast<std::size_t>(y) * width + x;
            for (std::uint32_t plane = 0; plane < channels; ++plane) {
                std::int32_t sample = tile.samples[i * channels + plane];
                std::int32_t up = y >= 1 ? tile.samples[(i - width) * channels + plane] : 0;
                std::int32_t left = x >= 1 ? tile.samples[(i - 1) * channels + plane] : up;
                std::int32_t up_left = y >= 1 && x >= 1 ? tile.samples[(i - width - 1) * channels + plane] : up;
                std::int32_t high = std::max(left, up);
                std::int32_t low = std::min(left, up);
                std::int32_t predicted = up_left >= high ? low : (up_left <= low ? high : left + up - up_left);
                out.plane[plane][i] = wrap(sample - predicted);
                if (plane == alpha_plane) {
                    out.quiet[i] = left == up && up == up_left ? 1 : 0;
                }
            }
        }
    }
}

// The class of every pixel from an energy plane: how many of 0, 3, 12, 48 the
// sum of the energies of L, LL, U, UU, UL and UR exceeds (zero outside).
void classes_of(const std::vector<std::int32_t>& energy, std::uint32_t width, std::uint32_t height, std::vector<std::uint8_t>& out) {
    out.resize(static_cast<std::size_t>(width) * height);
    for (std::uint32_t y = 0; y < height; ++y) {
        for (std::uint32_t x = 0; x < width; ++x) {
            std::size_t i = static_cast<std::size_t>(y) * width + x;
            std::int32_t activity = 0;
            if (x >= 1) {
                activity += energy[i - 1];
            }
            if (x >= 2) {
                activity += energy[i - 2];
            }
            if (y >= 1) {
                activity += energy[i - width];
                if (x >= 1) {
                    activity += energy[i - width - 1];
                }
                if (x + 1 < width) {
                    activity += energy[i - width + 1];
                }
            }
            if (y >= 2) {
                activity += energy[i - 2 * static_cast<std::size_t>(width)];
            }
            std::uint32_t level = (activity > 0 ? 1U : 0U) + (activity > 3 ? 1U : 0U) + (activity > 12 ? 1U : 0U)
                + (activity > 48 ? 1U : 0U);
            out[i] = static_cast<std::uint8_t>(level);
        }
    }
}

std::uint32_t green_bucket(std::int32_t green) noexcept {
    std::uint32_t bucket = green == 0 ? 0U : (green >= -2 && green <= 2 ? 1U : 2U);
    return bucket;
}

// Pixel i's packed residual key: plane c's residual + 128 in bits 8c to 8c + 7.
std::uint32_t packed_key(const Residuals& residuals, std::uint32_t channels, std::size_t i) noexcept {
    std::uint32_t key = 0;
    for (std::uint32_t plane = 0; plane < channels; ++plane) {
        key |= static_cast<std::uint32_t>(residuals.plane[plane][i] + 128) << (8 * plane);
    }
    return key;
}

// Pixel i's colour: sample c in bits 8c to 8c + 7.
std::uint32_t colour_key(const Tile& tile, std::size_t i) noexcept {
    std::uint32_t key = 0;
    for (std::uint32_t plane = 0; plane < tile.shape.channels; ++plane) {
        key |= static_cast<std::uint32_t>(tile.samples[i * tile.shape.channels + plane]) << (8 * plane);
    }
    return key;
}

// ---------------------------------------------------------------- symbol counts and code lengths
// Per (context, symbol): contexts * green_symbols counts.
struct Counts final {
    std::vector<std::uint64_t> value;
};

void clear_counts(Counts& counts) {
    counts.value.assign(symbol_entries, 0);
}

void add_count(Counts& counts, std::uint32_t context, std::uint32_t symbol) noexcept {
    ++counts.value[static_cast<std::size_t>(context) * green_symbols + symbol];
}

std::uint64_t count_at(const Counts& counts, std::uint32_t context, std::uint32_t symbol) noexcept {
    std::uint64_t count = counts.value[static_cast<std::size_t>(context) * green_symbols + symbol];
    return count;
}

// q bytes of every count (0 for an unused one): 1 + round(2 log2 f) with
// f = count * 4096 / total, clamped to [0, 24], and 1 for f below 1;
// rounding to nearest, ties to even (section 11, step 3).
void quantize(std::span<const std::uint64_t> counts, std::span<std::uint8_t> q) noexcept {
    std::uint64_t total = 0;
    for (std::uint64_t count : counts) {
        total += count;
    }
    std::fill(q.begin(), q.begin() + static_cast<std::ptrdiff_t>(counts.size()), std::uint8_t{0});
    if (total == 0) {
        return;
    }
    for (std::size_t entry = 0; entry < counts.size(); ++entry) {
        if (counts[entry] == 0) {
            continue;
        }
        double share = static_cast<double>(counts[entry] * total_frequency) / static_cast<double>(total);
        if (share < 1.0) {
            q[entry] = 1;
            continue;
        }
        double level = std::nearbyint(2.0 * std::log2(share));
        double clamped = std::min(24.0, std::max(0.0, level));
        q[entry] = static_cast<std::uint8_t>(1 + static_cast<std::int32_t>(clamped));
    }
}

// Code length of every (context, symbol): -log2(f / 4096) under the quantized
// table of the counts, unseen_length elsewhere.
struct CodeLengths final {
    std::vector<double> value;
};

const double* lengths_row(const CodeLengths& lengths, std::uint32_t context) noexcept {
    const double* row = lengths.value.data() + static_cast<std::size_t>(context) * green_symbols;
    return row;
}

struct LengthsWorkspace final {
    std::array<std::uint8_t, maximum_table_entries> q{};
    std::array<std::uint16_t, maximum_table_entries> frequency{};
    NormalizeWorkspace normalize{};
};

void code_lengths_of(const Counts& counts, LengthsWorkspace& workspace, CodeLengths& out) {
    out.value.assign(symbol_entries, unseen_length);
    for (std::uint32_t context = 0; context < contexts; ++context) {
        std::span<const std::uint64_t> row(counts.value.data() + static_cast<std::size_t>(context) * green_symbols, green_symbols);
        std::span<std::uint8_t> q(workspace.q.data(), green_symbols);
        quantize(row, q);
        if (!normalize(q, workspace.frequency, workspace.normalize)) {
            continue;
        }
        for (std::size_t symbol = 0; symbol < green_symbols; ++symbol) {
            std::uint16_t frequency = workspace.frequency[symbol];
            if (frequency != 0) {
                out.value[static_cast<std::size_t>(context) * green_symbols + symbol] = kCodeLength[frequency];
            }
        }
    }
}

// ---------------------------------------------------------------- distances
// The last three distances used, most recent first.
struct Repeats final {
    std::array<std::uint32_t, repeats> distance{1, 1, 2};
};

void reset_repeats(Repeats& list, std::uint32_t width) noexcept {
    list.distance = {1, width, 2};
}

std::int32_t repeat_index(const Repeats& list, std::uint32_t distance) noexcept {
    for (std::uint32_t index = 0; index < repeats; ++index) {
        if (list.distance[index] == distance) {
            return static_cast<std::int32_t>(index);
        }
    }
    return -1;
}

void use_distance(Repeats& list, std::uint32_t distance) noexcept {
    std::int32_t found = repeat_index(list, distance);
    std::uint32_t from = found >= 0 ? static_cast<std::uint32_t>(found) : repeats - 1;
    for (std::uint32_t index = from; index > 0; --index) {
        list.distance[index] = list.distance[index - 1];
    }
    list.distance[0] = distance;
}

using Specials = std::array<std::uint32_t, specials>;

// The six special distances of a tile of width w: 1, w, w - 1, w + 1, 2, 2w.
Specials specials_of(std::uint32_t width) noexcept {
    Specials special{1, width, width - 1, width + 1, 2, 2 * width};
    return special;
}

std::int32_t special_index(const Specials& special, std::uint32_t distance) noexcept {
    for (std::uint32_t index = 0; index < specials; ++index) {
        if (special[index] == distance) {
            return static_cast<std::int32_t>(index);
        }
    }
    return -1;
}

// ---------------------------------------------------------------- 4-gram chains
struct Gram final {
    std::uint64_t low = 0;
    std::uint64_t high = 0;
};

bool same_gram(const Gram& left, const Gram& right) noexcept {
    bool same = left.low == right.low && left.high == right.high;
    return same;
}

std::uint64_t gram_hash(const Gram& gram) noexcept {
    std::uint64_t mixed = gram.low * 0x9E3779B97F4A7C15ULL ^ (gram.high + 0x632BE59BD9B4E019ULL + (gram.low << 6) + (gram.low >> 2));
    std::uint64_t hash = mixed ^ (mixed >> 31);
    return hash;
}

// The newest chain_positions positions of one 4-gram in a ring: position k
// of the 4-gram's insertions is at ring[k % chain_positions]. The parse reads
// only those, newest first.
struct ChainEntry final {
    Gram gram{};
    std::uint32_t generation = 0;
    std::uint32_t count = 0;
    std::array<std::uint32_t, chain_positions> ring{};
};

// Open addressing over the 4-grams of one parse; entries of earlier parses are
// retired by a generation number instead of being cleared.
struct Chains final {
    std::vector<ChainEntry> entries;
    std::uint64_t mask = 0;
    std::uint32_t generation = 0;
};

Gram gram_at(const std::vector<std::uint32_t>& sequence, std::uint32_t at) noexcept {
    Gram gram;
    gram.low = static_cast<std::uint64_t>(sequence[at]) | (static_cast<std::uint64_t>(sequence[at + 1]) << 32);
    gram.high = static_cast<std::uint64_t>(sequence[at + 2]) | (static_cast<std::uint64_t>(sequence[at + 3]) << 32);
    return gram;
}

// Starts a parse of n positions: at most n distinct 4-grams, so at most half
// the table is used.
void reset_chains(Chains& chains, std::uint32_t count) {
    std::uint64_t capacity = 64;
    while (capacity < 2 * static_cast<std::uint64_t>(count)) {
        capacity *= 2;
    }
    if (chains.entries.size() < capacity || chains.generation == 0xFFFFFFFFU) {
        std::size_t size = static_cast<std::size_t>(std::max<std::uint64_t>(capacity, chains.entries.size()));
        chains.entries.assign(size, ChainEntry{});
        chains.generation = 0;
    }
    chains.mask = chains.entries.size() - 1;
    ++chains.generation;
}

void insert_chain(Chains& chains, const std::vector<std::uint32_t>& sequence, std::uint32_t count, std::uint32_t at) noexcept {
    if (at + 4 > count) {
        return;
    }
    Gram gram = gram_at(sequence, at);
    std::uint64_t slot = gram_hash(gram) & chains.mask;
    while (chains.entries[slot].generation == chains.generation && !same_gram(chains.entries[slot].gram, gram)) {
        slot = (slot + 1) & chains.mask;
    }
    ChainEntry& entry = chains.entries[slot];
    if (entry.generation != chains.generation) {
        entry.gram = gram;
        entry.generation = chains.generation;
        entry.count = 0;
    }
    entry.ring[entry.count % chain_positions] = at;
    ++entry.count;
}

// The entry of the 4-gram at position i; nullptr when none.
const ChainEntry* find_chain(const Chains& chains, const std::vector<std::uint32_t>& sequence, std::uint32_t count, std::uint32_t at) noexcept {
    if (at + 4 > count) {
        return nullptr;
    }
    Gram gram = gram_at(sequence, at);
    std::uint64_t slot = gram_hash(gram) & chains.mask;
    while (chains.entries[slot].generation == chains.generation) {
        if (same_gram(chains.entries[slot].gram, gram)) {
            return &chains.entries[slot];
        }
        slot = (slot + 1) & chains.mask;
    }
    return nullptr;
}

// Match candidates at one position, in insertion order without repeats.
struct Candidates final {
    std::array<std::uint32_t, maximum_candidates> distance{};
    std::size_t count = 0;
};

void add_unique(Candidates& candidates, std::uint32_t distance) noexcept {
    for (std::size_t index = 0; index < candidates.count; ++index) {
        if (candidates.distance[index] == distance) {
            return;
        }
    }
    candidates.distance[candidates.count] = distance;
    ++candidates.count;
}

void add_candidate(Candidates& candidates, std::uint32_t distance) noexcept {
    candidates.distance[candidates.count] = distance;
    ++candidates.count;
}

// ---------------------------------------------------------------- cost-aware parse
struct Match final {
    std::uint32_t position = 0;
    std::uint32_t distance = 0;
    std::uint32_t length = 0;
};

struct Step final {
    std::uint32_t from = 0;
    std::uint32_t distance = 0;   // 0 for a literal step
};

// Code lengths of the 30 length codes at each position: the row of the
// position's context, from symbol `base`.
struct LengthCosts final {
    const CodeLengths* lengths = nullptr;
    const std::vector<std::uint8_t>* context = nullptr;
    std::uint32_t base = 0;
};

const double* length_costs_at(const LengthCosts& costs, std::uint32_t at) noexcept {
    const double* row = lengths_row(*costs.lengths, (*costs.context)[at]) + costs.base;
    return row;
}

struct ParseWorkspace final {
    std::vector<std::uint32_t> limit_at;
    std::vector<double> price;
    std::vector<Step> back;
    std::vector<Repeats> state;
    Chains chains{};
};

// The cheapest parse of `sequence`: literal[i] is position i's cost without a
// match; a stop position takes no match and no match crosses it. Fills
// `matches` and, when `amortized` is given, each position's share of the cost
// of the step that covers it.
void costed_parse(const std::vector<std::uint32_t>& sequence, std::uint32_t count, std::uint32_t width, const std::vector<double>& literal,
                  const LengthCosts& lengths, const double* distance_costs, const std::vector<std::uint8_t>& stop, ParseWorkspace& workspace,
                  std::vector<Match>& matches, std::vector<double>* amortized) {
    Specials special = specials_of(width);
    workspace.limit_at.assign(static_cast<std::size_t>(count) + 1, count);
    std::uint32_t next_stop = count;
    for (std::uint32_t reverse = count; reverse > 0; --reverse) {
        std::uint32_t at = reverse - 1;
        if (stop[at] != 0) {
            next_stop = at;
        }
        workspace.limit_at[at] = next_stop;
    }
    workspace.price.assign(static_cast<std::size_t>(count) + 1, infinite_cost);
    workspace.price[0] = 0.0;
    workspace.back.assign(static_cast<std::size_t>(count) + 1, Step{});
    workspace.state.resize(static_cast<std::size_t>(count) + 1);
    reset_repeats(workspace.state[0], width);
    reset_chains(workspace.chains, count);
    Candidates candidates;
    std::uint32_t i = 0;
    while (i < count) {
        double base = workspace.price[i];
        Repeats repeats_here = workspace.state[i];
        if (base + literal[i] < workspace.price[i + 1]) {
            workspace.price[i + 1] = base + literal[i];
            workspace.back[i + 1] = Step{i, 0};
            workspace.state[i + 1] = repeats_here;
        }
        if (stop[i] != 0) {
            insert_chain(workspace.chains, sequence, count, i);
            ++i;
            continue;
        }
        std::uint32_t longest = 0;
        const double* length_cost = length_costs_at(lengths, i);
        std::uint32_t cap = std::min(workspace.limit_at[i] - i, maximum_length);
        candidates.count = 0;
        for (std::uint32_t distance : repeats_here.distance) {
            if (distance >= 1 && distance <= i) {
                add_unique(candidates, distance);
            }
        }
        for (std::uint32_t distance : special) {
            if (distance >= 1 && distance <= i) {
                add_unique(candidates, distance);
            }
        }
        const ChainEntry* chain = find_chain(workspace.chains, sequence, count, i);
        if (chain != nullptr) {
            std::uint32_t newest = std::min((*chain).count, chain_positions);
            for (std::uint32_t taken = 0; taken < newest; ++taken) {
                std::uint32_t position = (*chain).ring[((*chain).count - 1 - taken) % chain_positions];
                add_unique(candidates, i - position);
            }
        }
        for (std::size_t candidate = 0; candidate < candidates.count; ++candidate) {
            std::uint32_t distance = candidates.distance[candidate];
            if (sequence[i] != sequence[i - distance]) {
                continue;
            }
            std::uint32_t length = 1;
            std::uint32_t limit = std::min(cap, sufficient_length);
            while (length < limit && sequence[i + length] == sequence[i + length - distance]) {
                ++length;
            }
            if (length == sufficient_length) {
                while (length < cap && sequence[i + length] == sequence[i + length - distance]) {
                    ++length;
                }
            }
            std::int32_t repeat = repeat_index(repeats_here, distance);
            std::uint32_t shortest = repeat >= 0 ? minimum_length : new_minimum_length;
            if (length < shortest) {
                continue;
            }
            std::int32_t special_at = special_index(special, distance);
            if (repeat < 0 && special_at < 0 && distance > maximum_new_distance) {
                // Not expressible by a distance code.
                continue;
            }
            double distance_cost = 0.0;
            if (repeat >= 0) {
                distance_cost = distance_costs[repeat];
            } else if (special_at >= 0) {
                distance_cost = distance_costs[repeats + static_cast<std::uint32_t>(special_at)];
            } else {
                PrefixCode code = prefix(distance - 1);
                distance_cost = distance_costs[repeats + specials + code.code] + static_cast<double>(code.bits);
            }
            Repeats moved = repeats_here;
            use_distance(moved, distance);
            for (std::uint32_t trial_index = 0; trial_index <= trial_lengths.size(); ++trial_index) {
                bool listed = trial_index < trial_lengths.size();
                std::uint32_t trial = listed ? trial_lengths[trial_index] : length;
                if (listed && !(shortest <= trial && trial < length)) {
                    continue;
                }
                PrefixCode code = prefix(trial - minimum_length);
                double value = base + length_cost[code.code] + static_cast<double>(code.bits) + distance_cost;
                if (value < workspace.price[i + trial]) {
                    workspace.price[i + trial] = value;
                    workspace.back[i + trial] = Step{i, distance};
                    workspace.state[i + trial] = moved;
                }
            }
            longest = std::max(longest, length);
        }
        if (longest >= sufficient_length) {
            for (std::uint32_t at = i; at < i + longest; ++at) {
                insert_chain(workspace.chains, sequence, count, at);
            }
            i += longest;
            continue;
        }
        insert_chain(workspace.chains, sequence, count, i);
        ++i;
    }
    matches.clear();
    if (amortized != nullptr) {
        (*amortized).assign(count, 0.0);
    }
    std::uint32_t end = count;
    while (end > 0) {
        Step step = workspace.back[end];
        if (step.distance != 0) {
            matches.push_back(Match{step.from, step.distance, end - step.from});
        }
        if (amortized != nullptr) {
            double share = (workspace.price[end] - workspace.price[step.from]) / static_cast<double>(end - step.from);
            for (std::uint32_t at = step.from; at < end; ++at) {
                (*amortized)[at] = share;
            }
        }
        end = step.from;
    }
    std::reverse(matches.begin(), matches.end());
}

// ---------------------------------------------------------------- operations
// A coding operation: a table symbol (context, symbol), or raw bits (count,
// value) when `first` carries raw_operation.
struct Op final {
    std::uint16_t first = 0;
    std::uint16_t second = 0;
};

constexpr std::uint16_t raw_operation = 0x8000;

Op symbol_op(std::uint32_t context, std::uint32_t symbol) noexcept {
    Op op{static_cast<std::uint16_t>(context), static_cast<std::uint16_t>(symbol)};
    return op;
}

Op bits_op(std::uint32_t count, std::uint32_t value) noexcept {
    Op op{static_cast<std::uint16_t>(raw_operation | count), static_cast<std::uint16_t>(value)};
    return op;
}

bool is_bits(Op op) noexcept {
    bool bits = (op.first & raw_operation) != 0;
    return bits;
}

std::uint32_t bits_count(Op op) noexcept {
    std::uint32_t count = op.first & static_cast<std::uint16_t>(~raw_operation);
    return count;
}

void distance_ops(std::vector<Op>& ops, std::uint32_t distance, Repeats& list, const Specials& special) {
    std::int32_t repeat = repeat_index(list, distance);
    std::int32_t special_at = special_index(special, distance);
    if (repeat >= 0) {
        ops.push_back(symbol_op(distance_context, static_cast<std::uint32_t>(repeat)));
    } else if (special_at >= 0) {
        ops.push_back(symbol_op(distance_context, repeats + static_cast<std::uint32_t>(special_at)));
    } else {
        PrefixCode code = prefix(distance - 1);
        ops.push_back(symbol_op(distance_context, repeats + specials + code.code));
        if (code.bits != 0) {
            ops.push_back(bits_op(code.bits, code.extra));
        }
    }
    use_distance(list, distance);
}

void length_ops(std::vector<Op>& ops, std::uint32_t context, std::uint32_t base, std::uint32_t length) {
    PrefixCode code = prefix(length - minimum_length);
    ops.push_back(symbol_op(context, base + code.code));
    if (code.bits != 0) {
        ops.push_back(bits_op(code.bits, code.extra));
    }
}

// ---------------------------------------------------------------- the greedy first pass
struct GreedyWorkspace final {
    std::vector<std::int32_t> energy;
    std::vector<std::uint8_t> classes;
    std::vector<std::uint32_t> packed;
    std::vector<std::int32_t> hits;
};

// The shared-class view of the first pass: one class per pixel from
// max(|b|, |g|, |r|), contexts and literals as the plane contexts with that
// class, and a 256-slot colour cache hashed by colour.
void greedy_counts(const Tile& tile, Counts& counts, Residuals& residuals, Chains& chains, GreedyWorkspace& workspace) {
    std::uint32_t width = tile.shape.width;
    std::uint32_t height = tile.shape.height;
    std::uint32_t channels = tile.shape.channels;
    std::uint32_t pixels = tile_pixels(tile);
    residuals_of(tile, residuals);
    workspace.energy.resize(pixels);
    for (std::uint32_t i = 0; i < pixels; ++i) {
        std::int32_t energy = 0;
        for (std::uint32_t plane = 0; plane < 3; ++plane) {
            energy = std::max(energy, std::abs(residuals.plane[plane][i]));
        }
        workspace.energy[i] = energy;
    }
    classes_of(workspace.energy, width, height, workspace.classes);
    workspace.packed.resize(pixels);
    for (std::uint32_t i = 0; i < pixels; ++i) {
        workspace.packed[i] = packed_key(residuals, channels, i);
    }
    workspace.hits.assign(pixels, -1);
    std::array<std::int64_t, cache_symbols> cache{};
    cache.fill(-1);
    for (std::uint32_t i = 0; i < pixels; ++i) {
        std::uint32_t key = colour_key(tile, i);
        std::uint32_t slot = (key * 0x1E35A7BDU) >> 24;
        if (cache[slot] == static_cast<std::int64_t>(key)) {
            workspace.hits[i] = static_cast<std::int32_t>(slot);
        }
        cache[slot] = key;
    }
    const std::vector<std::uint32_t>& sequence = workspace.packed;
    Specials special = specials_of(width);
    Repeats list;
    reset_repeats(list, width);
    reset_chains(chains, pixels);
    Candidates candidates;
    std::uint32_t i = 0;
    while (i < pixels) {
        std::uint32_t best_length = 0;
        std::uint32_t best_distance = 0;
        std::uint32_t limit = std::min(maximum_length, pixels - i);
        candidates.count = 0;
        for (std::uint32_t distance : list.distance) {
            if (distance >= 1 && distance <= i) {
                add_candidate(candidates, distance);
            }
        }
        for (std::uint32_t distance : special) {
            if (distance >= 1 && distance <= i) {
                add_candidate(candidates, distance);
            }
        }
        const ChainEntry* chain = find_chain(chains, sequence, pixels, i);
        if (chain != nullptr) {
            std::uint32_t newest = std::min((*chain).count, chain_positions);
            for (std::uint32_t taken = 0; taken < newest; ++taken) {
                add_candidate(candidates, i - (*chain).ring[((*chain).count - 1 - taken) % chain_positions]);
            }
        }
        for (std::size_t candidate = 0; candidate < candidates.count; ++candidate) {
            std::uint32_t distance = candidates.distance[candidate];
            if (sequence[i] != sequence[i - distance]) {
                continue;
            }
            std::uint32_t length = 1;
            while (length < limit && sequence[i + length] == sequence[i + length - distance]) {
                ++length;
            }
            std::uint32_t shortest = repeat_index(list, distance) >= 0 ? minimum_length : new_minimum_length;
            if (length < shortest) {
                continue;
            }
            if (length > best_length) {
                best_length = length;
                best_distance = distance;
            }
            if (length == limit) {
                break;
            }
        }
        std::uint32_t pixel_class = workspace.classes[i];
        if (best_length != 0) {
            PrefixCode code = prefix(best_length - minimum_length);
            add_count(counts, pixel_class, whole_length0 + code.code);
            std::int32_t repeat = repeat_index(list, best_distance);
            std::int32_t special_at = special_index(special, best_distance);
            if (repeat >= 0) {
                add_count(counts, distance_context, static_cast<std::uint32_t>(repeat));
            } else if (special_at >= 0) {
                add_count(counts, distance_context, repeats + static_cast<std::uint32_t>(special_at));
            } else {
                add_count(counts, distance_context, repeats + specials + prefix(best_distance - 1).code);
            }
            use_distance(list, best_distance);
            for (std::uint32_t at = i; at < i + best_length; ++at) {
                insert_chain(chains, sequence, pixels, at);
            }
            i += best_length;
            continue;
        }
        if (workspace.hits[i] >= 0) {
            add_count(counts, pixel_class, cache0 + static_cast<std::uint32_t>(workspace.hits[i]));
        } else {
            std::int32_t green = residuals.plane[green_plane][i];
            std::int32_t blue = residuals.plane[blue_plane][i];
            std::int32_t red = residuals.plane[red_plane][i];
            if (pixel_class >= gate_class) {
                blue = wrap(blue - green);
                red = wrap(red - green);
            }
            std::uint32_t bucket = green_bucket(green);
            add_count(counts, pixel_class, zig(green));
            add_count(counts, blue_context0 + 3 * pixel_class + bucket, zig(blue));
            add_count(counts, red_context0 + 3 * pixel_class + bucket, zig(red));
            if (channels == 4) {
                std::uint32_t alpha_context = alpha_context0 + (residuals.quiet[i] != 0 ? 0U : 1U);
                add_count(counts, alpha_context, zig(residuals.plane[alpha_plane][i]));
            }
        }
        insert_chain(chains, sequence, pixels, i);
        ++i;
    }
}

// ---------------------------------------------------------------- the plane view, plan and emission
// Per-plane classes (green from |g|, blue from max(|g|, |b|), red from
// max(|g|, |r|)), contexts, literals, sequences for matching, and the
// green-keyed cache: per pixel, zigzag(green residual) when the last earlier
// pixel with its green sample has its whole colour, else -1.
struct View final {
    Shape shape{};
    std::uint32_t pixels = 0;
    std::array<std::vector<std::uint8_t>, maximum_channels> context;
    std::array<std::vector<std::uint16_t>, maximum_channels> literal;
    // Residual + 128.
    std::array<std::vector<std::uint32_t>, maximum_channels> plane;
    std::vector<std::uint32_t> packed;
    std::vector<std::int32_t> hits;
    // Scratch of view_of: per-plane energies and classes.
    std::array<std::vector<std::int32_t>, 3> energy;
    std::array<std::vector<std::uint8_t>, 3> classes;
};

void view_of(const Tile& tile, Residuals& residuals, View& view) {
    std::uint32_t width = tile.shape.width;
    std::uint32_t height = tile.shape.height;
    std::uint32_t channels = tile.shape.channels;
    std::uint32_t pixels = tile_pixels(tile);
    view.shape = tile.shape;
    view.pixels = pixels;
    residuals_of(tile, residuals);
    // Energies and classes indexed green, blue, red.
    for (std::vector<std::int32_t>& energy : view.energy) {
        energy.resize(pixels);
    }
    for (std::uint32_t i = 0; i < pixels; ++i) {
        std::int32_t green = std::abs(residuals.plane[green_plane][i]);
        view.energy[0][i] = green;
        view.energy[1][i] = std::max(green, std::abs(residuals.plane[blue_plane][i]));
        view.energy[2][i] = std::max(green, std::abs(residuals.plane[red_plane][i]));
    }
    for (std::size_t kind = 0; kind < 3; ++kind) {
        classes_of(view.energy[kind], width, height, view.classes[kind]);
    }
    const std::vector<std::uint8_t>& green_classes = view.classes[0];
    const std::vector<std::uint8_t>& blue_classes = view.classes[1];
    const std::vector<std::uint8_t>& red_classes = view.classes[2];
    for (std::uint32_t plane = 0; plane < channels; ++plane) {
        view.context[plane].resize(pixels);
        view.literal[plane].resize(pixels);
        view.plane[plane].resize(pixels);
        for (std::uint32_t i = 0; i < pixels; ++i) {
            view.plane[plane][i] = static_cast<std::uint32_t>(residuals.plane[plane][i] + 128);
        }
    }
    view.packed.resize(pixels);
    for (std::uint32_t i = 0; i < pixels; ++i) {
        std::int32_t green = residuals.plane[green_plane][i];
        std::int32_t blue = residuals.plane[blue_plane][i];
        std::int32_t red = residuals.plane[red_plane][i];
        if (blue_classes[i] >= gate_class) {
            blue = wrap(blue - green);
        }
        if (red_classes[i] >= gate_class) {
            red = wrap(red - green);
        }
        std::uint32_t bucket = green_bucket(green);
        view.context[green_plane][i] = green_classes[i];
        view.literal[green_plane][i] = static_cast<std::uint16_t>(zig(green));
        view.context[blue_plane][i] = static_cast<std::uint8_t>(blue_context0 + 3U * blue_classes[i] + bucket);
        view.literal[blue_plane][i] = static_cast<std::uint16_t>(zig(blue));
        view.context[red_plane][i] = static_cast<std::uint8_t>(red_context0 + 3U * red_classes[i] + bucket);
        view.literal[red_plane][i] = static_cast<std::uint16_t>(zig(red));
        if (channels == 4) {
            view.context[alpha_plane][i] = static_cast<std::uint8_t>(alpha_context0 + (residuals.quiet[i] != 0 ? 0U : 1U));
            view.literal[alpha_plane][i] = static_cast<std::uint16_t>(zig(residuals.plane[alpha_plane][i]));
        }
        view.packed[i] = packed_key(residuals, channels, i);
    }
    view.hits.assign(pixels, -1);
    std::array<std::int32_t, 256> last{};
    last.fill(-1);
    for (std::uint32_t i = 0; i < pixels; ++i) {
        std::uint32_t green = tile.samples[static_cast<std::size_t>(i) * channels + green_plane];
        std::int32_t earlier = last[green];
        if (earlier >= 0 && colour_key(tile, static_cast<std::size_t>(earlier)) == colour_key(tile, i)) {
            view.hits[i] = static_cast<std::int32_t>(zig(residuals.plane[green_plane][i]));
        }
        last[green] = static_cast<std::int32_t>(i);
    }
}

std::uint32_t plane_length_base(std::uint32_t plane) noexcept {
    std::uint32_t base = plane == green_plane ? green_only0 : plane_length0;
    return base;
}

struct Plan final {
    std::vector<Match> whole;
    std::vector<std::uint8_t> cached;
    std::array<std::vector<Match>, maximum_channels> per_plane;
};

struct PlanWorkspace final {
    ParseWorkspace parse{};
    std::array<std::vector<double>, maximum_channels> plane_literal;
    std::vector<double> cache_cost;
    std::vector<double> plane_route;
    std::vector<double> amortized;
    std::vector<double> literal;
    std::vector<std::uint8_t> open;
    std::vector<std::uint8_t> stop;
    std::vector<std::uint8_t> covered;
    std::vector<Match> discard;
};

void plan_of(const View& view, const CodeLengths& costs, PlanWorkspace& workspace, Plan& plan) {
    std::uint32_t pixels = view.pixels;
    std::uint32_t width = view.shape.width;
    std::uint32_t channels = view.shape.channels;
    const double* distance_costs = lengths_row(costs, distance_context);
    for (std::uint32_t plane = 0; plane < channels; ++plane) {
        workspace.plane_literal[plane].resize(pixels);
        for (std::uint32_t i = 0; i < pixels; ++i) {
            workspace.plane_literal[plane][i] = lengths_row(costs, view.context[plane][i])[view.literal[plane][i]];
        }
    }
    workspace.cache_cost.resize(pixels);
    for (std::uint32_t i = 0; i < pixels; ++i) {
        std::int32_t hit = view.hits[i];
        double cost = infinite_cost;
        if (hit >= 0) {
            cost = lengths_row(costs, view.context[green_plane][i])[cache0 + static_cast<std::uint32_t>(hit)];
        }
        workspace.cache_cost[i] = cost;
    }
    workspace.open.assign(pixels, 0);
    workspace.plane_route.assign(pixels, 0.0);
    for (std::uint32_t plane = 0; plane < channels; ++plane) {
        LengthCosts lengths{&costs, &view.context[plane], plane_length_base(plane)};
        costed_parse(view.plane[plane], pixels, width, workspace.plane_literal[plane], lengths, distance_costs, workspace.open,
                     workspace.parse, workspace.discard, &workspace.amortized);
        for (std::uint32_t i = 0; i < pixels; ++i) {
            workspace.plane_route[i] += workspace.amortized[i];
        }
    }
    workspace.literal.resize(pixels);
    for (std::uint32_t i = 0; i < pixels; ++i) {
        workspace.literal[i] = std::min(workspace.plane_route[i], workspace.cache_cost[i]);
    }
    LengthCosts whole_lengths{&costs, &view.context[green_plane], whole_length0};
    costed_parse(view.packed, pixels, width, workspace.literal, whole_lengths, distance_costs, workspace.open, workspace.parse,
                 plan.whole, nullptr);
    workspace.covered.assign(pixels, 0);
    for (const Match& match : plan.whole) {
        for (std::uint32_t at = match.position; at < match.position + match.length; ++at) {
            workspace.covered[at] = 1;
        }
    }
    plan.cached.assign(pixels, 0);
    workspace.stop.assign(pixels, 0);
    for (std::uint32_t i = 0; i < pixels; ++i) {
        bool cached = workspace.covered[i] == 0 && workspace.cache_cost[i] < workspace.plane_route[i];
        plan.cached[i] = cached ? 1 : 0;
        workspace.stop[i] = workspace.covered[i] != 0 || cached ? 1 : 0;
    }
    for (std::uint32_t plane = 0; plane < channels; ++plane) {
        for (std::uint32_t i = 0; i < pixels; ++i) {
            workspace.literal[i] = workspace.stop[i] != 0 ? 0.0 : workspace.plane_literal[plane][i];
        }
        LengthCosts lengths{&costs, &view.context[plane], plane_length_base(plane)};
        costed_parse(view.plane[plane], pixels, width, workspace.literal, lengths, distance_costs, workspace.stop, workspace.parse,
                     plan.per_plane[plane], nullptr);
    }
}

using PlaneStreams = std::array<std::vector<Op>, maximum_channels>;

struct EmitWorkspace final {
    std::vector<std::uint32_t> whole_length;
    std::vector<std::uint32_t> whole_distance;
    std::vector<std::uint32_t> plane_length;
    std::vector<std::uint32_t> plane_distance;
    std::vector<std::uint8_t> covered;
};

// The operations of every plane stream of a tile, indexed by plane.
void emit_streams(const View& view, const Plan& plan, PlaneStreams& streams, EmitWorkspace& workspace) {
    std::uint32_t pixels = view.pixels;
    std::uint32_t width = view.shape.width;
    std::uint32_t channels = view.shape.channels;
    Specials special = specials_of(width);
    workspace.whole_length.assign(pixels, 0);
    workspace.whole_distance.assign(pixels, 0);
    workspace.covered.assign(pixels, 0);
    for (const Match& match : plan.whole) {
        workspace.whole_length[match.position] = match.length;
        workspace.whole_distance[match.position] = match.distance;
        for (std::uint32_t at = match.position; at < match.position + match.length; ++at) {
            workspace.covered[at] = 1;
        }
    }
    workspace.plane_length.resize(pixels);
    workspace.plane_distance.resize(pixels);
    for (std::uint32_t plane = 0; plane < channels; ++plane) {
        std::vector<Op>& ops = streams[plane];
        ops.clear();
        std::fill(workspace.plane_length.begin(), workspace.plane_length.end(), 0);
        for (const Match& match : plan.per_plane[plane]) {
            workspace.plane_length[match.position] = match.length;
            workspace.plane_distance[match.position] = match.distance;
        }
        Repeats plane_list;
        Repeats whole_list;
        reset_repeats(plane_list, width);
        reset_repeats(whole_list, width);
        std::uint32_t until = 0;
        for (std::uint32_t i = 0; i < pixels; ++i) {
            if (plane == green_plane && workspace.whole_length[i] != 0) {
                length_ops(ops, view.context[green_plane][i], whole_length0, workspace.whole_length[i]);
                distance_ops(ops, workspace.whole_distance[i], whole_list, special);
                continue;
            }
            if (workspace.covered[i] != 0) {
                continue;
            }
            if (plan.cached[i] != 0) {
                if (plane == green_plane) {
                    ops.push_back(symbol_op(view.context[green_plane][i], cache0 + static_cast<std::uint32_t>(view.hits[i])));
                }
                continue;
            }
            if (until > i) {
                continue;
            }
            if (workspace.plane_length[i] != 0) {
                length_ops(ops, view.context[plane][i], plane_length_base(plane), workspace.plane_length[i]);
                distance_ops(ops, workspace.plane_distance[i], plane_list, special);
                until = i + workspace.plane_length[i];
            } else {
                ops.push_back(symbol_op(view.context[plane][i], view.literal[plane][i]));
            }
        }
    }
}

void count_streams(const PlaneStreams& streams, std::uint32_t channels, Counts& counts) noexcept {
    for (std::uint32_t plane = 0; plane < channels; ++plane) {
        for (Op op : streams[plane]) {
            if (!is_bits(op)) {
                add_count(counts, op.first, op.second);
            }
        }
    }
}

// ---------------------------------------------------------------- tables and rANS
struct EncodeTable final {
    std::array<std::uint16_t, maximum_table_entries> frequency{};
    std::array<std::uint32_t, maximum_table_entries> start{};
    // Symbols below this used more than escape_maximum times are coded
    // directly; the escape is entry `listed`.
    std::uint32_t listed = 0;
};

struct EscapedTables final {
    std::array<EncodeTable, contexts> table{};
    // Per context: its q bytes, `entries` of them (listed + 1).
    std::array<std::array<std::uint8_t, maximum_table_entries>, contexts> q{};
    std::array<std::uint64_t, maximum_table_entries> vector{};
    NormalizeWorkspace normalize{};
};

void escape_tables(const Counts& counts, EscapedTables& tables) noexcept {
    for (std::uint32_t context = 0; context < contexts; ++context) {
        std::uint32_t listed = 0;
        std::uint64_t total = 0;
        for (std::uint32_t symbol = 0; symbol < green_symbols; ++symbol) {
            std::uint64_t count = count_at(counts, context, symbol);
            total += count;
            if (count > escape_maximum) {
                listed = symbol + 1;
            }
        }
        std::size_t entries = static_cast<std::size_t>(listed) + 1;
        std::uint64_t kept = 0;
        for (std::uint32_t symbol = 0; symbol < listed; ++symbol) {
            std::uint64_t count = count_at(counts, context, symbol);
            tables.vector[symbol] = count > escape_maximum ? count : 0;
            kept += tables.vector[symbol];
        }
        tables.vector[listed] = total - kept;
        std::span<std::uint8_t> q(tables.q[context].data(), entries);
        quantize(std::span<const std::uint64_t>(tables.vector.data(), entries), q);
        EncodeTable& table = tables.table[context];
        table.listed = listed;
        // A context with no counts at all quantizes to zeros and is unused.
        if (!normalize(q, table.frequency, tables.normalize)) {
            std::fill_n(table.frequency.begin(), entries, std::uint16_t{0});
        }
        std::uint32_t accumulated = 0;
        for (std::size_t entry = 0; entry < entries; ++entry) {
            table.start[entry] = accumulated;
            accumulated += table.frequency[entry];
        }
    }
}

std::size_t table_entries(const EncodeTable& table) noexcept {
    std::size_t entries = static_cast<std::size_t>(table.listed) + 1;
    return entries;
}

// Replaces every symbol a table does not list (beyond its listed range, or
// used at most escape_maximum times) by the escape and the symbol in raw bits.
void escape_ops(const std::vector<Op>& ops, const Counts& counts, const EscapedTables& tables, std::vector<Op>& out) {
    out.clear();
    for (Op op : ops) {
        if (is_bits(op)) {
            out.push_back(op);
            continue;
        }
        const EncodeTable& table = tables.table[op.first];
        if (op.second >= table.listed || count_at(counts, op.first, op.second) <= escape_maximum) {
            out.push_back(symbol_op(op.first, table.listed));
            out.push_back(bits_op(raw_bits(op.first), op.second));
        } else {
            out.push_back(op);
        }
    }
}

// rANS over the operations in reverse; the stream is the final state, big
// endian, then the renormalisation bytes in decoding order.
void rans_encode(const std::vector<Op>& ops, const EscapedTables& tables, std::vector<std::uint8_t>& out, std::vector<std::uint8_t>& scratch) {
    std::uint32_t state = lower_bound;
    scratch.clear();
    for (std::size_t reverse = ops.size(); reverse > 0; --reverse) {
        Op op = ops[reverse - 1];
        std::uint32_t frequency = 0;
        std::uint32_t start = 0;
        if (!is_bits(op)) {
            frequency = tables.table[op.first].frequency[op.second];
            start = tables.table[op.first].start[op.second];
        } else {
            frequency = total_frequency >> bits_count(op);
            start = static_cast<std::uint32_t>(op.second) * frequency;
        }
        std::uint64_t limit = static_cast<std::uint64_t>((lower_bound >> precision) << 8) * frequency;
        while (state >= limit) {
            scratch.push_back(static_cast<std::uint8_t>(state & 255U));
            state >>= 8;
        }
        state = ((state / frequency) << precision) + (state % frequency) + start;
    }
    out.resize(4);
    write_u32(out, 0, state);
    out.insert(out.end(), scratch.rbegin(), scratch.rend());
}

void append_leb128(std::vector<std::uint8_t>& out, std::uint64_t value) {
    while (true) {
        std::uint8_t byte = static_cast<std::uint8_t>(value & 0x7FU);
        value >>= 7;
        out.push_back(static_cast<std::uint8_t>(byte | (value != 0 ? 0x80U : 0U)));
        if (value == 0) {
            return;
        }
    }
}

struct PackWorkspace final {
    std::vector<Op> escaped;
    std::array<std::vector<std::uint8_t>, maximum_channels> coded;
    std::vector<std::uint8_t> scratch;
};

// A payload: the lengths of the first C - 1 plane streams as LEB128, then
// green, blue, red, alpha.
void pack_streams(const PlaneStreams& streams, std::uint32_t channels, const Counts& counts, const EscapedTables& tables,
                  PackWorkspace& workspace, std::vector<std::uint8_t>& payload) {
    for (std::uint32_t stream = 0; stream < channels; ++stream) {
        escape_ops(streams[stream_planes[stream]], counts, tables, workspace.escaped);
        rans_encode(workspace.escaped, tables, workspace.coded[stream], workspace.scratch);
    }
    payload.clear();
    for (std::uint32_t stream = 0; stream + 1 < channels; ++stream) {
        append_leb128(payload, workspace.coded[stream].size());
    }
    for (std::uint32_t stream = 0; stream < channels; ++stream) {
        payload.insert(payload.end(), workspace.coded[stream].begin(), workspace.coded[stream].end());
    }
}

// ---------------------------------------------------------------- the passes
// Tile `index` of the image: the segments in raster order, then the mosaic.
struct TileWorkspace final {
    Tile tile{};
    Residuals residuals{};
    View view{};
    Plan plan{};
    PlanWorkspace plan_workspace{};
    PackWorkspace pack{};
    GreedyWorkspace greedy{};
    EmitWorkspace emit{};
    MosaicWorkspace mosaic{};
};

void tile_at(const Source& source, std::uint64_t index, std::uint64_t segments, TileWorkspace& workspace) {
    if (index == segments) {
        block_means(source, workspace.mosaic, workspace.tile);
        return;
    }
    cut_segment(source, segment_rectangle(source.shape, index), workspace.tile);
}

enum class Pass : std::uint8_t { greedy, plan, pack };

// One worker's share of a pass: tiles first, first + stride, ...
struct EncodeJob final {
    const Source* source = nullptr;
    std::uint64_t segments = 0;
    std::uint64_t first = 0;
    std::uint64_t stride = 1;
    Pass pass = Pass::greedy;
    const CodeLengths* costs = nullptr;           // plan
    const Counts* final_counts = nullptr;         // pack
    const EscapedTables* tables = nullptr;        // pack
    std::vector<PlaneStreams>* streams = nullptr;   // plan writes, pack reads
    std::vector<std::vector<std::uint8_t>>* payloads = nullptr;   // pack writes; the mosaic's last
    TileWorkspace* workspace = nullptr;
    Counts counts{};                              // greedy and plan: this worker's counts
    CngError error = CngError::none;
};

void run_encode_job(EncodeJob& job) noexcept {
    TileWorkspace& workspace = *job.workspace;
    std::uint64_t tiles = job.segments + 1;
    std::uint32_t channels = (*job.source).shape.channels;
    try {
        if (job.pass != Pass::pack) {
            clear_counts(job.counts);
        }
        for (std::uint64_t tile = job.first; tile < tiles; tile += job.stride) {
            std::size_t index = static_cast<std::size_t>(tile);
            if (job.pass == Pass::pack) {
                pack_streams((*job.streams)[index], channels, *job.final_counts, *job.tables, workspace.pack, (*job.payloads)[index]);
                continue;
            }
            tile_at(*job.source, tile, job.segments, workspace);
            if (job.pass == Pass::greedy) {
                greedy_counts(workspace.tile, job.counts, workspace.residuals, workspace.plan_workspace.parse.chains, workspace.greedy);
                continue;
            }
            view_of(workspace.tile, workspace.residuals, workspace.view);
            plan_of(workspace.view, *job.costs, workspace.plan_workspace, workspace.plan);
            PlaneStreams& streams = (*job.streams)[index];
            emit_streams(workspace.view, workspace.plan, streams, workspace.emit);
            count_streams(streams, channels, job.counts);
        }
    } catch (const std::bad_alloc&) {
        job.error = CngError::allocation_failed;
    } catch (const std::length_error&) {
        job.error = CngError::allocation_failed;
    }
}

} // namespace

// What the owner keeps between calls.
struct EncoderState final {
    std::array<TileWorkspace, maximum_threads> workspaces{};
    std::array<EncodeJob, maximum_threads> jobs{};
    std::array<std::thread, maximum_threads> threads{};
    std::vector<PlaneStreams> streams;
    std::vector<std::vector<std::uint8_t>> payloads;
    Counts counts{};
    CodeLengths costs{};
    LengthsWorkspace lengths{};
    EscapedTables tables{};
    std::vector<std::uint8_t> body;
    std::vector<std::uint8_t> blob;
    ContextStorage context_storage{};
    ZSTD_CCtx* context = nullptr;
};

namespace {

// Runs one pass with `workers` jobs and, when counting, sums their counts in
// job order. none, or the first job's failure.
CngError run_pass(EncoderState& state, std::size_t workers, Pass pass, bool counting) noexcept {
    for (std::size_t worker = 0; worker < workers; ++worker) {
        state.jobs[worker].pass = pass;
        state.jobs[worker].error = CngError::none;
    }
    std::span<EncodeJob> jobs(state.jobs.data(), workers);
    run_jobs(jobs, std::span<std::thread>(state.threads), run_encode_job);
    for (const EncodeJob& job : jobs) {
        if (job.error != CngError::none) {
            return job.error;
        }
    }
    if (!counting) {
        return CngError::none;
    }
    try {
        clear_counts(state.counts);
    } catch (const std::bad_alloc&) {
        return CngError::allocation_failed;
    }
    for (const EncodeJob& job : jobs) {
        for (std::size_t entry = 0; entry < symbol_entries; ++entry) {
            state.counts.value[entry] += job.counts.value[entry];
        }
    }
    return CngError::none;
}

// The tables body (per context: the q count minus one as u16, then the q
// bytes) in one Zstandard frame at level 19 without content size, checksum or
// dictionary id.
CngError serialize_tables(EncoderState& state) noexcept {
    std::size_t body_bytes = 0;
    for (const EncodeTable& table : state.tables.table) {
        body_bytes += 2 + table_entries(table);
    }
    std::size_t bound = ZSTD_compressBound(maximum_tables_body);
    if (!grow(state.body, maximum_tables_body) || !grow(state.blob, bound)) {
        return CngError::allocation_failed;
    }
    std::size_t position = 0;
    for (std::uint32_t context = 0; context < contexts; ++context) {
        std::size_t entries = table_entries(state.tables.table[context]);
        std::uint32_t count = static_cast<std::uint32_t>(entries) - 1;
        state.body[position] = static_cast<std::uint8_t>(count >> 8);
        state.body[position + 1] = static_cast<std::uint8_t>(count);
        std::copy_n(state.tables.q[context].begin(), entries, state.body.begin() + static_cast<std::ptrdiff_t>(position + 2));
        position += 2 + entries;
    }
    if (state.context == nullptr) {
        // Sized for the parameters level 19 selects for any tables body: the
        // body is at most maximum_tables_body bytes, and smaller bodies select
        // parameters no larger than those.
        ZSTD_compressionParameters small = ZSTD_getCParams(tables_level, 16384, 0);
        ZSTD_compressionParameters large = ZSTD_getCParams(tables_level, maximum_tables_body, 0);
        std::size_t required = std::max(ZSTD_estimateCCtxSize_usingCParams(small), ZSTD_estimateCCtxSize_usingCParams(large));
        if (!reserve_context(state.context_storage, required)) {
            return CngError::allocation_failed;
        }
        state.context = ZSTD_initStaticCCtx(state.context_storage.words.get(), state.context_storage.bytes);
        if (state.context == nullptr) {
            return CngError::allocation_failed;
        }
    }
    ZSTD_CCtx* context = state.context;
    bool configured = ZSTD_isError(ZSTD_CCtx_reset(context, ZSTD_reset_session_and_parameters)) == 0
        && ZSTD_isError(ZSTD_CCtx_setParameter(context, ZSTD_c_compressionLevel, tables_level)) == 0
        && ZSTD_isError(ZSTD_CCtx_setParameter(context, ZSTD_c_contentSizeFlag, 0)) == 0
        && ZSTD_isError(ZSTD_CCtx_setParameter(context, ZSTD_c_checksumFlag, 0)) == 0
        && ZSTD_isError(ZSTD_CCtx_setParameter(context, ZSTD_c_dictIDFlag, 0)) == 0;
    if (!configured) {
        return CngError::allocation_failed;
    }
    std::size_t written = ZSTD_compress2(context, state.blob.data(), bound, state.body.data(), body_bytes);
    if (ZSTD_isError(written) != 0) {
        return CngError::allocation_failed;
    }
    state.blob.resize(written);
    return CngError::none;
}

} // namespace
} // namespace detail

class CngEncoderStorage final {
public:
    detail::EncoderState state{};
};

namespace {

using detail::Shape;

constexpr std::size_t leb128_bytes = 5;

// The largest payload of a tile of `pixels` pixels and `channels` planes: a
// pixel costs a stream at most three operations (an escaped literal is two;
// a match of at least two pixels is at most six), and an operation at most two
// renormalisation bytes, since 12 bits of precision grow the state by at most
// 12 bits; each stream also stores its four-byte final state.
std::uint64_t payload_bound(std::uint64_t pixels, std::uint64_t channels) noexcept {
    std::uint64_t stream = 4 + 6 * pixels;
    std::uint64_t bound = (channels - 1) * leb128_bytes + channels * stream;
    return bound;
}

std::size_t storage_bytes(const CngEncoderStorage& storage) noexcept {
    const detail::EncoderState& state = storage.state;
    std::size_t bytes = sizeof(CngEncoderStorage) + state.context_storage.bytes;
    bytes += detail::capacity_bytes(state.counts.value) + detail::capacity_bytes(state.costs.value);
    bytes += detail::capacity_bytes(state.body) + detail::capacity_bytes(state.blob);
    bytes += detail::capacity_bytes(state.streams) + detail::capacity_bytes(state.payloads);
    for (const detail::PlaneStreams& streams : state.streams) {
        for (const std::vector<detail::Op>& ops : streams) {
            bytes += detail::capacity_bytes(ops);
        }
    }
    for (const std::vector<std::uint8_t>& payload : state.payloads) {
        bytes += detail::capacity_bytes(payload);
    }
    for (const detail::EncodeJob& job : state.jobs) {
        bytes += detail::capacity_bytes(job.counts.value);
    }
    // Per-worker tile workspaces are bounded by one tile and not itemized.
    return bytes;
}

void write_fields(std::span<std::uint8_t> out, Shape shape, std::size_t mosaic_bytes, std::uint32_t mosaic_crc,
                  std::size_t tables_bytes, std::uint32_t tables_crc) noexcept {
    detail::write_u32(out, 0, shape.width);
    detail::write_u32(out, 4, shape.height);
    out[8] = static_cast<std::uint8_t>(shape.channels);
    out[9] = detail::version;
    out[10] = static_cast<std::uint8_t>(detail::segment_columns);
    out[11] = static_cast<std::uint8_t>(detail::segment_rows);
    detail::write_u32(out, 12, static_cast<std::uint32_t>(mosaic_bytes));
    detail::write_u32(out, 16, mosaic_crc);
    detail::write_u32(out, 20, static_cast<std::uint32_t>(tables_bytes));
    detail::write_u32(out, 24, tables_crc);
    detail::write_u32(out, detail::field_bytes, detail::crc_of(out.subspan(0, detail::field_bytes)));
}

// Copies `bytes` to out at `position` and advances it.
void put(std::span<std::uint8_t> out, std::size_t& position, std::span<const std::uint8_t> bytes) noexcept {
    std::copy(bytes.begin(), bytes.end(), out.begin() + static_cast<std::ptrdiff_t>(position));
    position += bytes.size();
}

} // namespace

std::uint64_t cng_encoded_bound(CngImage image) noexcept {
    Shape shape = detail::shape_of(image);
    if (!detail::admitted_shape(shape)) {
        return 0;
    }
    std::uint64_t segments = detail::segment_count(shape);
    Shape mosaic = detail::mosaic_shape(shape);
    std::uint64_t channels = shape.channels;
    std::uint64_t pixels = static_cast<std::uint64_t>(shape.width) * shape.height;
    std::uint64_t mosaic_pixels = static_cast<std::uint64_t>(mosaic.width) * mosaic.height;
    std::uint64_t tables = ZSTD_compressBound(detail::maximum_tables_body);
    std::uint64_t directory = detail::directory_entry_bytes * segments + 4;
    // The payloads' fixed parts are per segment; their per-pixel parts sum to
    // the image's pixels.
    std::uint64_t payloads = segments * payload_bound(0, channels) + 6 * pixels * channels;
    std::uint64_t bound = detail::front_bytes + 2 * directory + 2 * tables + 2 * payload_bound(mosaic_pixels, channels)
        + payloads + detail::field_block_bytes;
    if (bound > std::numeric_limits<std::size_t>::max()) {
        return 0;
    }
    return bound;
}

CngEncoder::CngEncoder() noexcept {}

CngEncoder::~CngEncoder() {}

void CngEncoder::release() noexcept {
    storage_.reset();
}

std::size_t CngEncoder::workspace_bytes() const noexcept {
    std::size_t bytes = storage_ ? storage_bytes(*storage_) : 0;
    return bytes;
}

CngEncodeResult CngEncoder::encode(const CngEncodeInput& input, std::span<std::byte> destination) noexcept {
    CngEncodeResult result;
    result.workspace_bytes = workspace_bytes();
    Shape shape = detail::shape_of(input.image);
    bool threads = input.threads >= 1 && input.threads <= maximum_threads;
    if (!detail::admitted_shape(shape) || !threads) {
        result.error = CngError::invalid_argument;
        return result;
    }
    result.error = detail::check_extent(shape, input.samples.size(), input.row_bytes);
    if (result.error == CngError::insufficient_storage) {
        // The samples, not a destination, are short: a contract breach.
        result.error = CngError::invalid_argument;
    }
    if (result.error == CngError::none && detail::overlapping(input.samples, destination)) {
        result.error = CngError::overlap;
    }
    if (result.error != CngError::none) {
        return result;
    }
    if (!storage_) {
        try {
            storage_ = std::make_unique<CngEncoderStorage>();
        } catch (const std::bad_alloc&) {
            result.error = CngError::allocation_failed;
            return result;
        }
    }
    detail::EncoderState& state = (*storage_).state;
    detail::Source source;
    source.samples = detail::bytes_of(input.samples);
    source.row_bytes = static_cast<std::size_t>(input.row_bytes);
    source.shape = shape;
    std::uint64_t segments = detail::segment_count(shape);
    std::uint64_t tiles = segments + 1;
    std::size_t workers = static_cast<std::size_t>(std::min<std::uint64_t>(input.threads, tiles));
    if (!detail::grow(state.streams, static_cast<std::size_t>(tiles)) || !detail::grow(state.payloads, static_cast<std::size_t>(tiles))) {
        result.error = CngError::allocation_failed;
        result.workspace_bytes = workspace_bytes();
        return result;
    }
    for (std::size_t worker = 0; worker < workers; ++worker) {
        detail::EncodeJob& job = state.jobs[worker];
        job.source = &source;
        job.segments = segments;
        job.first = worker;
        job.stride = workers;
        job.costs = &state.costs;
        job.final_counts = &state.counts;
        job.tables = &state.tables;
        job.streams = &state.streams;
        job.payloads = &state.payloads;
        job.workspace = &state.workspaces[worker];
    }
    // 1. The greedy first pass, then 2. two planned emissions; the second
    // one's streams are coded.
    result.error = detail::run_pass(state, workers, detail::Pass::greedy, true);
    if (result.error == CngError::none) {
        try {
            detail::code_lengths_of(state.counts, state.lengths, state.costs);
        } catch (const std::bad_alloc&) {
            result.error = CngError::allocation_failed;
        }
    }
    for (std::uint32_t emission = 0; emission < 2 && result.error == CngError::none; ++emission) {
        result.error = detail::run_pass(state, workers, detail::Pass::plan, true);
        if (result.error == CngError::none && emission == 0) {
            try {
                detail::code_lengths_of(state.counts, state.lengths, state.costs);
            } catch (const std::bad_alloc&) {
                result.error = CngError::allocation_failed;
            }
        }
    }
    // 3. Tables and payloads.
    if (result.error == CngError::none) {
        detail::escape_tables(state.counts, state.tables);
        result.error = detail::serialize_tables(state);
    }
    if (result.error == CngError::none) {
        result.error = detail::run_pass(state, workers, detail::Pass::pack, false);
    }
    result.workspace_bytes = workspace_bytes();
    if (result.error != CngError::none) {
        return result;
    }
    // 4. The container (section 3), written straight to the destination.
    std::span<const std::uint8_t> mosaic(state.payloads[static_cast<std::size_t>(segments)]);
    std::span<const std::uint8_t> tables(state.blob);
    std::size_t block = detail::directory_entry_bytes * static_cast<std::size_t>(segments) + 4;
    std::size_t payload_bytes = 0;
    for (std::uint64_t index = 0; index < segments; ++index) {
        payload_bytes += state.payloads[static_cast<std::size_t>(index)].size();
    }
    std::size_t tables_copies = segments == 1 ? 1 : 2;
    std::size_t required = detail::front_bytes + 2 * block + 2 * mosaic.size() + tables_copies * tables.size() + payload_bytes
        + detail::field_block_bytes;
    result.required_bytes = required;
    if (destination.size() < required) {
        result.error = CngError::insufficient_storage;
        return result;
    }
    std::span<std::uint8_t> out = detail::bytes_of(destination);
    std::size_t position = 0;
    put(out, position, detail::image_signature);
    std::size_t fields_at = position;
    write_fields(out.subspan(fields_at, detail::field_block_bytes), shape, mosaic.size(), detail::crc_of(mosaic), tables.size(),
                 detail::crc_of(tables));
    position += detail::field_block_bytes;
    std::size_t directory_at = position;
    for (std::uint64_t index = 0; index < segments; ++index) {
        const std::vector<std::uint8_t>& payload = state.payloads[static_cast<std::size_t>(index)];
        std::size_t entry = position + static_cast<std::size_t>(index) * detail::directory_entry_bytes;
        detail::write_u32(out, entry, static_cast<std::uint32_t>(payload.size()));
        detail::write_u32(out, entry + 4, detail::crc_of(payload));
    }
    std::span<const std::uint8_t> entries = out.subspan(directory_at, block - 4);
    detail::write_u32(out, directory_at + block - 4, detail::crc_of(entries));
    position += block;
    put(out, position, tables);
    put(out, position, mosaic);
    for (std::uint64_t index = 0; index < segments; ++index) {
        put(out, position, state.payloads[static_cast<std::size_t>(index)]);
    }
    put(out, position, mosaic);
    if (tables_copies == 2) {
        put(out, position, tables);
    }
    std::copy_n(out.begin() + static_cast<std::ptrdiff_t>(directory_at), block, out.begin() + static_cast<std::ptrdiff_t>(position));
    position += block;
    std::copy_n(out.begin() + static_cast<std::ptrdiff_t>(fields_at), detail::field_block_bytes,
                out.begin() + static_cast<std::ptrdiff_t>(position));
    position += detail::field_block_bytes;
    result.encoded_bytes = position;
    result.error = CngError::none;
    return result;
}

} // namespace stx_cng
