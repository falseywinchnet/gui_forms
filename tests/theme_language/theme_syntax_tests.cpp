// The theme language's syntax tree and its byte-preserving round trip
// (stage T3, phase P2). Gate: the round-trip corpus returns identical
// bytes; every structural defect is located.

#include "../../src/core/theme_language/diagnostics/theme_diagnostics.hpp"
#include "../../src/core/theme_language/source_text/theme_source_text.hpp"
#include "../../src/core/theme_language/syntax_tree/theme_syntax_tree.hpp"
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

using gui_forms::theme_language::ColumnNumber;
using gui_forms::theme_language::DiagnosticCode;
using gui_forms::theme_language::LineNumber;
using gui_forms::theme_language::LineStartTable;
using gui_forms::theme_language::ParseOutcome;
using gui_forms::theme_language::SyntaxNode;
using gui_forms::theme_language::SyntaxNodeKind;
using gui_forms::theme_language::ThemeDiagnostic;
using gui_forms::theme_language::ThemeDiagnosticList;
using gui_forms::theme_language::ThemeSource;
using gui_forms::theme_language::ThemeSyntaxTree;
using gui_forms::theme_language::ThemeTokenBuffer;
using gui_forms::theme_language::TokenRange;
using gui_forms::theme_language::TokenizeOutcome;
using gui_forms::theme_language::WriteOutcome;
using gui_forms::theme_language::diagnostic_code_name;
using gui_forms::theme_language::maximum_declarations_per_rule;
using gui_forms::theme_language::maximum_imports;
using gui_forms::theme_language::maximum_nesting_depth;
using gui_forms::theme_language::maximum_rules;
using gui_forms::theme_language::parse_theme_sheet;
using gui_forms::theme_language::syntax_flag_important;
using gui_forms::theme_language::syntax_node_kind_name;
using gui_forms::theme_language::theme_diagnostic_log_line;
using gui_forms::theme_language::tokenize_theme_sheet;
using gui_forms::theme_language::write_theme_sheet;

int failures = 0;

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

// The caller-owned storage one compilation of a sheet fills.
struct Workspace final {
    std::string_view text{};
    ThemeTokenBuffer tokens{};
    LineStartTable lines{};
    ThemeSyntaxTree tree{};
    ThemeDiagnosticList diagnostics{};
};

ParseOutcome parse(Workspace& workspace, std::string_view text) {
    workspace.text = text;
    workspace.diagnostics.clear();
    ThemeSource source{};
    source.file = "test.theme.css";
    source.text = text;
    const TokenizeOutcome tokenized = tokenize_theme_sheet(source, workspace.tokens, workspace.lines,
                                                           workspace.diagnostics);
    require(tokenized == TokenizeOutcome::tokens_cover_sheet, "the sheet tokenizes");
    const ParseOutcome outcome =
        parse_theme_sheet(source, workspace.tokens, workspace.lines, workspace.tree, workspace.diagnostics);
    return outcome;
}

// The bytes of a token range.
std::string_view range_text(const Workspace& workspace, TokenRange range) {
    if (range.first.value >= range.end.value) return {};
    const gui_forms::theme_language::Token& first = workspace.tokens.tokens[range.first.value];
    const gui_forms::theme_language::Token& last = workspace.tokens.tokens[range.end.value - 1U];
    const std::size_t begin = first.offset.value;
    const std::size_t stop = static_cast<std::size_t>(last.offset.value) + last.length.value;
    return workspace.text.substr(begin, stop - begin);
}

bool writes_back(const Workspace& workspace) {
    std::string written{};
    const WriteOutcome outcome = write_theme_sheet(workspace.tree, workspace.tokens, workspace.text, written);
    return outcome == WriteOutcome::written && written == workspace.text;
}

struct ExpectedNode final {
    SyntaxNodeKind kind{};
    std::string_view content{};
    bool important{};
    // The node's children, counted to its subtree's end.
    std::uint32_t descendants{};
};

