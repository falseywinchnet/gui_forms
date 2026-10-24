#include "font_metrics_differential.hpp"

#include "gui_forms/text/types/text_types.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>
#include <ios>

// The tool's inputs: bounded file reads, corpus parsing, sfnt directory
// validation and SHA-256. Every function here validates a whole buffer before
// it builds anything from it.

namespace gui_forms::tools::font_metrics {

namespace {

std::uint16_t read_u16(std::span<const std::byte> bytes, std::size_t offset) {
    const std::uint16_t high = static_cast<std::uint16_t>(bytes[offset]);
    const std::uint16_t low = static_cast<std::uint16_t>(bytes[offset + 1U]);
    const std::uint16_t value = static_cast<std::uint16_t>((high << 8U) | low);
    return value;
}

std::uint32_t read_u32(std::span<const std::byte> bytes, std::size_t offset) {
    const std::uint32_t high = read_u16(bytes, offset);
    const std::uint32_t low = read_u16(bytes, offset + 2U);
    const std::uint32_t value = (high << 16U) | low;
    return value;
}

constexpr std::uint32_t tag(const char first, const char second, const char third,
                            const char fourth) {
    const std::uint32_t value = (static_cast<std::uint32_t>(static_cast<unsigned char>(first)) << 24U) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(second)) << 16U) |
        (static_cast<std::uint32_t>(static_cast<unsigned char>(third)) << 8U) |
        static_cast<std::uint32_t>(static_cast<unsigned char>(fourth));
    return value;
}

// The tables the measurement reads, with the least length each must have:
// head through unitsPerEm, hhea through numberOfHMetrics, OS/2 version 0
// through usWinDescent.
struct RequiredTable final {
    std::uint32_t tag{};
    std::uint32_t minimum_length{};
};

constexpr std::array<RequiredTable, 6> required_tables{{
    {tag('c', 'm', 'a', 'p'), 4U},
    {tag('h', 'e', 'a', 'd'), 20U},
    {tag('h', 'h', 'e', 'a'), 36U},
    {tag('h', 'm', 't', 'x'), 4U},
    {tag('n', 'a', 'm', 'e'), 6U},
    {tag('O', 'S', '/', '2'), 78U},
}};

// SHA-256 (FIPS 180-4) round constants.
constexpr std::array<std::uint32_t, 64> sha256_rounds{
    0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U, 0x3956c25bU, 0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U,
    0xd807aa98U, 0x12835b01U, 0x243185beU, 0x550c7dc3U, 0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U, 0xc19bf174U,
    0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU, 0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU,
    0x983e5152U, 0xa831c66dU, 0xb00327c8U, 0xbf597fc7U, 0xc6e00bf3U, 0xd5a79147U, 0x06ca6351U, 0x14292967U,
    0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU, 0x53380d13U, 0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
    0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U, 0xd192e819U, 0xd6990624U, 0xf40e3585U, 0x106aa070U,
    0x19a4c116U, 0x1e376c08U, 0x2748774cU, 0x34b0bcb5U, 0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU, 0x682e6ff3U,
    0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U, 0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U,
};

std::uint32_t rotate_right(std::uint32_t value, std::uint32_t count) {
    const std::uint32_t rotated = (value >> count) | (value << (32U - count));
    return rotated;
}

