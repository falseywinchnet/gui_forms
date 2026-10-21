// The cascade (T2 plan, phase E4): compiling a theme's rules against the
// primitive registry with diagnostics, and resolving one part.

#include "gui_forms/engine/theme_rules/theme_rules.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace gui_forms {
namespace {

constexpr std::size_t maximum_style_class_length = 64U;

bool lowercase_letter(char character) noexcept {
    const bool letter = character >= 'a' && character <= 'z';
    return letter;
}

bool class_character(char character) noexcept {
    const bool digit = character >= '0' && character <= '9';
    const bool joiner = character == '-' || character == '_';
    return lowercase_letter(character) || digit || joiner;
}

bool authored(CascadeLevel level) noexcept {
    const bool authored_level = level == CascadeLevel::theme_sheet || level == CascadeLevel::class_rule ||
                                level == CascadeLevel::stable_id_rule || level == CascadeLevel::inline_style;
    return authored_level;
}

bool paints_role(const PrimitiveDescriptor& descriptor, PartRole role) noexcept {
    for (const PartRole painted : descriptor.roles) {
        if (painted == role) return true;
    }
    return false;
}

RuleDiagnostic diagnostic(RuleDiagnosticKind kind, std::size_t rule, const ThemeRule& source,
                          std::string_view property) {
    RuleDiagnostic result{};
    result.kind = kind;
    result.rule = rule;
    result.label = source.label;
    result.property = std::string(property);
    return result;
}

// The diagnostic for a selector that cannot be used at its level, or none.
std::optional<RuleDiagnosticKind> selector_defect(const ThemeRule& rule) noexcept {
    const RuleSelector& selector = rule.selector;
    for (const std::string& name : selector.classes) {
        if (!valid_style_class_name(name)) return RuleDiagnosticKind::selector_invalid;
    }
    for (const AnatomyState state : selector.states) {
        if (static_cast<std::size_t>(state) >= anatomy_state_count) return RuleDiagnosticKind::selector_invalid;
    }
    if (selector.role && static_cast<std::size_t>(*selector.role) >= part_role_count) {
        return RuleDiagnosticKind::selector_invalid;
    }
    if (rule.level == CascadeLevel::class_rule && selector.classes.empty()) {
        return RuleDiagnosticKind::selector_missing_level_field;
    }
    const bool needs_id = rule.level == CascadeLevel::stable_id_rule || rule.level == CascadeLevel::inline_style;
    if (needs_id && selector.stable_id.empty()) return RuleDiagnosticKind::selector_missing_level_field;
    return std::nullopt;
}

// Why a primitive cannot be selected by this rule, or none.
std::optional<RuleDiagnosticKind> primitive_defect(const EnginePrimitive& primitive,
                                                   const RuleSelector& selector) noexcept {
    const PrimitiveDescriptor& descriptor = primitive.descriptor();
    if (descriptor.cost.per_device_pixel == 0U) return RuleDiagnosticKind::primitive_cost_unbounded;
    if (descriptor.cost.per_device_pixel > maximum_operations_per_device_pixel) {
        return RuleDiagnosticKind::primitive_over_budget;
    }
    if (selector.role && !paints_role(descriptor, *selector.role)) return RuleDiagnosticKind::primitive_role_mismatch;
    return std::nullopt;
}

// Why a parameter declaration cannot be used, or none; `index` is set to the
// parameter's position when it can.
std::optional<RuleDiagnosticKind> parameter_defect(const PrimitiveDescriptor& descriptor,
                                                   const RuleDeclaration& declaration, std::size_t& index) noexcept {
    const std::optional<std::size_t> found = find_parameter(descriptor, declaration.property);
    if (!found) return RuleDiagnosticKind::unknown_parameter;
    const PrimitiveParameterDeclaration& parameter = descriptor.parameters[*found];
    if (declaration.kind == RuleValueKind::name) return RuleDiagnosticKind::parameter_wrong_kind;
    // An expression gives a number; its fallback, in `number`, is checked
    // as a number value is.
    const bool number = declaration.kind == RuleValueKind::number || declaration.kind == RuleValueKind::expression;
    if (number != (parameter.kind == PrimitiveParameterKind::number)) return RuleDiagnosticKind::parameter_wrong_kind;
    if (number) {
        if (!std::isfinite(declaration.number)) return RuleDiagnosticKind::parameter_not_finite;
        if (declaration.number < parameter.minimum || declaration.number > parameter.maximum) {
            return RuleDiagnosticKind::parameter_out_of_range;
        }
    }
    index = *found;
    return std::nullopt;
}

// Whether an expression reads a parameter that is a colour.
bool reads_colour_parameter(const PrimitiveDescriptor& descriptor, const EngineExpression& expression) noexcept {
    for (std::size_t index = 0; index < descriptor.parameters.size(); ++index) {
        const ExpressionParameterIndex position{static_cast<std::uint8_t>(index)};
        if (!expression.dependencies().reads_parameter(position)) continue;
        if (descriptor.parameters[index].kind == PrimitiveParameterKind::colour) return true;
    }
    return false;
}

// The primitive's declared inputs as expression input bits.
std::uint64_t declared_input_bits(const PrimitiveDescriptor& descriptor) noexcept {
    std::uint64_t bits = 0U;
    for (const EngineInput input : descriptor.inputs_read) {
        bits = bits | expression_input_bit(engine_input_position(input));
    }
    return bits;
}

} // namespace

