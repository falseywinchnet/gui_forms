// The theme compiler (PLAN.md P5). One pass over the tree's nodes in
// document order: each style rule's selectors are lowered, then its
// `primitive` declarations (the last usable one selects the rule's
// primitive, which its parameters need), then its other declarations, each
// replacing an earlier one of the same engine property, as in CSS. Each
// expanded selector then gets one copy of the rule's records, counted
// against the lowered-size budget, so expansion builds a bounded amount.

#include "theme_compiler.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <string>
#include <utility>

namespace gui_forms::theme_language {

namespace {

// -- the property tables (PLAN.md section 1.7) ------------------------------------

// Known CSS properties the language leaves out (W107): layout belongs to
// forms and stage L, typography to the text stage, and geometry to the
// engine. Sorted, for binary_search. A property in one of the families
// below is left out too.
constexpr std::array<std::string_view, 147> not_admitted_properties{
    "accent-color", "align-content", "align-items", "align-self", "appearance", "aspect-ratio",
    "backface-visibility", "block-size", "bottom", "box-sizing", "caret-color", "clear", "clip", "clip-path",
    "column-count", "column-fill", "column-gap", "column-rule", "column-span", "column-width", "columns", "contain",
    "content", "content-visibility", "counter-increment", "counter-reset", "cursor", "direction", "display", "flex",
    "flex-basis", "flex-direction", "flex-flow", "flex-grow", "flex-shrink", "flex-wrap", "float", "font-family",
    "font-feature-settings", "font-kerning", "font-size", "font-stretch", "font-style", "font-variant", "font-weight",
    "gap", "grid", "grid-area", "grid-auto-columns", "grid-auto-flow", "grid-auto-rows", "grid-column",
    "grid-column-end", "grid-column-start", "grid-row", "grid-row-end", "grid-row-start", "grid-template",
    "grid-template-areas", "grid-template-columns", "grid-template-rows", "height", "hyphens", "inline-size", "inset",
    "inset-block", "inset-inline", "justify-content", "justify-items", "justify-self", "left", "letter-spacing",
    "line-height", "list-style", "margin", "margin-block", "margin-bottom", "margin-inline", "margin-left",
    "margin-right", "margin-top", "max-block-size", "max-height", "max-inline-size", "max-width", "min-block-size",
    "min-height", "min-inline-size", "min-width", "object-fit", "object-position", "order", "overflow",
    "overflow-wrap", "overflow-x", "overflow-y", "padding", "padding-block", "padding-bottom", "padding-inline",
    "padding-left", "padding-right", "padding-top", "perspective", "perspective-origin", "place-content",
    "place-items", "place-self", "pointer-events", "position", "quotes", "resize", "right", "rotate", "row-gap",
    "scroll-behavior", "tab-size", "table-layout", "text-align", "text-decoration", "text-decoration-color",
    "text-decoration-line", "text-decoration-style", "text-decoration-thickness", "text-indent", "text-overflow",
    "text-transform", "text-underline-offset", "text-wrap", "top", "transform", "transform-box", "transform-origin",
    "transform-style", "unicode-bidi", "user-select", "vertical-align", "visibility", "white-space", "width",
    "will-change", "word-break", "word-spacing", "word-wrap", "writing-mode", "z-index", "zoom",
};
static_assert(std::is_sorted(not_admitted_properties.begin(), not_admitted_properties.end()),
              "binary_search needs the table sorted");

// Families of left-out properties: the name, or the name and a '-'.
constexpr std::array<std::string_view, 12> not_admitted_families{
    "column", "flex",   "font",       "grid",   "inset",          "margin",
    "overflow", "overscroll", "padding", "scroll", "text-decoration", "transform",
};

// The properties of P6d to P8 (the rest of the authored materials, motion
// and the colour and font roles): refused with not_yet_built until their
// phases, so that no interim meaning ships. Sorted. A family's members the
// material lowering lists (P6b, P6c) are lowered before these are consulted.
constexpr std::array<std::string_view, 4> later_properties{
    "backdrop-filter", "color", "font", "text-shadow",
};
static_assert(std::is_sorted(later_properties.begin(), later_properties.end()), "binary_search needs the table sorted");

constexpr std::array<std::string_view, 6> later_families{
    "animation", "background", "border", "mask", "outline", "transition",
};

constexpr std::array<std::string_view, 3> blend_modes{"normal", "multiply", "screen"};

// The primitive a rule of material fields selects when it names none
// (owner C7).
constexpr std::string_view material_primitive_name = "material";

[[nodiscard]] bool listed(std::span<const std::string_view> sorted, std::string_view name) noexcept {
    const bool found = std::binary_search(sorted.begin(), sorted.end(), name);
    return found;
}

// Whether `name` is `family` or starts with `family` and a '-'.
[[nodiscard]] bool in_family(std::span<const std::string_view> families, std::string_view name) noexcept {
    for (const std::string_view family : families) {
        if (name == family) return true;
        const bool prefixed =
            name.size() > family.size() && name.substr(0U, family.size()) == family && name[family.size()] == '-';
        if (prefixed) return true;
    }
    return false;
}

enum class PropertyClass : std::uint8_t {
    custom,
    primitive,
    opacity,
    scale,
    translate,
    blend,
    isolation,
    // A material property (P6b): its fields are lowered by the material
    // lowering.
    material,
    later,
    not_admitted,
    // A primitive parameter, or an unknown name.
    other,
};

// `name` is the property in lower case.
[[nodiscard]] PropertyClass classify(std::string_view name) noexcept {
    if (name.substr(0U, 2U) == "--") return PropertyClass::custom;
    if (name == primitive_property) return PropertyClass::primitive;
    if (name == "opacity") return PropertyClass::opacity;
    if (name == "scale") return PropertyClass::scale;
    if (name == "translate") return PropertyClass::translate;
    if (name == "mix-blend-mode") return PropertyClass::blend;
    if (name == "isolation") return PropertyClass::isolation;
    if (material_property(name)) return PropertyClass::material;
    if (listed(later_properties, name) || in_family(later_families, name)) return PropertyClass::later;
    if (listed(not_admitted_properties, name) || in_family(not_admitted_families, name)) {
        return PropertyClass::not_admitted;
    }
    return PropertyClass::other;
}

// The CSS-wide keywords: `initial` is the built-in recipe's value (the
// primitive's default); `inherit` is admitted only on colour and font (P8);
// `unset`, `revert` and `revert-layer` are outside the language.
enum class WideKeyword : std::uint8_t { none, initial, inherit, outside };

void fold_ascii(std::string_view text, std::string& output) {
    output.assign(text);
    for (char& character : output) {
        if (character >= 'A' && character <= 'Z') character = static_cast<char>(character - 'A' + 'a');
    }
}

// A machine holding `target` in every state, reached at once: a static
// presentational value (CSS_COVERAGE section 4).
[[nodiscard]] PropertyTransition still_machine(std::string_view property, double target) {
    PropertyTransition machine{};
    machine.property = std::string(property);
    PresentationState state{};
    state.target = target;
    machine.states.push_back(std::move(state));
    machine.curve = MotionCurve::timed(FrameInterval{}, MotionEasing::linear);
    return machine;
}

[[nodiscard]] bool paints_role(const PrimitiveDescriptor& descriptor, PartRole role) noexcept {
    for (const PartRole painted : descriptor.roles) {
        if (painted == role) return true;
    }
    return false;
}

[[nodiscard]] std::size_t declaration_bytes(const RuleDeclaration& declaration) noexcept {
    const std::size_t bytes = lowered_record_bytes + declaration.property.size() + declaration.name.size() +
                              declaration.program.size() * sizeof(ExpressionInstruction);
    return bytes;
}

// A material declaration's lowered bytes: a record for it and for each of
// its layers and gradients, and each stop's bytes.
[[nodiscard]] std::size_t material_bytes(const MaterialDeclaration& declaration) noexcept {
    std::size_t records = 1U + declaration.fills.size() + declaration.gradients.size();
    std::size_t stops = 0U;
    for (const MaterialFillLayer& layer : declaration.fills) stops += layer.stops.size();
    for (const AuthoredGradient& gradient : declaration.gradients) stops += gradient.stops.size();
    records += declaration.blend_modes.size();
    const std::size_t bytes = records * lowered_record_bytes + stops * sizeof(AuthoredGradientStop);
    return bytes;
}

[[nodiscard]] std::size_t transition_bytes(const PropertyTransition& transition) noexcept {
    const std::size_t bytes =
        lowered_record_bytes + transition.property.size() + transition.states.size() * lowered_record_bytes;
    return bytes;
}

// The rule's primitive, as its `primitive` declarations select it.
struct PrimitiveSelection final {
    // Whether a usable declaration selected one: `primitive` is null for
    // the built-in recipe.
    bool selects{};
    const EnginePrimitive* primitive{};
    // Whether a declaration naming one was dropped (an unknown or unusable
    // primitive): parameters no selected primitive declares are then
    // dropped with it, without a warning each.
    bool dropped{};
};

// One compilation.
class CompilePass final {
public:
    CompilePass(const ThemeCompileInput& input, ThemeRuleSheet& sheet, ThemeDiagnosticList& diagnostics,
                ThemeTokenBuffer& tokens, LineStartTable& lines, ThemeSyntaxTree& tree,
                CustomPropertyTable& custom_properties, SelectorLowering& selectors,
                VariableSubstitution& substitution, ValueReader& reader,
                std::vector<LoweredSelector>& lowered_selectors, std::vector<ValueToken>& substituted,
                ThemeValue& value, std::vector<std::string_view>& parameter_names,
                std::vector<LoweredDeclaration>& declarations, std::vector<MaterialDeclaration>& materials,
                MaterialLowering& material_lowering, std::vector<MaterialField>& material_fields,
                std::string& folded) noexcept;

