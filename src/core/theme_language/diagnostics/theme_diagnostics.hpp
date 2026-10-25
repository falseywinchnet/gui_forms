#pragma once

#include "../source_text/theme_source_text.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms::theme_language {

// What the theme compiler reports (PLAN.md section 2). An error refuses the
// whole theme, or the whole inline style, and the previously active one
// stays; a warning drops one declaration or rule and the rest applies (T3
// decisions B2 and B3).
enum class DiagnosticSeverity : std::uint8_t { error, warning };

// Each code's number is its stable record; its class is its hundreds digit
// (PLAN.md section 2): E1xx lexical, E2xx structure, E5xx limits, W1xx
// unknown names. Later phases add the other classes. An error's enumerator
// value is its number; a warning's is 1000 plus its number, so W104 and
// E104 stay distinct values.
enum class DiagnosticCode : std::uint16_t {
    // E1xx lexical.
    invalid_utf8 = 101,
    nul_byte = 102,
    unterminated_string = 103,
    unterminated_comment = 104,
    // A backslash outside a string (round 002, item 16).
    escape_not_admitted = 105,
    // `url(` not followed by a quoted string.
    bad_url = 106,
    // `<!--` or `-->`.
    token_not_admitted = 107,
    // A number with more than maximum_significant_digits significant digits.
    number_too_precise = 108,
    // A number that is not zero and lies outside the admitted magnitudes.
    number_out_of_range = 109,
    // E2xx structure: the first one found stops the parse (B2).
    // A token where the sheet's structure admits none of its kind.
    unexpected_token = 201,
    // A `{`, `(`, `[` or function never closed, or a closer with no opener
    // or the wrong one.
    unbalanced_block = 202,
    // A declaration's `;` outside every rule's block.
    declaration_outside_rule = 203,
    // An `@import` after another item.
    import_after_rules = 204,
    important_in_keyframes = 205,
    // A rule or at-rule inside a rule's block, or an at-rule inside an
    // at-rule's block (B1).
    nesting_not_admitted = 206,
    // E5xx limits (T3 decision E2).
    sheet_too_large = 501,
    too_many_rules = 502,
    too_many_declarations = 503,
    nesting_too_deep = 504,
    string_too_long = 505,
    too_many_imports = 506,
    // W1xx unknown names (B3): the item is dropped and the rest applies.
    // An at-rule other than @import, @media and @keyframes; its block is
    // skipped.
    unknown_at_rule = 1104,
};

[[nodiscard]] DiagnosticSeverity diagnostic_severity(DiagnosticCode code) noexcept;
// The code's snake_case name, such as `invalid_utf8`.
[[nodiscard]] std::string_view diagnostic_code_name(DiagnosticCode code) noexcept;
// The code as diagnostics print it: its severity letter and number, such as
// `E101`.
[[nodiscard]] std::string diagnostic_code_label(DiagnosticCode code);

// One diagnostic. `file` is the sheet's name (ThemeSource::file); `rule` is
// the rule's label and `property` the declaration's property, each empty
// where there is none (always in lexical diagnostics); `detail` is one
// argument, such as the limit exceeded.
struct ThemeDiagnostic final {
    DiagnosticCode code{DiagnosticCode::invalid_utf8};
    std::string file{};
    SourcePosition position{};
    std::string rule{};
    std::string property{};
    std::string detail{};

    friend bool operator==(const ThemeDiagnostic& left, const ThemeDiagnostic& right) noexcept {
        return left.code == right.code && left.file == right.file && left.position == right.position &&
               left.rule == right.rule && left.property == right.property && left.detail == right.detail;
    }
};

// The most diagnostic records one compilation keeps (a derived limit, T3
// round 002 item 14); the rest are counted.
inline constexpr std::size_t maximum_diagnostic_records = 1000U;

// A compilation's diagnostics, in the order found. Caller-owned and reused:
// clear() ends the records' lifetimes and keeps the capacity.
class ThemeDiagnosticList final {
public:
    // Keeps the record if fewer than maximum_diagnostic_records are kept,
    // and counts it either way.
    void add(ThemeDiagnostic diagnostic);
    void clear() noexcept;

    [[nodiscard]] std::span<const ThemeDiagnostic> records() const& noexcept { return records_; }
    std::span<const ThemeDiagnostic> records() const&& = delete;
    [[nodiscard]] std::size_t error_count() const noexcept { return errors_; }
    [[nodiscard]] std::size_t warning_count() const noexcept { return warnings_; }
    // Records counted but not kept.
    [[nodiscard]] std::size_t dropped_count() const noexcept { return dropped_; }

private:
    std::vector<ThemeDiagnostic> records_{};
    std::size_t errors_{};
    std::size_t warnings_{};
    std::size_t dropped_{};
};

// The diagnostic's log line, without a line break:
//   gui-forms-theme=error|code:E101 invalid_utf8|file:a.theme.css|line:3|column:7|rule:|property:|detail:
// In the file, rule, property and detail fields, '%', '|' and the bytes
// below 0x20 and 0x7F are written as '%' and two uppercase hex digits, so a
// line holds exactly the fields it names.
[[nodiscard]] std::string theme_diagnostic_log_line(const ThemeDiagnostic& diagnostic);

// Writes each kept record's log line to `stream` (the library passes
// stderr), then, when records were dropped, one line counting them:
//   gui-forms-theme=dropped|count:<n>
void write_theme_diagnostics(const ThemeDiagnosticList& diagnostics, std::FILE* stream);

} // namespace gui_forms::theme_language
