// Stage L, phase L2: the layout style reader. One conformance case per
// admitted property and form (read, described canonically, and read back
// to an equal style), one per refused form with its exact diagnostic, the
// warnings that drop a declaration, the track and cell limits, every style
// attribute of the layout corpus, and a deterministic mutation run in which
// a refused read must leave the style as it was.

#include "../src/core/layout/style_reader/layout_style_reader.hpp"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

using gui_forms::DisplayKind;
using gui_forms::LayoutStyle;
using gui_forms::layout_style::LayoutStyleReader;
using gui_forms::layout_style::StyleReadOutcome;
using gui_forms::layout_style::describe_layout_style;
using gui_forms::theme_language::DiagnosticCode;
using gui_forms::theme_language::ThemeDiagnostic;
using gui_forms::theme_language::ThemeDiagnosticList;
using gui_forms::theme_language::ThemeSource;
using gui_forms::theme_language::diagnostic_code_label;

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

std::string describe_diagnostics(const ThemeDiagnosticList& diagnostics) {
    std::string text;
    for (const ThemeDiagnostic& diagnostic : diagnostics.records()) {
        text += ' ';
        text += diagnostic_code_label(diagnostic.code);
        text += '@';
        text += std::to_string(diagnostic.position.column.value);
        text += '[';
        text += diagnostic.property;
        text += "|";
        text += diagnostic.detail;
        text += ']';
    }
    return text;
}

// -- accepted forms -------------------------------------------------------------

struct AcceptedCase final {
    std::string_view style;
    // describe_layout_style over a default LayoutStyle.
    std::string_view expected;
};

