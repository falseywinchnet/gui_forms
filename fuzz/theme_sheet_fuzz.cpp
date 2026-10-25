#include "../src/core/theme_language/diagnostics/theme_diagnostics.hpp"
#include "../src/core/theme_language/source_text/theme_source_text.hpp"
#include "../src/core/theme_language/syntax_tree/theme_syntax_tree.hpp"
#include "../src/core/theme_language/tokenizer/theme_tokenizer.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <span>
#include <string>
#include <string_view>

#if defined(GUI_FORMS_FUZZ_STANDALONE)
#include <array>
#endif

namespace {

namespace theme_language = gui_forms::theme_language;

// Names the broken invariant on stderr and stops.
[[noreturn]] void fail(const char* invariant) noexcept {
    std::fputs("theme sheet fuzz: ", stderr);
    std::fputs(invariant, stderr);
    std::fputc('\n', stderr);
    std::abort();
}

// Whether the tokens cover `size` bytes in order, each token non-empty, and
// each numeric token naming the next number in the buffer.
bool tokens_cover(const theme_language::ThemeTokenBuffer& buffer, std::size_t size) noexcept {
    std::size_t next = 0U;
    std::uint32_t next_number = 0U;
    for (const theme_language::Token& token : buffer.tokens) {
        if (token.offset.value != next || token.length.value == 0U) return false;
        next += token.length.value;
        const bool numeric = token.kind == theme_language::TokenKind::number ||
                             token.kind == theme_language::TokenKind::percentage ||
                             token.kind == theme_language::TokenKind::dimension;
        if (numeric) {
            if (token.number != next_number || next_number >= buffer.numbers.size()) return false;
            const theme_language::ThemeNumber& number = buffer.numbers[next_number];
            if (number.numeric_length.value == 0U || number.numeric_length.value > token.length.value) return false;
            ++next_number;
        }
    }
    return next == size && next_number == buffer.numbers.size();
}

// Whether the diagnostics lie within the sheet as at most `runs_admitted`
// runs, each in offset order.
bool diagnostics_in_order(std::span<const theme_language::ThemeDiagnostic> records, std::size_t size,
                          std::size_t runs_admitted) noexcept {
    std::uint32_t previous = 0U;
    std::size_t runs = records.empty() ? 0U : 1U;
    for (const theme_language::ThemeDiagnostic& record : records) {
        const std::uint32_t offset = record.position.offset.value;
        if (offset > size) return false;
        if (offset < previous) ++runs;
        previous = offset;
    }
    return runs <= runs_admitted;
}

// Whether each node's content lies within its own tokens.
bool contents_within_nodes(const theme_language::ThemeSyntaxTree& tree) noexcept {
    for (const theme_language::SyntaxNode& node : tree.nodes) {
        const bool within = node.content.first.value >= node.tokens.first.value &&
                            node.content.first.value <= node.content.end.value &&
                            node.content.end.value <= node.tokens.end.value;
        if (!within) return false;
    }
    return true;
}

bool same_records(std::span<const theme_language::ThemeDiagnostic> first,
                  std::span<const theme_language::ThemeDiagnostic> second) noexcept {
    if (first.size() != second.size()) return false;
    for (std::size_t index = 0U; index < first.size(); ++index) {
        if (!(first[index] == second[index])) return false;
    }
    return true;
}

} // namespace

