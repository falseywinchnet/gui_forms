#pragma once

#include "gui_forms/xml/types/xml_types.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace gui_forms::detail {

// One decoded UTF-8 sequence. length is the sequence's byte count, or zero
// when the bytes at the offset are not a well-formed UTF-8 sequence
// (overlong forms, surrogates, values above U+10FFFF and truncated
// sequences are all ill-formed).
struct XmlDecodedCharacter final {
    std::uint32_t scalar{};
    std::uint32_t length{};
};

// Decodes the sequence starting at offset; offset must be below text.size().
[[nodiscard]] XmlDecodedCharacter decode_xml_utf8(std::string_view text,
                                                  std::size_t offset) noexcept;

// XML 1.0 production [2] Char.
[[nodiscard]] bool is_xml_character(std::uint32_t scalar) noexcept;
// XML 1.0 productions [4] NameStartChar and [4a] NameChar.
[[nodiscard]] bool is_xml_name_start(std::uint32_t scalar) noexcept;
[[nodiscard]] bool is_xml_name_character(std::uint32_t scalar) noexcept;
// XML 1.0 production [3] S: space, tab, CR and LF.
[[nodiscard]] bool is_xml_space(std::uint32_t scalar) noexcept;

// Checks a whole name: nonempty, at most maximum_bytes, well-formed UTF-8,
// a name start character followed by name characters.
[[nodiscard]] XmlError validate_xml_name(std::string_view name,
                                         std::size_t maximum_bytes) noexcept;

// Checks that text is well-formed UTF-8 made of XML characters.
[[nodiscard]] XmlError validate_xml_characters(std::string_view text) noexcept;

// Appends the UTF-8 form of a scalar value; scalar must be a Unicode scalar
// value (not a surrogate, at most U+10FFFF).
void append_xml_utf8(std::string& output, std::uint32_t scalar);

} // namespace gui_forms::detail
