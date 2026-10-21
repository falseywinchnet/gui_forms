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
    const bool number = declaration.kind == RuleValueKind::number;
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
    case RuleDiagnosticKind::rule_empty: return "rule_empty";
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

CompiledThemeRules CompiledThemeRules::compile(const ThemeRuleSheet& sheet, const PrimitiveRegistry& registry) {
    CompiledThemeRules result{};
    result.rules_.reserve(sheet.rules.size());
    for (std::size_t index = 0; index < sheet.rules.size(); ++index) {
        const ThemeRule& rule = sheet.rules[index];
        const EngineVersion needed = rule.requires_engine.value_or(sheet.requires_engine);
        if (current_engine_version < needed) {
            result.diagnostics_.push_back(diagnostic(RuleDiagnosticKind::needs_newer_engine, index, rule, {}));
            continue;
        }
        if (!authored(rule.level)) {
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
            if (declaration.kind == RuleValueKind::number) {
                value.value.kind = PrimitiveParameterKind::number;
                value.value.number = declaration.number;
            } else {
                value.value.kind = PrimitiveParameterKind::colour;
                value.value.colour = declaration.colour;
            }
            compiled.parameters.push_back(value);
        }
        if (!compiled.selects && compiled.parameters.empty()) {
            result.diagnostics_.push_back(diagnostic(RuleDiagnosticKind::rule_empty, index, rule, {}));
            continue;
        }
        result.rules_.push_back(std::move(compiled));
    }
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
    if (winner == rules_.size()) return result;
    const CompiledRule& chosen = rules_[winner];
    result.level = chosen.level;
    if (chosen.primitive == nullptr) {
        result.outcome = Outcome::built_in_selected;
        return result;
    }
    const PrimitiveDescriptor& descriptor = (*chosen.primitive).descriptor();
    if (!paints_role(descriptor, facts.role)) {
        result.outcome = Outcome::role_mismatch;
        return result;
    }
    result.parameters = default_parameters(descriptor);
    // Every matching rule for the same primitive contributes, in ascending
    // precedence, so the last write of each parameter is its winner.
    for (const CompiledRule& rule : rules_) {
        if (rule.primitive != chosen.primitive || !matches(rule, facts)) continue;
        for (const CompiledDeclaration& declaration : rule.parameters) {
            result.parameters.values[declaration.parameter] = declaration.value;
        }
    }
    result.outcome = Outcome::primitive;
    result.primitive = chosen.primitive;
    return result;
}

} // namespace gui_forms