const AcceptedCase accepted_cases[] = {
    {"display: block", "display: block"},
    {"display: flex; display: GRID", "display: grid"},
    {"display: inline-block", "display: inline-block"},
    {"display: none", "display: none"},
    {"display: inline", ""},
    {"position: absolute; top: 10px; left: -5%", "position: absolute; top: 10px; left: -5%"},
    {"position: relative; right: auto; bottom: 0", "position: relative; bottom: 0px"},
    {"inset: 1px 2px", "top: 1px; right: 2px; bottom: 1px; left: 2px"},
    {"width: 100px; height: 50%", "width: 100px; height: 50%"},
    {"width: calc(100% - 24px)", "width: calc(100% - 24px)"},
    {"width: calc((50% + 10px) * 2 - 4px / 2)", "width: calc(100% + 18px)"},
    {"height: calc(2 * calc(5px + 1%))", "height: calc(2% + 10px)"},
    {"width: min-content; height: max-content", "width: min-content; height: max-content"},
    {"width: fit-content(200px)", "width: fit-content(200px)"},
    {"min-width: 0; min-height: 4px", "min-width: 0px; min-height: 4px"},
    {"max-width: 300px; max-height: none", "max-width: 300px"},
    {"margin: auto", "margin-top: auto; margin-right: auto; margin-bottom: auto; margin-left: auto"},
    {"margin: 10px 0 0 5px", "margin-top: 10px; margin-left: 5px"},
    {"margin: -4px 2px 3px", "margin-top: -4px; margin-right: 2px; margin-bottom: 3px; margin-left: 2px"},
    {"margin-right: 1px; margin-bottom: auto", "margin-right: 1px; margin-bottom: auto"},
    {"margin-top: 2px; margin-left: -1px", "margin-top: 2px; margin-left: -1px"},
    {"padding: 12px 6px 2px 20px", "padding-top: 12px; padding-right: 6px; padding-bottom: 2px; padding-left: 20px"},
    {"padding: 4px; padding-left: 3px", "padding-top: 4px; padding-right: 4px; padding-bottom: 4px; padding-left: 3px"},
    {"padding-top: 1%; padding-right: 2px; padding-bottom: 3px", "padding-top: 1%; padding-right: 2px; padding-bottom: 3px"},
    {"box-sizing: border-box", ""},
    {"gap: 6px", "row-gap: 6px; column-gap: 6px"},
    {"gap: 4px 8px; row-gap: normal", "column-gap: 8px"},
    {"column-gap: 10%", "column-gap: 10%"},
    {"flex-direction: column-reverse; flex-wrap: wrap", "flex-direction: column-reverse; flex-wrap: wrap"},
    {"flex-flow: wrap-reverse column", "flex-direction: column; flex-wrap: wrap-reverse"},
    {"flex-flow: row-reverse", "flex-direction: row-reverse"},
    {"flex: none", "flex-shrink: 0"},
    {"flex: auto", "flex-grow: 1"},
    {"flex: 2", "flex-grow: 2; flex-basis: 0px"},
    {"flex: 1 0 auto", "flex-grow: 1; flex-shrink: 0"},
    {"flex: 1 1 0", "flex-grow: 1; flex-basis: 0px"},
    {"flex: 30px", "flex-grow: 1; flex-basis: 30px"},
    {"flex: 3 content", "flex-grow: 3; flex-basis: content"},
    {"flex: 50% 0 2", "flex-shrink: 2; flex-basis: 50%"},
    {"flex-grow: 1.5; flex-shrink: 0; flex-basis: content; order: -2",
     "flex-grow: 1.5; flex-shrink: 0; flex-basis: content; order: -2"},
    {"justify-content: space-between; align-content: stretch",
     "justify-content: space-between; align-content: stretch"},
    {"justify-items: center; align-items: baseline", "justify-items: center; align-items: baseline"},
    {"justify-self: end; align-self: flex-start", "justify-self: end; align-self: flex-start"},
    {"place-items: center", "justify-items: center; align-items: center"},
    {"place-self: start end", "justify-self: end; align-self: start"},
    {"grid-template-columns: 50px minmax(0, 1fr) 2fr", "grid-template-columns: 50px minmax(0px, 1fr) 2fr"},
    {"grid-template-columns: repeat(3, 10px auto)", "grid-template-columns: 10px auto 10px auto 10px auto"},
    {"grid-template-columns: repeat(auto-fill, minmax(86px, 1fr))",
     "grid-template-columns: repeat(auto-fill, minmax(86px, 1fr))"},
    {"grid-template-columns: 20px repeat(auto-fit, 40px) 30px",
     "grid-template-columns: 20px repeat(auto-fit, 40px) 30px"},
    {"grid-template-rows: fit-content(40%) min-content max-content auto",
     "grid-template-rows: fit-content(40%) min-content max-content auto"},
    {"grid-template-rows: minmax(min-content, 25%)", "grid-template-rows: minmax(min-content, 25%)"},
    {"grid-template-columns: none", ""},
    {"grid-template-areas: \"a a b\" \"c . b\"", "grid-template-areas: \"a a b\" \"c . b\""},
    {"grid-template-areas: \"head head\" \"nav main\" \"nav ...\"",
     "grid-template-areas: \"head head\" \"nav main\" \"nav .\""},
    {"grid-auto-flow: column dense", "grid-auto-flow: column dense"},
    {"grid-auto-flow: dense", "grid-auto-flow: row dense"},
    {"grid-auto-rows: 20px minmax(10px, auto)", "grid-auto-rows: 20px minmax(10px, auto)"},
    {"grid-auto-columns: 1fr", "grid-auto-columns: 1fr"},
    {"grid-column: 1 / span 2", "grid-column-start: 1; grid-column-end: span 2"},
    {"grid-row: -1", "grid-row-start: -1"},
    {"grid-row: main", "grid-row-start: main; grid-row-end: main"},
    {"grid-area: header", "grid-column-start: header; grid-column-end: header; grid-row-start: header; "
                          "grid-row-end: header"},
    {"grid-area: 1 / 2 / 3 / 4", "grid-column-start: 2; grid-column-end: 4; grid-row-start: 1; grid-row-end: 3"},
    {"grid-column-start: span 3; grid-row-end: 2 span", "grid-column-start: span 3; grid-row-end: span 2"},
    {"grid-column-end: -2; grid-row-start: span 1", "grid-column-end: -2; grid-row-start: span 1"},
    {"text-align: center; vertical-align: middle", "text-align: center; vertical-align: middle"},
    {"  /* note */ width : 10px ;; ", "width: 10px"},
    {"WIDTH: 10PX; Display: Flex", "display: flex; width: 10px"},
    {"", ""},
};

// Every admitted property (PLAN sections 2.2 to 2.5), each of which some
// accepted case writes.
const std::string_view admitted_property_names[] = {
    "display", "position", "inset", "top", "right", "bottom", "left", "width", "height", "min-width",
    "min-height", "max-width", "max-height", "margin", "margin-top", "margin-right", "margin-bottom",
    "margin-left", "padding", "padding-top", "padding-right", "padding-bottom", "padding-left", "box-sizing",
    "gap", "row-gap", "column-gap", "flex-direction", "flex-wrap", "flex-flow", "flex", "flex-grow",
    "flex-shrink", "flex-basis", "order", "justify-content", "align-content", "justify-items", "align-items",
    "justify-self", "align-self", "place-items", "place-self", "grid-template-columns", "grid-template-rows",
    "grid-template-areas", "grid-auto-flow", "grid-auto-columns", "grid-auto-rows", "grid-column", "grid-row",
    "grid-area", "grid-column-start", "grid-column-end", "grid-row-start", "grid-row-end", "text-align",
    "vertical-align",
};