std::string_view cascade_level_name(CascadeLevel level) noexcept {
    switch (level) {
    case CascadeLevel::built_in: return "built_in";
    case CascadeLevel::theme_sheet: return "theme_sheet";
    case CascadeLevel::class_rule: return "class_rule";
    case CascadeLevel::stable_id_rule: return "stable_id_rule";
    case CascadeLevel::inline_style: return "inline_style";
    case CascadeLevel::forced_high_contrast: return "forced_high_contrast";
    case CascadeLevel::count: return {};
    }
    return {};
}

bool valid_style_class_name(std::string_view name) noexcept {
    if (name.empty() || name.size() > maximum_style_class_length) return false;
    if (!lowercase_letter(name.front())) return false;
    for (const char character : name) {
        if (!class_character(character)) return false;
    }
    return true;
}

RuleSpecificity rule_specificity(const RuleSelector& selector) noexcept {
    RuleSpecificity result{};
    result.ids = selector.stable_id.empty() ? 0U : 1U;
    result.classes = static_cast<std::uint32_t>(selector.classes.size());
    result.types = selector.control_type.empty() ? 0U : 1U;
    const std::uint32_t named = selector.part.empty() ? 0U : 1U;
    const std::uint32_t role = selector.role ? 1U : 0U;
    result.parts = named + role;
    result.states = static_cast<std::uint32_t>(selector.states.size());
    return result;
}

