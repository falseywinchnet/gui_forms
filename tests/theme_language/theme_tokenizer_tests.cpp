// The theme language's tokenizer, source positions and diagnostics (stage
// T3, phase P1). Gate: a fixture per token kind; every lexical defect with
// its line and column; tokens covering the input exactly.

#include "../../src/core/theme_language/diagnostics/theme_diagnostics.hpp"
#include "../../src/core/theme_language/source_text/theme_source_text.hpp"
#include "../../src/core/theme_language/tokenizer/theme_tokenizer.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {

using gui_forms::theme_language::ByteOffset;
using gui_forms::theme_language::ColumnNumber;
using gui_forms::theme_language::DiagnosticCode;
using gui_forms::theme_language::LineNumber;
using gui_forms::theme_language::LineStartTable;
using gui_forms::theme_language::SourcePosition;
using gui_forms::theme_language::SourcePositionCursor;
using gui_forms::theme_language::ThemeDiagnostic;
using gui_forms::theme_language::ThemeDiagnosticList;
using gui_forms::theme_language::ThemeNumber;
using gui_forms::theme_language::ThemeSource;
using gui_forms::theme_language::ThemeTokenBuffer;
using gui_forms::theme_language::Token;
using gui_forms::theme_language::TokenizeOutcome;
using gui_forms::theme_language::TokenKind;
using gui_forms::theme_language::diagnostic_code_label;
using gui_forms::theme_language::diagnostic_code_name;
using gui_forms::theme_language::maximum_diagnostic_records;
using gui_forms::theme_language::maximum_sheet_bytes;
using gui_forms::theme_language::maximum_string_bytes;
using gui_forms::theme_language::theme_diagnostic_log_line;
using gui_forms::theme_language::token_flag_hash_identifier;
using gui_forms::theme_language::token_kind_name;
using gui_forms::theme_language::tokenize_theme_sheet;

int failures = 0;

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

// The caller-owned storage one tokenization fills.
struct Workspace final {
    ThemeTokenBuffer buffer{};
    LineStartTable lines{};
    ThemeDiagnosticList diagnostics{};
};

TokenizeOutcome tokenize(Workspace& workspace, std::string_view text) {
    workspace.diagnostics.clear();
    ThemeSource source{};
    source.file = "test.theme.css";
    source.text = text;
    const TokenizeOutcome outcome =
        tokenize_theme_sheet(source, workspace.buffer, workspace.lines, workspace.diagnostics);
    return outcome;
}

// Whether the tokens cover `text` in order, with no gap and no overlap.
bool covers(const ThemeTokenBuffer& buffer, std::string_view text) {
    std::size_t next = 0U;
    for (const Token& token : buffer.tokens) {
        if (token.offset.value != next || token.length.value == 0U) return false;
        next += token.length.value;
    }
    return next == text.size();
}

struct ExpectedToken final {
    TokenKind kind{};
    std::string_view text{};
};

void require_tokens(std::string_view text, std::span<const ExpectedToken> expected, std::string_view what) {
    Workspace workspace{};
    const TokenizeOutcome outcome = tokenize(workspace, text);
    require(outcome == TokenizeOutcome::tokens_cover_sheet, std::string(what) + ": outcome");
    require(covers(workspace.buffer, text), std::string(what) + ": coverage");
    require(workspace.diagnostics.records().empty(), std::string(what) + ": no diagnostics");
    const std::vector<Token>& tokens = workspace.buffer.tokens;
    require(tokens.size() == expected.size(), std::string(what) + ": token count");
    const std::size_t count = tokens.size() < expected.size() ? tokens.size() : expected.size();
    for (std::size_t index = 0U; index < count; ++index) {
        const Token& token = tokens[index];
        const std::string_view written = text.substr(token.offset.value, token.length.value);
        const bool same = token.kind == expected[index].kind && written == expected[index].text;
        if (!same) {
            std::cerr << "  token " << index << ": " << token_kind_name(token.kind) << " '" << written
                      << "', expected " << token_kind_name(expected[index].kind) << " '" << expected[index].text
                      << "'\n";
        }
        require(same, std::string(what) + ": token");
    }
}