// Whether `style` declares `name`: the name at the start or after `;` and
// spaces, followed by optional spaces and `:`.
bool declares(std::string_view style, std::string_view name) {
    std::size_t position = style.find(name);
    while (position != std::string_view::npos) {
        std::size_t before = position;
        while (before > 0U && style[before - 1U] == ' ') --before;
        const bool starts = before == 0U || style[before - 1U] == ';';
        std::size_t after = position + name.size();
        while (after < style.size() && style[after] == ' ') ++after;
        if (starts && after < style.size() && style[after] == ':') return true;
        position = style.find(name, position + 1U);
    }
    return false;
}

void test_every_property_has_a_case() {
    for (const std::string_view name : admitted_property_names) {
        bool found = false;
        for (const AcceptedCase& accepted : accepted_cases) found = found || declares(accepted.style, name);
        require(found, "no accepted case writes " + std::string(name));
    }
}

void test_accepted_forms_and_round_trip() {
    LayoutStyleReader reader;
    ThemeDiagnosticList diagnostics;
    for (const AcceptedCase& accepted : accepted_cases) {
        diagnostics.clear();
        LayoutStyle style{};
        const StyleReadOutcome outcome = reader.read_inline_style(ThemeSource{"inline:case", accepted.style}, style,
                                                                  diagnostics);
        const std::string where = "accepted \"" + std::string(accepted.style) + "\"";
        require(outcome == StyleReadOutcome::applied, where + " refused:" + describe_diagnostics(diagnostics));
        require(diagnostics.records().empty(), where + " reported:" + describe_diagnostics(diagnostics));
        const std::string described = describe_layout_style(style, LayoutStyle{});
        require(described == accepted.expected, where + " described as \"" + described + "\"");
        LayoutStyle read_back{};
        const StyleReadOutcome again =
            reader.read_inline_style(ThemeSource{"inline:round-trip", described}, read_back, diagnostics);
        require(again == StyleReadOutcome::applied && read_back == style, where + " does not read back");
    }
}

// -- refused forms --------------------------------------------------------------

struct RefusedCase final {
    std::string_view style;
    DiagnosticCode code;
    // The diagnostic's column is that of this text's first occurrence.
    std::string_view at;
    std::string_view property;
    std::string_view detail;
};

