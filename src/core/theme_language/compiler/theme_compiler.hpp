#pragma once

// The theme compiler (stage T3, PLAN.md P5): a sheet's text in, the engine's
// rule records out. It runs the stages in order (tokenize, parse, collect
// the custom properties, then per style rule lower its selectors and its
// declarations) and lowers each declaration onto today's engine: the
// `primitive` a rule selects, that primitive's parameters by name, the
// presentational properties as one-state machines, and `mix-blend-mode`.
// The parser and the records between the stages stay internal (E3); the
// compiler is what the package loader (P9) and the tools call.

#include "../custom_properties/theme_custom_properties.hpp"
#include "../diagnostics/theme_diagnostics.hpp"
#include "../material/theme_material_lowering.hpp"
#include "../selector/theme_selector.hpp"
#include "../source_text/theme_source_text.hpp"
#include "../syntax_tree/theme_syntax_tree.hpp"
#include "../tokenizer/theme_tokenizer.hpp"
#include "../value/theme_value.hpp"

#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"
#include "gui_forms/engine/theme_rules/theme_rules.hpp"
#include "gui_forms/engine/types/engine_types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms::theme_language {

// What one compilation reads besides the sheet, all borrowed for the call:
// - `source`: the sheet's name and text;
// - `registry`: the primitives the rules may select;
// - `vocabulary`: the control types, parts and states selectors may name;
// - `published_values`: the names the theme's manifest publishes (R2-06),
//   which `var()` reads and which become the sheet's published values;
// - `resources`: the resource names the manifest lists, for `url()`;
// - `images`: those that are images, bound to the engine's images (P10's
//   loader binds them; until then the caller does);
// - `requires_engine`: the engine version the manifest declares (P9
//   supplies it; until then the caller does).
struct ThemeCompileInput final {
    const ThemeSource& source;
    const PrimitiveRegistry& registry;
    SelectorVocabulary vocabulary{};
    std::span<const std::string> published_values{};
    std::span<const std::string_view> resources{};
    std::span<const ThemeImageResource> images{};
    EngineVersion requires_engine{current_engine_version};
};

enum class CompileOutcome : std::uint8_t {
    // The sheet holds the lowered rules; warnings, if any, are diagnostics.
    compiled,
    // An error refused the theme (B2): the compilation stopped at it, so
    // it is the last diagnostic (the tokenizer reports every lexical one
    // before stopping), and the sheet is empty.
    refused,
};

// One kind of lowered declaration, for the cascade's last-wins rule within
// a rule: a later declaration of the same kind (and parameter) replaces an
// earlier one.
enum class LoweredKind : std::uint8_t { parameter, opacity, scale, translate, blend };

// A rule's declaration once lowered, kept until the rule's last
// declaration is read: the engine records it becomes, at most two
// (`translate` becomes one machine for each axis).
struct LoweredDeclaration final {
    LoweredKind kind{LoweredKind::parameter};
    // The parameter's position in the primitive's declarations (parameters
    // only).
    std::size_t parameter{};
    RuleDeclaration declaration{};
    std::array<PropertyTransition, 2> transitions{};
    std::size_t transition_count{};
};

// Compiles theme sheets. It keeps every stage's storage from sheet to
// sheet; one instance serves one compilation at a time.
class ThemeCompiler final {
public:
    // Replaces `sheet` with the lowered rules of `input.source` and appends
    // the diagnostics to `diagnostics`, in the order found. Each selector a
    // style rule expands into gives one rule, in written order, with the
    // rule's declarations; a rule left with nothing to declare gives none.
    // The first error stops the compilation (`refused`, `sheet` empty).
    // The sheet requires the declared engine version, or the running one
    // when the declared one is newer: what the running engine lacks has
    // then been dropped with a warning (R2-07), so the rest applies.
    [[nodiscard]] CompileOutcome compile(const ThemeCompileInput& input, ThemeRuleSheet& sheet,
                                         ThemeDiagnosticList& diagnostics);

private:
    ThemeTokenBuffer tokens_{};
    LineStartTable lines_{};
    ThemeSyntaxTree tree_{};
    CustomPropertyTable custom_properties_{};
    SelectorLowering selectors_{};
    VariableSubstitution substitution_{};
    ValueReader reader_{};
    std::vector<LoweredSelector> lowered_selectors_{};
    std::vector<ValueToken> substituted_{};
    ThemeValue value_{};
    std::vector<std::string_view> parameter_names_{};
    std::vector<LoweredDeclaration> declarations_{};
    std::vector<MaterialDeclaration> materials_{};
    MaterialLowering material_lowering_{};
    std::vector<MaterialField> material_fields_{};
    std::vector<RecipeDeclaration> recipe_fields_{};
    std::string folded_{};
};

} // namespace gui_forms::theme_language