// Tokenizes and parses the input. The tokens must cover it exactly; the
// tree, complete or stopped, must nest and write back the input's bytes;
// the parse adds at most one error; and a second tokenization and parse
// into fresh storage must give the same tokens, numbers, nodes and
// diagnostics.
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    theme_language::ThemeSource source{};
    source.file = "fuzz.theme.css";
    source.text = std::string_view(reinterpret_cast<const char*>(data), size);
    theme_language::ThemeTokenBuffer buffer{};
    theme_language::LineStartTable lines{};
    theme_language::ThemeDiagnosticList diagnostics{};
    const theme_language::TokenizeOutcome outcome =
        theme_language::tokenize_theme_sheet(source, buffer, lines, diagnostics);
    if (outcome == theme_language::TokenizeOutcome::refused_too_large) {
        if (!buffer.tokens.empty() || diagnostics.error_count() != 1U) fail("a refused sheet keeps nothing");
        return 0;
    }
    if (!tokens_cover(buffer, size)) fail("the tokens cover the sheet");
    if (!diagnostics_in_order(diagnostics.records(), size, 2U)) {
        fail("token diagnostics lie in the sheet, in two ordered runs");
    }
    bool has_invalid = false;
    for (const theme_language::Token& token : buffer.tokens) {
        if (token.kind == theme_language::TokenKind::invalid) has_invalid = true;
    }
    if (has_invalid && diagnostics.error_count() == 0U) fail("an invalid token has an error");

    const std::size_t lexical_errors = diagnostics.error_count();
    const std::size_t lexical_records = diagnostics.records().size();
    theme_language::ThemeSyntaxTree tree{};
    const theme_language::ParseOutcome parsed = theme_language::parse_theme_sheet(source, buffer, lines, tree, diagnostics);
    if (parsed == theme_language::ParseOutcome::tree_complete && has_invalid) fail("an invalid token stops the parse");
    if (parsed == theme_language::ParseOutcome::stopped_at_lexical_error && !has_invalid) {
        fail("a lexical stop needs an invalid token");
    }
    const std::size_t parse_errors = diagnostics.error_count() - lexical_errors;
    const std::size_t parse_errors_admitted = parsed == theme_language::ParseOutcome::stopped_at_error ? 1U : 0U;
    if (parse_errors != parse_errors_admitted) fail("a stopped parse reports one error, a complete one none");
    const std::span<const theme_language::ThemeDiagnostic> parse_records =
        diagnostics.records().subspan(lexical_records);
    if (!diagnostics_in_order(parse_records, size, 2U)) fail("parse diagnostics lie in the sheet");
    if (!contents_within_nodes(tree)) fail("contents lie within their nodes");
    std::string written{};
    const theme_language::WriteOutcome write = theme_language::write_theme_sheet(tree, buffer, source.text, written);
    if (write != theme_language::WriteOutcome::written) fail("the tree nests");
    if (written != source.text) fail("the tree writes back the sheet's bytes");

    theme_language::ThemeTokenBuffer again{};
    theme_language::ThemeDiagnosticList again_diagnostics{};
    static_cast<void>(theme_language::tokenize_theme_sheet(source, again, lines, again_diagnostics));
    if (again.tokens != buffer.tokens || again.numbers != buffer.numbers) fail("tokenizing is repeatable");
    theme_language::ThemeSyntaxTree again_tree{};
    const theme_language::ParseOutcome reparsed =
        theme_language::parse_theme_sheet(source, again, lines, again_tree, again_diagnostics);
    if (reparsed != parsed || again_tree.nodes != tree.nodes) fail("parsing is repeatable");
    if (!same_records(diagnostics.records(), again_diagnostics.records())) fail("diagnostics are repeatable");
    return 0;
}

#if defined(GUI_FORMS_FUZZ_STANDALONE)
namespace {

// One xorshift64 step: advances `state` and returns it.
std::uint64_t next_random(std::uint64_t& state) noexcept {
    state ^= state << 13U;
    state ^= state >> 7U;
    state ^= state << 17U;
    return state;
}

} // namespace

int main() {
    // Each seed parses completely; the mutations explore around them.
    const std::array<std::string_view, 4> seeds{
        "@import \"palette.theme.css\";\n"
        "@gf-theme \"aurora\" { version: 1; }\n"
        "button:hover > part(face), #player-play[checked] {\n"
        "  background: rgb(10 20 30 / 50%);\n"
        "  --glow: 1.5e1px;\r\n"
        "  icon: url(\"resource:icon.check\") !important;\n"
        "}\n",
        "/* comment */ @media (forced-colors: active) { * { color: CanvasText } }\r"
        "a{b:'c\\\nd';e:-.5em+3%}",
        "\xEF\xBB\xBF" "caf\xC3\xA9 { width: calc(100% - 12px); }",
        "@keyframes pulse { from { scale: 1 } 50% { scale: f(g([1])); } }\n"
        "@x y { z { w } ; }\n"
        "a { b: ; c: /* */ }"};
    std::uint64_t state = 0xbb67ae8584caa73bULL;
    for (std::size_t run = 0; run < 100'000; ++run) {
        const std::uint64_t run_draw = next_random(state);
        std::string input(seeds[run % seeds.size()]);
        const std::size_t mutations = 1U + static_cast<std::size_t>(run_draw % 8U);
        // Each mutation flips one bit of one byte.
        for (std::size_t mutation = 0; mutation < mutations; ++mutation) {
            const std::uint64_t draw = next_random(state);
            if (input.empty()) continue;
            const std::size_t offset = static_cast<std::size_t>(draw % input.size());
            const char flipped_bit = static_cast<char>(1U << (draw % 8U));
            input[offset] = static_cast<char>(input[offset] ^ flipped_bit);
        }
        if (run % 5U == 0 && !input.empty()) {
            input.resize(static_cast<std::size_t>(state % input.size()));
        } else if (run % 7U == 0) {
            input.insert(input.size() / 2U, static_cast<std::size_t>(state % 512U), '9');
        } else if (run % 11U == 0) {
            input.insert(input.size() / 2U, static_cast<std::size_t>(state % 512U), '"');
        }
        LLVMFuzzerTestOneInput(reinterpret_cast<const std::uint8_t*>(input.data()), input.size());
    }
    return 0;
}
#endif