const RefusedCase refused_cases[] = {
    // E401 value_syntax.
    {"width: 10px 20px", DiagnosticCode::value_syntax, "20px", "width", "20px"},
    {"width: calc(100%-24px)", DiagnosticCode::value_syntax, "-24px", "width", "-24px"},
    {"width: calc(100% -24px)", DiagnosticCode::value_syntax, "-24px", "width", "-24px"},
    {"width: calc(1px +2px)", DiagnosticCode::value_syntax, "+2px", "width", "+2px"},
    {"width: calc(1px+ 2px)", DiagnosticCode::value_syntax, "+", "width", "+ and - need whitespace on both sides"},
    {"grid-template-areas: \"a b\" \"b a\"", DiagnosticCode::value_syntax, "\"a b\"", "grid-template-areas", "a"},
    {"grid-template-areas: \"a b\" \"c\"", DiagnosticCode::value_syntax, "\"c\"", "grid-template-areas",
     "rows of different lengths"},
    {"grid-template-areas: \"a $\"", DiagnosticCode::value_syntax, "\"a $\"", "grid-template-areas", "\"a $\""},
    {"grid-template-areas: \"  \"", DiagnosticCode::value_syntax, "\"  \"", "grid-template-areas",
     "a row without cells"},
    {"grid-template-columns: repeat(2, repeat(2, 1px))", DiagnosticCode::value_syntax, "repeat(2, 1px", "grid-template-columns",
     "repeat() inside repeat()"},
    {"grid-template-columns: repeat(auto-fill, 1px) repeat(auto-fit, 2px)", DiagnosticCode::value_syntax,
     "repeat(auto-fit", "grid-template-columns", "a second automatic repeat"},
    {"grid-auto-rows: repeat(2, 1px)", DiagnosticCode::value_syntax, "repeat", "grid-auto-rows",
     "repeat() in implicit tracks"},
    {"grid-column: 1 / 2 / 3", DiagnosticCode::value_syntax, "/ 3", "grid-column", "/"},
    {"grid-column-start: 1 / 2", DiagnosticCode::value_syntax, "/", "grid-column-start", "/"},
    {"grid-row: span", DiagnosticCode::value_syntax, "span", "grid-row", "span needs a count"},
    {"flex: 1 2 3", DiagnosticCode::value_syntax, "3", "flex", "3"},
    {"flex-flow: row column", DiagnosticCode::value_syntax, "column", "flex-flow", "column"},
    {"width: ", DiagnosticCode::value_syntax, "", "width", "an empty value"},
    {"width: calc()", DiagnosticCode::value_syntax, ")", "width", "a value is missing"},
    {"width: !", DiagnosticCode::value_syntax, "!", "width", "!"},
    // E402 value_wrong_kind.
    {"flex-grow: 10px", DiagnosticCode::value_wrong_kind, "10px", "flex-grow", "10px"},
    {"order: 1.5", DiagnosticCode::value_wrong_kind, "1.5", "order", "1.5"},
    {"order: 50%", DiagnosticCode::value_wrong_kind, "50%", "order", "50%"},
    {"width: 10", DiagnosticCode::value_wrong_kind, "10", "width", "a length needs a unit"},
    {"width: \"10px\"", DiagnosticCode::value_wrong_kind, "\"10px\"", "width", "\"10px\""},
    {"width: calc(10px * 5px)", DiagnosticCode::value_wrong_kind, "*", "width", "multiply by a number"},
    {"width: calc(10px / 5px)", DiagnosticCode::value_wrong_kind, "/", "width", "divide by a number"},
    {"width: calc(1 + 1px)", DiagnosticCode::value_wrong_kind, "+", "width", "a number and a length"},
    {"width: calc(2 * 3)", DiagnosticCode::value_wrong_kind, "calc", "width", "calc() of a number"},
    {"grid-template-columns: minmax(1fr, 2fr)", DiagnosticCode::value_wrong_kind, "1fr", "grid-template-columns",
     "a fraction is not a minimum"},
    {"grid-template-columns: repeat(auto-fill, 1fr)", DiagnosticCode::value_wrong_kind, "repeat",
     "grid-template-columns", "an automatic repeat needs fixed tracks"},
    {"grid-template-columns: auto repeat(auto-fit, 10px)", DiagnosticCode::value_wrong_kind, "repeat",
     "grid-template-columns", "an automatic repeat needs fixed tracks"},
    {"grid-template-columns: repeat(2.5, 1px)", DiagnosticCode::value_wrong_kind, "2.5", "grid-template-columns",
     "2.5"},
    {"grid-template-areas: x", DiagnosticCode::keyword_unknown, "x", "grid-template-areas", "x"},
    {"grid-row: 1.5", DiagnosticCode::value_wrong_kind, "1.5", "grid-row", "1.5"},
    {"display: 1px", DiagnosticCode::value_wrong_kind, "1px", "display", "1px"},
    // E403 value_out_of_range.
    {"padding: -1px", DiagnosticCode::value_out_of_range, "-1px", "padding", "-1px"},
    {"width: -5px", DiagnosticCode::value_out_of_range, "-5px", "width", "-5px"},
    {"gap: -2%", DiagnosticCode::value_out_of_range, "-2%", "gap", "-2%"},
    {"flex-shrink: -1", DiagnosticCode::value_out_of_range, "-1", "flex-shrink", "-1"},
    {"order: 3000000000", DiagnosticCode::value_out_of_range, "3000000000", "order", "3000000000"},
    {"grid-template-columns: -1fr", DiagnosticCode::value_out_of_range, "-1fr", "grid-template-columns", "-1fr"},
    {"grid-template-columns: repeat(10001, 1px)", DiagnosticCode::value_out_of_range, "10001",
     "grid-template-columns", "10001"},
    {"grid-template-columns: repeat(0, 1px)", DiagnosticCode::value_out_of_range, "0", "grid-template-columns", "0"},
    {"grid-template-columns: repeat(5000, 1px 1px) 2px", DiagnosticCode::value_out_of_range, "2px",
     "grid-template-columns", "more than 10000 tracks"},
    {"grid-row: 0", DiagnosticCode::value_out_of_range, "0", "grid-row", "0"},
    {"grid-column: span 0", DiagnosticCode::value_out_of_range, "0", "grid-column", "0"},
    {"width: calc(10px / 0)", DiagnosticCode::value_out_of_range, "/", "width", "division by zero"},
    // E404 unit_not_admitted.
    {"width: 2em", DiagnosticCode::unit_not_admitted, "2em", "width", "em"},
    {"margin: 1rem", DiagnosticCode::unit_not_admitted, "1rem", "margin", "rem"},
    {"height: 50vh", DiagnosticCode::unit_not_admitted, "50vh", "height", "vh"},
    {"gap: 1fr", DiagnosticCode::unit_not_admitted, "1fr", "gap", "fr"},
    {"width: calc(100% - 1em)", DiagnosticCode::unit_not_admitted, "1em", "width", "em"},
    // E405 value_form_not_admitted.
    {"display: inline-flex", DiagnosticCode::value_form_not_admitted, "inline-flex", "display", "inline-flex"},
    {"display: inline-grid", DiagnosticCode::value_form_not_admitted, "inline-grid", "display", "inline-grid"},
    {"display: contents", DiagnosticCode::value_form_not_admitted, "contents", "display", "contents"},
    {"display: table", DiagnosticCode::value_form_not_admitted, "table", "display", "table"},
    {"display: block flex", DiagnosticCode::value_form_not_admitted, "block", "display", "two-keyword display"},
    {"position: fixed", DiagnosticCode::value_form_not_admitted, "fixed", "position", "fixed"},
    {"position: sticky", DiagnosticCode::value_form_not_admitted, "sticky", "position", "sticky"},
    {"width: min(10px, 5%)", DiagnosticCode::value_form_not_admitted, "min(", "width", "min("},
    {"width: max(10px, 5%)", DiagnosticCode::value_form_not_admitted, "max(", "width", "max("},
    {"width: clamp(1px, 5%, 9px)", DiagnosticCode::value_form_not_admitted, "clamp(", "width", "clamp("},
    {"width: var(--x)", DiagnosticCode::value_form_not_admitted, "var(", "width", "var("},
    {"width: 10px !important", DiagnosticCode::value_form_not_admitted, "!", "width", "!important"},
    {"width: inherit", DiagnosticCode::value_form_not_admitted, "inherit", "width", "inherit"},
    {"display: initial", DiagnosticCode::value_form_not_admitted, "initial", "display", "initial"},
    {"box-sizing: content-box", DiagnosticCode::value_form_not_admitted, "content-box", "box-sizing", "content-box"},
    {"width: stretch", DiagnosticCode::value_form_not_admitted, "stretch", "width", "stretch"},
    {"width: fit-content", DiagnosticCode::value_form_not_admitted, "fit-content", "width", "fit-content"},
    {"flex-basis: min-content", DiagnosticCode::value_form_not_admitted, "min-content", "flex-basis", "min-content"},
    {"flex-basis: fit-content(10px)", DiagnosticCode::value_form_not_admitted, "fit-content", "flex-basis",
     "fit-content()"},
    {"flex-grow: calc(2)", DiagnosticCode::value_form_not_admitted, "calc", "flex-grow", "calc() of a number"},
    {"grid-template-columns: subgrid", DiagnosticCode::value_form_not_admitted, "subgrid", "grid-template-columns",
     "subgrid"},
    {"grid-template-rows: masonry", DiagnosticCode::value_form_not_admitted, "masonry", "grid-template-rows",
     "masonry"},
    {"grid-template-columns: [a] 10px", DiagnosticCode::value_form_not_admitted, "[", "grid-template-columns",
     "named grid lines"},
    {"grid-column: a 2", DiagnosticCode::value_form_not_admitted, "a 2", "grid-column", "named grid lines"},
    {"grid-column: span x", DiagnosticCode::value_form_not_admitted, "x", "grid-column", "named grid lines"},
    {"justify-content: left", DiagnosticCode::value_form_not_admitted, "left", "justify-content", "left"},
    {"align-content: baseline", DiagnosticCode::value_form_not_admitted, "baseline", "align-content", "baseline"},
    {"align-items: last", DiagnosticCode::value_form_not_admitted, "last", "align-items", "last"},
    {"justify-items: legacy", DiagnosticCode::value_form_not_admitted, "legacy", "justify-items", "legacy"},
    {"align-self: safe", DiagnosticCode::value_form_not_admitted, "safe", "align-self", "safe"},
    {"text-align: justify", DiagnosticCode::value_form_not_admitted, "justify", "text-align", "justify"},
    {"vertical-align: 10px", DiagnosticCode::value_form_not_admitted, "10px", "vertical-align",
     "a length in vertical-align"},
    {"vertical-align: text-top", DiagnosticCode::value_form_not_admitted, "text-top", "vertical-align", "text-top"},
    // E406 keyword_unknown.
    {"display: blok", DiagnosticCode::keyword_unknown, "blok", "display", "blok"},
    {"justify-content: baseline", DiagnosticCode::keyword_unknown, "baseline", "justify-content", "baseline"},
    {"padding: auto", DiagnosticCode::keyword_unknown, "auto", "padding", "auto"},
    {"max-width: auto", DiagnosticCode::keyword_unknown, "auto", "max-width", "auto"},
    {"min-width: none", DiagnosticCode::keyword_unknown, "none", "min-width", "none"},
    {"flex-grow: auto", DiagnosticCode::keyword_unknown, "auto", "flex-grow", "auto"},
    {"flex-direction: across", DiagnosticCode::keyword_unknown, "across", "flex-direction", "across"},
    {"flex-wrap: yes", DiagnosticCode::keyword_unknown, "yes", "flex-wrap", "yes"},
    {"grid-auto-flow: sideways", DiagnosticCode::keyword_unknown, "sideways", "grid-auto-flow", "sideways"},
    {"grid-template-columns: wide", DiagnosticCode::keyword_unknown, "wide", "grid-template-columns", "wide"},
    {"text-align: middle", DiagnosticCode::keyword_unknown, "middle", "text-align", "middle"},
    // Structure: E201, E202, E504.
    {"width 10px", DiagnosticCode::unexpected_token, "10px", "width", "10px"},
    {"10px: 1px", DiagnosticCode::unexpected_token, "10px", "", "10px"},
    {"width: {10px}", DiagnosticCode::unexpected_token, "{", "width", "{"},
    {"width: calc(10px", DiagnosticCode::unbalanced_block, "calc(", "width", "calc("},
    {"width: 10px)", DiagnosticCode::unbalanced_block, ")", "width", ")"},
    {"grid-template-columns: [a)", DiagnosticCode::unbalanced_block, ")", "grid-template-columns", ")"},
    // Lexical: the tokenizer's own diagnostic ends the read.
    {"width: \"10px", DiagnosticCode::unterminated_string, "\"", "", ""},
};