void test_structure() {
    const std::string_view sheet = "@import \"palette\";\n"
                                   "/* lead */\n"
                                   "button:hover > part(face), #x { background: rgb(1 2 3) ; --glow: 1px !important }\n"
                                   "@media (forced-colors: active) {\n"
                                   "  * { color: CanvasText; }\n"
                                   "}\n"
                                   "@keyframes pulse { from { scale: 1 } 50% { scale: 1.1; } }\n"
                                   "@gf-extension x { anything { goes } here; }\n"
                                   "check-box{}";
    const std::array<ExpectedNode, 15> expected{{
        {SyntaxNodeKind::sheet, "", false, 14U},
        {SyntaxNodeKind::import_rule, "\"palette\"", false, 0U},
        {SyntaxNodeKind::style_rule, "button:hover > part(face), #x", false, 2U},
        {SyntaxNodeKind::declaration, "rgb(1 2 3)", false, 0U},
        {SyntaxNodeKind::declaration, "1px", true, 0U},
        {SyntaxNodeKind::media_rule, "(forced-colors: active)", false, 2U},
        {SyntaxNodeKind::style_rule, "*", false, 1U},
        {SyntaxNodeKind::declaration, "CanvasText", false, 0U},
        {SyntaxNodeKind::keyframes_rule, "pulse", false, 4U},
        {SyntaxNodeKind::keyframe_block, "from", false, 1U},
        {SyntaxNodeKind::declaration, "1", false, 0U},
        {SyntaxNodeKind::keyframe_block, "50%", false, 1U},
        {SyntaxNodeKind::declaration, "1.1", false, 0U},
        {SyntaxNodeKind::unknown_at_rule, "x", false, 0U},
        {SyntaxNodeKind::style_rule, "check-box", false, 0U},
    }};
    Workspace workspace{};
    const ParseOutcome outcome = parse(workspace, sheet);
    require(outcome == ParseOutcome::tree_complete, "structure: complete");
    const std::vector<SyntaxNode>& nodes = workspace.tree.nodes;
    require(nodes.size() == expected.size(), "structure: node count");
    const std::size_t count = nodes.size() < expected.size() ? nodes.size() : expected.size();
    for (std::size_t index = 0U; index < count; ++index) {
        const SyntaxNode& node = nodes[index];
        const ExpectedNode& entry = expected[index];
        const std::string_view content = index == 0U ? std::string_view{} : range_text(workspace, node.content);
        const bool important = (node.flags & syntax_flag_important) != 0U;
        const std::uint32_t descendants = node.subtree_end.value - static_cast<std::uint32_t>(index) - 1U;
        const bool same = node.kind == entry.kind && content == entry.content && important == entry.important &&
                          descendants == entry.descendants;
        if (!same) {
            std::cerr << "  node " << index << ": " << syntax_node_kind_name(node.kind) << " '" << content << "' "
                      << descendants << ", expected " << syntax_node_kind_name(entry.kind) << " '" << entry.content
                      << "' " << entry.descendants << '\n';
        }
        require(same, "structure: node");
    }
    if (nodes.size() == expected.size()) {
        // A declaration's property is its first token; one ended by the
        // block's '}' ends at its last written token.
        require(range_text(workspace, nodes[4].tokens) == "--glow: 1px !important", "a declaration's span");
        require(range_text(workspace, nodes[3].tokens) == "background: rgb(1 2 3) ;", "a declaration with its ';'");
        require(range_text(workspace, nodes[1].tokens) == "@import \"palette\";", "an import's span");
    }
    const std::span<const ThemeDiagnostic> records = workspace.diagnostics.records();
    require(records.size() == 1U && records[0].code == DiagnosticCode::unknown_at_rule &&
                records[0].detail == "@gf-extension" && records[0].position.line == LineNumber{8U},
            "an unknown at-rule is one warning");
    require(workspace.diagnostics.error_count() == 0U && workspace.diagnostics.warning_count() == 1U,
            "warnings are not errors");
    if (!records.empty()) {
        require(theme_diagnostic_log_line(records[0]) ==
                    "gui-forms-theme=warning|code:W104 unknown_at_rule|file:test.theme.css|line:8|column:1|"
                    "rule:|property:|detail:@gf-extension",
                "the warning's log line");
    }
    require(writes_back(workspace), "structure: round trip");
}

