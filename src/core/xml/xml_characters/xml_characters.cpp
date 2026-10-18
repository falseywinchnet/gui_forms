#include "xml_characters.hpp"

namespace gui_forms::detail {

namespace {

[[nodiscard]] bool is_continuation(unsigned char byte) noexcept {
    const bool continuation = (byte & 0xC0U) == 0x80U;
    return continuation;
}

[[nodiscard]] bool within(std::uint32_t scalar, std::uint32_t first,
                          std::uint32_t last) noexcept {
    const bool inside = scalar >= first && scalar <= last;
    return inside;
}

} // namespace

XmlDecodedCharacter decode_xml_utf8(std::string_view text, std::size_t offset) noexcept {
    const XmlDecodedCharacter invalid{};
    const std::size_t available = text.size() - offset;
    const unsigned char lead = static_cast<unsigned char>(text[offset]);
    if (lead < 0x80U) {
        const XmlDecodedCharacter ascii{.scalar = lead, .length = 1};
        return ascii;
    }
    // The second byte's admitted range depends on the lead byte (Unicode
    // table 3-7): this is what rejects overlong forms, surrogates and values
    // above U+10FFFF.
    std::uint32_t length = 0;
    unsigned char second_low = 0x80U;
    unsigned char second_high = 0xBFU;
    std::uint32_t scalar = 0;
    if (lead >= 0xC2U && lead <= 0xDFU) {
        length = 2;
        scalar = lead & 0x1FU;
    } else if (lead >= 0xE0U && lead <= 0xEFU) {
        length = 3;
        scalar = lead & 0x0FU;
        if (lead == 0xE0U) second_low = 0xA0U;
        if (lead == 0xEDU) second_high = 0x9FU;
    } else if (lead >= 0xF0U && lead <= 0xF4U) {
        length = 4;
        scalar = lead & 0x07U;
        if (lead == 0xF0U) second_low = 0x90U;
        if (lead == 0xF4U) second_high = 0x8FU;
    } else {
        return invalid;
    }
    if (available < length) return invalid;
    const unsigned char second = static_cast<unsigned char>(text[offset + 1U]);
    if (second < second_low || second > second_high) return invalid;
    scalar = (scalar << 6U) | (second & 0x3FU);
    for (std::uint32_t index = 2; index < length; ++index) {
        const unsigned char next = static_cast<unsigned char>(text[offset + index]);
        if (!is_continuation(next)) return invalid;
        scalar = (scalar << 6U) | (next & 0x3FU);
    }
    const XmlDecodedCharacter decoded{.scalar = scalar, .length = length};
    return decoded;
}

bool is_xml_character(std::uint32_t scalar) noexcept {
    if (scalar == 0x9U || scalar == 0xAU || scalar == 0xDU) return true;
    if (within(scalar, 0x20U, 0xD7FFU)) return true;
    if (within(scalar, 0xE000U, 0xFFFDU)) return true;
    const bool supplementary = within(scalar, 0x10000U, 0x10FFFFU);
    return supplementary;
}

bool is_xml_name_start(std::uint32_t scalar) noexcept {
    if (scalar == ':' || scalar == '_') return true;
    if (within(scalar, 'A', 'Z') || within(scalar, 'a', 'z')) return true;
    if (within(scalar, 0xC0U, 0xD6U) || within(scalar, 0xD8U, 0xF6U)) return true;
    if (within(scalar, 0xF8U, 0x2FFU) || within(scalar, 0x370U, 0x37DU)) return true;
    if (within(scalar, 0x37FU, 0x1FFFU) || within(scalar, 0x200CU, 0x200DU)) return true;
    if (within(scalar, 0x2070U, 0x218FU) || within(scalar, 0x2C00U, 0x2FEFU)) return true;
    if (within(scalar, 0x3001U, 0xD7FFU) || within(scalar, 0xF900U, 0xFDCFU)) return true;
    if (within(scalar, 0xFDF0U, 0xFFFDU)) return true;
    const bool supplementary = within(scalar, 0x10000U, 0xEFFFFU);
    return supplementary;
}

bool is_xml_name_character(std::uint32_t scalar) noexcept {
    if (is_xml_name_start(scalar)) return true;
    if (scalar == '-' || scalar == '.' || scalar == 0xB7U) return true;
    if (within(scalar, '0', '9')) return true;
    const bool combining = within(scalar, 0x300U, 0x36FU) || within(scalar, 0x203FU, 0x2040U);
    return combining;
}

bool is_xml_space(std::uint32_t scalar) noexcept {
    const bool space = scalar == 0x20U || scalar == 0x9U || scalar == 0xDU || scalar == 0xAU;
    return space;
}

XmlError validate_xml_name(std::string_view name, std::size_t maximum_bytes) noexcept {
    if (name.empty()) return XmlError::invalid_name;
    if (name.size() > maximum_bytes) return XmlError::name_too_long;
    std::size_t offset = 0;
    bool first = true;
    while (offset < name.size()) {
        const XmlDecodedCharacter decoded = decode_xml_utf8(name, offset);
        if (decoded.length == 0U) return XmlError::invalid_utf8;
        const bool admitted = first ? is_xml_name_start(decoded.scalar)
                                    : is_xml_name_character(decoded.scalar);
        if (!admitted) return XmlError::invalid_name;
        first = false;
        offset += decoded.length;
    }
    return XmlError::none;
}

XmlError validate_xml_characters(std::string_view text) noexcept {
    std::size_t offset = 0;
    while (offset < text.size()) {
        const XmlDecodedCharacter decoded = decode_xml_utf8(text, offset);
        if (decoded.length == 0U) return XmlError::invalid_utf8;
        if (!is_xml_character(decoded.scalar)) return XmlError::invalid_character;
        offset += decoded.length;
    }
    return XmlError::none;
}

void append_xml_utf8(std::string& output, std::uint32_t scalar) {
    if (scalar < 0x80U) {
        output.push_back(static_cast<char>(scalar));
    } else if (scalar < 0x800U) {
        output.push_back(static_cast<char>(0xC0U | (scalar >> 6U)));
        output.push_back(static_cast<char>(0x80U | (scalar & 0x3FU)));
    } else if (scalar < 0x10000U) {
        output.push_back(static_cast<char>(0xE0U | (scalar >> 12U)));
        output.push_back(static_cast<char>(0x80U | ((scalar >> 6U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | (scalar & 0x3FU)));
    } else {
        output.push_back(static_cast<char>(0xF0U | (scalar >> 18U)));
        output.push_back(static_cast<char>(0x80U | ((scalar >> 12U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | ((scalar >> 6U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | (scalar & 0x3FU)));
    }
}

} // namespace gui_forms::detail
