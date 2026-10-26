#pragma once

#include "../diagnostics/theme_diagnostics.hpp"
#include "../source_text/theme_source_text.hpp"
#include "../syntax_tree/theme_syntax_tree.hpp"
#include "../tokenizer/theme_tokenizer.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms::theme_language {

// The derived custom-property limits (PLAN.md section 6; R2-14 starting
// values): distinct custom property names in one sheet, and the tokens a
// value holds once every var() in it is substituted.
inline constexpr std::size_t maximum_custom_properties = 1024U;
inline constexpr std::size_t maximum_substituted_tokens = 4096U;

// One definition, `--name: <tokens>`: the name as written (with its `--`),
// the value's tokens, the declaration's node, and the name's position among
// the sheet's distinct names in name order (from 0 to name_count() - 1).
struct CustomPropertyDefinition final {
    std::string_view name{};
    TokenRange value{};
    NodeIndex declaration{};
    std::uint32_t name_index{};

    friend bool operator==(const CustomPropertyDefinition& left, const CustomPropertyDefinition& right) noexcept {
        return left.name == right.name && left.value == right.value && left.declaration == right.declaration &&
               left.name_index == right.name_index;
    }
};

// The names a theme reads but may not define (R2-06): the engine inputs,
// written `--pointer-x` and so on, and the values the application publishes
// that the theme's manifest lists, written `--<name>`. Borrowed.
struct ReservedInputs final {
    std::span<const std::string> published_values{};
};

// Where a reserved name reads from: an engine input, a published value, or
// neither (the name is not reserved).
enum class ReservedInputKind : std::uint8_t { none, engine_input, published_value };
struct ReservedInput final {
    ReservedInputKind kind{ReservedInputKind::none};
    // The EngineInput, or the published value's position in the list.
    std::uint32_t index{};
};

// The reserved input `name` (written with its `--`) names, if any.
[[nodiscard]] ReservedInput find_reserved_input(std::string_view name, const ReservedInputs& reserved) noexcept;

// A sheet's custom property definitions (PLAN.md section 1.6, R2-05),
// collected once per compilation and looked up by name. It keeps its
// storage from sheet to sheet.
class CustomPropertyTable final {
public:
    // Collects every custom property declared in a style rule of `tree`
    // (inside @media too; @keyframes blocks define none). Defining a
    // reserved input is an error (E701), and so is a sheet of more than
    // maximum_custom_properties distinct names (E511). Returns false after
    // an error, the table then empty. `tokens`, `tree` and `source` are
    // borrowed until the table is cleared or collects again.
    [[nodiscard]] bool collect(const ThemeSource& source, const ThemeTokenBuffer& tokens, const ThemeSyntaxTree& tree,
                               const ReservedInputs& reserved, SourcePositionCursor& cursor,
                               ThemeDiagnosticList& diagnostics);
    void clear() noexcept;

    // The definitions of `name` (written with its `--`), in document order.
    [[nodiscard]] std::span<const CustomPropertyDefinition> definitions_of(std::string_view name) const& noexcept;
    std::span<const CustomPropertyDefinition> definitions_of(std::string_view name) const&& = delete;
    [[nodiscard]] std::size_t name_count() const noexcept;

private:
    // Sorted by name, then document order.
    std::vector<CustomPropertyDefinition> definitions_{};
    std::size_t name_count_{};
};

// One token of a value once its var()s are substituted: a token of the
// sheet, or a reserved input that a var() read (`token` is then the var()'s
// function token, for diagnostics).
enum class ValueTokenSource : std::uint8_t { sheet, engine_input, published_value };
struct ValueToken final {
    ValueTokenSource source{ValueTokenSource::sheet};
    std::uint32_t token{};
    // The EngineInput or published value position, for a reserved input.
    std::uint32_t input{};

    friend bool operator==(ValueToken left, ValueToken right) noexcept {
        return left.source == right.source && left.token == right.token && left.input == right.input;
    }
};

// What a substitution reads, all borrowed for the call.
struct SubstitutionInput final {
    const ThemeSource& source;
    const ThemeTokenBuffer& tokens;
    const CustomPropertyTable& table;
    const ReservedInputs& reserved;
    SourcePositionCursor& cursor;
};

enum class SubstitutionOutcome : std::uint8_t { substituted, refused };

// Substitutes the var()s of declaration values (PLAN.md section 1.6). It
// keeps its working storage from value to value.
class VariableSubstitution final {
public:
    // Fills `output` (replacing its contents) with the tokens of `value`,
    // each `var(--name)` or `var(--name, fallback)` replaced by the name's
    // value (substituted in turn), by a reserved input, or by the fallback
    // when no rule defines the name. Whitespace and comments are left out:
    // tokens already separate the components. Until P12 resolves custom
    // properties per part, a name with more than one definition is refused
    // with the temporary not_yet_built (E999), so that no interim meaning
    // ships. Errors: a name undefined without a fallback (E409), a cycle
    // (E410), a malformed var() (E401), and a substitution that passes over
    // more than maximum_substituted_tokens tokens (E512). Every token it
    // passes over counts once: whitespace and comments, each var()'s own
    // tokens and a fallback it skips. So its work stays within a constant
    // of the bound however the definitions nest. On `refused`, `output` is
    // empty.
    [[nodiscard]] SubstitutionOutcome substitute(const SubstitutionInput& input, TokenRange value,
                                                 std::vector<ValueToken>& output, ThemeDiagnosticList& diagnostics);

    // One text being read: the rest of its tokens, and, when it is a
    // definition's value (`defines`), the position of the name it defines,
    // active until the text ends.
    struct Frame final {
        std::uint32_t cursor{};
        std::uint32_t end{};
        bool defines{};
        std::uint32_t name_index{};
    };

private:
    // Counts `added` more tokens visited; false, after reporting E512 at
    // `token`, past the bound.
    [[nodiscard]] bool visit(const SubstitutionInput& input, std::size_t added, std::uint32_t token,
                             ThemeDiagnosticList& diagnostics);

    std::vector<Frame> frames_{};
    // One flag per distinct name: set while its definition is being read,
    // so a cycle is found without searching the open frames.
    std::vector<std::uint8_t> active_{};
    std::size_t visited_{};
};

} // namespace gui_forms::theme_language