    [[nodiscard]] CompileOutcome run();

private:
    [[nodiscard]] bool lower_rule(std::size_t rule_index);
    [[nodiscard]] bool select_primitive(const SyntaxNode& node, PrimitiveSelection& selection);
    [[nodiscard]] bool lower_declaration(const SyntaxNode& node, PropertyClass kind,
                                         const PrimitiveSelection& selection);
    [[nodiscard]] bool lower_parameter(const SyntaxNode& node, const PrimitiveDescriptor& descriptor,
                                       std::size_t parameter);
    [[nodiscard]] bool lower_opacity_or_scale(const SyntaxNode& node, PropertyClass kind);
    [[nodiscard]] bool lower_translate(const SyntaxNode& node);
    [[nodiscard]] bool lower_keyword_property(const SyntaxNode& node, PropertyClass kind);
    [[nodiscard]] bool lower_material(const SyntaxNode& node);
    void add_material(MaterialDeclaration declaration);
    [[nodiscard]] bool emit_rule(const PrimitiveSelection& selection, std::uint32_t at);

    // Substitutes and reads the declaration's value into value_, the
    // parameters being parameter_names_; false after an error.
    [[nodiscard]] bool read_value(const SyntaxNode& node);
    [[nodiscard]] WideKeyword wide_keyword() const noexcept;
    // The first generic function in value_, reported as unknown_function
    // (the declaration is dropped); false when there is none.
    [[nodiscard]] bool drop_unknown_function();
    // A plain number or a percentage of value_'s only component, as a
    // fraction; reports and returns false otherwise.
    [[nodiscard]] bool fraction_of(const ValueComponent& component, double& fraction);
    [[nodiscard]] bool keyword_of(const ValueComponent& component, std::string_view expected);
    void add_lowered(LoweredDeclaration lowered);
    [[nodiscard]] std::string_view token_text(std::uint32_t index) const noexcept;
    // The text from the range's first token through its last; empty for an
    // empty range.
    [[nodiscard]] std::string_view range_text(TokenRange range) const noexcept;
    void report(DiagnosticCode code, std::uint32_t token, std::string detail);