// A fixture per token kind.
void test_token_kinds() {
    const std::array<ExpectedToken, 1> bom{{{TokenKind::byte_order_mark, "\xEF\xBB\xBF"}}};
    require_tokens("\xEF\xBB\xBF", bom, "byte order mark");
    const std::array<ExpectedToken, 1> space{{{TokenKind::whitespace, " \t\r\n\f "}}};
    require_tokens(" \t\r\n\f ", space, "whitespace");
    const std::array<ExpectedToken, 1> comment{{{TokenKind::comment, "/* a * / b **/"}}};
    require_tokens("/* a * / b **/", comment, "comment");
    const std::array<ExpectedToken, 5> idents{{
        {TokenKind::ident, "button"},
        {TokenKind::whitespace, " "},
        {TokenKind::ident, "--glow"},
        {TokenKind::whitespace, " "},
        {TokenKind::ident, "-gf-x_1"},
    }};
    require_tokens("button --glow -gf-x_1", idents, "idents");
    const std::array<ExpectedToken, 1> non_ascii{{{TokenKind::ident, "caf\xC3\xA9"}}};
    require_tokens("caf\xC3\xA9", non_ascii, "non-ASCII ident");
    // A function token holds its name and its parenthesis.
    const std::array<ExpectedToken, 3> functions{{
        {TokenKind::function, "calc("},
        {TokenKind::number, "1"},
        {TokenKind::right_paren, ")"},
    }};
    require_tokens("calc(1)", functions, "a function");
    // url( is a function token only when a quoted string follows.
    const std::array<ExpectedToken, 6> url{{
        {TokenKind::function, "url("},
        {TokenKind::whitespace, " "},
        {TokenKind::string, "\"resource:icon.check\""},
        {TokenKind::right_paren, ")"},
        {TokenKind::function, "role("},
        {TokenKind::right_paren, ")"},
    }};
    require_tokens("url( \"resource:icon.check\")role()", url, "url with a string");
    const std::array<ExpectedToken, 3> at{{
        {TokenKind::at_keyword, "@media"},
        {TokenKind::whitespace, " "},
        {TokenKind::at_keyword, "@-x"},
    }};
    require_tokens("@media @-x", at, "at-keywords");
    const std::array<ExpectedToken, 2> lone_at{{{TokenKind::delim, "@"}, {TokenKind::number, "1"}}};
    require_tokens("@1", lone_at, "a lone @ is a delim");
    const std::array<ExpectedToken, 3> strings{{
        {TokenKind::string, "\"a \\\" b\""},
        {TokenKind::string, "'c\\\nd'"},
        {TokenKind::string, "\"\""},
    }};
    require_tokens("\"a \\\" b\"'c\\\nd'\"\"", strings, "strings with escapes and a continuation");
    const std::array<ExpectedToken, 3> numbers{{
        {TokenKind::number, "-1.5e3"},
        {TokenKind::percentage, "+50%"},
        {TokenKind::dimension, ".5em"},
    }};
    require_tokens("-1.5e3+50%.5em", numbers, "numbers, percentages and dimensions");
    const std::array<ExpectedToken, 3> exponent_unit{{
        {TokenKind::dimension, "1e3px"},
        {TokenKind::whitespace, " "},
        {TokenKind::dimension, "1e"},
    }};
    require_tokens("1e3px 1e", exponent_unit, "exponent before a unit");
    const std::array<ExpectedToken, 8> delims{{
        {TokenKind::delim, "."},
        {TokenKind::delim, "*"},
        {TokenKind::delim, "!"},
        {TokenKind::delim, ">"},
        {TokenKind::delim, "+"},
        {TokenKind::delim, "~"},
        {TokenKind::delim, "-"},
        {TokenKind::delim, "#"},
    }};
    require_tokens(".*!>+~-#", delims, "delims");
    const std::array<ExpectedToken, 10> punctuation{{
        {TokenKind::colon, ":"},
        {TokenKind::semicolon, ";"},
        {TokenKind::comma, ","},
        {TokenKind::left_square, "["},
        {TokenKind::right_square, "]"},
        {TokenKind::left_paren, "("},
        {TokenKind::right_paren, ")"},
        {TokenKind::left_brace, "{"},
        {TokenKind::right_brace, "}"},
        {TokenKind::colon, ":"},
    }};
    require_tokens(":;,[](){}:", punctuation, "punctuation");
    const std::array<ExpectedToken, 3> hashes{{
        {TokenKind::hash, "#player-play"},
        {TokenKind::whitespace, " "},
        {TokenKind::hash, "#fff"},
    }};
    require_tokens("#player-play #fff", hashes, "hashes");
}

