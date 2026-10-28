#pragma once

// The transitions' lowering (stage T3, PLAN.md P7a, section 1.8): CSS's
// `transition` and its four longhands, read from a rule's declarations,
// become the curves of the engine's presentational state machines
// (PropertyTransition); and, once every rule is lowered, each transition is
// grouped with the rules that refine its own (R2-18). The compiler reads
// the values and handles the CSS-wide keywords; this lowers what is left
// into result records and reports nothing itself.

#include "../diagnostics/theme_diagnostics.hpp"
#include "../value/theme_value.hpp"

#include "gui_forms/engine/motion_value/motion_value.hpp"
#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"
#include "gui_forms/engine/theme_rules/theme_rules.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms::theme_language {

// The transition properties: the shorthand and CSS's four longhands.
enum class TransitionProperty : std::uint8_t { transition, property, duration, timing_function, delay };

// The property `name` (lower case) names, or none for another property.
[[nodiscard]] std::optional<TransitionProperty> transition_property(std::string_view name) noexcept;

// What one written transition moves:
// - engine: one of the engine's presentational properties, `name` being
//   opacity_property, scale_property or face_property;
// - translate: CSS's `translate`, the engine's two translations;
// - parameter: a parameter of the rule's primitive, `name` its registered
//   name, checked when the rule's transitions are built.
enum class TransitionTargetKind : std::uint8_t { engine, translate, parameter };

struct TransitionTarget final {
    TransitionTargetKind kind{TransitionTargetKind::engine};
    std::string name{};
    // Where the sheet names it.
    std::uint32_t token{};
};

// A timing function as written, without the transition's duration and
// delay: a timed easing (with its control points or steps), or one of the
// engine's physical curves with its own time (`constant`: follow's and
// decay's time constant, spring's response) and, for a spring, its
// overshoot.
struct TimingFunction final {
    MotionKind kind{MotionKind::timed};
    MotionEasing easing{MotionEasing::ease};
    std::array<double, 4> bezier{0.0, 0.0, 1.0, 1.0};
    std::uint32_t steps{1U};
    MotionStepPosition step_position{MotionStepPosition::end};
    FrameInterval constant{};
    double overshoot{};
    std::uint32_t token{};
};

// One rule's transition lists as its declarations wrote them, CSS's four
// longhands; the shorthand sets all four, and a later declaration replaces
// the lists it sets. A list not set takes CSS's initial value when the
// transitions are built: no properties, 0s, `ease`, 0s. `properties` empty
// with `properties_set` is `none`. Caller-owned and reused from rule to
// rule (clear_written_transitions).
struct WrittenTransitions final {
    bool properties_set{};
    std::vector<TransitionTarget> properties{};
    bool durations_set{};
    std::vector<FrameInterval> durations{};
    bool timings_set{};
    std::vector<TimingFunction> timings{};
    bool delays_set{};
    std::vector<FrameInterval> delays{};
    // Where the rule's last transition declaration starts, where a problem
    // with the rule's transitions as a whole points.
    std::uint32_t token{};
};

// Empties `written` for the next rule, keeping its capacity.
void clear_written_transitions(WrittenTransitions& written) noexcept;

// Whether the rule declared any transition property.
[[nodiscard]] bool transitions_written(const WrittenTransitions& written) noexcept;

enum class MotionLoweringOutcome : std::uint8_t {
    lowered,
    // A warning (`problem`): the declaration is dropped, the rule kept.
    dropped,
    // An error (`problem`) refuses the sheet.
    refused,
};

// What one lowering found wrong: the diagnostic the compiler reports, at
// the sheet token `token`.
struct MotionLoweringProblem final {
    DiagnosticCode code{DiagnosticCode::value_syntax};
    std::uint32_t token{};
    std::string detail{};
};

// Lowers `value`, a value of `property` that is not a CSS-wide keyword,
// into the lists of `written` it sets; on a problem `written` keeps its
// earlier lists. `start` is the token the value starts at. `value` is
// borrowed for the call.
[[nodiscard]] MotionLoweringOutcome lower_transition_value(TransitionProperty property, const ThemeValue& value,
                                                           std::uint32_t start, WrittenTransitions& written,
                                                           MotionLoweringProblem& problem);

// Resets the lists `property` sets to CSS's initial values (`initial`).
void reset_transition_lists(TransitionProperty property, WrittenTransitions& written) noexcept;

// One machine's curve a rule declares: `property` as ThemeRule records it
// (CSS's `translate` gives two, translate_x_property then
// translate_y_property).
struct DeclaredTransition final {
    std::string property{};
    MotionCurve curve{};
    std::uint32_t token{};
};

// The rule's transitions from `written`, one per written property in
// order, each taking the duration, timing function and delay at its
// position in their lists, repeated as CSS repeats a short list, into
// `declared`, which is cleared first. A parameter must be an animated
// number parameter of `primitive`, the rule's selected primitive (null
// when it selects none or the built-in recipe). A later transition of the
// same property replaces an earlier one, as CSS's does.
[[nodiscard]] MotionLoweringOutcome build_declared_transitions(const WrittenTransitions& written,
                                                               const PrimitiveDescriptor* primitive,
                                                               std::vector<DeclaredTransition>& declared,
                                                               MotionLoweringProblem& problem);

// Where one machine of the lowered sheet came from: rule `rule`'s
// transition at `machine`, either a transition declaration's (`curved`,
// which `token` names) or a static value's, an instant machine of one
// state.
struct MachineOrigin final {
    std::size_t rule{};
    std::size_t machine{};
    bool curved{};
    std::uint32_t token{};
};

// Groups each curved machine of `sheet` with the static values of the
// rules that refine its rule (R2-18): rules at the same level whose
// selector is the same but for more states, setting the same property
// (an engine property's instant machine, or a number declaration of the
// parameter in a rule selecting the same primitive). Their values become
// the machine's states, ahead of its own, ordered most precedent first
// (the higher specificity, then the later rule), as the engine's machine
// takes the first state that matches; their own machines and
// declarations are removed, and a rule left declaring nothing is
// removed. A curved machine left with no state, other than a face
// cross-fade, moves nothing and is removed. More than
// maximum_presentation_states states is E407. `origins` lists every
// machine of `sheet` in rule order; `sheet` is changed in place, and on a
// problem may be left half grouped (the compiler then refuses it).
[[nodiscard]] MotionLoweringOutcome group_transitions(ThemeRuleSheet& sheet, std::span<const MachineOrigin> origins,
                                                      MotionLoweringProblem& problem);

} // namespace gui_forms::theme_language