void test_case_folding() {
    Workspace workspace{};
    const ParseOutcome outcome = parse(workspace, "@MEDIA screen { a { b: c ! /* x */ IMPORTANT } }");
    require(outcome == ParseOutcome::tree_complete, "folded: complete");
    const std::vector<SyntaxNode>& nodes = workspace.tree.nodes;
    require(nodes.size() == 4U, "folded: nodes");
    if (nodes.size() != 4U) return;
    require(nodes[1].kind == SyntaxNodeKind::media_rule, "@MEDIA is @media");
    require(nodes[3].flags == syntax_flag_important && range_text(workspace, nodes[3].content) == "c",
            "! IMPORTANT with a comment between");
}

// Real-world formatting returns identical bytes.
void test_round_trip_corpus() {
    const std::array<std::string_view, 14> corpus{{
        "",
        "   \n\t",
        "/* only a comment */",
        "\xEF\xBB\xBF" "button { color: red; }\n",
        "a {\r\n  b: c;\r\n}\r\n",
        "a{b:c}",
        // Empty values parse; P4 judges them.
        "a { b: }",
        "a { b:; c: /* */ }",
        "a/**/{/**/b/**/:/**/c/**/;/**/}/**/",
        "a {\r  b: c;\r}",
        "@media (prefers-color-scheme: dark) and (min-resolution: 2dppx) {\n"
        "  button::part(face):hover { background: role(accent background) !important; }\n"
        "}",
        "@keyframes k {\n  from { opacity: 0 }\n  to { opacity: 1 }\n}\n",
        "a, b { c: calc((1px + 2px) * 3); d: \"str;{}\"; e: f([g]); ; ; }",
        "\xEF\xBB\xBF/* lead */\r\n@import 'base';\r\n#x.y:state(drop-accepted) { --v: 1 2 3 }",
    }};
    for (const std::string_view sheet : corpus) {
        Workspace workspace{};
        const ParseOutcome outcome = parse(workspace, sheet);
        require(outcome == ParseOutcome::tree_complete, std::string("corpus parses: ") + std::string(sheet));
        require(workspace.diagnostics.records().empty(), std::string("corpus is clean: ") + std::string(sheet));
        require(writes_back(workspace), std::string("corpus round trip: ") + std::string(sheet));
    }
}

struct DefectCase final {
    std::string_view text{};
    DiagnosticCode code{};
    std::uint32_t line{};
    std::uint32_t column{};
};