void test_token_details() {
    Workspace workspace{};
    const std::string_view text = "#player-play #1a -12.5px 50% 3";
    static_cast<void>(tokenize(workspace, text));
    const std::vector<Token>& tokens = workspace.buffer.tokens;
    require(tokens.size() == 9U, "detail token count");
    if (tokens.size() != 9U) return;
    require(tokens[0].flags == token_flag_hash_identifier, "a stable ID hash has the identifier flag");
    require(tokens[2].flags == 0U, "#1a has no identifier flag");
    const std::vector<ThemeNumber>& numbers = workspace.buffer.numbers;
    require(numbers.size() == 3U, "three numbers");
    if (numbers.size() != 3U) return;
    require(tokens[4].number == 0U && numbers[0].value == -12.5 && numbers[0].numeric_length.value == 5U &&
                !numbers[0].integer,
            "dimension value and unit split");
    require(tokens[6].number == 1U && numbers[1].value == 50.0 && numbers[1].integer, "percentage value");
    require(tokens[8].number == 2U && numbers[2].value == 3.0, "number value");
}

struct DefectCase final {
    std::string_view text{};
    DiagnosticCode code{};
    std::uint32_t line{};
    std::uint32_t column{};
};

// Every lexical defect, each with its line and column.
void test_defects() {
    const std::array<DefectCase, 14> cases{{
        {"a {\n  b: \xFF\xFE;\n}", DiagnosticCode::invalid_utf8, 2U, 6U},
        {"a\xC3\xA9 \xC0\xAF", DiagnosticCode::invalid_utf8, 1U, 4U},
        {std::string_view("x\r\ny\0z", 6U), DiagnosticCode::nul_byte, 2U, 2U},
        {"a\n  \"open\nb", DiagnosticCode::unterminated_string, 2U, 3U},
        {"'never closed", DiagnosticCode::unterminated_string, 1U, 1U},
        {"a\r\r/* never", DiagnosticCode::unterminated_comment, 3U, 1U},
        {"\xC3\xA9\xC3\xA9\\66", DiagnosticCode::escape_not_admitted, 1U, 3U},
        {"b: url(icon.png);", DiagnosticCode::bad_url, 1U, 4U},
        {"b: URL( x )", DiagnosticCode::bad_url, 1U, 4U},
        {"\n<!-- x", DiagnosticCode::token_not_admitted, 2U, 1U},
        {"a -->", DiagnosticCode::token_not_admitted, 1U, 3U},
        {"w: 12345678901234567891px", DiagnosticCode::number_too_precise, 1U, 4U},
        {"w: 1e40", DiagnosticCode::number_out_of_range, 1U, 4U},
        {"\t\tw: 9e-41%", DiagnosticCode::number_out_of_range, 1U, 6U},
    }};
    for (const DefectCase& entry : cases) {
        Workspace workspace{};
        const TokenizeOutcome outcome = tokenize(workspace, entry.text);
        const std::string what = std::string(diagnostic_code_name(entry.code));
        require(outcome == TokenizeOutcome::tokens_cover_sheet, what + ": outcome");
        require(covers(workspace.buffer, entry.text), what + ": coverage despite the defect");
        const std::span<const ThemeDiagnostic> records = workspace.diagnostics.records();
        require(records.size() == 1U, what + ": one diagnostic");
        if (records.size() != 1U) continue;
        const ThemeDiagnostic& diagnostic = records[0];
        require(diagnostic.code == entry.code, what + ": code");
        require(diagnostic.file == "test.theme.css", what + ": file");
        require(diagnostic.position.line == LineNumber{entry.line}, what + ": line");
        require(diagnostic.position.column == ColumnNumber{entry.column}, what + ": column");
        require(workspace.diagnostics.error_count() == 1U, what + ": an error");
        bool has_invalid = false;
        for (const Token& token : workspace.buffer.tokens) {
            if (token.kind == TokenKind::invalid) has_invalid = true;
        }
        require(has_invalid, what + ": an invalid token");
    }
}