void test_refused_forms_have_exact_diagnostics() {
    LayoutStyleReader reader;
    ThemeDiagnosticList diagnostics;
    LayoutStyle original{};
    original.display = DisplayKind::block;
    for (const RefusedCase& refused : refused_cases) {
        diagnostics.clear();
        LayoutStyle style = original;
        const StyleReadOutcome outcome =
            reader.read_inline_style(ThemeSource{"inline:refused", refused.style}, style, diagnostics);
        const std::string where = "refused \"" + std::string(refused.style) + "\"";
        require(outcome == StyleReadOutcome::refused, where + " was applied");
        require(style == original, where + " changed the style");
        require(diagnostics.records().size() == 1U, where + " reported:" + describe_diagnostics(diagnostics));
        const ThemeDiagnostic& diagnostic = diagnostics.records()[0];
        const std::size_t at = refused.at.empty() ? refused.style.size() : refused.style.find(refused.at);
        require(at != std::string_view::npos, where + ": its marker is not in the style");
        const std::uint32_t column = static_cast<std::uint32_t>(at + 1U);
        require(diagnostic.code == refused.code, where + " reported" + describe_diagnostics(diagnostics));
        require(diagnostic.file == "inline:refused", where + " named another file");
        require(diagnostic.position.line.value == 1U && diagnostic.position.column.value == column,
                where + " at the wrong column:" + describe_diagnostics(diagnostics));
        if (refused.code != DiagnosticCode::unterminated_string) {
            require(diagnostic.property == refused.property, where + " named property \"" + diagnostic.property + "\"");
            require(diagnostic.detail == refused.detail, where + " detail \"" + diagnostic.detail + "\"");
        }
    }
}

