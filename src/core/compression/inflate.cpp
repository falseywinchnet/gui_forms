// Altered C++ implementation: canonical Huffman construction/decoding derived
// from Mark Adler's puff (zlib pin in PROVENANCE.md). Original notice is retained
// in LICENSE-puff.txt. Streaming, ownership, quotas and wrapper are rewritten.
#include "gui_forms/compression/inflate.hpp"

#include <limits>

namespace gui_forms::compression {
namespace {

// Integer-address ordering is confined to this buffer-boundary overlap check.
// Subtraction avoids overflowing end-address arithmetic. Spans must themselves
// denote valid live storage; no decoder can validate arbitrary caller pointers.
bool overlaps(const void* first, std::size_t first_size,
              const void* second, std::size_t second_size) noexcept {
    if (first_size == 0 || second_size == 0) return false;
    const std::uintptr_t a = reinterpret_cast<std::uintptr_t>(first);
    const std::uintptr_t b = reinterpret_cast<std::uintptr_t>(second);
    if (a <= b) {
        const std::uintptr_t distance = b - a;
        const bool overlap = distance < first_size;
        return overlap;
    }
    const std::uintptr_t distance = a - b;
    const bool overlap = distance < second_size;
    return overlap;
}

constexpr std::array<unsigned, 29> length_base{
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27,
    31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
constexpr std::array<unsigned, 29> length_extra{
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2,
    2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
constexpr std::array<unsigned, 30> distance_base{
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129,
    193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097,
    6145, 8193, 12289, 16385, 24577};
constexpr std::array<unsigned, 30> distance_extra{
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6,
    6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
constexpr std::array<unsigned, 19> alphabet_order{
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

} // namespace

InflateError InflateStream::reset(const InflateOptions& options) noexcept {
    const std::uint64_t unlimited = std::numeric_limits<std::uint64_t>::max();
    if ((options.format != InflateFormat::zlib && options.format != InflateFormat::raw_deflate) ||
        options.window_bits < 8 || options.window_bits > 15 ||
        options.limits.input_bytes.value == unlimited ||
        options.limits.output_bytes.value == unlimited || options.limits.work.value == unlimited) {
        return InflateError::invalid_argument;
    }
    options_ = options;
    phase_ = Phase::header;
    if (options.format == InflateFormat::raw_deflate) phase_ = Phase::block;
    error_ = InflateError::none;
    input_total_ = ByteCount{};
    output_total_ = ByteCount{};
    work_total_ = WorkCount{};
    history_next_ = 0;
    history_live_ = 0;
    window_size_ = std::size_t{1} << options.window_bits;
    code_ = CodeCursor{};
    bit_buffer_ = 0;
    bit_count_ = 0;
    literal_count_ = 0;
    distance_count_ = 0;
    alphabet_count_ = 0;
    length_index_ = 0;
    repeat_symbol_ = 0;
    match_length_ = 0;
    match_distance_ = 0;
    extra_bits_ = 0;
    literal_byte_ = 0;
    stored_left_ = 0;
    final_block_ = false;
    adler_low_ = 1;
    adler_high_ = 0;
    trailer_ = 0;
    trailer_bytes_ = 0;
    dictionary_id_ = 0;
    // Old history and trees are outside the live ranges until replaced. Reset
    // does not clear caller output or read history belonging to an old stream.
    return InflateError::none;
}

bool InflateStream::fail(InflateError error) noexcept {
    error_ = error;
    phase_ = Phase::failed;
    return false;
}

bool InflateStream::charge(std::uint64_t amount) noexcept {
    const std::uint64_t remaining = options_.limits.work.value - work_total_.value;
    if (amount > remaining) {
        fail(InflateError::work_limit_exceeded);
        return false;
    }
    work_total_.value += amount;
    return true;
}

bool InflateStream::bits(Call& call, unsigned count, unsigned& value) noexcept {
    // All callers request at most 16 bits. At most seven surplus bits survive.
    while (bit_count_ < count) {
        if (call.read == call.input.size()) {
            if (call.final_input) fail(InflateError::truncated_input);
            call.status = InflateStatus::need_input;
            return false;
        }
        if (input_total_.value == options_.limits.input_bytes.value) {
            fail(InflateError::input_limit_exceeded);
            return false;
        }
        if (!charge(1)) return false;
        const std::uint32_t next = std::to_integer<std::uint32_t>(call.input[call.read]);
        bit_buffer_ |= next << bit_count_;
        bit_count_ += 8;
        ++call.read;
        ++input_total_.value;
    }
    if (!charge(1)) return false;
    const std::uint32_t mask = (std::uint32_t{1} << count) - 1U;
    value = bit_buffer_ & mask;
    bit_buffer_ >>= count;
    bit_count_ -= count;
    return true;
}

bool InflateStream::symbol(Call& call, const Tree& tree, unsigned& value) noexcept {
    while (code_.length <= tree.maximum_length) {
        unsigned bit = 0;
        if (!bits(call, 1, bit)) return false;
        code_.code |= bit;
        const unsigned count = tree.counts[code_.length];
        const unsigned end = code_.first + count;
        if (code_.code >= code_.first && code_.code < end) {
            const unsigned offset = code_.code - code_.first;
            value = tree.symbols[code_.index + offset];
            code_ = CodeCursor{};
            return true;
        }
        code_.index += count;
        code_.first = end << 1U;
        code_.code <<= 1U;
        ++code_.length;
    }
    fail(InflateError::invalid_huffman_code);
    return false;
}

bool InflateStream::build_tree(Tree& tree, std::span<const std::uint8_t> lengths,
                               bool code_alphabet) noexcept {
    // Precharge all count/offset initialization, two symbol passes and 15
    // Kraft-space steps. No table size depends on unvalidated stream values.
    const std::uint64_t work = 64U + 2U * lengths.size();
    if (!charge(work)) return false;
    tree.counts.fill(0);
    tree.maximum_length = 0;
    for (std::size_t i = 0; i < lengths.size(); ++i) {
        const unsigned length = lengths[i];
        ++tree.counts[length];
        if (length > tree.maximum_length) tree.maximum_length = length;
    }
    if (tree.maximum_length == 0) {
        if (code_alphabet) {
            fail(InflateError::incomplete_tree);
            return false;
        }
        return true; // Empty distance alphabet is legal until a match uses it.
    }
    int available = 1;
    for (unsigned length = 1; length <= 15; ++length) {
        available *= 2;
        available -= tree.counts[length];
        if (available < 0) {
            fail(InflateError::oversubscribed_tree);
            return false;
        }
    }
    if (available != 0 && (code_alphabet || tree.maximum_length != 1)) {
        fail(InflateError::incomplete_tree);
        return false;
    }
    std::array<unsigned, 16> offsets{};
    for (unsigned length = 1; length < 15; ++length) {
        offsets[length + 1] = offsets[length] + tree.counts[length];
    }
    for (std::size_t i = 0; i < lengths.size(); ++i) {
        const unsigned length = lengths[i];
        if (length != 0) {
            const unsigned offset = offsets[length];
            tree.symbols[offset] = static_cast<std::uint16_t>(i);
            ++offsets[length];
        }
    }
    return true;
}

bool InflateStream::fixed_trees() noexcept {
    if (!charge(320)) return false;
    for (unsigned i = 0; i < 144; ++i) lengths_[i] = 8;
    for (unsigned i = 144; i < 256; ++i) lengths_[i] = 9;
    for (unsigned i = 256; i < 280; ++i) lengths_[i] = 7;
    for (unsigned i = 280; i < 288; ++i) lengths_[i] = 8;
    const std::span<const std::uint8_t> literal_lengths(lengths_.data(), 288);
    if (!build_tree(literals_, literal_lengths, false)) return false;
    for (unsigned i = 0; i < 32; ++i) lengths_[i] = 5;
    const std::span<const std::uint8_t> distance_lengths(lengths_.data(), 32);
    const bool built = build_tree(distances_, distance_lengths, false);
    return built;
}

bool InflateStream::output_ready(Call& call) noexcept {
    if (output_total_.value == options_.limits.output_bytes.value) {
        fail(InflateError::output_limit_exceeded);
        return false;
    }
    if (call.written == call.output.size()) {
        call.status = InflateStatus::need_output;
        return false;
    }
    const bool charged = charge(1);
    return charged;
}

void InflateStream::emit(Call& call, std::byte value) noexcept {
    call.output[call.written] = value;
    ++call.written;
    ++output_total_.value;
    history_[history_next_] = value;
    history_next_ = (history_next_ + 1U) % history_.size();
    if (history_live_ < history_.size()) ++history_live_;
    // Each sum is less than 65521 before adding. The largest intermediate is
    // 131040, independent of stream length; no deferred-reduction overflow.
    adler_low_ += std::to_integer<std::uint32_t>(value);
    if (adler_low_ >= 65521U) adler_low_ -= 65521U;
    adler_high_ += adler_low_;
    if (adler_high_ >= 65521U) adler_high_ -= 65521U;
}

void InflateStream::end_block() noexcept {
    phase_ = Phase::block;
    if (final_block_) {
        // No speculative byte reads: only unused bits of the final byte remain.
        bit_buffer_ = 0;
        bit_count_ = 0;
        phase_ = Phase::trailer;
        if (options_.format == InflateFormat::raw_deflate) phase_ = Phase::done;
    }
}

bool InflateStream::step(Call& call) noexcept {
    unsigned value = 0;
    switch (phase_) {
    case Phase::header: {
        if (!bits(call, 16, value)) return false;
        const unsigned cmf = value & 255U;
        const unsigned flags = value >> 8U;
        if ((cmf & 15U) != 8) {
            fail(InflateError::invalid_method);
            return false;
        }
        const unsigned window_bits = (cmf >> 4U) + 8U;
        if (window_bits > options_.window_bits || window_bits > 15) {
            fail(InflateError::invalid_window);
            return false;
        }
        const unsigned header = (cmf << 8U) | flags;
        if (header % 31U != 0) {
            fail(InflateError::invalid_header_check);
            return false;
        }
        window_size_ = std::size_t{1} << window_bits;
        phase_ = Phase::block;
        if ((flags & 32U) != 0) phase_ = Phase::dictionary;
        return true;
    }
    case Phase::dictionary:
        if (!bits(call, 8, value)) return false;
        dictionary_id_ = (dictionary_id_ << 8U) | value;
        ++trailer_bytes_;
        if (trailer_bytes_ == 4) {
            fail(InflateError::dictionary_required);
            return false;
        }
        return true;
    case Phase::block: {
        if (!bits(call, 3, value)) return false;
        final_block_ = (value & 1U) != 0;
        const unsigned type = value >> 1U;
        if (type == 3) {
            fail(InflateError::invalid_block_type);
            return false;
        }
        if (type == 0) {
            bit_buffer_ = 0;
            bit_count_ = 0;
            phase_ = Phase::stored_length;
        } else if (type == 1) {
            if (!fixed_trees()) return false;
            phase_ = Phase::literal;
        } else {
            phase_ = Phase::dynamic_counts;
        }
        return true;
    }
    case Phase::stored_length:
        if (!bits(call, 16, stored_left_)) return false;
        phase_ = Phase::stored_complement;
        return true;
    case Phase::stored_complement:
        if (!bits(call, 16, value)) return false;
        if ((stored_left_ ^ value) != 65535U) {
            fail(InflateError::stored_length_mismatch);
            return false;
        }
        phase_ = Phase::stored_bytes;
        return true;
    case Phase::stored_bytes:
        if (stored_left_ == 0) {
            end_block();
            return true;
        }
        if (!output_ready(call)) return false;
        if (!bits(call, 8, value)) return false;
        emit(call, static_cast<std::byte>(value));
        --stored_left_;
        return true;
    case Phase::dynamic_counts:
        if (!bits(call, 14, value)) return false;
        literal_count_ = (value & 31U) + 257U;
        distance_count_ = ((value >> 5U) & 31U) + 1U;
        alphabet_count_ = (value >> 10U) + 4U;
        if (literal_count_ > 286 || distance_count_ > 30) {
            fail(InflateError::invalid_code_counts);
            return false;
        }
        if (!charge(19)) return false;
        alphabet_lengths_.fill(0);
        length_index_ = 0;
        phase_ = Phase::code_lengths;
        return true;
    case Phase::code_lengths:
        if (length_index_ < alphabet_count_) {
            if (!bits(call, 3, value)) return false;
            const unsigned index = alphabet_order[length_index_];
            alphabet_lengths_[index] = static_cast<std::uint8_t>(value);
            ++length_index_;
            return true;
        }
        if (!build_tree(code_lengths_, alphabet_lengths_, true)) return false;
        length_index_ = 0;
        phase_ = Phase::expanded_lengths;
        return true;
    case Phase::expanded_lengths: {
        const unsigned total = literal_count_ + distance_count_;
        if (length_index_ == total) {
            if (lengths_[256] == 0) {
                fail(InflateError::missing_end_of_block);
                return false;
            }
            const std::span<const std::uint8_t> literal_lengths(lengths_.data(), literal_count_);
            if (!build_tree(literals_, literal_lengths, false)) return false;
            const std::span<const std::uint8_t> distance_lengths(lengths_.data() + literal_count_, distance_count_);
            if (!build_tree(distances_, distance_lengths, false)) return false;
            phase_ = Phase::literal;
            return true;
        }
        if (!symbol(call, code_lengths_, value)) return false;
        if (value < 16) {
            lengths_[length_index_] = static_cast<std::uint8_t>(value);
            ++length_index_;
        } else {
            if (value == 16 && length_index_ == 0) {
                fail(InflateError::invalid_code_length_repeat);
                return false;
            }
            repeat_symbol_ = value;
            phase_ = Phase::repeat;
        }
        return true;
    }
    case Phase::repeat: {
        unsigned count_bits = 2;
        unsigned base = 3;
        std::uint8_t length = 0;
        if (repeat_symbol_ == 16) length = lengths_[length_index_ - 1];
        if (repeat_symbol_ == 17) count_bits = 3;
        if (repeat_symbol_ == 18) {
            count_bits = 7;
            base = 11;
        }
        if (!bits(call, count_bits, value)) return false;
        const unsigned repeat = base + value;
        const unsigned remaining = literal_count_ + distance_count_ - length_index_;
        if (repeat > remaining) {
            fail(InflateError::invalid_code_length_repeat);
            return false;
        }
        if (!charge(repeat)) return false;
        for (unsigned i = 0; i < repeat; ++i) {
            lengths_[length_index_] = length;
            ++length_index_;
        }
        phase_ = Phase::expanded_lengths;
        return true;
    }
    case Phase::literal:
        if (!symbol(call, literals_, value)) return false;
        if (value < 256) {
            literal_byte_ = value;
            phase_ = Phase::emit_literal;
        } else if (value == 256) {
            end_block();
        } else {
            if (value > 285) {
                fail(InflateError::invalid_literal_length);
                return false;
            }
            const unsigned index = value - 257U;
            match_length_ = length_base[index];
            extra_bits_ = length_extra[index];
            phase_ = Phase::length_extra;
        }
        return true;
    case Phase::emit_literal:
        if (!output_ready(call)) return false;
        emit(call, static_cast<std::byte>(literal_byte_));
        phase_ = Phase::literal;
        return true;
    case Phase::length_extra:
        if (!bits(call, extra_bits_, value)) return false;
        match_length_ += value;
        phase_ = Phase::distance;
        return true;
    case Phase::distance:
        if (!symbol(call, distances_, value)) return false;
        if (value > 29) {
            fail(InflateError::invalid_distance_code);
            return false;
        }
        match_distance_ = distance_base[value];
        extra_bits_ = distance_extra[value];
        phase_ = Phase::distance_extra;
        return true;
    case Phase::distance_extra:
        if (!bits(call, extra_bits_, value)) return false;
        match_distance_ += value;
        if (match_distance_ > window_size_) {
            fail(InflateError::distance_exceeds_window);
            return false;
        }
        if (match_distance_ > history_live_) {
            fail(InflateError::distance_before_history);
            return false;
        }
        phase_ = Phase::match;
        return true;
    case Phase::match: {
        if (match_length_ == 0) {
            phase_ = Phase::literal;
            return true;
        }
        if (!output_ready(call)) return false;
        const std::size_t source = (history_next_ + history_.size() - match_distance_) % history_.size();
        const std::byte byte = history_[source];
        emit(call, byte);
        --match_length_;
        return true;
    }
    case Phase::trailer:
        if (!bits(call, 8, value)) return false;
        trailer_ = (trailer_ << 8U) | value;
        ++trailer_bytes_;
        if (trailer_bytes_ == 4) {
            const std::uint32_t adler = (adler_high_ << 16U) | adler_low_;
            if (trailer_ != adler) {
                fail(InflateError::adler_mismatch);
                return false;
            }
            phase_ = Phase::done;
        }
        return true;
    case Phase::idle:
        fail(InflateError::invalid_state);
        return false;
    case Phase::failed:
    case Phase::done:
        return false;
    }
    fail(InflateError::invalid_state);
    return false;
}

InflateResult InflateStream::process(std::span<const std::byte> input,
                                     std::span<std::byte> output,
                                     bool final_input) noexcept {
    Call call{.input = input, .output = output, .final_input = final_input};
    const bool input_output_overlap = overlaps(input.data(), input.size(), output.data(), output.size());
    const bool input_state_overlap = overlaps(input.data(), input.size(), this, sizeof(*this));
    const bool output_state_overlap = overlaps(output.data(), output.size(), this, sizeof(*this));
    if (phase_ != Phase::failed && phase_ != Phase::done &&
        (input_output_overlap || input_state_overlap || output_state_overlap)) {
        fail(InflateError::invalid_argument);
    }
    if (phase_ == Phase::idle) fail(InflateError::invalid_state);
    while (phase_ != Phase::done && phase_ != Phase::failed) {
        if (!charge(1)) break;
        if (!step(call)) break;
    }
    if (phase_ == Phase::done) call.status = InflateStatus::stream_end;
    if (phase_ == Phase::failed) call.status = InflateStatus::error;
    const InflateResult result{
        .status = call.status, .error = error_,
        .input_consumed = call.read, .output_produced = call.written,
        .total_input = input_total_, .total_output = output_total_,
        .total_work = work_total_, .dictionary_id = dictionary_id_};
    return result;
}

} // namespace gui_forms::compression