void test_limits() {
    // A string of exactly the limit passes; one byte more is refused.
    std::string at_limit = "\"";
    at_limit.append(maximum_string_bytes, 'a');
    at_limit += '"';
    Workspace workspace{};
    static_cast<void>(tokenize(workspace, at_limit));
    require(workspace.diagnostics.records().empty(), "a string at the limit passes");
    std::string over_limit = "\"";
    over_limit.append(maximum_string_bytes + 1U, 'a');
    over_limit += '"';
    static_cast<void>(tokenize(workspace, over_limit));
    const std::span<const ThemeDiagnostic> string_records = workspace.diagnostics.records();
    require(string_records.size() == 1U && string_records[0].code == DiagnosticCode::string_too_long &&
                string_records[0].detail == "4097",
            "a string over the limit is an error");
    require(covers(workspace.buffer, over_limit), "the long string is still covered");

    // A sheet at the size limit tokenizes; one byte more is refused before
    // anything is allocated.
    const std::string at_size(maximum_sheet_bytes, ' ');
    require(tokenize(workspace, at_size) == TokenizeOutcome::tokens_cover_sheet, "a sheet at the size limit");
    require(workspace.buffer.tokens.size() == 1U, "one whitespace token");
    const std::string over_size(maximum_sheet_bytes + 1U, ' ');
    require(tokenize(workspace, over_size) == TokenizeOutcome::refused_too_large, "a sheet over the size limit");
    const std::span<const ThemeDiagnostic> size_records = workspace.diagnostics.records();
    require(size_records.size() == 1U && size_records[0].code == DiagnosticCode::sheet_too_large,
            "sheet_too_large");
    require(workspace.buffer.tokens.empty() && workspace.lines.line_count() == 0U, "nothing kept");
}

void test_positions() {
    const std::string_view text = "a\r\nb\rc\nd\xC3\xA9" "e\n";
    LineStartTable lines{};
    lines.reset(text);
    require(lines.line_count() == 5U, "LF, CR LF and CR each end a line");
    const SourcePosition e = lines.position(text, ByteOffset{10U});
    require(e.line == LineNumber{4U} && e.column == ColumnNumber{3U}, "a column counts scalar values");
    const SourcePosition end = lines.position(text, ByteOffset{static_cast<std::uint32_t>(text.size())});
    require(end.line == LineNumber{5U} && end.column == ColumnNumber{1U}, "the end of the text");
    // The cursor gives the same positions, forwards and backwards.
    SourcePositionCursor cursor(lines, text);
    for (std::uint32_t offset = 0U; offset <= text.size(); ++offset) {
        const SourcePosition expected = lines.position(text, ByteOffset{offset});
        require(cursor.locate(ByteOffset{offset}) == expected, "cursor forwards");
    }
    require(cursor.locate(ByteOffset{1U}) == lines.position(text, ByteOffset{1U}), "cursor backwards");
    LineStartTable empty{};
    empty.reset("");
    require(empty.line_count() == 1U, "an empty text has one line");
}

void test_diagnostic_list_and_log() {
    ThemeDiagnosticList list{};
    for (std::size_t index = 0U; index < maximum_diagnostic_records + 5U; ++index) {
        ThemeDiagnostic diagnostic{};
        diagnostic.code = DiagnosticCode::nul_byte;
        list.add(diagnostic);
    }
    require(list.records().size() == maximum_diagnostic_records, "records are bounded");
    require(list.dropped_count() == 5U && list.error_count() == maximum_diagnostic_records + 5U,
            "the rest are counted");
    list.clear();
    require(list.records().empty() && list.error_count() == 0U && list.dropped_count() == 0U, "clear");

    require(diagnostic_code_label(DiagnosticCode::invalid_utf8) == "E101", "code label");
    require(diagnostic_code_label(DiagnosticCode::string_too_long) == "E505", "limit code label");
    ThemeDiagnostic diagnostic{};
    diagnostic.code = DiagnosticCode::bad_url;
    diagnostic.file = "aurora/main.theme.css";
    diagnostic.position.line = LineNumber{12U};
    diagnostic.position.column = ColumnNumber{9U};
    diagnostic.rule = "a|b";
    diagnostic.detail = "50%\n";
    const std::string line = theme_diagnostic_log_line(diagnostic);
    require(line == "gui-forms-theme=error|code:E106 bad_url|file:aurora/main.theme.css|line:12|column:9|"
                    "rule:a%7Cb|property:|detail:50%25%0A",
            "log line");
}

// Tokenizing many sheets reuses the caller's storage.
void test_reuse() {
    Workspace workspace{};
    static_cast<void>(tokenize(workspace, "a b c d e f g h"));
    const std::size_t capacity = workspace.buffer.tokens.capacity();
    static_cast<void>(tokenize(workspace, "x"));
    require(workspace.buffer.tokens.size() == 1U, "the buffer holds the latest sheet");
    require(workspace.buffer.tokens.capacity() == capacity, "the capacity is kept");
}

} // namespace

int main() {
    test_token_kinds();
    test_token_details();
    test_defects();
    test_limits();
    test_positions();
    test_diagnostic_list_and_log();
    test_reuse();
    if (failures != 0) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "theme tokenizer tests passed\n";
    return 0;
}
