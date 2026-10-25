#include "theme_number.hpp"

#include <array>
#include <bit>

namespace gui_forms::theme_language {

namespace {

// The decimal form of a number: value = sign × digits × 10^exponent, with
// digits free of trailing zeros (zero when the number is zero).
struct DecimalForm final {
    bool negative{};
    std::uint64_t digits{};
    // Significant digits in `digits`.
    std::size_t digit_count{};
    // The decimal exponent, saturated far outside the admitted range.
    std::int64_t exponent{};
    bool integer{};
    NumberDefect defect{NumberDefect::none};
};

// An exponent written with more digits saturates here: any value past it is
// out of range anyway, and saturation keeps the arithmetic in 64 bits.
inline constexpr std::int64_t exponent_saturation = 100000;

[[nodiscard]] bool decimal_digit(char byte) noexcept {
    const bool digit = byte >= '0' && byte <= '9';
    return digit;
}

// Reads the text into its decimal form, or names its defect. One pass over
// the text, so a number of any length costs its length.
[[nodiscard]] DecimalForm read_decimal_form(std::string_view text) noexcept {
    DecimalForm form{};
    form.integer = true;
    const std::size_t size = text.size();
    std::size_t index = 0U;
    if (index < size && (text[index] == '+' || text[index] == '-')) {
        form.negative = text[index] == '-';
        ++index;
    }
    // Zeros after the last nonzero digit, not yet in `digits`.
    std::int64_t pending_zeros = 0;
    // Digits after the decimal point.
    std::int64_t fraction_digits = 0;
    std::size_t mantissa_digits = 0U;
    bool fraction = false;
    while (index < size) {
        const char byte = text[index];
        if (byte == '.' && !fraction) {
            fraction = true;
            form.integer = false;
            ++index;
            // CSS requires a digit after the point.
            if (index >= size || !decimal_digit(text[index])) {
                form.defect = NumberDefect::not_a_number;
                return form;
            }
            continue;
        }
        if (!decimal_digit(byte)) break;
        ++mantissa_digits;
        if (fraction) ++fraction_digits;
        if (byte == '0') {
            if (form.digit_count != 0U) ++pending_zeros;
            ++index;
            continue;
        }
        // A nonzero digit: the pending zeros are significant after all.
        const std::size_t needed = form.digit_count + static_cast<std::size_t>(pending_zeros) + 1U;
        if (needed > maximum_significant_digits) {
            form.defect = NumberDefect::too_precise;
            return form;
        }
        for (std::int64_t zero = 0; zero < pending_zeros; ++zero) form.digits *= 10U;
        form.digit_count += static_cast<std::size_t>(pending_zeros);
        pending_zeros = 0;
        const std::uint64_t digit = static_cast<std::uint64_t>(byte - '0');
        form.digits = form.digits * 10U + digit;
        ++form.digit_count;
        ++index;
    }
    if (mantissa_digits == 0U) {
        form.defect = NumberDefect::not_a_number;
        return form;
    }
    std::int64_t written_exponent = 0;
    if (index < size && (text[index] == 'e' || text[index] == 'E')) {
        form.integer = false;
        ++index;
        bool exponent_negative = false;
        if (index < size && (text[index] == '+' || text[index] == '-')) {
            exponent_negative = text[index] == '-';
            ++index;
        }
        std::size_t exponent_digits = 0U;
        while (index < size && decimal_digit(text[index])) {
            const std::int64_t digit = static_cast<std::int64_t>(text[index] - '0');
            if (written_exponent < exponent_saturation) written_exponent = written_exponent * 10 + digit;
            ++exponent_digits;
            ++index;
        }
        if (exponent_digits == 0U) {
            form.defect = NumberDefect::not_a_number;
            return form;
        }
        if (exponent_negative) written_exponent = -written_exponent;
    }
    if (index != size) {
        form.defect = NumberDefect::not_a_number;
        return form;
    }
    if (form.digit_count == 0U) {
        form.exponent = 0;
        return form;
    }
    // value = digits × 10^(pending_zeros − fraction_digits + written).
    form.exponent = pending_zeros - fraction_digits + written_exponent;
    return form;
}

// An unsigned integer of at most bignum_limbs 32-bit limbs, least
// significant first. The admitted range needs about 250 bits (10^58 shifted
// by 53 bits); the capacity is 512.
inline constexpr std::size_t bignum_limbs = 16U;

struct Bignum final {
    std::array<std::uint32_t, bignum_limbs> limbs{};
    // Live limbs; the most significant live limb is nonzero, or size is 0.
    std::size_t size{};
};

[[nodiscard]] Bignum bignum_from(std::uint64_t value) noexcept {
    Bignum result{};
    while (value != 0U) {
        result.limbs[result.size] = static_cast<std::uint32_t>(value & 0xFFFFFFFFU);
        ++result.size;
        value >>= 32U;
    }
    return result;
}

// number ×= factor.
void bignum_multiply_small(Bignum& number, std::uint32_t factor) noexcept {
    std::uint64_t carry = 0U;
    for (std::size_t limb = 0U; limb < number.size; ++limb) {
        const std::uint64_t product = static_cast<std::uint64_t>(number.limbs[limb]) * factor + carry;
        number.limbs[limb] = static_cast<std::uint32_t>(product & 0xFFFFFFFFU);
        carry = product >> 32U;
    }
    if (carry != 0U && number.size < bignum_limbs) {
        number.limbs[number.size] = static_cast<std::uint32_t>(carry);
        ++number.size;
    }
}

// number ×= 10^power.
void bignum_multiply_power_of_ten(Bignum& number, std::int64_t power) noexcept {
    // 10^9 is the largest power of ten in 32 bits.
    while (power >= 9) {
        bignum_multiply_small(number, 1000000000U);
        power -= 9;
    }
    std::uint32_t rest = 1U;
    for (std::int64_t step = 0; step < power; ++step) rest *= 10U;
    if (rest != 1U) bignum_multiply_small(number, rest);
}

[[nodiscard]] std::size_t bignum_bit_length(const Bignum& number) noexcept {
    if (number.size == 0U) return 0U;
    const std::uint32_t top = number.limbs[number.size - 1U];
    const std::size_t top_bits = 32U - static_cast<std::size_t>(std::countl_zero(top));
    const std::size_t length = (number.size - 1U) * 32U + top_bits;
    return length;
}

// number <<= bits. The admitted range never needs more than bignum_limbs;
// a shift past them would be an internal defect and leaves number as it is.
void bignum_shift_left(Bignum& number, std::size_t bits) noexcept {
    if (number.size == 0U || bits == 0U) return;
    const std::size_t limb_shift = bits / 32U;
    const std::size_t bit_shift = bits % 32U;
    const std::size_t new_size = number.size + limb_shift + 1U;
    if (new_size > bignum_limbs) return;
    std::array<std::uint32_t, bignum_limbs> shifted{};
    for (std::size_t source = 0U; source < number.size; ++source) {
        const std::uint64_t moved = static_cast<std::uint64_t>(number.limbs[source]) << bit_shift;
        const std::size_t target = source + limb_shift;
        shifted[target] |= static_cast<std::uint32_t>(moved & 0xFFFFFFFFU);
        shifted[target + 1U] |= static_cast<std::uint32_t>(moved >> 32U);
    }
    number.limbs = shifted;
    number.size = new_size;
    while (number.size > 0U && number.limbs[number.size - 1U] == 0U) --number.size;
}

// number >>= 1.
void bignum_halve(Bignum& number) noexcept {
    for (std::size_t limb = 0U; limb < number.size; ++limb) {
        const std::uint32_t next = limb + 1U < number.size ? number.limbs[limb + 1U] : 0U;
        number.limbs[limb] = (number.limbs[limb] >> 1U) | (next << 31U);
    }
    while (number.size > 0U && number.limbs[number.size - 1U] == 0U) --number.size;
}

// −1, 0 or 1 as left is below, equal to or above right.
[[nodiscard]] int bignum_compare(const Bignum& left, const Bignum& right) noexcept {
    if (left.size != right.size) return left.size < right.size ? -1 : 1;
    for (std::size_t limb = left.size; limb > 0U; --limb) {
        const std::uint32_t a = left.limbs[limb - 1U];
        const std::uint32_t b = right.limbs[limb - 1U];
        if (a != b) return a < b ? -1 : 1;
    }
    return 0;
}

// left −= right; left must not be below right.
void bignum_subtract(Bignum& left, const Bignum& right) noexcept {
    std::uint64_t borrow = 0U;
    for (std::size_t limb = 0U; limb < left.size; ++limb) {
        const std::uint64_t subtrahend = (limb < right.size ? right.limbs[limb] : 0U) + borrow;
        const std::uint64_t minuend = left.limbs[limb];
        if (minuend >= subtrahend) {
            left.limbs[limb] = static_cast<std::uint32_t>(minuend - subtrahend);
            borrow = 0U;
        } else {
            left.limbs[limb] = static_cast<std::uint32_t>((minuend + 0x100000000ULL) - subtrahend);
            borrow = 1U;
        }
    }
    while (left.size > 0U && left.limbs[left.size - 1U] == 0U) --left.size;
}

// The quotient of numerator ÷ (denominator × 2^scale) to 54 bits, with its
// remainder and the divisor the remainder is of, for rounding.
struct ScaledQuotient final {
    std::uint64_t quotient{};
    Bignum remainder{};
    Bignum divisor{};
};

// Requires numerator ÷ (denominator × 2^scale) below 2^54.
[[nodiscard]] ScaledQuotient divide_scaled(const Bignum& numerator, const Bignum& denominator,
                                           std::int64_t scale) noexcept {
    ScaledQuotient result{};
    result.remainder = numerator;
    result.divisor = denominator;
    if (scale >= 0) {
        bignum_shift_left(result.divisor, static_cast<std::size_t>(scale));
    } else {
        bignum_shift_left(result.remainder, static_cast<std::size_t>(-scale));
    }
    // Long division, one quotient bit at a time from bit 53 down.
    Bignum step = result.divisor;
    bignum_shift_left(step, 53U);
    for (std::size_t bit = 0U; bit < 54U; ++bit) {
        result.quotient <<= 1U;
        if (bignum_compare(result.remainder, step) >= 0) {
            bignum_subtract(result.remainder, step);
            result.quotient |= 1U;
        }
        bignum_halve(step);
    }
    return result;
}

// Powers of ten that doubles hold exactly: 10^0 to 10^22.
inline constexpr std::array<double, 23> exact_powers_of_ten{
    1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,  1e10, 1e11,
    1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22,
};

inline constexpr std::uint64_t exact_integer_limit = std::uint64_t{1} << 53U;

// The correctly rounded double of digits × 10^exponent, for a value inside
// the admitted magnitudes (a normal double).
[[nodiscard]] double round_decimal(std::uint64_t digits, std::int64_t exponent) noexcept {
    // Exact operands and one IEEE operation, which rounds correctly.
    if (digits <= exact_integer_limit && exponent >= -22 && exponent <= 22) {
        const double significand = static_cast<double>(digits);
        if (exponent >= 0) {
            const double product = significand * exact_powers_of_ten[static_cast<std::size_t>(exponent)];
            return product;
        }
        const double quotient = significand / exact_powers_of_ten[static_cast<std::size_t>(-exponent)];
        return quotient;
    }
    // Exactly: value = numerator ÷ denominator.
    Bignum numerator = bignum_from(digits);
    Bignum denominator = bignum_from(1U);
    if (exponent >= 0) {
        bignum_multiply_power_of_ten(numerator, exponent);
    } else {
        bignum_multiply_power_of_ten(denominator, -exponent);
    }
    // Choose the binary scale so the quotient has 53 or 54 bits.
    const std::int64_t numerator_bits = static_cast<std::int64_t>(bignum_bit_length(numerator));
    const std::int64_t denominator_bits = static_cast<std::int64_t>(bignum_bit_length(denominator));
    std::int64_t scale = numerator_bits - denominator_bits - 53;
    ScaledQuotient division = divide_scaled(numerator, denominator, scale);
    if (division.quotient >= exact_integer_limit) {
        ++scale;
        division = divide_scaled(numerator, denominator, scale);
    }
    // Round to nearest, ties to even: compare twice the remainder with the
    // divisor.
    Bignum twice_remainder = division.remainder;
    bignum_shift_left(twice_remainder, 1U);
    const int half = bignum_compare(twice_remainder, division.divisor);
    std::uint64_t quotient = division.quotient;
    const bool odd = (quotient & 1U) != 0U;
    if (half > 0 || (half == 0 && odd)) ++quotient;
    if (quotient == exact_integer_limit) {
        quotient >>= 1U;
        ++scale;
    }
    // value = quotient × 2^scale, quotient in [2^52, 2^53).
    const std::uint64_t biased_exponent = static_cast<std::uint64_t>(scale + 52 + 1023);
    const std::uint64_t fraction_bits = quotient - (exact_integer_limit >> 1U);
    const std::uint64_t bits = (biased_exponent << 52U) | fraction_bits;
    const double value = std::bit_cast<double>(bits);
    return value;
}

} // namespace

NumberConversion convert_theme_number(std::string_view text) noexcept {
    NumberConversion result{};
    const DecimalForm form = read_decimal_form(text);
    result.integer = form.integer;
    if (form.defect != NumberDefect::none) {
        result.defect = form.defect;
        return result;
    }
    if (form.digits == 0U) {
        result.value = form.negative ? -0.0 : 0.0;
        return result;
    }
    // The value lies in [10^(count − 1 + exponent), 10^(count + exponent)).
    const std::int64_t count = static_cast<std::int64_t>(form.digit_count);
    const std::int64_t lowest_magnitude = count - 1 + form.exponent;
    const std::int64_t highest_magnitude = count + form.exponent;
    if (lowest_magnitude < minimum_decimal_magnitude || highest_magnitude > maximum_decimal_magnitude) {
        result.defect = NumberDefect::out_of_range;
        return result;
    }
    const double magnitude = round_decimal(form.digits, form.exponent);
    result.value = form.negative ? -magnitude : magnitude;
    return result;
}

} // namespace gui_forms::theme_language