std::string_view rule_diagnostic_name(RuleDiagnosticKind kind) noexcept {
    switch (kind) {
    case RuleDiagnosticKind::needs_newer_engine: return "needs_newer_engine";
    case RuleDiagnosticKind::level_not_authored: return "level_not_authored";
    case RuleDiagnosticKind::selector_missing_level_field: return "selector_missing_level_field";
    case RuleDiagnosticKind::selector_invalid: return "selector_invalid";
    case RuleDiagnosticKind::unknown_primitive: return "unknown_primitive";
    case RuleDiagnosticKind::primitive_role_mismatch: return "primitive_role_mismatch";
    case RuleDiagnosticKind::primitive_over_budget: return "primitive_over_budget";
    case RuleDiagnosticKind::primitive_cost_unbounded: return "primitive_cost_unbounded";
    case RuleDiagnosticKind::primitive_value_not_a_name: return "primitive_value_not_a_name";
    case RuleDiagnosticKind::parameter_without_primitive: return "parameter_without_primitive";
    case RuleDiagnosticKind::unknown_parameter: return "unknown_parameter";
    case RuleDiagnosticKind::parameter_wrong_kind: return "parameter_wrong_kind";
    case RuleDiagnosticKind::parameter_not_finite: return "parameter_not_finite";
    case RuleDiagnosticKind::parameter_out_of_range: return "parameter_out_of_range";
    case RuleDiagnosticKind::duplicate_declaration: return "duplicate_declaration";
    case RuleDiagnosticKind::expression_invalid: return "expression_invalid";
    case RuleDiagnosticKind::expression_reads_colour: return "expression_reads_colour";
    case RuleDiagnosticKind::rule_empty: return "rule_empty";
    case RuleDiagnosticKind::transition_property_unknown: return "transition_property_unknown";
    case RuleDiagnosticKind::transition_property_not_animated: return "transition_property_not_animated";
    case RuleDiagnosticKind::transition_invalid: return "transition_invalid";
    case RuleDiagnosticKind::ambient_over_frame_cap: return "ambient_over_frame_cap";
    case RuleDiagnosticKind::ambient_invalid: return "ambient_invalid";
    case RuleDiagnosticKind::published_value_invalid: return "published_value_invalid";
    }
    return {};
}

RuleDeclaration RuleDeclaration::primitive(std::string_view primitive_name) {
    RuleDeclaration result{};
    result.property = std::string(primitive_property);
    result.kind = RuleValueKind::name;
    result.name = std::string(primitive_name);
    return result;
}

RuleDeclaration RuleDeclaration::number_value(std::string_view parameter, double value) {
    RuleDeclaration result{};
    result.property = std::string(parameter);
    result.kind = RuleValueKind::number;
    result.number = value;
    return result;
}

RuleDeclaration RuleDeclaration::colour_value(std::string_view parameter, Color value) {
    RuleDeclaration result{};
    result.property = std::string(parameter);
    result.kind = RuleValueKind::colour;
    result.colour = value;
    return result;
}

RuleDeclaration RuleDeclaration::expression_value(std::string_view parameter,
                                                  std::span<const ExpressionInstruction> program, double fallback) {
    RuleDeclaration result{};
    result.property = std::string(parameter);
    result.kind = RuleValueKind::expression;
    result.number = fallback;
    result.program.assign(program.begin(), program.end());
    return result;
}

std::optional<ExpressionInputIndex> CompiledThemeRules::published_value_input(std::string_view name) const noexcept {
    if (name.empty()) return std::nullopt;
    for (std::size_t index = 0; index < published_values_.size(); ++index) {
        if (published_values_[index] == name) return published_value_position(index);
    }
    return std::nullopt;
}

// -- compiling -----------------------------------------------------------------

namespace {

// Precedence, lowest first; std::stable_sort keeps authored order among
// equals, so the later rule stays later.
struct LowerPrecedence final {
    template <typename Rule>
    bool operator()(const Rule& left, const Rule& right) const noexcept {
        if (left.level != right.level) return left.level < right.level;
        return left.specificity < right.specificity;
    }
};

} // namespace