// Every structural defect, each with its line and column.
void test_defects() {
    const std::array<DefectCase, 25> cases{{
        {"a { b: c;", DiagnosticCode::unbalanced_block, 1U, 3U},
        {"a { b: c; } }", DiagnosticCode::unbalanced_block, 1U, 13U},
        {"\n)", DiagnosticCode::unbalanced_block, 2U, 1U},
        {"a { b: f(1; }", DiagnosticCode::unbalanced_block, 1U, 8U},
        {"a { b: f(1] }", DiagnosticCode::unbalanced_block, 1U, 11U},
        {"a ( { }", DiagnosticCode::unbalanced_block, 1U, 3U},
        {"@media x {\n a {}", DiagnosticCode::unbalanced_block, 1U, 10U},
        {"a {\r\n  b: c;\r\n  d: f(\r\n}", DiagnosticCode::unbalanced_block, 3U, 6U},
        {"@x { a ( }", DiagnosticCode::unbalanced_block, 1U, 10U},
        {"b: c;", DiagnosticCode::declaration_outside_rule, 1U, 1U},
        {"@media x { b: c; }", DiagnosticCode::declaration_outside_rule, 1U, 12U},
        {"a {}\n@import \"x\";", DiagnosticCode::import_after_rules, 2U, 1U},
        {"@keyframes k { from { a: 1 !important } }", DiagnosticCode::important_in_keyframes, 1U, 28U},
        {"a { b:hover { } }", DiagnosticCode::nesting_not_admitted, 1U, 5U},
        {"a { b { } }", DiagnosticCode::nesting_not_admitted, 1U, 5U},
        {"a { &:hover { } }", DiagnosticCode::nesting_not_admitted, 1U, 5U},
        {"a { .x { } }", DiagnosticCode::nesting_not_admitted, 1U, 5U},
        {"a { @media x { } }", DiagnosticCode::nesting_not_admitted, 1U, 5U},
        {"@media x { @media y { } }", DiagnosticCode::nesting_not_admitted, 1U, 12U},
        {"a { b c; }", DiagnosticCode::unexpected_token, 1U, 7U},
        {"a { 12px; }", DiagnosticCode::unexpected_token, 1U, 5U},
        {"@import x;", DiagnosticCode::unexpected_token, 1U, 9U},
        {"@import \"a\" \"b\";", DiagnosticCode::unexpected_token, 1U, 13U},
        {"@import \"a\"", DiagnosticCode::unexpected_token, 1U, 12U},
        {"@media x { a }", DiagnosticCode::unexpected_token, 1U, 14U},
    }};
    for (const DefectCase& entry : cases) {
        Workspace workspace{};
        const ParseOutcome outcome = parse(workspace, entry.text);
        const std::string what = std::string(diagnostic_code_name(entry.code)) + ": " + std::string(entry.text);
        require(outcome == ParseOutcome::stopped_at_error, what + ": stops");
        const std::span<const ThemeDiagnostic> records = workspace.diagnostics.records();
        const std::size_t errors = workspace.diagnostics.error_count();
        require(errors == 1U, what + ": one error");
        if (records.empty()) continue;
        // The error is the last record; an unknown at-rule's warning may
        // come before it.
        const ThemeDiagnostic& diagnostic = records[records.size() - 1U];
        const bool located = diagnostic.code == entry.code && diagnostic.position.line == LineNumber{entry.line} &&
                             diagnostic.position.column == ColumnNumber{entry.column};
        if (!located) {
            std::cerr << "  got " << diagnostic_code_name(diagnostic.code) << " at "
                      << diagnostic.position.line.value << ':' << diagnostic.position.column.value << '\n';
        }
        require(located, what + ": code, line and column");
        require(writes_back(workspace), what + ": a stopped tree still nests");
    }
    // The end of the sheet cuts off an item.
    Workspace workspace{};
    require(parse(workspace, "a") == ParseOutcome::stopped_at_error, "a lone prelude stops");
    const std::span<const ThemeDiagnostic> records = workspace.diagnostics.records();
    require(records.size() == 1U && records[0].code == DiagnosticCode::unexpected_token &&
                records[0].position.column == ColumnNumber{2U} && records[0].detail == "end of sheet",
            "the end of the sheet is located after its last byte");
}

std::string repeated(std::string_view piece, std::size_t count) {
    std::string text{};
    text.reserve(piece.size() * count);
    for (std::size_t index = 0U; index < count; ++index) text.append(piece);
    return text;
}

void require_limit(const std::string& text, DiagnosticCode code, std::size_t offset, std::string_view what) {
    Workspace workspace{};
    const ParseOutcome outcome = parse(workspace, text);
    const std::span<const ThemeDiagnostic> records = workspace.diagnostics.records();
    const bool refused = outcome == ParseOutcome::stopped_at_error && records.size() == 1U &&
                         records[0].code == code && records[0].position.offset.value == offset;
    require(refused, what);
}