void test_every_error_is_reported_and_nothing_applies() {
    LayoutStyleReader reader;
    ThemeDiagnosticList diagnostics;
    LayoutStyle style{};
    style.display = DisplayKind::flex;
    const LayoutStyle original = style;
    const StyleReadOutcome outcome = reader.read_inline_style(
        ThemeSource{"inline:two", "width: 2em; display: grid; height: -1px;\nmargin: 1vw"}, style, diagnostics);
    require(outcome == StyleReadOutcome::refused && style == original, "an error must refuse the whole style");
    require(diagnostics.error_count() == 3U, "every value error is reported:" + describe_diagnostics(diagnostics));
    require(diagnostics.records()[2].position.line.value == 2U && diagnostics.records()[2].position.column.value == 9U,
            "the third error is on line 2, column 9");
}

void test_warnings_drop_one_declaration() {
    LayoutStyleReader reader;
    ThemeDiagnosticList diagnostics;
    LayoutStyle style{};
    const StyleReadOutcome outcome = reader.read_inline_style(
        ThemeSource{"inline:warnings", "float: left; width: 5px; colour: red; --gap: 1px; -webkit-box-flex: 1; "
                                       "grid: auto / 1fr; height: 2px"},
        style, diagnostics);
    require(outcome == StyleReadOutcome::applied, "warnings must not refuse");
    require(diagnostics.error_count() == 0U && diagnostics.warning_count() == 5U,
            "five warnings:" + describe_diagnostics(diagnostics));
    const DiagnosticCode expected[] = {DiagnosticCode::property_not_admitted, DiagnosticCode::unknown_property,
                                       DiagnosticCode::unknown_property, DiagnosticCode::unknown_property,
                                       DiagnosticCode::property_not_admitted};
    const std::string_view properties[] = {"float", "colour", "--gap", "-webkit-box-flex", "grid"};
    for (std::size_t index = 0U; index < 5U; ++index) {
        const ThemeDiagnostic& diagnostic = diagnostics.records()[index];
        require(diagnostic.code == expected[index] && diagnostic.property == properties[index],
                "warning order:" + describe_diagnostics(diagnostics));
    }
    require(describe_layout_style(style, LayoutStyle{}) == "width: 5px; height: 2px",
            "the admitted declarations apply around the warnings");
}