void CompiledThemeRules::add_rules(CompiledThemeRules& result, const ThemeRuleSheet& sheet,
                                   const PrimitiveRegistry& registry, bool engine_table) {
    const std::size_t input_count = result.input_count();
    result.rules_.reserve(result.rules_.size() + sheet.rules.size());
    for (std::size_t index = 0; index < sheet.rules.size(); ++index) {
        const ThemeRule& rule = sheet.rules[index];
        const EngineVersion needed = rule.requires_engine.value_or(sheet.requires_engine);
        if (current_engine_version < needed) {
            result.diagnostics_.push_back(diagnostic(RuleDiagnosticKind::needs_newer_engine, index, rule, {}));
            continue;
        }
        // The engine's own table is all built_in; a theme authors only the
        // four authored levels.
        const bool level_allowed = engine_table ? rule.level == CascadeLevel::built_in : authored(rule.level);
        if (!level_allowed) {
            result.diagnostics_.push_back(diagnostic(RuleDiagnosticKind::level_not_authored, index, rule, {}));
            continue;
        }
        const std::optional<RuleDiagnosticKind> selector = selector_defect(rule);
        if (selector) {
            result.diagnostics_.push_back(diagnostic(*selector, index, rule, {}));
            continue;
        }
        CompiledRule compiled{};
        compiled.level = rule.level;
        compiled.specificity = rule_specificity(rule.selector);
        compiled.order = index;
        compiled.selector = rule.selector;
        for (const AnatomyState state : rule.selector.states) compiled.states.insert(state);
        // The primitive first, so that the parameters can be checked against
        // its declarations.
        for (const RuleDeclaration& declaration : rule.declarations) {
            if (declaration.property != primitive_property) continue;
            if (compiled.selects) {
                result.diagnostics_.push_back(
                    diagnostic(RuleDiagnosticKind::duplicate_declaration, index, rule, declaration.property));
                continue;
            }
            if (declaration.kind != RuleValueKind::name) {
                result.diagnostics_.push_back(
                    diagnostic(RuleDiagnosticKind::primitive_value_not_a_name, index, rule, declaration.property));
                continue;
            }
            if (declaration.name == built_in_primitive_name) {
                compiled.selects = true;
                compiled.primitive = nullptr;
                continue;
            }
            const EnginePrimitive* const primitive = registry.find(declaration.name);
            if (primitive == nullptr) {
                result.diagnostics_.push_back(
                    diagnostic(RuleDiagnosticKind::unknown_primitive, index, rule, declaration.property));
                continue;
            }
            const std::optional<RuleDiagnosticKind> defect = primitive_defect(*primitive, rule.selector);
            if (defect) {
                result.diagnostics_.push_back(diagnostic(*defect, index, rule, declaration.property));
                continue;
            }
            compiled.selects = true;
            compiled.primitive = primitive;
        }
        for (const RuleDeclaration& declaration : rule.declarations) {
            if (declaration.property == primitive_property) continue;
            if (compiled.primitive == nullptr) {
                result.diagnostics_.push_back(
                    diagnostic(RuleDiagnosticKind::parameter_without_primitive, index, rule, declaration.property));
                continue;
            }
            std::size_t parameter = 0U;
            const std::optional<RuleDiagnosticKind> defect =
                parameter_defect((*compiled.primitive).descriptor(), declaration, parameter);
            if (defect) {
                result.diagnostics_.push_back(diagnostic(*defect, index, rule, declaration.property));
                continue;
            }
            bool repeated = false;
            for (const CompiledDeclaration& earlier : compiled.parameters) {
                if (earlier.parameter == parameter) repeated = true;
            }
            if (repeated) {
                result.diagnostics_.push_back(
                    diagnostic(RuleDiagnosticKind::duplicate_declaration, index, rule, declaration.property));
                continue;
            }
            CompiledDeclaration value{};
            value.parameter = parameter;
            if (declaration.kind == RuleValueKind::expression) {
                const PrimitiveDescriptor& descriptor = (*compiled.primitive).descriptor();
                ExpressionLimits limits{};
                limits.inputs = input_count;
                limits.parameters = descriptor.parameters.size();
                const ExpressionCompilation built =
                    value.program.compile(declaration.program, declaration.number, limits);
                if (built.defect != ExpressionDefect::none) {
                    RuleDiagnostic invalid =
                        diagnostic(RuleDiagnosticKind::expression_invalid, index, rule, declaration.property);
                    invalid.expression = built.defect;
                    result.diagnostics_.push_back(std::move(invalid));
                    continue;
                }
                if (reads_colour_parameter(descriptor, value.program)) {
                    result.diagnostics_.push_back(
                        diagnostic(RuleDiagnosticKind::expression_reads_colour, index, rule, declaration.property));
                    continue;
                }
                value.expression = true;
                value.value.kind = PrimitiveParameterKind::number;
                value.value.number = declaration.number;
            } else if (declaration.kind == RuleValueKind::number) {
                value.value.kind = PrimitiveParameterKind::number;
                value.value.number = declaration.number;
            } else {
                value.value.kind = PrimitiveParameterKind::colour;
                value.value.colour = declaration.colour;
            }
            compiled.parameters.push_back(value);
        }
        add_transitions(result.diagnostics_, index, rule, compiled);
        add_ambient(result.diagnostics_, index, rule, compiled);
        const bool declares = compiled.selects || !compiled.parameters.empty() || !compiled.transitions.empty() ||
                              compiled.ambient.has_value();
        if (!declares) {
            result.diagnostics_.push_back(diagnostic(RuleDiagnosticKind::rule_empty, index, rule, {}));
            continue;
        }
        result.rules_.push_back(std::move(compiled));
    }
}