// Compresses one 64-byte block into state.
void sha256_block(std::array<std::uint32_t, 8>& state, const std::array<std::uint8_t, 64>& block) {
    std::array<std::uint32_t, 64> schedule{};
    for (std::size_t index = 0U; index < 16U; ++index) {
        const std::size_t base = index * 4U;
        schedule[index] = (static_cast<std::uint32_t>(block[base]) << 24U) |
            (static_cast<std::uint32_t>(block[base + 1U]) << 16U) |
            (static_cast<std::uint32_t>(block[base + 2U]) << 8U) |
            static_cast<std::uint32_t>(block[base + 3U]);
    }
    for (std::size_t index = 16U; index < 64U; ++index) {
        const std::uint32_t before = schedule[index - 15U];
        const std::uint32_t recent = schedule[index - 2U];
        const std::uint32_t sigma0 = rotate_right(before, 7U) ^ rotate_right(before, 18U) ^ (before >> 3U);
        const std::uint32_t sigma1 = rotate_right(recent, 17U) ^ rotate_right(recent, 19U) ^ (recent >> 10U);
        schedule[index] = schedule[index - 16U] + sigma0 + schedule[index - 7U] + sigma1;
    }
    std::array<std::uint32_t, 8> working = state;
    for (std::size_t index = 0U; index < 64U; ++index) {
        const std::uint32_t e = working[4];
        const std::uint32_t a = working[0];
        const std::uint32_t sum1 = rotate_right(e, 6U) ^ rotate_right(e, 11U) ^ rotate_right(e, 25U);
        const std::uint32_t choose = (e & working[5]) ^ (~e & working[6]);
        const std::uint32_t first = working[7] + sum1 + choose + sha256_rounds[index] + schedule[index];
        const std::uint32_t sum0 = rotate_right(a, 2U) ^ rotate_right(a, 13U) ^ rotate_right(a, 22U);
        const std::uint32_t majority = (a & working[1]) ^ (a & working[2]) ^ (working[1] & working[2]);
        const std::uint32_t second = sum0 + majority;
        working[7] = working[6];
        working[6] = working[5];
        working[5] = working[4];
        working[4] = working[3] + first;
        working[3] = working[2];
        working[2] = working[1];
        working[1] = working[0];
        working[0] = first + second;
    }
    for (std::size_t index = 0U; index < 8U; ++index) state[index] += working[index];
}

bool is_skipped_line(std::string_view line) {
    const bool skipped = line.empty() || line.front() == '#';
    return skipped;
}

std::string_view strip_carriage_return(std::string_view line) {
    if (!line.empty() && line.back() == '\r') line.remove_suffix(1U);
    return line;
}

enum class CorpusSection : std::uint8_t {
    none,
    strings,
    paragraphs,
};

} // namespace

std::vector<std::byte> read_bounded_file(const std::filesystem::path& path, std::size_t limit) {
    std::ifstream input(path, std::ios::binary | std::ios::ate);
    if (!input) throw InputRejected("cannot open " + path.string());
    const std::streamoff extent = input.tellg();
    if (extent <= 0) throw InputRejected("empty or unreadable file " + path.string());
    if (static_cast<std::uint64_t>(extent) > limit) throw InputRejected("file exceeds its bound: " + path.string());
    std::vector<std::byte> bytes(static_cast<std::size_t>(extent));
    input.seekg(0);
    input.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(extent));
    if (!input) throw InputRejected("incomplete read of " + path.string());
    return bytes;
}

Corpus parse_corpus(std::string_view bytes) {
    // Whole-buffer validation first: bound, encoding, then every line's role.
    if (bytes.empty() || bytes.size() > maximum_corpus_bytes) throw InputRejected("corpus size outside its bound");
    const Utf8ValidationResult encoding = validate_utf8(bytes);
    if (!encoding.valid()) throw InputRejected("corpus is not valid UTF-8");
    std::size_t string_count = 0U;
    std::size_t paragraph_count = 0U;
    CorpusSection section = CorpusSection::none;
    std::size_t start = 0U;
    while (start < bytes.size()) {
        const std::size_t newline = bytes.find('\n', start);
        const std::size_t end = newline == std::string_view::npos ? bytes.size() : newline;
        const std::string_view line = strip_carriage_return(bytes.substr(start, end - start));
        start = end + 1U;
        if (is_skipped_line(line)) continue;
        if (line == "[strings]") {
            section = CorpusSection::strings;
        } else if (line == "[paragraphs]") {
            section = CorpusSection::paragraphs;
        } else if (section == CorpusSection::strings) {
            ++string_count;
        } else if (section == CorpusSection::paragraphs) {
            const std::size_t words = static_cast<std::size_t>(std::count(line.begin(), line.end(), ' ')) + 1U;
            if (words > maximum_paragraph_words) throw InputRejected("corpus paragraph has too many words");
            if (line.front() == ' ' || line.back() == ' ' || line.find("  ") != std::string_view::npos) {
                throw InputRejected("corpus paragraph words must be separated by single spaces");
            }
            ++paragraph_count;
        } else {
            throw InputRejected("corpus text before a [strings] or [paragraphs] section");
        }
    }
    if (string_count == 0U || paragraph_count == 0U) throw InputRejected("corpus needs strings and paragraphs");
    if (string_count + paragraph_count > maximum_corpus_entries) throw InputRejected("corpus has too many entries");

    Corpus corpus{};
    corpus.strings.reserve(string_count);
    corpus.paragraphs.reserve(paragraph_count);
    section = CorpusSection::none;
    start = 0U;
    while (start < bytes.size()) {
        const std::size_t newline = bytes.find('\n', start);
        const std::size_t end = newline == std::string_view::npos ? bytes.size() : newline;
        const std::string_view line = strip_carriage_return(bytes.substr(start, end - start));
        start = end + 1U;
        if (is_skipped_line(line)) continue;
        if (line == "[strings]") {
            section = CorpusSection::strings;
        } else if (line == "[paragraphs]") {
            section = CorpusSection::paragraphs;
        } else if (section == CorpusSection::strings) {
            corpus.strings.emplace_back(line);
        } else {
            corpus.paragraphs.emplace_back(line);
        }
    }
    return corpus;
}

