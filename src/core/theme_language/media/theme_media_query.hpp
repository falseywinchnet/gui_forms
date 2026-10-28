#pragma once

// The `@media` query reader (stage T3, PLAN.md P8, section 1.9, engine
// change EC3): an `@media` rule's prelude becomes one engine media
// condition (MediaCondition) a query, and the compiler lowers the block's
// rules once for each query.
//
// Admitted:
// - a query is an optional media type, `all` or `screen`, then features
//   joined by `and`, or only the features; an empty prelude is one
//   unconditional query, as CSS's empty list is;
// - a comma list of queries;
// - the features `prefers-color-scheme: light | dark`,
//   `forced-colors: active | none`,
//   `prefers-reduced-motion: reduce | no-preference`, and `resolution`,
//   `min-resolution` and `max-resolution` in `dppx` or `x`, above zero.
//   Names and keywords are ASCII case-insensitive.
// Errors: `not`, `only` and `or`; any other media type; a feature without
// a value, a range form, a nested group or a function (E405); a keyword
// the feature lacks (E406); a resolution unit other than dppx and x
// (E404); a resolution not above zero (E403); anything else out of place
// (E401). A feature the language does not know (W105) leaves its query
// unread: the compiler warns and lowers nothing for it, so a block whose
// only query names one is skipped, as CSS treats it as `not all`.
//
// A query naming one feature twice joins them: two resolution bounds keep
// the narrower, and two different keywords make a query no window meets,
// which lowers nothing.
//
// The reader reports nothing itself: it returns the queries, or the first
// problem.

#include "../diagnostics/theme_diagnostics.hpp"
#include "../tokenizer/theme_tokenizer.hpp"
#include "../syntax_tree/theme_syntax_tree.hpp"

#include "gui_forms/engine/theme_rules/theme_rules.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms::theme_language {

// One query of a list, read.
struct MediaQuery final {
    MediaCondition condition{};
    // A feature the language does not know: the query is not read and
    // lowers nothing. `unknown_token` is its name's token.
    bool unknown_feature{};
    std::uint32_t unknown_token{};
    // The query names one feature with two different keywords: no window
    // meets it.
    bool never{};
};

enum class MediaQueryOutcome : std::uint8_t { read, refused };

struct MediaQueryProblem final {
    DiagnosticCode code{DiagnosticCode::value_syntax};
    std::uint32_t token{};
    std::string detail{};
};

// Reads the queries of an `@media` prelude, `prelude` being the node's
// content range in `tokens`, made from `text`, into `queries` (replacing
// its contents; its capacity is kept). On a problem `queries` holds what
// was read before it.
[[nodiscard]] MediaQueryOutcome read_media_queries(std::string_view text, const ThemeTokenBuffer& tokens,
                                                   TokenRange prelude, std::vector<MediaQuery>& queries,
                                                   MediaQueryProblem& problem);

} // namespace gui_forms::theme_language