// -- transitions and ambient effects (E7) ------------------------------------------

namespace {

bool in_range(double value, double minimum, double maximum) noexcept {
    const bool inside = std::isfinite(value) && value >= minimum && value <= maximum;
    return inside;
}

// Whether a machine's states are usable for a property ranging over
// [minimum, maximum].
bool usable_states(const PropertyTransition& transition, double minimum, double maximum) noexcept {
    if (transition.states.empty() || transition.states.size() > maximum_presentation_states) return false;
    for (const PresentationState& state : transition.states) {
        for (const AnatomyState when : state.when) {
            if (static_cast<std::size_t>(when) >= anatomy_state_count) return false;
        }
        if (!in_range(state.target, minimum, maximum)) return false;
        if (state.enter_from && !in_range(*state.enter_from, minimum, maximum)) return false;
    }
    return true;
}

} // namespace

void CompiledThemeRules::add_transitions(std::vector<RuleDiagnostic>& diagnostics, std::size_t index,
                                         const ThemeRule& rule, CompiledRule& compiled) {
    for (const PropertyTransition& transition : rule.transitions) {
        CompiledTransition made{};
        double minimum = 0.0;
        double maximum = 1.0;
        if (transition.property == opacity_property) {
            made.opacity = true;
        } else {
            if (compiled.primitive == nullptr) {
                diagnostics.push_back(
                    diagnostic(RuleDiagnosticKind::parameter_without_primitive, index, rule, transition.property));
                continue;
            }
            const PrimitiveDescriptor& descriptor = (*compiled.primitive).descriptor();
            const std::optional<std::size_t> found = find_parameter(descriptor, transition.property);
            if (!found) {
                diagnostics.push_back(
                    diagnostic(RuleDiagnosticKind::transition_property_unknown, index, rule, transition.property));
                continue;
            }
            const PrimitiveParameterDeclaration& declared = descriptor.parameters[*found];
            if (declared.kind != PrimitiveParameterKind::number || !declared.animated) {
                diagnostics.push_back(diagnostic(RuleDiagnosticKind::transition_property_not_animated, index, rule,
                                                 transition.property));
                continue;
            }
            made.parameter = *found;
            minimum = declared.minimum;
            maximum = declared.maximum;
        }
        bool repeated = false;
        for (const CompiledTransition& earlier : compiled.transitions) {
            if (earlier.opacity == made.opacity && earlier.parameter == made.parameter) repeated = true;
        }
        if (repeated) {
            diagnostics.push_back(
                diagnostic(RuleDiagnosticKind::duplicate_declaration, index, rule, transition.property));
            continue;
        }
        if (compiled.transitions.size() >= maximum_part_transitions ||
            !usable_states(transition, minimum, maximum)) {
            diagnostics.push_back(diagnostic(RuleDiagnosticKind::transition_invalid, index, rule, transition.property));
            continue;
        }
        made.states.reserve(transition.states.size());
        for (const PresentationState& state : transition.states) {
            CompiledPresentationState compiled_state{};
            for (const AnatomyState when : state.when) compiled_state.when.insert(when);
            compiled_state.target = state.target;
            compiled_state.enter_from = state.enter_from;
            made.states.push_back(compiled_state);
        }
        made.curve = transition.curve;
        compiled.transitions.push_back(std::move(made));
    }
}

