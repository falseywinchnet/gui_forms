// SHA-256 against the FIPS 180-2 / NIST CAVP examples ("abc", the empty message, the 448-bit
// two-block message and one million 'a'), every padding boundary from 0 to 130 bytes (checked
// for agreement between one call and a call over the same bytes held elsewhere), and the
// hexadecimal parser, including the rejections.

#include "../src/core/resources/integrity/sha256.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>
#include <string_view>
#include <vector>

namespace {

using gui_forms::detail::Sha256Digest;
using gui_forms::detail::parse_sha256_hex;
using gui_forms::detail::sha256;

struct KnownAnswer final {
    std::string_view message;
    std::string_view digest;
};

Sha256Digest digest_of(std::string_view text) {
    const std::span<const std::uint8_t> bytes(
        reinterpret_cast<const std::uint8_t*>(text.data()), text.size());
    const Sha256Digest digest = sha256(bytes);
    return digest;
}

bool matches(const Sha256Digest& actual, std::string_view expected_hex) {
    Sha256Digest expected{};
    if (!parse_sha256_hex(std::span<const char>(expected_hex.data(), expected_hex.size()),
                          expected)) {
        return false;
    }
    const bool equal = actual == expected;
    return equal;
}

bool known_answers() {
    const std::array<KnownAnswer, 4> answers{{
        {"abc", "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"},
        {"", "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
        {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
         "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"},
        {"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopq"
         "klmnopqrlmnopqrsmnopqrstnopqrstu",
         "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1"},
    }};
    for (const KnownAnswer& answer : answers) {
        if (!matches(digest_of(answer.message), answer.digest)) {
            std::printf("sha256: wrong digest for a %zu-byte message\n", answer.message.size());
            return false;
        }
    }
    const std::vector<std::uint8_t> million(1000000U, static_cast<std::uint8_t>('a'));
    if (!matches(sha256(million),
                 "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0")) {
        std::puts("sha256: wrong digest for one million 'a'");
        return false;
    }
    return true;
}

// Every tail length around the one- and two-block padding boundaries (55, 56, 63, 64, 119,
// 120): the digest of a prefix must not depend on where the bytes live or what follows them.
bool padding_boundaries() {
    std::vector<std::uint8_t> source(140U);
    for (std::size_t index = 0; index < source.size(); ++index) {
        source[index] = static_cast<std::uint8_t>(index * 37U + 11U);
    }
    std::vector<std::uint8_t> copy(140U, 0xA5U);
    std::vector<Sha256Digest> seen{};
    seen.reserve(131U);
    for (std::size_t length = 0; length <= 130U; ++length) {
        std::memcpy(copy.data(), source.data(), length);
        const Sha256Digest first = sha256(std::span<const std::uint8_t>(source.data(), length));
        const Sha256Digest second = sha256(std::span<const std::uint8_t>(copy.data(), length));
        if (first != second) {
            std::printf("sha256: a %zu-byte prefix depends on its storage\n", length);
            return false;
        }
        for (const Sha256Digest& earlier : seen) {
            if (earlier == first) {
                std::printf("sha256: a %zu-byte prefix repeats an earlier digest\n", length);
                return false;
            }
        }
        seen.push_back(first);
    }
    return true;
}

bool hex_parsing() {
    const std::string_view upper =
        "BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD";
    if (!matches(digest_of("abc"), upper)) return false;
    Sha256Digest untouched{};
    untouched.bytes[0] = 0x5AU;
    const std::string_view short_text = "ba7816bf";
    const std::string_view bad_digit =
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ag";
    if (parse_sha256_hex(std::span<const char>(short_text.data(), short_text.size()),
                         untouched)) {
        return false;
    }
    if (parse_sha256_hex(std::span<const char>(bad_digit.data(), bad_digit.size()), untouched)) {
        return false;
    }
    const bool unchanged = untouched.bytes[0] == 0x5AU && untouched.bytes[1] == 0U;
    return unchanged;
}

} // namespace

int main() {
    if (!known_answers()) return 1;
    if (!padding_boundaries()) return 2;
    if (!hex_parsing()) {
        std::puts("sha256: the hexadecimal parser accepted bad text or changed its output");
        return 3;
    }
    std::puts("sha256: FIPS examples, padding boundaries and hexadecimal parsing hold");
    return 0;
}