void test_declaration_limit() {
    std::string text;
    for (std::size_t index = 0U; index < 64U; ++index) text += "width: 1px;";
    LayoutStyleReader reader;
    ThemeDiagnosticList diagnostics;
    LayoutStyle style{};
    require(reader.read_inline_style(ThemeSource{"inline:64", text}, style, diagnostics) == StyleReadOutcome::applied,
            "64 declarations are admitted");
    const std::size_t sixty_fifth = text.size();
    text += "height: 1px";
    style = LayoutStyle{};
    require(reader.read_inline_style(ThemeSource{"inline:65", text}, style, diagnostics) == StyleReadOutcome::refused,
            "65 declarations are refused");
    const ThemeDiagnostic& diagnostic = diagnostics.records().back();
    require(diagnostic.code == DiagnosticCode::too_many_declarations &&
                diagnostic.position.column.value == sixty_fifth + 1U && diagnostic.detail == "64",
            "E503 at the 65th declaration:" + describe_diagnostics(diagnostics));
}

void test_nesting_limit() {
    std::string text = "width: calc(";
    for (std::size_t index = 0U; index < 31U; ++index) text += '(';
    text += "1px";
    for (std::size_t index = 0U; index < 32U; ++index) text += ')';
    LayoutStyleReader reader;
    ThemeDiagnosticList diagnostics;
    LayoutStyle style{};
    require(reader.read_inline_style(ThemeSource{"inline:32", text}, style, diagnostics) == StyleReadOutcome::applied,
            "32 levels are admitted:" + describe_diagnostics(diagnostics));
    require(describe_layout_style(style, LayoutStyle{}) == "width: 1px", "nested calc() reads its value");
    text.insert(12U, "(");
    text += ')';
    style = LayoutStyle{};
    diagnostics.clear();
    require(reader.read_inline_style(ThemeSource{"inline:33", text}, style, diagnostics) == StyleReadOutcome::refused,
            "33 levels are refused");
    require(diagnostics.records().size() == 1U && diagnostics.records()[0].code == DiagnosticCode::nesting_too_deep,
            "E504 past 32 levels:" + describe_diagnostics(diagnostics));
}