void validate_face_bytes(std::span<const std::byte> encoded) {
    if (encoded.size() < 12U || encoded.size() > maximum_face_bytes) throw InputRejected("face size outside its bound");
    const std::uint32_t version = read_u32(encoded, 0U);
    if (version != 0x00010000U && version != tag('t', 'r', 'u', 'e') && version != tag('O', 'T', 'T', 'O')) {
        throw InputRejected("not a single sfnt face (collections are not measured)");
    }
    const std::size_t table_count = read_u16(encoded, 4U);
    const std::size_t directory_end = 12U + table_count * 16U;
    if (table_count == 0U || table_count > 128U || directory_end > encoded.size()) {
        throw InputRejected("sfnt table directory outside the file");
    }
    std::array<bool, required_tables.size()> found{};
    for (std::size_t index = 0U; index < table_count; ++index) {
        const std::size_t record = 12U + index * 16U;
        const std::uint32_t table_tag = read_u32(encoded, record);
        const std::uint64_t offset = read_u32(encoded, record + 8U);
        const std::uint64_t length = read_u32(encoded, record + 12U);
        if (offset + length > encoded.size()) throw InputRejected("sfnt table extends past the file");
        for (std::size_t required = 0U; required < required_tables.size(); ++required) {
            if (required_tables[required].tag != table_tag) continue;
            if (length < required_tables[required].minimum_length) throw InputRejected("sfnt table too short");
            found[required] = true;
        }
    }
    const bool complete = std::find(found.begin(), found.end(), false) == found.end();
    if (!complete) throw InputRejected("face lacks a table the measurement reads");
}

std::string sha256_hex(std::span<const std::byte> bytes) {
    std::array<std::uint32_t, 8> state{0x6a09e667U, 0xbb67ae85U, 0x3c6ef372U, 0xa54ff53aU,
                                       0x510e527fU, 0x9b05688cU, 0x1f83d9abU, 0x5be0cd19U};
    std::array<std::uint8_t, 64> block{};
    const std::size_t whole_blocks = bytes.size() / 64U;
    for (std::size_t index = 0U; index < whole_blocks; ++index) {
        std::memcpy(block.data(), bytes.data() + index * 64U, 64U);
        sha256_block(state, block);
    }
    // The tail: remaining bytes, the 0x80 marker, zero fill and the bit length,
    // in one or two blocks.
    const std::size_t tail = bytes.size() - whole_blocks * 64U;
    block.fill(0U);
    if (tail != 0U) std::memcpy(block.data(), bytes.data() + whole_blocks * 64U, tail);
    block[tail] = 0x80U;
    if (tail >= 56U) {
        sha256_block(state, block);
        block.fill(0U);
    }
    const std::uint64_t bit_length = static_cast<std::uint64_t>(bytes.size()) * 8U;
    for (std::size_t index = 0U; index < 8U; ++index) {
        block[63U - index] = static_cast<std::uint8_t>(bit_length >> (index * 8U));
    }
    sha256_block(state, block);
    constexpr std::string_view digits = "0123456789abcdef";
    std::string hex(64U, '0');
    for (std::size_t index = 0U; index < 32U; ++index) {
        const std::uint32_t word = state[index / 4U];
        const std::uint32_t octet = (word >> (24U - (index % 4U) * 8U)) & 0xFFU;
        hex[index * 2U] = digits[octet >> 4U];
        hex[index * 2U + 1U] = digits[octet & 0x0FU];
    }
    return hex;
}

} // namespace gui_forms::tools::font_metrics
