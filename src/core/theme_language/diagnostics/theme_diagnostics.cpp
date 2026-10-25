#include "theme_diagnostics.hpp"

#include <utility>

namespace gui_forms::theme_language {

namespace {

// Appends `field` to `line`, writing '%', '|' and control bytes as '%XX'.
void append_log_field(std::string& line, std::string_view field) {
    static constexpr char hex_digits[] = "0123456789ABCDEF";
    for (const char byte : field) {
        const unsigned char value = static_cast<unsigned char>(byte);
        const bool escaped = value == '%' || value == '|' || value < 0x20U || value == 0x7FU;
        if (!escaped) {
            line.push_back(byte);
            continue;
        }
        line.push_back('%');
        line.push_back(hex_digits[value >> 4U]);
        line.push_back(hex_digits[value & 0x0FU]);
    }
}

} // namespace

DiagnosticSeverity diagnostic_severity(DiagnosticCode code) noexcept {
    switch (code) {
    case DiagnosticCode::invalid_utf8:
    case DiagnosticCode::nul_byte:
    case DiagnosticCode::unterminated_string:
    case DiagnosticCode::unterminated_comment:
    case DiagnosticCode::escape_not_admitted:
    case DiagnosticCode::bad_url:
    case DiagnosticCode::token_not_admitted:
    case DiagnosticCode::number_too_precise:
    case DiagnosticCode::number_out_of_range:
    case DiagnosticCode::unexpected_token:
    case DiagnosticCode::unbalanced_block:
    case DiagnosticCode::declaration_outside_rule:
    case DiagnosticCode::import_after_rules:
    case DiagnosticCode::important_in_keyframes:
    case DiagnosticCode::nesting_not_admitted:
    case DiagnosticCode::sheet_too_large:
    case DiagnosticCode::too_many_rules:
    case DiagnosticCode::too_many_declarations:
    case DiagnosticCode::nesting_too_deep:
    case DiagnosticCode::string_too_long:
    case DiagnosticCode::too_many_imports:
        return DiagnosticSeverity::error;
    case DiagnosticCode::unknown_at_rule:
        return DiagnosticSeverity::warning;
    }
    // A value outside the enumeration is treated as the stricter class.
    return DiagnosticSeverity::error;
}

std::string_view diagnostic_code_name(DiagnosticCode code) noexcept {
    switch (code) {
    case DiagnosticCode::invalid_utf8: return "invalid_utf8";
    case DiagnosticCode::nul_byte: return "nul_byte";
    case DiagnosticCode::unterminated_string: return "unterminated_string";
    case DiagnosticCode::unterminated_comment: return "unterminated_comment";
    case DiagnosticCode::escape_not_admitted: return "escape_not_admitted";
    case DiagnosticCode::bad_url: return "bad_url";
    case DiagnosticCode::token_not_admitted: return "token_not_admitted";
    case DiagnosticCode::number_too_precise: return "number_too_precise";
    case DiagnosticCode::number_out_of_range: return "number_out_of_range";
    case DiagnosticCode::unexpected_token: return "unexpected_token";
    case DiagnosticCode::unbalanced_block: return "unbalanced_block";
    case DiagnosticCode::declaration_outside_rule: return "declaration_outside_rule";
    case DiagnosticCode::import_after_rules: return "import_after_rules";
    case DiagnosticCode::important_in_keyframes: return "important_in_keyframes";
    case DiagnosticCode::nesting_not_admitted: return "nesting_not_admitted";
    case DiagnosticCode::sheet_too_large: return "sheet_too_large";
    case DiagnosticCode::too_many_rules: return "too_many_rules";
    case DiagnosticCode::too_many_declarations: return "too_many_declarations";
    case DiagnosticCode::nesting_too_deep: return "nesting_too_deep";
    case DiagnosticCode::string_too_long: return "string_too_long";
    case DiagnosticCode::too_many_imports: return "too_many_imports";
    case DiagnosticCode::unknown_at_rule: return "unknown_at_rule";
    }
    return "unknown";
}

std::string diagnostic_code_label(DiagnosticCode code) {
    const DiagnosticSeverity severity = diagnostic_severity(code);
    std::string label = severity == DiagnosticSeverity::error ? "E" : "W";
    // A warning's enumerator value carries 1000 above its number.
    const std::uint16_t value = static_cast<std::uint16_t>(code);
    const std::uint16_t number = static_cast<std::uint16_t>(value % 1000U);
    label += std::to_string(number);
    return label;
}

void ThemeDiagnosticList::add(ThemeDiagnostic diagnostic) {
    const DiagnosticSeverity severity = diagnostic_severity(diagnostic.code);
    if (severity == DiagnosticSeverity::error) {
        ++errors_;
    } else {
        ++warnings_;
    }
    if (records_.size() < maximum_diagnostic_records) {
        records_.push_back(std::move(diagnostic));
    } else {
        ++dropped_;
    }
}

void ThemeDiagnosticList::clear() noexcept {
    records_.clear();
    errors_ = 0U;
    warnings_ = 0U;
    dropped_ = 0U;
}

std::string theme_diagnostic_log_line(const ThemeDiagnostic& diagnostic) {
    const DiagnosticSeverity severity = diagnostic_severity(diagnostic.code);
    std::string line = "gui-forms-theme=";
    line += severity == DiagnosticSeverity::error ? "error" : "warning";
    line += "|code:";
    line += diagnostic_code_label(diagnostic.code);
    line += ' ';
    line += diagnostic_code_name(diagnostic.code);
    line += "|file:";
    append_log_field(line, diagnostic.file);
    line += "|line:";
    line += std::to_string(diagnostic.position.line.value);
    line += "|column:";
    line += std::to_string(diagnostic.position.column.value);
    line += "|rule:";
    append_log_field(line, diagnostic.rule);
    line += "|property:";
    append_log_field(line, diagnostic.property);
    line += "|detail:";
    append_log_field(line, diagnostic.detail);
    return line;
}

void write_theme_diagnostics(const ThemeDiagnosticList& diagnostics, std::FILE* stream) {
    const std::span<const ThemeDiagnostic> records = diagnostics.records();
    for (const ThemeDiagnostic& diagnostic : records) {
        const std::string line = theme_diagnostic_log_line(diagnostic);
        std::fputs(line.c_str(), stream);
        std::fputc('\n', stream);
    }
    if (diagnostics.dropped_count() != 0U) {
        const std::string count = std::to_string(diagnostics.dropped_count());
        std::fputs("gui-forms-theme=dropped|count:", stream);
        std::fputs(count.c_str(), stream);
        std::fputc('\n', stream);
    }
}

} // namespace gui_forms::theme_language
