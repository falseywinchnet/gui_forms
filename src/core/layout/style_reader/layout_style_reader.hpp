#pragma once

#include "../../theme_language/diagnostics/theme_diagnostics.hpp"
#include "../../theme_language/source_text/theme_source_text.hpp"
#include "../../theme_language/tokenizer/theme_tokenizer.hpp"

#include "gui_forms/layout/style/layout_style.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms::layout_style {

// The layout style reader (stage L, phase L2): the admitted CSS layout
// declarations of PLAN sections 2.2 to 2.5, read from an inline style (a
// form document's or a fixture's `style` attribute) over the theme
// language's tokens, into a LayoutStyle. Internal to the core, like the
// theme language (T3 decision E3).
//
// Diagnostics are the theme language's records and codes (T3 owns the code
// list; L defines none):
// - lexical defects, from the tokenizer (E1xx), which end the read;
// - a declaration that is not `<property>: <value>` (E201), an unclosed or
//   unmatched parenthesis or bracket (E202);
// - more than maximum_declarations_per_rule declarations (E503), and
//   values nested past maximum_nesting_depth (E504);
// - the value defects: E401 value_syntax, E402 value_wrong_kind, E403
//   value_out_of_range, E404 unit_not_admitted (every unit but `px`, and
//   `fr` in grid tracks), E405 value_form_not_admitted (a CSS form L leaves
//   out: `min()`, `max()`, `clamp()`, `var()`, named grid lines,
//   `!important`, the global keywords and the keywords each property lists),
//   E406 keyword_unknown;
// - warnings that drop one declaration: W101 unknown_property (a name L
//   does not know, custom properties and vendor prefixes included) and W107
//   property_not_admitted (a CSS layout property L leaves out, such as
//   `float` or the `grid` shorthand).
// An error refuses the whole style and leaves the caller's style as it was
// (T3 decision B2); the read continues to the end so every error is
// reported. Warnings drop their declaration and the rest applies (B3). As in
// CSS, a later declaration of a property replaces an earlier one, and
// property names and keywords match ASCII case-insensitively; area names
// match exactly.

enum class StyleReadOutcome : std::uint8_t {
    // Every admitted declaration was applied; warnings, if any, are in the
    // diagnostics.
    applied,
    // An error was found; the style is unchanged.
    refused,
};

// The track limit is gui_forms::maximum_grid_tracks (layout_style.hpp).
// grid-template-areas' rows are explicit tracks too, so it may write at
// most that many; its columns are bounded by the longest string the
// tokenizer admits (maximum_string_bytes, so 2,048 cells).

// One named cell of a grid-template-areas value, as read: its name borrows
// the source text for the read.
struct TemplateAreaCell final {
    std::string_view name{};
    std::uint32_t row{};
    std::uint32_t column{};
};

class LayoutStyleReader final {
public:
    // Applies the declarations of `source` on top of `style`. The source is
    // borrowed for the call; diagnostics are appended in the order found.
    [[nodiscard]] StyleReadOutcome read_inline_style(const theme_language::ThemeSource& source,
                                                     LayoutStyle& style,
                                                     theme_language::ThemeDiagnosticList& diagnostics);

private:
    // Scratch storage, reused from read to read (house style section 1).
    theme_language::ThemeTokenBuffer tokens_{};
    theme_language::LineStartTable lines_{};
    // One declaration's value: the positions in tokens_ of its tokens other
    // than whitespace and comments, and for each, the position in this list
    // of the bracket that closes it (its own position for a token that opens
    // nothing).
    std::vector<std::uint32_t> value_tokens_{};
    std::vector<std::uint32_t> value_closers_{};
    std::vector<TemplateAreaCell> template_cells_{};
    // The style the declarations are applied to; copied to the caller's
    // style only when the read finds no error.
    LayoutStyle scratch_{};
};

// The declarations whose values differ from `base`, in a fixed property
// order and in longhand form with CSS's spelling, separated by "; ". Tests
// and inspection read it; reading it back over `base` gives an equal style.
[[nodiscard]] std::string describe_layout_style(const LayoutStyle& style, const LayoutStyle& base);

} // namespace gui_forms::layout_style
