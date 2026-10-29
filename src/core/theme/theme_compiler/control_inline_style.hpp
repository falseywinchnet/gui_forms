#pragma once

#include "gui_forms/anatomy/types/anatomy_types.hpp"
#include "gui_forms/engine/theme_rules/theme_rules.hpp"
#include "gui_forms/theme/theme_compiler/theme_compiler.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

// Internal: a control's compiled inline style (T3 P9, R2-10), which the
// control owns and the cascade reads through PartFacts::inline_rules.

namespace gui_forms::detail {

struct ControlInlineStyle final {
    // The text as set.
    std::string text{};
    // Its rules, at the inline levels (CompiledThemeRules::compile_inline).
    CompiledThemeRules rules{};
};

// Compiles `text` as the inline style of a control of `anatomy`, its
// diagnostics naming `file` (`inline:<control>`), appended to
// `diagnostics`: the compiled style, or null where an error refuses it.
[[nodiscard]] std::shared_ptr<const ControlInlineStyle> compile_inline_style(
    std::string_view text, const ControlAnatomy& anatomy, std::string_view file,
    std::vector<ThemeCompileDiagnostic>& diagnostics);

} // namespace gui_forms::detail