void CompiledThemeRules::add_ambient(std::vector<RuleDiagnostic>& diagnostics, std::size_t index,
                                     const ThemeRule& rule, CompiledRule& compiled) {
    if (!rule.ambient) return;
    const AmbientEffect& ambient = *rule.ambient;
    const double rate = ambient.frames_per_second;
    // Refused at load (design language §9.4): no frame cap, or one above 30.
    if (!(rate > 0.0) || !(rate <= maximum_ambient_frames_per_second)) {
        diagnostics.push_back(diagnostic(RuleDiagnosticKind::ambient_over_frame_cap, index, rule, "ambient"));
        return;
    }
    const bool timed = ambient.period > FrameInterval::zero() && ambient.pose >= FrameInterval::zero() &&
                       ambient.pose < ambient.period;
    if (!timed) {
        diagnostics.push_back(diagnostic(RuleDiagnosticKind::ambient_invalid, index, rule, "ambient"));
        return;
    }
    compiled.ambient = ambient;
}

CompiledThemeRules CompiledThemeRules::compile(const ThemeRuleSheet& sheet, const PrimitiveRegistry& registry) {
    CompiledThemeRules result{};
    // The published values first: expressions are checked against their
    // count. An unusable name keeps its position, empty, so that the
    // positions after it stay where the sheet's expressions expect them.
    const std::size_t published = std::min(sheet.published_values.size(), maximum_published_values);
    result.published_values_.reserve(published);
    for (std::size_t index = 0; index < sheet.published_values.size(); ++index) {
        const std::string& name = sheet.published_values[index];
        bool usable = index < maximum_published_values && valid_style_class_name(name);
        for (std::size_t earlier = 0; usable && earlier < index; ++earlier) {
            if (sheet.published_values[earlier] == name) usable = false;
        }
        if (!usable) {
            RuleDiagnostic invalid{};
            invalid.kind = RuleDiagnosticKind::published_value_invalid;
            invalid.rule = sheet_diagnostic;
            invalid.property = name;
            result.diagnostics_.push_back(std::move(invalid));
        }
        if (index >= maximum_published_values) continue;
        result.published_values_.push_back(usable ? name : std::string{});
    }
    add_rules(result, sheet, registry, false);
    std::stable_sort(result.rules_.begin(), result.rules_.end(), LowerPrecedence{});
    return result;
}

CompiledThemeRules CompiledThemeRules::compile_with_built_in(const ThemeRuleSheet& sheet,
                                                            const PrimitiveRegistry& registry) {
    CompiledThemeRules result = compile(sheet, registry);
    // The engine's rules after the sheet's, so that the sheet's diagnostics
    // keep their positions; precedence puts them first.
    const std::size_t authored_rules = result.rules_.size();
    add_rules(result, built_in_theme_rules(), registry, true);
    result.built_in_rules_ = result.rules_.size() - authored_rules;
    std::stable_sort(result.rules_.begin(), result.rules_.end(), LowerPrecedence{});
    return result;
}

// -- resolving -----------------------------------------------------------------

bool CompiledThemeRules::matches(const CompiledRule& rule, const PartFacts& facts) noexcept {
    const RuleSelector& selector = rule.selector;
    if (!selector.control_type.empty() && selector.control_type != facts.control_type) return false;
    if (!selector.stable_id.empty() && selector.stable_id != facts.stable_id) return false;
    if (!selector.part.empty() && selector.part != facts.part) return false;
    if (selector.role && *selector.role != facts.role) return false;
    const std::uint64_t needed = rule.states.standard_bits();
    if ((facts.states.standard_bits() & needed) != needed) return false;
    for (const std::string& name : selector.classes) {
        bool present = false;
        for (const std::string& held : facts.classes) {
            if (held == name) present = true;
        }
        if (!present) return false;
    }
    return true;
}