    const ThemeCompileInput& input_;
    ThemeRuleSheet& sheet_;
    ThemeDiagnosticList& diagnostics_;
    ThemeTokenBuffer& tokens_;
    LineStartTable& lines_;
    ThemeSyntaxTree& tree_;
    CustomPropertyTable& custom_properties_;
    SelectorLowering& selectors_;
    VariableSubstitution& substitution_;
    ValueReader& reader_;
    std::vector<LoweredSelector>& lowered_selectors_;
    std::vector<ValueToken>& substituted_;
    ThemeValue& value_;
    std::vector<std::string_view>& parameter_names_;
    std::vector<LoweredDeclaration>& declarations_;
    std::vector<MaterialDeclaration>& materials_;
    MaterialLowering& material_lowering_;
    std::vector<MaterialField>& material_fields_;
    std::string& folded_;
    SourcePositionCursor cursor_;
    ReservedInputs reserved_{};
    LoweringBudget budget_{};
    bool newer_engine_{};
    // The rule and the property being lowered, as diagnostics name them.
    std::string rule_{};
    std::string property_{};
};

CompilePass::CompilePass(const ThemeCompileInput& input, ThemeRuleSheet& sheet, ThemeDiagnosticList& diagnostics,
                         ThemeTokenBuffer& tokens, LineStartTable& lines, ThemeSyntaxTree& tree,
                         CustomPropertyTable& custom_properties, SelectorLowering& selectors,
                         VariableSubstitution& substitution, ValueReader& reader,
                         std::vector<LoweredSelector>& lowered_selectors, std::vector<ValueToken>& substituted,
                         ThemeValue& value, std::vector<std::string_view>& parameter_names,
                         std::vector<LoweredDeclaration>& declarations, std::vector<MaterialDeclaration>& materials,
                         MaterialLowering& material_lowering, std::vector<MaterialField>& material_fields,
                         std::string& folded) noexcept
    : input_(input), sheet_(sheet), diagnostics_(diagnostics), tokens_(tokens), lines_(lines), tree_(tree),
      custom_properties_(custom_properties), selectors_(selectors), substitution_(substitution), reader_(reader),
      lowered_selectors_(lowered_selectors), substituted_(substituted), value_(value),
      parameter_names_(parameter_names), declarations_(declarations), materials_(materials),
      material_lowering_(material_lowering), material_fields_(material_fields), folded_(folded),
      cursor_(lines, input.source.text) {}

CompileOutcome CompilePass::run() {
    const TokenizeOutcome tokenized = tokenize_theme_sheet(input_.source, tokens_, lines_, diagnostics_);
    if (tokenized != TokenizeOutcome::tokens_cover_sheet) return CompileOutcome::refused;
    const ParseOutcome parsed = parse_theme_sheet(input_.source, tokens_, lines_, tree_, diagnostics_);
    if (parsed != ParseOutcome::tree_complete) return CompileOutcome::refused;
    newer_engine_ = current_engine_version < input_.requires_engine;
    if (newer_engine_) {
        const std::string version =
            std::to_string(input_.requires_engine.major) + "." + std::to_string(input_.requires_engine.minor);
        report(DiagnosticCode::needs_newer_engine, 0U, version);
    }
    reserved_.published_values = input_.published_values;
    if (!custom_properties_.collect(input_.source, tokens_, tree_, reserved_, cursor_, diagnostics_)) {
        return CompileOutcome::refused;
    }
    parameter_names_.reserve(maximum_primitive_parameters);
    const std::vector<SyntaxNode>& nodes = tree_.nodes;
    std::size_t index = 1U;
    while (index < nodes.size()) {
        const SyntaxNode& node = nodes[index];
        const std::size_t next = node.subtree_end.value;
        if (node.kind == SyntaxNodeKind::import_rule || node.kind == SyntaxNodeKind::media_rule ||
            node.kind == SyntaxNodeKind::keyframes_rule) {
            // P10 builds @import, P7 @media and @keyframes.
            const std::uint32_t keyword = node.tokens.first.value;
            report(DiagnosticCode::not_yet_built, keyword, clipped_theme_detail(token_text(keyword)));
            return CompileOutcome::refused;
        }
        if (node.kind == SyntaxNodeKind::style_rule && !lower_rule(index)) return CompileOutcome::refused;
        // An unknown at-rule was reported by the parser and is skipped.
        index = next;
    }
    sheet_.requires_engine = newer_engine_ ? current_engine_version : input_.requires_engine;
    sheet_.published_values.assign(input_.published_values.begin(), input_.published_values.end());
    return CompileOutcome::compiled;
}

bool CompilePass::lower_rule(std::size_t rule_index) {
    const SyntaxNode& rule = tree_.nodes[rule_index];
    const std::string_view written = range_text(rule.content);
    rule_ = clipped_theme_detail(written);
    property_.clear();
    lowered_selectors_.clear();
    const SelectorInput selector_input{.source = input_.source,
                                       .tokens = tokens_,
                                       .selectors = rule.content,
                                       .rule_label = written,
                                       .cursor = cursor_};
    const SelectorOutcome selected =
        selectors_.lower(selector_input, input_.vocabulary, budget_, lowered_selectors_, diagnostics_);
    if (selected == SelectorOutcome::refused) return false;
    // A dropped rule (an unknown name, reported) declares nothing.
    if (selected == SelectorOutcome::dropped) return true;

    const std::size_t end = rule.subtree_end.value;
    PrimitiveSelection selection{};
    for (std::size_t child = rule_index + 1U; child < end; child = tree_.nodes[child].subtree_end.value) {
        const SyntaxNode& node = tree_.nodes[child];
        const std::uint32_t property = node.tokens.first.value;
        property_ = clipped_theme_detail(token_text(property));
        if ((node.flags & syntax_flag_important) != 0U) {
            // P9 builds the important levels.
            report(DiagnosticCode::not_yet_built, property, "!important");
            return false;
        }
        fold_ascii(token_text(property), folded_);
        if (classify(folded_) == PropertyClass::primitive && !select_primitive(node, selection)) return false;
    }
    declarations_.clear();
    materials_.clear();
    for (std::size_t child = rule_index + 1U; child < end; child = tree_.nodes[child].subtree_end.value) {
        const SyntaxNode& node = tree_.nodes[child];
        const std::uint32_t property = node.tokens.first.value;
        property_ = clipped_theme_detail(token_text(property));
        fold_ascii(token_text(property), folded_);
        const PropertyClass kind = classify(folded_);
        if (kind == PropertyClass::custom || kind == PropertyClass::primitive) continue;
        if (!lower_declaration(node, kind, selection)) return false;
    }
    property_.clear();
    return emit_rule(selection, rule.content.first.value);
}

bool CompilePass::select_primitive(const SyntaxNode& node, PrimitiveSelection& selection) {
    parameter_names_.clear();
    if (!read_value(node)) return false;
    const WideKeyword wide = wide_keyword();
    const ValueComponent& first = value_.components[0];
    if (wide == WideKeyword::inherit || wide == WideKeyword::outside) {
        const DiagnosticCode code =
            wide == WideKeyword::inherit ? DiagnosticCode::value_wrong_kind : DiagnosticCode::value_form_not_admitted;
        report(code, first.token, clipped_theme_detail(first.text));
        return false;
    }
    if (wide == WideKeyword::initial) {
        selection.selects = true;
        selection.primitive = nullptr;
        return true;
    }
    if (drop_unknown_function()) {
        selection.dropped = true;
        return true;
    }
    if (value_.components.size() != 1U || first.kind != ValueComponentKind::keyword) {
        report(DiagnosticCode::value_wrong_kind, first.token, "a primitive's name");
        return false;
    }
    fold_ascii(first.text, folded_);
    if (folded_ == "built-in") {
        selection.selects = true;
        selection.primitive = nullptr;
        return true;
    }
    const std::string registered = registered_name_from_kebab(folded_);
    const EnginePrimitive* const primitive = registered.empty() ? nullptr : input_.registry.find(registered);
    if (primitive == nullptr) {
        report(DiagnosticCode::unknown_primitive, first.token, clipped_theme_detail(first.text));
        selection.dropped = true;
        return true;
    }
    const PrimitiveDescriptor& descriptor = (*primitive).descriptor();
    if (descriptor.cost.per_device_pixel == 0U) {
        report(DiagnosticCode::primitive_cost_unbounded, first.token, std::string(descriptor.name));
        selection.dropped = true;
        return true;
    }
    if (descriptor.cost.per_device_pixel > maximum_operations_per_device_pixel) {
        report(DiagnosticCode::primitive_over_budget, first.token,
               std::to_string(descriptor.cost.per_device_pixel));
        selection.dropped = true;
        return true;
    }
    for (const LoweredSelector& lowered : lowered_selectors_) {
        const std::optional<PartRole> role = lowered.selector.role;
        if (role && !paints_role(descriptor, *role)) {
            // R2-08: a primitive that cannot paint the selected role.
            report(DiagnosticCode::value_wrong_kind, first.token,
                   "the primitive does not paint the role " + std::string(part_role_name(*role)));
            return false;
        }
    }
    selection.selects = true;
    selection.primitive = primitive;
    return true;
}

bool CompilePass::lower_declaration(const SyntaxNode& node, PropertyClass kind, const PrimitiveSelection& selection) {
    const std::uint32_t property = node.tokens.first.value;
    switch (kind) {
    case PropertyClass::opacity:
    case PropertyClass::scale: return lower_opacity_or_scale(node, kind);
    case PropertyClass::translate: return lower_translate(node);
    case PropertyClass::blend:
    case PropertyClass::isolation: return lower_keyword_property(node, kind);
    case PropertyClass::material: return lower_material(node);
    case PropertyClass::later:
        report(DiagnosticCode::not_yet_built, property, property_);
        return false;
    case PropertyClass::not_admitted:
        report(DiagnosticCode::property_not_admitted, property, property_);
        return true;
    case PropertyClass::custom:
    case PropertyClass::primitive:
    case PropertyClass::other: break;
    }
    if (selection.primitive != nullptr) {
        const PrimitiveDescriptor& descriptor = (*selection.primitive).descriptor();
        const std::string registered = registered_name_from_kebab(folded_);
        const std::optional<std::size_t> parameter =
            registered.empty() ? std::optional<std::size_t>{} : find_parameter(descriptor, registered);
        if (parameter) return lower_parameter(node, descriptor, *parameter);
    }
    // The parameters of a dropped primitive go with it.
    if (!selection.selects && selection.dropped) return true;
    report(DiagnosticCode::unknown_property, property, property_);
    return true;
}

bool CompilePass::lower_parameter(const SyntaxNode& node, const PrimitiveDescriptor& descriptor,
                                  std::size_t parameter) {
    parameter_names_.clear();
    for (const PrimitiveParameterDeclaration& declared : descriptor.parameters) {
        parameter_names_.push_back(declared.name);
    }
    if (!read_value(node)) return false;
    const PrimitiveParameterDeclaration& declared = descriptor.parameters[parameter];
    LoweredDeclaration lowered{};
    lowered.kind = LoweredKind::parameter;
    lowered.parameter = parameter;
    const ValueComponent& first = value_.components[0];
    const WideKeyword wide = wide_keyword();
    if (wide == WideKeyword::inherit || wide == WideKeyword::outside) {
        const DiagnosticCode code =
            wide == WideKeyword::inherit ? DiagnosticCode::value_wrong_kind : DiagnosticCode::value_form_not_admitted;
        report(code, first.token, clipped_theme_detail(first.text));
        return false;
    }
    if (wide == WideKeyword::initial) {
        if (declared.kind == PrimitiveParameterKind::number) {
            lowered.declaration = RuleDeclaration::number_value(declared.name, declared.default_number);
        } else if (declared.kind == PrimitiveParameterKind::colour) {
            lowered.declaration = RuleDeclaration::colour_value(declared.name, declared.default_colour);
        } else {
            const std::size_t choice = static_cast<std::size_t>(declared.default_number);
            lowered.declaration = RuleDeclaration::choice_value(declared.name, declared.choices[choice]);
        }
        add_lowered(std::move(lowered));
        return true;
    }
    if (drop_unknown_function()) return true;
    if (value_.components.size() != 1U) {
        report(DiagnosticCode::value_syntax, value_.components[1].token, "one value");
        return false;
    }
    if (declared.kind == PrimitiveParameterKind::number) {
        const bool length_or_number = first.unit == ValueUnit::none || first.unit == ValueUnit::px;
        if (first.kind == ValueComponentKind::number && length_or_number) {
            if (first.number < declared.minimum || first.number > declared.maximum) {
                report(DiagnosticCode::value_out_of_range, first.token, clipped_theme_detail(token_text(first.token)));
                return false;
            }
            lowered.declaration = RuleDeclaration::number_value(declared.name, first.number);
            add_lowered(std::move(lowered));
            return true;
        }
        if (first.kind == ValueComponentKind::expression) {
            const ValueProgram program = value_.programs[first.program];
            const bool program_unit = program.unit == ValueUnit::none || program.unit == ValueUnit::px;
            if (!program_unit) {
                report(DiagnosticCode::value_wrong_kind, first.token, "a number or a length");
                return false;
            }
            const std::span<const ExpressionInstruction> instructions = value_.program_instructions(program);
            for (const ExpressionInstruction instruction : instructions) {
                if (instruction.operation != ExpressionOperation::parameter) continue;
                const PrimitiveParameterKind read = descriptor.parameters[instruction.index].kind;
                if (read != PrimitiveParameterKind::number) {
                    report(DiagnosticCode::value_wrong_kind, first.token,
                           "param() of " + std::string(descriptor.parameters[instruction.index].name) +
                               ", not a number");
                    return false;
                }
            }
            lowered.declaration =
                RuleDeclaration::expression_value(declared.name, instructions, declared.default_number);
            add_lowered(std::move(lowered));
            return true;
        }
        report(DiagnosticCode::value_wrong_kind, first.token, "a number or a length");
        return false;
    }
    if (declared.kind == PrimitiveParameterKind::colour) {
        std::optional<Color> colour{};
        if (first.kind == ValueComponentKind::colour) colour = first.colour;
        if (first.kind == ValueComponentKind::keyword) colour = named_colour(first.text);
        if (!colour) {
            report(DiagnosticCode::value_wrong_kind, first.token, "a colour");
            return false;
        }
        lowered.declaration = RuleDeclaration::colour_value(declared.name, *colour);
        add_lowered(std::move(lowered));
        return true;
    }
    if (!keyword_of(first, "a keyword")) return false;
    fold_ascii(first.text, folded_);
    const std::string registered = registered_name_from_kebab(folded_);
    const std::optional<std::size_t> choice =
        registered.empty() ? std::optional<std::size_t>{} : find_choice(declared, registered);
    if (!choice) {
        // R2-07: a keyword the engine lacks is an error, or a warning when
        // the sheet declares a newer engine.
        const DiagnosticCode code =
            newer_engine_ ? DiagnosticCode::keyword_from_newer_engine : DiagnosticCode::keyword_unknown;
        report(code, first.token, clipped_theme_detail(first.text));
        return newer_engine_;
    }
    lowered.declaration = RuleDeclaration::choice_value(declared.name, declared.choices[*choice]);
    add_lowered(std::move(lowered));
    return true;
}

bool CompilePass::lower_opacity_or_scale(const SyntaxNode& node, PropertyClass kind) {
    parameter_names_.clear();
    if (!read_value(node)) return false;
    const bool scale = kind == PropertyClass::scale;
    const ValueComponent& first = value_.components[0];
    const WideKeyword wide = wide_keyword();
    double target = 1.0;
    if (wide == WideKeyword::inherit || wide == WideKeyword::outside) {
        const DiagnosticCode code =
            wide == WideKeyword::inherit ? DiagnosticCode::value_wrong_kind : DiagnosticCode::value_form_not_admitted;
        report(code, first.token, clipped_theme_detail(first.text));
        return false;
    }
    if (wide == WideKeyword::none) {
        if (drop_unknown_function()) return true;
        const std::size_t count = value_.components.size();
        if (scale && (count == 2U || count == 3U)) {
            // The engine scales a part uniformly.
            report(DiagnosticCode::value_form_not_admitted, value_.components[1].token, "a scale on each axis");
            return false;
        }
        if (count != 1U) {
            report(DiagnosticCode::value_syntax, value_.components[1].token, "one value");
            return false;
        }
        const bool none = scale && first.kind == ValueComponentKind::keyword && equals_ascii_folded(first.text, "none");
        if (!none && !fraction_of(first, target)) return false;
    }
    const double minimum = scale ? minimum_presentational_scale : 0.0;
    const double maximum = scale ? maximum_presentational_scale : 1.0;
    if (target < minimum || target > maximum) {
        report(DiagnosticCode::value_out_of_range, first.token, clipped_theme_detail(token_text(first.token)));
        return false;
    }
    LoweredDeclaration lowered{};
    lowered.kind = scale ? LoweredKind::scale : LoweredKind::opacity;
    lowered.transitions[0] = still_machine(scale ? scale_property : opacity_property, target);
    lowered.transition_count = 1U;
    add_lowered(std::move(lowered));
    return true;
}

bool CompilePass::lower_translate(const SyntaxNode& node) {
    parameter_names_.clear();
    if (!read_value(node)) return false;
    const ValueComponent& first = value_.components[0];
    const WideKeyword wide = wide_keyword();
    std::array<double, 2> offsets{0.0, 0.0};
    if (wide == WideKeyword::inherit || wide == WideKeyword::outside) {
        const DiagnosticCode code =
            wide == WideKeyword::inherit ? DiagnosticCode::value_wrong_kind : DiagnosticCode::value_form_not_admitted;
        report(code, first.token, clipped_theme_detail(first.text));
        return false;
    }
    const bool none = first.kind == ValueComponentKind::keyword && equals_ascii_folded(first.text, "none");
    if (wide == WideKeyword::none && !none) {
        if (drop_unknown_function()) return true;
        const std::size_t count = value_.components.size();
        if (count == 3U) {
            report(DiagnosticCode::value_form_not_admitted, value_.components[2].token, "a translation in depth");
            return false;
        }
        if (count > 3U) {
            report(DiagnosticCode::value_syntax, value_.components[3].token, "one or two lengths");
            return false;
        }
        for (std::size_t axis = 0U; axis < count; ++axis) {
            const ValueComponent& component = value_.components[axis];
            const bool number = component.kind == ValueComponentKind::number;
            const bool length = number && (component.unit == ValueUnit::px ||
                                            (component.unit == ValueUnit::none && component.number == 0.0));
            if (number && component.unit == ValueUnit::percent) {
                // A percentage of the part's box; the engine translates by
                // logical units.
                report(DiagnosticCode::value_form_not_admitted, component.token, "a percentage");
                return false;
            }
            if (component.kind == ValueComponentKind::expression) {
                report(DiagnosticCode::value_form_not_admitted, component.token, "an expression that reads inputs");
                return false;
            }
            if (!length) {
                report(DiagnosticCode::value_wrong_kind, component.token, "a length");
                return false;
            }
            if (std::abs(component.number) > maximum_presentational_translate) {
                report(DiagnosticCode::value_out_of_range, component.token,
                       clipped_theme_detail(token_text(component.token)));
                return false;
            }
            offsets[axis] = component.number;
        }
    }
    LoweredDeclaration lowered{};
    lowered.kind = LoweredKind::translate;
    lowered.transitions[0] = still_machine(translate_x_property, offsets[0]);
    lowered.transitions[1] = still_machine(translate_y_property, offsets[1]);
    lowered.transition_count = 2U;
    add_lowered(std::move(lowered));
    return true;
}

bool CompilePass::lower_keyword_property(const SyntaxNode& node, PropertyClass kind) {
    parameter_names_.clear();
    if (!read_value(node)) return false;
    const bool blend = kind == PropertyClass::blend;
    const ValueComponent& first = value_.components[0];
    const WideKeyword wide = wide_keyword();
    if (wide == WideKeyword::inherit || wide == WideKeyword::outside) {
        const DiagnosticCode code =
            wide == WideKeyword::inherit ? DiagnosticCode::value_wrong_kind : DiagnosticCode::value_form_not_admitted;
        report(code, first.token, clipped_theme_detail(first.text));
        return false;
    }
    std::string_view mode = blend_modes[0];
    if (wide == WideKeyword::none) {
        if (drop_unknown_function()) return true;
        if (value_.components.size() != 1U) {
            report(DiagnosticCode::value_syntax, value_.components[1].token, "one keyword");
            return false;
        }
        if (!keyword_of(first, "a keyword")) return false;
        fold_ascii(first.text, folded_);
        bool known = false;
        if (blend) {
            for (const std::string_view name : blend_modes) {
                if (name != folded_) continue;
                known = true;
                mode = name;
            }
            if (!known && css_blend_mode_not_admitted(folded_)) {
                report(DiagnosticCode::value_form_not_admitted, first.token, clipped_theme_detail(first.text));
                return false;
            }
        } else {
            known = folded_ == "auto" || folded_ == "isolate";
        }
        if (!known) {
            const DiagnosticCode code =
                newer_engine_ ? DiagnosticCode::keyword_from_newer_engine : DiagnosticCode::keyword_unknown;
            report(code, first.token, clipped_theme_detail(first.text));
            return newer_engine_;
        }
    }
    // `isolation` has no effect: every blend already draws in its own
    // group (CSS_COVERAGE section 7).
    if (!blend) return true;
    LoweredDeclaration lowered{};
    lowered.kind = LoweredKind::blend;
    lowered.declaration = RuleDeclaration::choice_value(blend_property, mode);
    add_lowered(std::move(lowered));
    return true;
}

// A material property's fields (P6b): its CSS-wide `initial` resets each
// field it sets; any other value is lowered by the material lowering.
bool CompilePass::lower_material(const SyntaxNode& node) {
    // folded_ holds the property's name, which classify() found listed.
    const MaterialProperty property = *material_property(folded_);
    parameter_names_.clear();
    if (!read_value(node)) return false;
    const ValueComponent& first = value_.components[0];
    const WideKeyword wide = wide_keyword();
    if (wide == WideKeyword::inherit || wide == WideKeyword::outside) {
        const DiagnosticCode code =
            wide == WideKeyword::inherit ? DiagnosticCode::value_wrong_kind : DiagnosticCode::value_form_not_admitted;
        report(code, first.token, clipped_theme_detail(first.text));
        return false;
    }
    if (wide == WideKeyword::initial) {
        material_property_fields(property, material_fields_);
        for (const MaterialField field : material_fields_) {
            MaterialDeclaration reset{};
            reset.field = field;
            reset.initial = true;
            add_material(std::move(reset));
        }
        return true;
    }
    lower_material_value(property, value_, node.content.first.value, material_lowering_);
    if (material_lowering_.outcome != MaterialLoweringOutcome::lowered) {
        MaterialLoweringProblem& problem = material_lowering_.problem;
        // A keyword the running engine lacks, in a sheet for a newer one,
        // is dropped with a warning (R2-07).
        const bool newer = newer_engine_ && problem.code == DiagnosticCode::keyword_unknown;
        const DiagnosticCode code = newer ? DiagnosticCode::keyword_from_newer_engine : problem.code;
        report(code, problem.token, std::move(problem.detail));
        return newer || material_lowering_.outcome == MaterialLoweringOutcome::dropped;
    }
    for (MaterialDeclaration& declaration : material_lowering_.declarations) add_material(std::move(declaration));
    return true;
}

void CompilePass::add_material(MaterialDeclaration declaration) {
    // As in CSS, the later declaration of a longhand wins, whichever
    // property (a shorthand or the longhand) set it.
    for (std::size_t index = 0U; index < materials_.size(); ++index) {
        if (materials_[index].field != declaration.field) continue;
        materials_.erase(materials_.begin() + static_cast<std::ptrdiff_t>(index));
        break;
    }
    materials_.push_back(std::move(declaration));
}

bool CompilePass::emit_rule(const PrimitiveSelection& selection, std::uint32_t at) {
    ThemeRule lowered{};
    lowered.label = rule_;
    if (selection.selects) {
        const std::string_view name =
            selection.primitive == nullptr ? built_in_primitive_name : (*selection.primitive).descriptor().name;
        lowered.declarations.push_back(RuleDeclaration::primitive(name));
    } else if (!selection.dropped && !materials_.empty() && input_.registry.find(material_primitive_name) != nullptr) {
        // A rule of material fields and no `primitive` selects the
        // material primitive (owner C7, 2026-10-26).
        lowered.declarations.push_back(RuleDeclaration::primitive(material_primitive_name));
    }
    for (LoweredDeclaration& declaration : declarations_) {
        if (declaration.kind == LoweredKind::parameter || declaration.kind == LoweredKind::blend) {
            lowered.declarations.push_back(std::move(declaration.declaration));
            continue;
        }
        for (std::size_t index = 0U; index < declaration.transition_count; ++index) {
            lowered.transitions.push_back(std::move(declaration.transitions[index]));
        }
    }
    lowered.materials = materials_;
    if (lowered.declarations.empty() && lowered.transitions.empty() && lowered.materials.empty()) return true;
    std::size_t bytes = lowered_record_bytes + lowered.label.size();
    for (const RuleDeclaration& declaration : lowered.declarations) bytes += declaration_bytes(declaration);
    for (const MaterialDeclaration& declaration : lowered.materials) bytes += material_bytes(declaration);
    for (const PropertyTransition& transition : lowered.transitions) bytes += transition_bytes(transition);
    for (const LoweredSelector& selector : lowered_selectors_) {
        if (bytes > budget_.bytes) {
            report(DiagnosticCode::lowered_sheet_too_large, at, std::to_string(maximum_lowered_bytes));
            return false;
        }
        budget_.bytes -= bytes;
        sheet_.rules.push_back(lowered);
        ThemeRule& rule = sheet_.rules.back();
        rule.selector = selector.selector;
        rule.level = selector.level;
        // A given specificity only where CSS's differs from the engine's
        // own count of the selector (EC2: an `:is()` argument's).
        if (!(selector.specificity == rule_specificity(selector.selector))) rule.specificity = selector.specificity;
    }
    return true;
}

bool CompilePass::read_value(const SyntaxNode& node) {
    const std::size_t first_record = diagnostics_.records().size();
    ValueContext context{};
    context.parameters = parameter_names_;
    context.published_values = input_.published_values.size();
    context.resources = input_.resources;
    const SubstitutionInput substitution{.source = input_.source,
                                         .tokens = tokens_,
                                         .table = custom_properties_,
                                         .reserved = reserved_,
                                         .cursor = cursor_};
    bool read = substitution_.substitute(substitution, node.content, substituted_, diagnostics_) ==
                SubstitutionOutcome::substituted;
    if (read) {
        const ValueInput value_input{.source = input_.source,
                                     .tokens = tokens_,
                                     .written = node.content,
                                     .substituted = substituted_,
                                     .context = context,
                                     .cursor = cursor_};
        read = reader_.read(value_input, value_, diagnostics_) == ValueOutcome::read;
    }
    diagnostics_.name_context(first_record, rule_, property_);
    return read;
}

WideKeyword CompilePass::wide_keyword() const noexcept {
    if (value_.components.size() != 1U) return WideKeyword::none;
    const ValueComponent& only = value_.components[0];
    if (only.kind != ValueComponentKind::keyword) return WideKeyword::none;
    if (equals_ascii_folded(only.text, "initial")) return WideKeyword::initial;
    if (equals_ascii_folded(only.text, "inherit")) return WideKeyword::inherit;
    const bool outside = equals_ascii_folded(only.text, "unset") || equals_ascii_folded(only.text, "revert") ||
                         equals_ascii_folded(only.text, "revert-layer");
    return outside ? WideKeyword::outside : WideKeyword::none;
}

bool CompilePass::drop_unknown_function() {
    for (const ValueComponent& component : value_.components) {
        if (component.kind != ValueComponentKind::function) continue;
        report(DiagnosticCode::unknown_function, component.token, clipped_theme_detail(component.text));
        return true;
    }
    return false;
}

bool CompilePass::fraction_of(const ValueComponent& component, double& fraction) {
    if (component.kind == ValueComponentKind::expression) {
        report(DiagnosticCode::value_form_not_admitted, component.token, "an expression that reads inputs");
        return false;
    }
    const bool number = component.kind == ValueComponentKind::number;
    if (number && component.unit == ValueUnit::none) {
        fraction = component.number;
        return true;
    }
    if (number && component.unit == ValueUnit::percent) {
        fraction = component.number / 100.0;
        return true;
    }
    report(DiagnosticCode::value_wrong_kind, component.token, "a number or a percentage");
    return false;
}

bool CompilePass::keyword_of(const ValueComponent& component, std::string_view expected) {
    if (component.kind == ValueComponentKind::keyword) return true;
    report(DiagnosticCode::value_wrong_kind, component.token, std::string(expected));
    return false;
}

void CompilePass::add_lowered(LoweredDeclaration lowered) {
    // As in CSS, the later of two declarations of one property wins.
    for (std::size_t index = 0U; index < declarations_.size(); ++index) {
        const LoweredDeclaration& earlier = declarations_[index];
        const bool same = earlier.kind == lowered.kind &&
                          (lowered.kind != LoweredKind::parameter || earlier.parameter == lowered.parameter);
        if (!same) continue;
        declarations_.erase(declarations_.begin() + static_cast<std::ptrdiff_t>(index));
        break;
    }
    declarations_.push_back(std::move(lowered));
}

std::string_view CompilePass::token_text(std::uint32_t index) const noexcept {
    const Token& token = tokens_.tokens[index];
    const std::string_view text = input_.source.text.substr(token.offset.value, token.length.value);
    return text;
}

std::string_view CompilePass::range_text(TokenRange range) const noexcept {
    if (range.first.value >= range.end.value) return {};
    const Token& first = tokens_.tokens[range.first.value];
    const Token& last = tokens_.tokens[range.end.value - 1U];
    const std::size_t end = static_cast<std::size_t>(last.offset.value) + last.length.value;
    const std::string_view text = input_.source.text.substr(first.offset.value, end - first.offset.value);
    return text;
}

void CompilePass::report(DiagnosticCode code, std::uint32_t token, std::string detail) {
    ThemeDiagnostic diagnostic{};
    diagnostic.code = code;
    diagnostic.file = std::string(input_.source.file);
    const ByteOffset offset = tokens_.tokens.empty() ? ByteOffset{} : tokens_.tokens[token].offset;
    diagnostic.position = cursor_.locate(offset);
    diagnostic.rule = rule_;
    diagnostic.property = property_;
    diagnostic.detail = std::move(detail);
    diagnostics_.add(std::move(diagnostic));
}

} // namespace

CompileOutcome ThemeCompiler::compile(const ThemeCompileInput& input, ThemeRuleSheet& sheet,
                                      ThemeDiagnosticList& diagnostics) {
    sheet.rules.clear();
    sheet.published_values.clear();
    sheet.requires_engine = current_engine_version;
    CompilePass pass(input, sheet, diagnostics, tokens_, lines_, tree_, custom_properties_, selectors_, substitution_,
                     reader_, lowered_selectors_, substituted_, value_, parameter_names_, declarations_, materials_,
                     material_lowering_, material_fields_, folded_);
    const CompileOutcome outcome = pass.run();
    if (outcome == CompileOutcome::refused) {
        sheet.rules.clear();
        sheet.published_values.clear();
        sheet.requires_engine = current_engine_version;
    }
    return outcome;
}

} // namespace gui_forms::theme_language
