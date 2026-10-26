#include "gui_forms/components/file_dialog_types/file_dialog_types.hpp"

#include "file_dialog_support.hpp"
#include "gui_forms/host/services/host_services.hpp"
#include "gui_forms/text.hpp"

#include <filesystem>
#include <utility>

namespace gui_forms {
namespace {

bool is_pattern_space(char character) noexcept {
    const bool result = character == ' ' || character == '\t';
    return result;
}

std::string_view trimmed(std::string_view text) noexcept {
    std::size_t first = 0;
    while (first < text.size() && is_pattern_space(text[first])) ++first;
    std::size_t last = text.size();
    while (last > first && is_pattern_space(text[last - 1U])) --last;
    const std::string_view result = text.substr(first, last - first);
    return result;
}

// One pattern as a host extension: `*` for any file, else the text after
// `*.`. Writes `extension` only when the pattern is supported.
FileDialogFilterError pattern_extension(std::string_view pattern, std::string& extension) {
    if (pattern.empty()) return FileDialogFilterError::empty_pattern;
    if (pattern == "*" || pattern == "*.*") {
        extension = "*";
        return FileDialogFilterError::none;
    }
    if (pattern.size() < 3U || pattern.substr(0U, 2U) != "*.") return FileDialogFilterError::unsupported_pattern;
    const std::string_view rest = pattern.substr(2U);
    if (!detail::file_dialog_extension_valid(rest)) return FileDialogFilterError::unsupported_pattern;
    extension.assign(rest);
    return FileDialogFilterError::none;
}

// The `;`-separated patterns of one filter, appended to `extensions` in
// order. On an error `extensions` may hold the patterns before it.
FileDialogFilterError append_extensions(std::string_view patterns, std::vector<std::string>& extensions) {
    std::size_t begin = 0;
    while (true) {
        const std::size_t separator = patterns.find(';', begin);
        const std::size_t end = separator == std::string_view::npos ? patterns.size() : separator;
        const std::string_view pattern = trimmed(patterns.substr(begin, end - begin));
        if (extensions.size() == HostServices::maximum_dialog_extensions) return FileDialogFilterError::too_many;
        std::string extension;
        const FileDialogFilterError error = pattern_extension(pattern, extension);
        if (error != FileDialogFilterError::none) return error;
        extensions.push_back(std::move(extension));
        if (separator == std::string_view::npos) return FileDialogFilterError::none;
        begin = separator + 1U;
    }
}

} // namespace

bool file_dialog_text_valid(std::string_view text) {
    if (text.size() > HostServices::maximum_dialog_text_bytes) return false;
    if (text.find('\0') != std::string_view::npos) return false;
    const bool result = validate_utf8(text).valid();
    return result;
}

FileDialogFilterParse parse_file_dialog_filter(std::string_view filter) {
    FileDialogFilterParse result;
    if (filter.empty()) return result;
    if (!file_dialog_text_valid(filter)) {
        result.error = FileDialogFilterError::invalid_text;
        return result;
    }
    std::vector<HostFileDialogFilter> parsed;
    std::size_t begin = 0;
    while (true) {
        const std::size_t label_end = filter.find('|', begin);
        if (label_end == std::string_view::npos) {
            result.error = FileDialogFilterError::unpaired_label;
            return result;
        }
        const std::size_t next = filter.find('|', label_end + 1U);
        const std::size_t patterns_end = next == std::string_view::npos ? filter.size() : next;
        if (parsed.size() == HostServices::maximum_dialog_filters) {
            result.error = FileDialogFilterError::too_many;
            return result;
        }
        HostFileDialogFilter item;
        item.label.assign(filter.substr(begin, label_end - begin));
        const std::string_view patterns = filter.substr(label_end + 1U, patterns_end - label_end - 1U);
        const FileDialogFilterError error = append_extensions(patterns, item.extensions);
        if (error != FileDialogFilterError::none) {
            result.error = error;
            return result;
        }
        parsed.push_back(std::move(item));
        if (next == std::string_view::npos) break;
        begin = next + 1U;
    }
    result.filters = std::move(parsed);
    return result;
}

std::string file_dialog_name_part(std::string_view path) {
    // std::filesystem::path reads UTF-8 through std::u8string; each code
    // unit is copied as its unsigned byte value.
    std::u8string encoded;
    encoded.reserve(path.size());
    for (const char unit : path) {
        const unsigned char byte = static_cast<unsigned char>(unit);
        encoded.push_back(static_cast<char8_t>(byte));
    }
    const std::filesystem::path parsed(encoded);
    const std::u8string name = parsed.filename().u8string();
    std::string result;
    result.reserve(name.size());
    for (const char8_t unit : name) {
        const unsigned char byte = static_cast<unsigned char>(unit);
        result.push_back(static_cast<char>(byte));
    }
    return result;
}

} // namespace gui_forms