CompiledThemeRules::Resolution CompiledThemeRules::resolve(const PartFacts& facts) const noexcept {
    Resolution result{};
    if (rules_.empty()) return result;
    // The winner is the last matching rule that selects, rules being in
    // ascending precedence.
    std::size_t winner = rules_.size();
    for (std::size_t index = rules_.size(); index > 0U; --index) {
        const CompiledRule& rule = rules_[index - 1U];
        if (rule.selects && matches(rule, facts)) {
            winner = index - 1U;
            break;
        }
    }
    if (winner != rules_.size()) resolve_primitive(facts, rules_[winner], result);
    resolve_transitions(facts, result);
    return result;
}

void CompiledThemeRules::resolve_primitive(const PartFacts& facts, const CompiledRule& chosen,
                                           Resolution& result) const noexcept {
    result.level = chosen.level;
    if (chosen.primitive == nullptr) {
        result.outcome = Outcome::built_in_selected;
        return;
    }
    const PrimitiveDescriptor& descriptor = (*chosen.primitive).descriptor();
    if (!paints_role(descriptor, facts.role)) {
        result.outcome = Outcome::role_mismatch;
        return;
    }
    result.parameters = default_parameters(descriptor);
    // Every matching rule for the same primitive contributes, in ascending
    // precedence, so the last write of each parameter is its winner. A
    // winning expression keeps its fallback as the parameter's value until
    // the engine evaluates it.
    for (const CompiledRule& rule : rules_) {
        if (rule.primitive != chosen.primitive || !matches(rule, facts)) continue;
        for (const CompiledDeclaration& declaration : rule.parameters) {
            result.parameters.values[declaration.parameter] = declaration.value;
            result.expressions[declaration.parameter] = declaration.expression ? &declaration.program : nullptr;
        }
    }
    result.inputs_read = declared_input_bits(descriptor);
    for (std::size_t index = 0; index < result.parameters.count; ++index) {
        const EngineExpression* const expression = result.expressions[index];
        if (expression != nullptr) result.inputs_read = result.inputs_read | (*expression).dependencies().inputs;
    }
    result.outcome = Outcome::primitive;
    result.primitive = chosen.primitive;
}

void CompiledThemeRules::resolve_transitions(const PartFacts& facts, Resolution& result) const noexcept {
    // The last matching declaration of each property wins: opacity from any
    // rule, a parameter only from the rules selecting the part's primitive.
    const CompiledTransition* opacity = nullptr;
    std::array<const CompiledTransition*, maximum_primitive_parameters> parameters{};
    const bool primitive = result.outcome == Outcome::primitive;
    for (const CompiledRule& rule : rules_) {
        if ((rule.transitions.empty() && !rule.ambient) || !matches(rule, facts)) continue;
        if (rule.ambient) result.ambient = &*rule.ambient;
        const bool same_primitive = primitive && rule.primitive == result.primitive;
        for (const CompiledTransition& transition : rule.transitions) {
            if (transition.opacity) {
                opacity = &transition;
            } else if (same_primitive) {
                parameters[transition.parameter] = &transition;
            }
        }
    }
    result.transition_count = 0U;
    if (opacity != nullptr) {
        result.transitions[result.transition_count] = opacity;
        result.transition_count = result.transition_count + 1U;
    }
    for (std::size_t index = 0; index < result.parameters.count; ++index) {
        if (parameters[index] == nullptr || result.transition_count >= maximum_part_transitions) continue;
        result.transitions[result.transition_count] = parameters[index];
        result.transition_count = result.transition_count + 1U;
    }
}

} // namespace gui_forms