void test_track_limits() {
    LayoutStyleReader reader;
    ThemeDiagnosticList diagnostics;
    LayoutStyle style{};
    require(reader.read_inline_style(ThemeSource{"inline:tracks", "grid-template-columns: repeat(10000, 1px)"}, style,
                                     diagnostics) == StyleReadOutcome::applied,
            "10000 tracks are admitted");
    require(style.grid.template_columns.tracks.size() == 10'000U, "repeat(10000) expands to 10000 tracks");
    // grid-template-areas' rows are explicit tracks: at most 10000. Its
    // columns are bounded by the tokenizer's string limit (4096 bytes).
    std::string row = "\"";
    for (std::size_t column = 0U; column < 2048U; ++column) row += column == 0U ? "a" : " a";
    row += '"';
    style = LayoutStyle{};
    require(reader.read_inline_style(ThemeSource{"inline:columns", "grid-template-areas: " + row}, style,
                                     diagnostics) == StyleReadOutcome::applied,
            "2048 columns are admitted:" + describe_diagnostics(diagnostics));
    require(style.grid.template_areas.columns == 2048U && style.grid.template_areas.areas.size() == 1U,
            "one area 2048 columns wide");
    const std::string wider = "grid-template-areas: " + row.substr(0U, row.size() - 1U) + " a\"";
    diagnostics.clear();
    require(reader.read_inline_style(ThemeSource{"inline:columns", wider}, style, diagnostics) ==
                StyleReadOutcome::refused,
            "a longer row string is refused");
    require(diagnostics.records()[0].code == DiagnosticCode::string_too_long,
            "the string limit is E505:" + describe_diagnostics(diagnostics));
    std::string rows = "grid-template-areas:";
    for (std::size_t line = 0U; line < 10'000U; ++line) rows += " \"a\"";
    style = LayoutStyle{};
    diagnostics.clear();
    require(reader.read_inline_style(ThemeSource{"inline:rows", rows}, style, diagnostics) ==
                StyleReadOutcome::applied,
            "10000 rows are admitted:" + describe_diagnostics(diagnostics));
    rows += " \"a\"";
    require(reader.read_inline_style(ThemeSource{"inline:rows", rows}, style, diagnostics) ==
                StyleReadOutcome::refused,
            "10001 rows are refused");
    require(diagnostics.records()[0].detail == "more than 10000 rows", "the row limit is E403");
}

void test_template_areas_are_rectangles_in_written_order() {
    LayoutStyleReader reader;
    ThemeDiagnosticList diagnostics;
    LayoutStyle style{};
    const StyleReadOutcome outcome = reader.read_inline_style(
        ThemeSource{"inline:areas", "grid-template-areas: \"z z a\" \"m m a\" \". . a\""}, style, diagnostics);
    require(outcome == StyleReadOutcome::applied, "a valid template is applied");
    const std::vector<gui_forms::GridNamedArea>& areas = style.grid.template_areas.areas;
    require(areas.size() == 3U, "three named areas");
    require(areas[0].name == "z" && areas[1].name == "a" && areas[2].name == "m", "areas in written order");
    require(areas[1].row_start == 0U && areas[1].row_end == 3U && areas[1].column_start == 2U &&
                areas[1].column_end == 3U,
            "area a spans rows 0 to 3 of column 2");
}

// -- the corpus -------------------------------------------------------------------

// Every `style="..."` attribute of the corpus's fixtures (the L0 reader's
// declarations and the flex and grid ones its fixtures already write) reads
// without a diagnostic. The fixtures use no character references in styles.
void test_corpus_styles_read_cleanly(const std::filesystem::path& corpus) {
    LayoutStyleReader reader;
    ThemeDiagnosticList diagnostics;
    std::size_t styles = 0U;
    for (const std::filesystem::directory_entry& entry :
         std::filesystem::recursive_directory_iterator(corpus / "fixtures")) {
        if (!entry.is_regular_file() || entry.path().extension() != ".html") continue;
        std::ifstream file(entry.path(), std::ios::binary);
        const std::string text{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
        std::size_t position = text.find("style=\"");
        while (position != std::string::npos) {
            const std::size_t start = position + 7U;
            const std::size_t end = text.find('"', start);
            require(end != std::string::npos, entry.path().string() + ": an unclosed style attribute");
            const std::string_view style_text = std::string_view(text).substr(start, end - start);
            require(style_text.find('&') == std::string_view::npos, entry.path().string() + ": a reference in a style");
            diagnostics.clear();
            LayoutStyle style{};
            const StyleReadOutcome outcome =
                reader.read_inline_style(ThemeSource{"inline:corpus", style_text}, style, diagnostics);
            require(outcome == StyleReadOutcome::applied && diagnostics.records().empty(),
                    entry.path().string() + ": \"" + std::string(style_text) + "\"" + describe_diagnostics(diagnostics));
            ++styles;
            position = text.find("style=\"", end);
        }
    }
    require(styles > 100U, "the corpus has its style attributes");
    std::cout << "layout-style-tests: corpus styles read: " << styles << '\n';
}

// -- mutation -----------------------------------------------------------------------

// Every accepted case with each byte deleted, and each byte replaced by
// each of a set of structural characters: every read terminates, and a
// refused read leaves the style exactly as it was.
void test_mutations_never_half_apply() {
    const std::string_view replacements = "();:\"-0/![{%. ";
    LayoutStyleReader reader;
    ThemeDiagnosticList diagnostics;
    LayoutStyle original{};
    original.display = DisplayKind::grid;
    original.grid.template_columns.tracks.resize(3U);
    std::string mutated;
    std::size_t reads = 0U;
    std::size_t refused = 0U;
    for (const AcceptedCase& accepted : accepted_cases) {
        const std::string_view source = accepted.style;
        for (std::size_t position = 0U; position < source.size(); ++position) {
            for (std::size_t variant = 0U; variant <= replacements.size(); ++variant) {
                mutated.assign(source.data(), source.size());
                if (variant == replacements.size()) {
                    mutated.erase(position, 1U);
                } else {
                    mutated[position] = replacements[variant];
                }
                diagnostics.clear();
                LayoutStyle style = original;
                const StyleReadOutcome outcome =
                    reader.read_inline_style(ThemeSource{"inline:mutation", mutated}, style, diagnostics);
                ++reads;
                if (outcome == StyleReadOutcome::refused) {
                    ++refused;
                    require(style == original, "a refused mutation changed the style: \"" + mutated + "\"");
                    require(diagnostics.error_count() > 0U, "a refusal without an error: \"" + mutated + "\"");
                } else {
                    require(diagnostics.error_count() == 0U, "an applied read with an error: \"" + mutated + "\"");
                }
            }
        }
    }
    std::cout << "layout-style-tests: mutations read: " << reads << ", refused: " << refused << '\n';
}

} // namespace

int main(int argument_count, char** arguments) {
    try {
        require(argument_count == 2, "usage: gui_forms_layout_style_tests <tests/layout_corpus>");
        const std::filesystem::path corpus(arguments[1]);
        test_every_property_has_a_case();
        test_accepted_forms_and_round_trip();
        test_refused_forms_have_exact_diagnostics();
        test_every_error_is_reported_and_nothing_applies();
        test_warnings_drop_one_declaration();
        test_declaration_limit();
        test_nesting_limit();
        test_track_limits();
        test_template_areas_are_rectangles_in_written_order();
        test_corpus_styles_read_cleanly(corpus);
        test_mutations_never_half_apply();
        std::cout << "layout-style-tests: pass\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "layout-style-tests: fail: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
