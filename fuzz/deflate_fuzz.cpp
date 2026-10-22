// stx_zlib deflate contract fuzzer: settings and input split from the fuzz
// bytes; the stream is encoded with a fuzz-chosen buffer schedule and must
// be byte-identical to a whole-buffer encode, and inflate must restore the
// input exactly. Any contract break aborts. A build check for CI smoke runs
// (owner revision of decision 6A), not a certificate.
#include "stx_zlib/deflate.hpp"
#include "stx_zlib/inflate.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <span>
#include <vector>

namespace {
using namespace stx_zlib;
constexpr std::size_t input_limit = 16384;

struct Workspace final {
    std::unique_ptr<DeflateStream> deflate = std::make_unique<DeflateStream>();
    std::unique_ptr<InflateStream> inflate = std::make_unique<InflateStream>();
    std::vector<std::byte> whole = std::vector<std::byte>(input_limit * 2U + 4096U);
    std::vector<std::byte> split = std::vector<std::byte>(input_limit * 2U + 4096U);
    std::vector<std::byte> restored = std::vector<std::byte>(input_limit);
};

// Encodes input with at most `step` input and output bytes per call (0 means
// whole buffers), flushing with none until the final input extent; returns
// the produced length or aborts on a broken contract.
std::size_t encode(DeflateStream& stream, const DeflateOptions& options, std::span<const std::byte> input,
                   std::span<std::byte> output, std::size_t step) {
    if (stream.reset(options) != DeflateError::none) std::abort();
    std::size_t consumed = 0;
    std::size_t produced = 0;
    // Each call either moves a byte or acknowledges a boundary; the bound is
    // generous and finite.
    const std::size_t bound = (input.size() + output.size()) * 4U + 4096U;
    for (std::size_t call = 0; call < bound; ++call) {
        std::size_t take = input.size() - consumed;
        std::size_t room = output.size() - produced;
        if (step != 0U) {
            take = std::min(take, step);
            room = std::min(room, step);
        }
        const std::span<const std::byte> source = input.subspan(consumed, take);
        const std::span<std::byte> destination = output.subspan(produced, room);
        // finish only with the final input extent: finish closes the stream
        // after the input it is given.
        const bool final_extent = consumed + take == input.size();
        const DeflateFlush flush = final_extent ? DeflateFlush::finish : DeflateFlush::none;
        const DeflateResult result = stream.process(source, destination, flush);
        if (result.input_consumed > take || result.output_produced > room) std::abort();
        if (result.status == DeflateStatus::error) std::abort();
        consumed += result.input_consumed;
        produced += result.output_produced;
        if (result.status == DeflateStatus::stream_end) {
            if (consumed != input.size()) std::abort();
            return produced;
        }
    }
    std::abort();
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    if (size < 4U) return 0;
    static Workspace work{};
    const std::uint8_t settings = data[0];
    const std::uint8_t strategy_byte = data[1];
    const std::size_t step = data[2] % 9U;
    const std::span<const std::byte> payload =
        std::as_bytes(std::span<const std::uint8_t>(data + 3U, std::min(size - 3U, input_limit)));
    DeflateOptions options{};
    options.format = (settings & 0x80U) != 0U ? DeflateFormat::raw_deflate : DeflateFormat::zlib;
    options.level = static_cast<int>(settings % 10U);
    const std::array<DeflateStrategy, 5> strategies{DeflateStrategy::normal, DeflateStrategy::filtered,
        DeflateStrategy::huffman_only, DeflateStrategy::rle, DeflateStrategy::fixed};
    options.strategy = strategies[strategy_byte % strategies.size()];
    options.window_bits = 9U + (strategy_byte >> 4U) % 7U;
    options.memory_level = 1U + (strategy_byte >> 1U) % 9U;
    options.input_limit.value = payload.size();
    options.output_limit.value = work.whole.size();
    options.work_limit.value = 1ULL << 40U;
    const std::size_t whole = encode(*work.deflate, options, payload, work.whole, 0U);
    const std::size_t split = encode(*work.deflate, options, payload, work.split, step);
    if (whole != split || !std::equal(work.whole.begin(), work.whole.begin() + static_cast<std::ptrdiff_t>(whole),
                                      work.split.begin())) {
        std::abort();
    }
    InflateOptions inflate{};
    inflate.format = options.format == DeflateFormat::zlib ? InflateFormat::zlib : InflateFormat::raw_deflate;
    inflate.window_bits = 15U;
    inflate.limits.input_bytes.value = whole;
    inflate.limits.output_bytes.value = payload.size();
    inflate.limits.work.value = 1ULL << 40U;
    if ((*work.inflate).reset(inflate) != InflateError::none) std::abort();
    const std::span<const std::byte> encoded(work.whole.data(), whole);
    const std::span<std::byte> restored(work.restored.data(), payload.size());
    const InflateResult result = (*work.inflate).process(encoded, restored, true);
    if (result.status != InflateStatus::stream_end || result.output_produced != payload.size()) std::abort();
    if (!std::equal(payload.begin(), payload.end(), work.restored.begin())) std::abort();
    return 0;
}

#if defined(GUI_FORMS_FUZZ_STANDALONE)
int main() {
    // A few fixed inputs so the standalone build exercises the harness.
    std::vector<std::uint8_t> input(300);
    for (std::size_t index = 0; index < input.size(); ++index) input[index] = static_cast<std::uint8_t>(index * 7U);
    for (std::uint8_t settings = 0; settings < 20U; ++settings) {
        input[0] = static_cast<std::uint8_t>(settings * 13U);
        input[1] = static_cast<std::uint8_t>(settings * 29U);
        input[2] = settings;
        LLVMFuzzerTestOneInput(input.data(), input.size());
    }
    return 0;
}
#endif
