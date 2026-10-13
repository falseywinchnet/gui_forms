#include "gui_forms/schema/types/schema_types.hpp"

#include <array>
#include <stdexcept>

namespace gui_forms {
namespace {

constexpr std::array<std::string_view, 4> interaction_defect_names{
    "none",
    "too_many_interactions",
    "name_not_snake_case",
    "duplicate_name",
};
static_assert(interaction_defect_names.size() ==
              static_cast<std::size_t>(InteractionDefect::duplicate_name) + 1U);

[[nodiscard]] InteractionValidation interaction_defect_at(InteractionDefect defect,
                                                          std::size_t index) noexcept {
    InteractionValidation result;
    result.defect = defect;
    result.index = index;
    return result;
}

[[nodiscard]] bool is_lowercase_letter(char character) noexcept {
    return character >= 'a' && character <= 'z';
}

[[nodiscard]] bool is_uppercase_letter(char character) noexcept {
    return character >= 'A' && character <= 'Z';
}

[[nodiscard]] bool is_digit(char character) noexcept {
    return character >= '0' && character <= '9';
}

[[nodiscard]] char lowercase(char character) noexcept {
    if (!is_uppercase_letter(character)) {
        return character;
    }
    return static_cast<char>(character + ('a' - 'A'));
}

[[nodiscard]] char uppercase(char character) noexcept {
    if (!is_lowercase_letter(character)) {
        return character;
    }
    return static_cast<char>(character - ('a' - 'A'));
}

// True when an uppercase letter at `index` starts a new word: after a
// lowercase letter or digit ("sizeMode"), or as the last capital of an
// acronym followed by a lowercase letter ("HScroll").
[[nodiscard]] bool starts_word(std::string_view text, std::size_t index) noexcept {
    if (index == 0U || !is_uppercase_letter(text[index])) {
        return false;
    }
    const char previous = text[index - 1U];
    if (is_lowercase_letter(previous) || is_digit(previous)) {
        return true;
    }
    const bool next_is_lowercase =
        index + 1U < text.size() && is_lowercase_letter(text[index + 1U]);
    return is_uppercase_letter(previous) && next_is_lowercase;
}

} // namespace

bool is_registered_name(std::string_view name) noexcept {
    if (name.empty() || !is_lowercase_letter(name.front()) || name.back() == '_') {
        return false;
    }
    char previous = '\0';
    for (std::size_t index = 0; index < name.size(); ++index) {
        const char character = name[index];
        if (character == '_') {
            if (previous == '_') {
                return false;
            }
        } else if (!is_lowercase_letter(character) && !is_digit(character)) {
            return false;
        }
        previous = character;
    }
    return true;
}

std::string registered_name(std::string_view spelling) {
    const std::string_view::size_type first = spelling.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) {
        throw std::invalid_argument("GUI.Forms name may not be empty");
    }
    const std::string_view::size_type last = spelling.find_last_not_of(" \t\r\n");
    const std::string_view trimmed = spelling.substr(first, last - first + 1U);
    std::string registered;
    registered.reserve(trimmed.size() + 8U);
    for (std::size_t index = 0; index < trimmed.size(); ++index) {
        const char character = trimmed[index];
        if (character == '-') {
            registered += '_';
            continue;
        }
        if (starts_word(trimmed, index) && registered.back() != '_') {
            registered += '_';
        }
        registered += lowercase(character);
    }
    return registered;
}

std::string css_spelling(std::string_view registered) {
    std::string css(registered);
    for (std::size_t index = 0; index < css.size(); ++index) {
        if (css[index] == '_') {
            css[index] = '-';
        }
    }
    return css;
}

std::string display_spelling(std::string_view registered) {
    std::string display;
    display.reserve(registered.size());
    bool word_start = true;
    for (std::size_t index = 0; index < registered.size(); ++index) {
        const char character = registered[index];
        if (character == '_') {
            word_start = true;
            continue;
        }
        display += word_start ? uppercase(character) : character;
        // A path segment or index starts a new word.
        word_start = character == '.' || character == '[';
    }
    return display;
}

std::string display_label(std::string_view text) {
    constexpr std::string_view separator = ", ";
    std::string label;
    label.reserve(text.size());
    std::size_t start = 0;
    while (start <= text.size()) {
        const std::size_t found = text.find(separator, start);
        const std::size_t end = found == std::string_view::npos ? text.size() : found;
        const std::string_view token = text.substr(start, end - start);
        if (is_registered_name(token)) {
            label += display_spelling(token);
        } else {
            label += token;
        }
        if (found == std::string_view::npos) {
            break;
        }
        label += separator;
        start = found + separator.size();
    }
    return label;
}

std::string_view interaction_defect_name(InteractionDefect defect) noexcept {
    const std::size_t index = static_cast<std::size_t>(defect);
    if (index >= interaction_defect_names.size()) {
        return {};
    }
    return interaction_defect_names[index];
}

InteractionValidation validate_interactions(
    std::span<const InteractionDeclaration> interactions) noexcept {
    if (interactions.size() > maximum_interactions) {
        return interaction_defect_at(InteractionDefect::too_many_interactions, 0U);
    }
    for (std::size_t index = 0; index < interactions.size(); ++index) {
        const std::string_view name = interactions[index].name;
        if (!is_registered_name(name)) {
            return interaction_defect_at(InteractionDefect::name_not_snake_case, index);
        }
        for (std::size_t earlier = 0; earlier < index; ++earlier) {
            if (interactions[earlier].name == name) {
                return interaction_defect_at(InteractionDefect::duplicate_name, index);
            }
        }
    }
    InteractionValidation valid;
    return valid;
}

std::size_t find_interaction(std::span<const InteractionDeclaration> interactions,
                             std::string_view name) noexcept {
    for (std::size_t index = 0; index < interactions.size(); ++index) {
        if (interactions[index].name == name) {
            return index;
        }
    }
    return interactions.size();
}

} // namespace gui_forms
