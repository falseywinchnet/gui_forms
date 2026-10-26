#include "theme_source_text.hpp"

#include "gui_forms/detail/algorithm/binary_search.hpp"
#include "gui_forms/schema/types/schema_types.hpp"

#include <span>

namespace gui_forms::theme_language {

namespace {

// Whether `byte` continues a UTF-8 sequence (10xxxxxx) rather than starting
// one; a column counts the bytes that start one.
[[nodiscard]] bool continuation_byte(char byte) noexcept {
    const unsigned char value = static_cast<unsigned char>(byte);
    const bool continues = (value & 0xC0U) == 0x80U;
    return continues;
}

// How many scalar values start in text[first, end).
[[nodiscard]] std::uint32_t scalars_between(std::string_view text, std::size_t first, std::size_t end) noexcept {
    std::uint32_t scalars = 0U;
    for (std::size_t index = first; index < end; ++index) {
        if (!continuation_byte(text[index])) ++scalars;
    }
    return scalars;
}

} // namespace

void LineStartTable::reset(std::string_view text) {
    starts_.clear();
    starts_.push_back(ByteOffset{0U});
    const std::size_t size = text.size();
    std::size_t index = 0U;
    while (index < size) {
        const char byte = text[index];
        ++index;
        if (byte == '\r' && index < size && text[index] == '\n') ++index;
        if (byte == '\n' || byte == '\r') {
            const ByteOffset start{static_cast<std::uint32_t>(index)};
            starts_.push_back(start);
        }
    }
}

void LineStartTable::clear() noexcept { starts_.clear(); }

std::size_t LineStartTable::line_index(ByteOffset offset) const noexcept {
    if (starts_.empty()) return 0U;
    const std::span<const ByteOffset> starts(starts_);
    // The first start after `offset`; the line before it holds the offset.
    const std::size_t following = detail::upper_bound_index(starts, offset);
    const std::size_t index = following == 0U ? 0U : following - 1U;
    return index;
}

SourcePosition LineStartTable::position(std::string_view text, ByteOffset offset) const noexcept {
    SourcePosition result{};
    result.offset = offset;
    if (starts_.empty()) return result;
    const std::size_t index = line_index(offset);
    result.line = LineNumber{static_cast<std::uint32_t>(index + 1U)};
    const std::size_t first = starts_[index].value;
    const std::size_t end = offset.value < text.size() ? offset.value : text.size();
    result.column = ColumnNumber{scalars_between(text, first, end) + 1U};
    return result;
}

bool equals_ascii_folded(std::string_view text, std::string_view lower_name) noexcept {
    if (text.size() != lower_name.size()) return false;
    for (std::size_t index = 0U; index < text.size(); ++index) {
        const char character = text[index];
        const bool upper = character >= 'A' && character <= 'Z';
        const char lower = upper ? static_cast<char>(character - 'A' + 'a') : character;
        if (lower != lower_name[index]) return false;
    }
    return true;
}

SourcePositionCursor::SourcePositionCursor(const LineStartTable& lines, std::string_view text) noexcept
    : lines_(lines), text_(text) {}

SourcePosition SourcePositionCursor::locate(ByteOffset offset) noexcept {
    const std::size_t index = lines_.line_index(offset);
    const bool continues = started_ && index == previous_line_ && !(offset < previous_.offset);
    SourcePosition result{};
    if (!continues) {
        result = lines_.position(text_, offset);
    } else {
        const std::size_t first = previous_.offset.value < text_.size() ? previous_.offset.value : text_.size();
        const std::size_t end = offset.value < text_.size() ? offset.value : text_.size();
        const std::uint32_t added = scalars_between(text_, first, end);
        result.offset = offset;
        result.line = previous_.line;
        result.column = ColumnNumber{previous_.column.value + added};
    }
    started_ = true;
    previous_line_ = index;
    previous_ = result;
    return result;
}

std::string registered_name_from_kebab(std::string_view written) {
    std::string registered(written);
    for (char& character : registered) {
        if (character == '_') return {};
        if (character == '-') character = '_';
    }
    if (!is_registered_name(registered)) return {};
    return registered;
}

} // namespace gui_forms::theme_language
