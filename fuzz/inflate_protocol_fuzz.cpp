#include "gui_forms/compression/inflate.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <memory>
#include <span>

namespace {
using namespace gui_forms::compression;
constexpr std::size_t control_bytes = 64;
constexpr std::size_t output_capacity = 32768;
constexpr std::size_t guard_bytes = 16;

void require(bool condition, const char* message) {
    if (condition) return;
    std::fprintf(stderr, "inflate protocol: %s\n", message);
    std::abort();
}

struct Output final {
    // Only the caller-selected prefix after the first guard is writable.
    std::array<std::byte, output_capacity + 2 * guard_bytes> bytes{};
};

InflateOptions ordinary_options() {
    const InflateOptions result{.format = InflateFormat::raw_deflate, .window_bits = 15,
        .limits = {.input_bytes = ByteCount{65536}, .output_bytes = ByteCount{output_capacity},
                   .work = WorkCount{4000000}}};
    return result;
}

std::uint32_t little32(std::span<const std::uint8_t> bytes) {
    std::uint32_t result = 0;
    for (unsigned index = 0; index < 4; ++index) {
        result |= static_cast<std::uint32_t>(bytes[index]) << (8U * index);
    }
    return result;
}

InflateOptions selected_options(std::span<const std::uint8_t> controls) {
    InflateOptions result = ordinary_options();
    if ((controls[0] & 1U) != 0) result.format = InflateFormat::zlib;
    result.window_bits = 8U + (controls[1] & 7U);
    if ((controls[2] & 1U) != 0) {
        result.limits.input_bytes = ByteCount{static_cast<std::uint64_t>(controls[4]) + 256U * controls[5]};
    }
    if ((controls[2] & 2U) != 0) {
        const unsigned count = static_cast<unsigned>(controls[6]) + 256U * controls[7];
        result.limits.output_bytes = ByteCount{count % (output_capacity + 1)};
    }
    if ((controls[2] & 4U) != 0) {
        const std::uint32_t count = little32(controls.subspan(8, 4));
        result.limits.work = WorkCount{count % 4000001U};
    }
    return result;
}

void invalid_reset(InflateStream& stream, const InflateOptions& valid, unsigned choice) {
    InflateOptions invalid = valid;
    switch (choice % 6U) {
    case 0: invalid.window_bits = 7; break;
    case 1: invalid.format = static_cast<InflateFormat>(99); break;
    case 2: invalid.window_bits = 16; break;
    case 3: invalid.limits.input_bytes.value = std::numeric_limits<std::uint64_t>::max(); break;
    case 4: invalid.limits.output_bytes.value = std::numeric_limits<std::uint64_t>::max(); break;
    case 5: invalid.limits.work.value = std::numeric_limits<std::uint64_t>::max(); break;
    }
    const InflateError error = stream.reset(invalid);
    require(error == InflateError::invalid_argument, "invalid reset accepted");
}

void equal_results(const InflateResult& left, const InflateResult& right) {
    require(left.status == right.status && left.error == right.error, "invalid reset changed status");
    require(left.input_consumed == right.input_consumed && left.output_produced == right.output_produced,
            "invalid reset changed progress");
    require(left.total_input.value == right.total_input.value && left.total_output.value == right.total_output.value &&
            left.total_work.value == right.total_work.value && left.dictionary_id == right.dictionary_id,
            "invalid reset changed accounting");
}

// The input is borrowed for this call only. Output storage is disjoint from
// input and both workspaces; all bytes outside the published prefix are guards.
InflateResult checked_call(InflateStream& stream, const InflateOptions& options,
                           const InflateResult& previous, std::span<const std::byte> input,
                           Output& output, std::size_t room, bool final, std::byte sentinel) {
    require(room <= output_capacity, "harness output extent");
    output.bytes.fill(sentinel);
    const std::span<std::byte> destination(output.bytes.data() + guard_bytes, room);
    const InflateResult result = stream.process(input, destination, final);
    require(result.input_consumed <= input.size() && result.output_produced <= room, "call bounds exceeded");
    require(result.total_input.value == previous.total_input.value + result.input_consumed, "input total mismatch");
    require(result.total_output.value == previous.total_output.value + result.output_produced, "output total mismatch");
    require(result.total_input.value <= options.limits.input_bytes.value &&
            result.total_output.value <= options.limits.output_bytes.value, "byte quota exceeded");
    require(result.total_work.value >= previous.total_work.value && result.total_work.value <= options.limits.work.value,
            "work quota or monotonicity violated");
    for (std::size_t index = 0; index < guard_bytes; ++index) {
        require(output.bytes[index] == sentinel, "output prefix guard changed");
    }
    for (std::size_t index = guard_bytes + result.output_produced; index < output.bytes.size(); ++index) {
        require(output.bytes[index] == sentinel, "unpublished output changed");
    }
    return result;
}

std::size_t selected_extent(unsigned selector, std::size_t available) {
    constexpr std::array<std::size_t, 15> extents{0, 1, 2, 3, 7, 8, 15, 16, 31, 32, 63, 64, 257, 4096, output_capacity};
    if ((selector & 15U) == 15U) return available;
    const std::size_t extent = extents[selector & 15U];
    if (extent > available) return available;
    return extent;
}

void terminal_probe(InflateStream& stream, const InflateOptions& options, const InflateResult& terminal, Output& output) {
    constexpr std::array<std::byte, 3> input{std::byte{0x4b}, std::byte{0x04}, std::byte{0}};
    invalid_reset(stream, options, 0);
    const InflateResult repeated = checked_call(stream, options, terminal, input, output, 16, false, std::byte{0xa5});
    require(repeated.status == terminal.status && repeated.error == terminal.error &&
            repeated.dictionary_id == terminal.dictionary_id, "terminal result changed");
    require(repeated.input_consumed == 0 && repeated.output_produced == 0 &&
            repeated.total_work.value == terminal.total_work.value, "terminal stream advanced");
}

void scheduled(InflateStream& stream, InflateStream& control, const InflateOptions& options,
               std::span<const std::uint8_t> controls, std::span<const std::byte> input,
               Output& output, Output& reference) {
    require(stream.reset(options) == InflateError::none, "valid reset failed");
    require(control.reset(options) == InflateError::none, "control reset failed");
    std::size_t read = 0;
    InflateResult previous{};
    const bool delayed_final = (controls[2] & 8U) != 0;
    // Twenty-six independently selected calls can include zero-sized buffers.
    // Then use full extents to force termination within the finite work budget.
    for (unsigned call = 0; call < 64; ++call) {
        std::size_t take = input.size() - read;
        std::size_t room = output_capacity;
        if (call < 26) {
            const unsigned input_selector = controls[12 + 2 * call];
            const unsigned output_selector = controls[13 + 2 * call];
            take = selected_extent(input_selector, take);
            room = selected_extent(output_selector, room);
            if ((input_selector & 128U) != 0) invalid_reset(stream, options, controls[3] + call);
        }
        const bool final = read + take == input.size() && (!delayed_final || read == input.size());
        const std::span<const std::byte> source = input.subspan(read, take);
        const InflateResult actual = checked_call(stream, options, previous, source, output, room, final, std::byte{0xa5});
        const InflateResult expected = checked_call(control, options, previous, source, reference, room, final, std::byte{0x5a});
        equal_results(actual, expected);
        for (std::size_t index = 0; index < actual.output_produced; ++index) {
            require(output.bytes[guard_bytes + index] == reference.bytes[guard_bytes + index], "invalid reset changed output");
        }
        read += actual.input_consumed;
        previous = actual;
        if (actual.status == InflateStatus::error || actual.status == InflateStatus::stream_end) {
            terminal_probe(stream, options, actual, output);
            return;
        }
    }
    require(false, "scheduled calls failed to terminate");
}

void reset_and_alias_probes(InflateStream& stream, Output& output) {
    InflateOptions options = ordinary_options();
    constexpr std::array<std::byte, 3> literal{std::byte{0x4b}, std::byte{0x04}, std::byte{0}};
    require(stream.reset(options) == InflateError::none, "reset after arbitrary state failed");
    InflateResult result = checked_call(stream, options, {}, literal, output, 8, true, std::byte{0xa5});
    require(result.status == InflateStatus::stream_end && result.output_produced == 1 &&
            output.bytes[guard_bytes] == std::byte{'a'}, "reset changed literal decoding");
    // A fixed length-3/distance-1 match followed by EOB must not see old 'a'.
    constexpr std::array<std::byte, 3> no_history{std::byte{0x03}, std::byte{0x02}, std::byte{0}};
    require(stream.reset(options) == InflateError::none, "history reset failed");
    result = checked_call(stream, options, {}, no_history, output, 8, true, std::byte{0xa5});
    require(result.error == InflateError::distance_before_history, "reset retained old history");
    options.format = InflateFormat::zlib;
    constexpr std::array<std::byte, 8> empty_zlib{std::byte{0x78}, std::byte{0x9c}, std::byte{3}, std::byte{0},
        std::byte{0}, std::byte{0}, std::byte{0}, std::byte{1}};
    require(stream.reset(options) == InflateError::none, "format reset failed");
    result = checked_call(stream, options, {}, empty_zlib, output, 0, true, std::byte{0xa5});
    require(result.status == InflateStatus::stream_end, "reset retained checksum or format state");
    require(stream.reset(options) == InflateError::none, "alias reset failed");
    std::array<std::byte, 32> overlap{};
    overlap.fill(std::byte{0x9b});
    const std::span<const std::byte> alias_input(overlap.data(), 16);
    const std::span<std::byte> alias_output(overlap.data() + 8, 16);
    result = stream.process(alias_input, alias_output, true);
    require(result.error == InflateError::invalid_argument && result.input_consumed == 0 && result.output_produced == 0,
            "overlapping caller buffers accepted");
    for (std::byte value : overlap) require(value == std::byte{0x9b}, "overlap rejection changed caller storage");
    require(stream.reset(options) == InflateError::none, "workspace alias reset failed");
    const std::span<InflateStream> object(&stream, 1);
    const std::span<std::byte> workspace = std::as_writable_bytes(object);
    result = stream.process({}, workspace.first(8), true);
    require(result.error == InflateError::invalid_argument && result.output_produced == 0, "workspace alias accepted");
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    if (size < control_bytes || size > 65536) return 0;
    const std::span<const std::uint8_t> controls(data, control_bytes);
    const std::span<const std::byte> input(reinterpret_cast<const std::byte*>(data + control_bytes), size - control_bytes);
    std::unique_ptr<InflateStream> stream = std::make_unique<InflateStream>();
    std::unique_ptr<InflateStream> control = std::make_unique<InflateStream>();
    std::unique_ptr<Output> output = std::make_unique<Output>();
    std::unique_ptr<Output> reference = std::make_unique<Output>();
    const InflateResult uninitialized = (*stream).process({}, {}, false);
    require(uninitialized.error == InflateError::invalid_state, "uninitialized stream accepted");
    const InflateOptions options = selected_options(controls);
    scheduled(*stream, *control, options, controls, input, *output, *reference);
    reset_and_alias_probes(*stream, *output);
    return 0;
}

#if defined(GUI_FORMS_FUZZ_STANDALONE)
int main() {
    std::array<std::uint8_t, 192> bytes{};
    std::uint32_t random = 0x20c0ffeeU;
    for (unsigned iteration = 0; iteration < 2048; ++iteration) {
        for (std::uint8_t& value : bytes) {
            random = random * 1664525U + 1013904223U;
            value = static_cast<std::uint8_t>(random >> 24U);
        }
        const std::size_t size = control_bytes + iteration % 129U;
        LLVMFuzzerTestOneInput(bytes.data(), size);
    }
    std::puts("inflate protocol: 2048 deterministic schedules passed");
    return 0;
}
#endif