void require_within_limit(const std::string& text, std::string_view what) {
    Workspace workspace{};
    const ParseOutcome outcome = parse(workspace, text);
    require(outcome == ParseOutcome::tree_complete && workspace.diagnostics.records().empty(), what);
}

void test_limits() {
    require_within_limit(repeated("a{}", maximum_rules), "10,000 rules");
    require_limit(repeated("a{}", maximum_rules + 1U), DiagnosticCode::too_many_rules, 3U * maximum_rules,
                  "the 10,001st rule");

    const std::string declarations = "a{" + repeated("b:c;", maximum_declarations_per_rule) + "}";
    require_within_limit(declarations, "64 declarations");
    const std::string too_many = "a{" + repeated("b:c;", maximum_declarations_per_rule + 1U) + "}";
    require_limit(too_many, DiagnosticCode::too_many_declarations, 2U + 4U * maximum_declarations_per_rule,
                  "the 65th declaration");

    // The rule's block is one level, so 31 functions reach the limit.
    const std::size_t functions = maximum_nesting_depth - 1U;
    const std::string deep = "a{b:" + repeated("f(", functions) + repeated(")", functions) + "}";
    require_within_limit(deep, "nesting 32 deep");
    const std::string too_deep = "a{b:" + repeated("f(", functions + 1U) + repeated(")", functions + 1U) + "}";
    require_limit(too_deep, DiagnosticCode::nesting_too_deep, 4U + 2U * functions, "nesting 33 deep");
    const std::string deep_unknown = "@x" + repeated("{", maximum_nesting_depth + 1U);
    Workspace workspace{};
    const ParseOutcome unknown_outcome = parse(workspace, deep_unknown);
    const std::span<const ThemeDiagnostic> unknown_records = workspace.diagnostics.records();
    require(unknown_outcome == ParseOutcome::stopped_at_error && unknown_records.size() == 2U &&
                unknown_records[1].code == DiagnosticCode::nesting_too_deep &&
                unknown_records[1].position.offset.value == 2U + maximum_nesting_depth,
            "an unknown at-rule's block is bounded too");

    require_within_limit(repeated("@import \"a\";", maximum_imports), "16 imports");
    require_limit(repeated("@import \"a\";", maximum_imports + 1U), DiagnosticCode::too_many_imports,
                  12U * maximum_imports, "the 17th import");
}

void test_lexical_stop() {
    Workspace workspace{};
    // An unterminated string: the tokenizer's error, and no other.
    require(parse(workspace, "a { b: \"open\n }") == ParseOutcome::stopped_at_lexical_error,
            "an invalid token stops the parse");
    require(workspace.diagnostics.records().size() == 1U &&
                workspace.diagnostics.records()[0].code == DiagnosticCode::unterminated_string,
            "the lexical error alone");
    require(writes_back(workspace), "a lexically stopped tree nests");
    require(parse(workspace, "a { b: c } \\") == ParseOutcome::stopped_at_lexical_error,
            "an invalid token after complete items");
    require(workspace.diagnostics.error_count() == 1U, "one error");
}

void test_reuse() {
    Workspace workspace{};
    static_cast<void>(parse(workspace, "a { b: c; d: e; } f { g: h; }"));
    const std::size_t capacity = workspace.tree.nodes.capacity();
    static_cast<void>(parse(workspace, "a{}"));
    require(workspace.tree.nodes.size() == 2U, "the tree holds the latest sheet");
    require(workspace.tree.nodes.capacity() == capacity, "the capacity is kept");
}

} // namespace

int main() {
    test_structure();
    test_case_folding();
    test_round_trip_corpus();
    test_defects();
    test_limits();
    test_lexical_stop();
    test_reuse();
    if (failures != 0) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "theme syntax tests passed\n";
    return 0;
}
