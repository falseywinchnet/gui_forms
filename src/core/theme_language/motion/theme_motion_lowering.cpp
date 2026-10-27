#include "theme_motion_lowering.hpp"

#include "../source_text/theme_source_text.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// A spring's overshoot is part of the curve the engine runs, so it is
// computed without fused multiply-add.
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace gui_forms::theme_language {

namespace {

// -- the names --------------------------------------------------------------------

struct NamedTransitionProperty final {
    std::string_view name{};
    TransitionProperty property{TransitionProperty::transition};
};

constexpr std::array<NamedTransitionProperty, 5> transition_properties{{
    {.name = "transition", .property = TransitionProperty::transition},
    {.name = "transition-delay", .property = TransitionProperty::delay},
    {.name = "transition-duration", .property = TransitionProperty::duration},
    {.name = "transition-property", .property = TransitionProperty::property},
    {.name = "transition-timing-function", .property = TransitionProperty::timing_function},
}};

// The timing keywords and what each lowers to: the engine's own easings
// for `linear`, `ease` (its cubic ease) and `ease-in-out` (smoothstep),
// CSS's exact cubic-bezier() for `ease-in` and `ease-out` (R2-17), and
// one step for `step-start` and `step-end`.
struct TimingKeyword final {
    std::string_view name{};
    MotionEasing easing{MotionEasing::linear};
    std::array<double, 4> bezier{0.0, 0.0, 1.0, 1.0};
    MotionStepPosition step_position{MotionStepPosition::end};
};

constexpr std::array<TimingKeyword, 7> timing_keywords{{
    {.name = "linear", .easing = MotionEasing::linear, .bezier = {0.0, 0.0, 1.0, 1.0},
     .step_position = MotionStepPosition::end},
    {.name = "ease", .easing = MotionEasing::ease, .bezier = {0.0, 0.0, 1.0, 1.0},
     .step_position = MotionStepPosition::end},
    {.name = "ease-in-out", .easing = MotionEasing::smoothstep, .bezier = {0.0, 0.0, 1.0, 1.0},
     .step_position = MotionStepPosition::end},
    {.name = "ease-in", .easing = MotionEasing::cubic_bezier, .bezier = {0.42, 0.0, 1.0, 1.0},
     .step_position = MotionStepPosition::end},
    {.name = "ease-out", .easing = MotionEasing::cubic_bezier, .bezier = {0.0, 0.0, 0.58, 1.0},
     .step_position = MotionStepPosition::end},
    {.name = "step-start", .easing = MotionEasing::steps, .bezier = {0.0, 0.0, 1.0, 1.0},
     .step_position = MotionStepPosition::start},
    {.name = "step-end", .easing = MotionEasing::steps, .bezier = {0.0, 0.0, 1.0, 1.0},
     .step_position = MotionStepPosition::end},
}};

[[nodiscard]] const TimingKeyword* timing_keyword(std::string_view name) noexcept {
    for (const TimingKeyword& keyword : timing_keywords) {
        if (equals_ascii_folded(name, keyword.name)) return &keyword;
    }
    return nullptr;
}

// The timing functions written as functions.
enum class TimingFunctionName : std::uint8_t { cubic_bezier, steps, follow, spring, decay };

struct NamedTimingFunction final {
    std::string_view name{};
    TimingFunctionName function{TimingFunctionName::cubic_bezier};
};

constexpr std::array<NamedTimingFunction, 5> timing_functions{{
    {.name = "cubic-bezier", .function = TimingFunctionName::cubic_bezier},
    {.name = "steps", .function = TimingFunctionName::steps},
    {.name = "follow", .function = TimingFunctionName::follow},
    {.name = "spring", .function = TimingFunctionName::spring},
    {.name = "decay", .function = TimingFunctionName::decay},
}};

[[nodiscard]] const NamedTimingFunction* timing_function(std::string_view name) noexcept {
    for (const NamedTimingFunction& function : timing_functions) {
        if (equals_ascii_folded(name, function.name)) return &function;
    }
    return nullptr;
}

// A time in seconds as a frame interval, rounded to the clock's tick and
// at most the engine's ceiling, which every curve is clamped to anyway.
[[nodiscard]] FrameInterval interval_of(double seconds) noexcept {
    const double ceiling = std::chrono::duration<double>(motion_duration_ceiling).count();
    const double bounded = std::min(seconds, ceiling);
    const FrameInterval interval = std::chrono::round<FrameInterval>(std::chrono::duration<double>(bounded));
    return interval;
}

// A spring's overshoot for the damping ratio `damping` (0 < damping <= 1;
// owner decision, 2026-10-26): e^(-πζ / √(1 − ζ²)), none at 1.
[[nodiscard]] double spring_overshoot(double damping) noexcept {
    if (damping >= 1.0) return 0.0;
    const double pi = 3.14159265358979323846;
    const double exponent = -pi * damping / std::sqrt(1.0 - damping * damping);
    const double overshoot = std::exp(exponent);
    return overshoot;
}

// Components [first, end) of a value, at one nesting level.
struct ComponentRange final {
    std::size_t first{};
    std::size_t end{};
};

// One transition of the shorthand as read.
struct SingleTransition final {
    bool named{};
    bool none{};
    TransitionTarget target{};
    std::size_t times{};
    FrameInterval duration{};
    FrameInterval delay{};
    bool timed{};
    TimingFunction timing{};
};

// One lowering: reads `value_` and fills `written_`'s lists through
// staging lists, which replace them only when the whole value is read.
// Each reading function returns false once it has recorded a problem.
class TransitionReader final {
public:
    TransitionReader(const ThemeValue& value, std::uint32_t start, MotionLoweringProblem& problem) noexcept
        : value_(value), start_(start), problem_(problem) {}

    [[nodiscard]] MotionLoweringOutcome lower(TransitionProperty property, WrittenTransitions& written);

private:
    bool fail(DiagnosticCode code, std::uint32_t token, std::string detail);
    [[nodiscard]] std::size_t next(std::size_t index) const noexcept;
    void split(ComponentRange range, std::vector<ComponentRange>& parts) const;
    [[nodiscard]] bool keyword(std::size_t index, std::string_view lower_name) const noexcept;
    bool read_time(std::size_t index, bool negatives, double& seconds);
    bool read_number(std::size_t index, double& number);
    bool read_timing(std::size_t index, TimingFunction& timing);
    bool read_bezier(std::size_t function, TimingFunction& timing);
    bool read_steps(std::size_t function, TimingFunction& timing);
    bool read_physical(std::size_t function, TimingFunctionName name, TimingFunction& timing);
    bool read_target(std::size_t index, TransitionTarget& target);
    bool read_single(ComponentRange range, SingleTransition& single);
    // Whether `index` names a timing function, by keyword or function.
    [[nodiscard]] bool timing_component(std::size_t index) const noexcept;

    bool lower_shorthand();
    bool lower_properties();
    bool lower_times(bool delays);
    bool lower_timings();

    const ThemeValue& value_;
    std::uint32_t start_{};
    MotionLoweringProblem& problem_;
    bool dropped_{};
    std::vector<ComponentRange> parts_{};
    std::vector<TransitionTarget> properties_{};
    std::vector<FrameInterval> durations_{};
    std::vector<TimingFunction> timings_{};
    std::vector<FrameInterval> delays_{};
    bool none_{};
};

bool TransitionReader::fail(DiagnosticCode code, std::uint32_t token, std::string detail) {
    dropped_ = diagnostic_severity(code) == DiagnosticSeverity::warning;
    problem_.code = code;
    problem_.token = token;
    problem_.detail = std::move(detail);
    return false;
}

std::size_t TransitionReader::next(std::size_t index) const noexcept {
    const std::size_t after = value_.components[index].subtree_end;
    return after;
}

// The comma-separated parts of `range`, into `parts`, which is cleared
// first. An empty part is kept, for its reader to refuse.
void TransitionReader::split(ComponentRange range, std::vector<ComponentRange>& parts) const {
    parts.clear();
    std::size_t first = range.first;
    for (std::size_t index = range.first; index < range.end; index = next(index)) {
        if (value_.components[index].kind != ValueComponentKind::comma) continue;
        parts.push_back(ComponentRange{first, index});
        first = index + 1U;
    }
    parts.push_back(ComponentRange{first, range.end});
}

bool TransitionReader::keyword(std::size_t index, std::string_view lower_name) const noexcept {
    const ValueComponent& component = value_.components[index];
    const bool matches = component.kind == ValueComponentKind::keyword && equals_ascii_folded(component.text, lower_name);
    return matches;
}

// A <time> (s or ms, read in seconds); negative only with `negatives`.
bool TransitionReader::read_time(std::size_t index, bool negatives, double& seconds) {
    const ValueComponent& component = value_.components[index];
    if (component.kind == ValueComponentKind::expression) {
        return fail(DiagnosticCode::value_form_not_admitted, component.token, "an expression that reads inputs");
    }
    if (component.kind != ValueComponentKind::number || component.unit != ValueUnit::seconds) {
        return fail(DiagnosticCode::value_wrong_kind, component.token, "a time");
    }
    if (component.number < 0.0) {
        // A negative delay starts a move part of the way through, which the
        // engine's moves do not; a negative duration is no time at all.
        const DiagnosticCode code = negatives ? DiagnosticCode::value_form_not_admitted
                                              : DiagnosticCode::value_out_of_range;
        return fail(code, component.token, clipped_theme_detail(component.text));
    }
    seconds = component.number;
    return true;
}

bool TransitionReader::read_number(std::size_t index, double& number) {
    const ValueComponent& component = value_.components[index];
    if (component.kind == ValueComponentKind::expression) {
        return fail(DiagnosticCode::value_form_not_admitted, component.token, "an expression that reads inputs");
    }
    if (component.kind != ValueComponentKind::number || component.unit != ValueUnit::none) {
        return fail(DiagnosticCode::value_wrong_kind, component.token, "a number");
    }
    number = component.number;
    return true;
}

bool TransitionReader::timing_component(std::size_t index) const noexcept {
    const ValueComponent& component = value_.components[index];
    if (component.kind == ValueComponentKind::keyword) return timing_keyword(component.text) != nullptr;
    if (component.kind == ValueComponentKind::function) return timing_function(component.text) != nullptr;
    return false;
}

// cubic-bezier(x1, y1, x2, y2): each x within [0, 1], as CSS requires;
// the engine clamps each y to its overshoot.
bool TransitionReader::read_bezier(std::size_t function, TimingFunction& timing) {
    const ValueComponent& written = value_.components[function];
    std::vector<ComponentRange> arguments{};
    split(ComponentRange{function + 1U, written.subtree_end}, arguments);
    if (arguments.size() != 4U) return fail(DiagnosticCode::value_syntax, written.token, "four numbers");
    for (std::size_t position = 0U; position < 4U; ++position) {
        const ComponentRange argument = arguments[position];
        if (argument.first == argument.end || next(argument.first) != argument.end) {
            return fail(DiagnosticCode::value_syntax, written.token, "four numbers");
        }
        double number = 0.0;
        if (!read_number(argument.first, number)) return false;
        const bool across = position == 0U || position == 2U;
        if (across && !(number >= 0.0 && number <= 1.0)) {
            const ValueComponent& component = value_.components[argument.first];
            return fail(DiagnosticCode::value_out_of_range, component.token, clipped_theme_detail(component.text));
        }
        timing.bezier[position] = number;
    }
    timing.kind = MotionKind::timed;
    timing.easing = MotionEasing::cubic_bezier;
    return true;
}

// steps(n[, jump-start | jump-end | start | end]): 1 to maximum_motion_steps
// levels. jump-none and jump-both are CSS's, not the engine's.
bool TransitionReader::read_steps(std::size_t function, TimingFunction& timing) {
    const ValueComponent& written = value_.components[function];
    std::vector<ComponentRange> arguments{};
    split(ComponentRange{function + 1U, written.subtree_end}, arguments);
    if (arguments.size() > 2U) return fail(DiagnosticCode::value_syntax, written.token, "a count and a position");
    for (const ComponentRange argument : arguments) {
        if (argument.first == argument.end || next(argument.first) != argument.end) {
            return fail(DiagnosticCode::value_syntax, written.token, "a count and a position");
        }
    }
    double count = 0.0;
    if (!read_number(arguments[0].first, count)) return false;
    const ValueComponent& counted = value_.components[arguments[0].first];
    const bool whole = std::floor(count) == count;
    if (!whole || count < 1.0 || count > static_cast<double>(maximum_motion_steps)) {
        return fail(DiagnosticCode::value_out_of_range, counted.token, clipped_theme_detail(counted.text));
    }
    timing.kind = MotionKind::timed;
    timing.easing = MotionEasing::steps;
    timing.steps = static_cast<std::uint32_t>(count);
    timing.step_position = MotionStepPosition::end;
    if (arguments.size() == 1U) return true;
    const std::size_t position = arguments[1].first;
    const ValueComponent& placed = value_.components[position];
    if (keyword(position, "jump-start") || keyword(position, "start")) {
        timing.step_position = MotionStepPosition::start;
        return true;
    }
    if (keyword(position, "jump-end") || keyword(position, "end")) return true;
    if (keyword(position, "jump-none") || keyword(position, "jump-both")) {
        return fail(DiagnosticCode::value_form_not_admitted, placed.token, clipped_theme_detail(placed.text));
    }
    return fail(DiagnosticCode::keyword_unknown, placed.token, clipped_theme_detail(placed.text));
}

// follow(<time>), decay(<time>) and spring(<time>, <damping>), the
// engine's physical curves (C2). A spring's damping is its damping ratio,
// from the ratio that overshoots by maximum_motion_overshoot (about
// 0.627) to 1, critically damped (owner decision, 2026-10-26).
bool TransitionReader::read_physical(std::size_t function, TimingFunctionName name, TimingFunction& timing) {
    const ValueComponent& written = value_.components[function];
    std::vector<ComponentRange> arguments{};
    split(ComponentRange{function + 1U, written.subtree_end}, arguments);
    const std::size_t expected = name == TimingFunctionName::spring ? 2U : 1U;
    const std::string shape = name == TimingFunctionName::spring ? "a time and a damping ratio" : "a time";
    if (arguments.size() != expected) return fail(DiagnosticCode::value_syntax, written.token, shape);
    for (const ComponentRange argument : arguments) {
        if (argument.first == argument.end || next(argument.first) != argument.end) {
            return fail(DiagnosticCode::value_syntax, written.token, shape);
        }
    }
    double seconds = 0.0;
    if (!read_time(arguments[0].first, false, seconds)) return false;
    timing.constant = interval_of(seconds);
    timing.easing = MotionEasing::linear;
    if (name == TimingFunctionName::follow) {
        timing.kind = MotionKind::follow;
        return true;
    }
    if (name == TimingFunctionName::decay) {
        timing.kind = MotionKind::decay;
        return true;
    }
    double damping = 0.0;
    if (!read_number(arguments[1].first, damping)) return false;
    const double overshoot = damping > 0.0 ? spring_overshoot(damping) : 1.0;
    if (!(damping > 0.0 && damping <= 1.0) || overshoot > maximum_motion_overshoot) {
        const ValueComponent& component = value_.components[arguments[1].first];
        return fail(DiagnosticCode::value_out_of_range, component.token, clipped_theme_detail(component.text));
    }
    timing.kind = MotionKind::spring;
    timing.overshoot = overshoot;
    return true;
}

bool TransitionReader::read_timing(std::size_t index, TimingFunction& timing) {
    const ValueComponent& component = value_.components[index];
    timing = TimingFunction{};
    timing.token = component.token;
    if (component.kind == ValueComponentKind::keyword) {
        const TimingKeyword* const named = timing_keyword(component.text);
        if (named == nullptr) {
            return fail(DiagnosticCode::keyword_unknown, component.token, clipped_theme_detail(component.text));
        }
        timing.kind = MotionKind::timed;
        timing.easing = (*named).easing;
        timing.bezier = (*named).bezier;
        timing.step_position = (*named).step_position;
        return true;
    }
    if (component.kind != ValueComponentKind::function) {
        return fail(DiagnosticCode::value_wrong_kind, component.token, "a timing function");
    }
    const NamedTimingFunction* const function = timing_function(component.text);
    if (function == nullptr) {
        return fail(DiagnosticCode::unknown_function, component.token, clipped_theme_detail(component.text));
    }
    switch ((*function).function) {
    case TimingFunctionName::cubic_bezier: return read_bezier(index, timing);
    case TimingFunctionName::steps: return read_steps(index, timing);
    case TimingFunctionName::follow:
    case TimingFunctionName::spring:
    case TimingFunctionName::decay: return read_physical(index, (*function).function, timing);
    }
    return fail(DiagnosticCode::value_wrong_kind, component.token, "a timing function");
}

// A property a transition moves: the engine's presentational ones, or a
// parameter's kebab-case name. `all` would move every property, which the
// engine's machines cannot; `none` is read by the callers.
bool TransitionReader::read_target(std::size_t index, TransitionTarget& target) {
    const ValueComponent& component = value_.components[index];
    if (component.kind != ValueComponentKind::keyword) {
        return fail(DiagnosticCode::value_wrong_kind, component.token, "a property");
    }
    target = TransitionTarget{};
    target.token = component.token;
    if (keyword(index, "all")) {
        return fail(DiagnosticCode::value_form_not_admitted, component.token, "all: a transition names its property");
    }
    if (keyword(index, "opacity")) {
        target.name = std::string(opacity_property);
        return true;
    }
    if (keyword(index, "scale")) {
        target.name = std::string(scale_property);
        return true;
    }
    if (keyword(index, "face")) {
        target.name = std::string(face_property);
        return true;
    }
    if (keyword(index, "translate")) {
        target.kind = TransitionTargetKind::translate;
        return true;
    }
    std::string folded(component.text);
    for (char& character : folded) {
        if (character >= 'A' && character <= 'Z') character = static_cast<char>(character - 'A' + 'a');
    }
    target.kind = TransitionTargetKind::parameter;
    target.name = registered_name_from_kebab(folded);
    if (target.name.empty()) target.name = folded;
    return true;
}

// One transition of the shorthand: a property, up to two times (the
// duration, then the delay) and a timing function, each at most once and
// in any order.
bool TransitionReader::read_single(ComponentRange range, SingleTransition& single) {
    single = SingleTransition{};
    if (range.first == range.end) {
        const std::uint32_t token = range.first < value_.components.size() ? value_.components[range.first].token
                                                                            : start_;
        return fail(DiagnosticCode::value_syntax, token, "a transition");
    }
    for (std::size_t index = range.first; index < range.end; index = next(index)) {
        const ValueComponent& component = value_.components[index];
        if (component.kind == ValueComponentKind::number || component.kind == ValueComponentKind::expression) {
            if (single.times == 2U) return fail(DiagnosticCode::value_syntax, component.token, "two times");
            double seconds = 0.0;
            if (!read_time(index, single.times == 1U, seconds)) return false;
            if (single.times == 0U) {
                single.duration = interval_of(seconds);
            } else {
                single.delay = interval_of(seconds);
            }
            single.times = single.times + 1U;
            continue;
        }
        if (timing_component(index) || (component.kind == ValueComponentKind::function)) {
            if (single.timed) return fail(DiagnosticCode::value_syntax, component.token, "one timing function");
            single.timed = true;
            if (!read_timing(index, single.timing)) return false;
            continue;
        }
        if (single.named) return fail(DiagnosticCode::value_syntax, component.token, "one property");
        single.named = true;
        if (keyword(index, "none")) {
            single.none = true;
            continue;
        }
        if (!read_target(index, single.target)) return false;
    }
    if (!single.named) {
        const ValueComponent& first = value_.components[range.first];
        return fail(DiagnosticCode::value_form_not_admitted, first.token, "a transition names its property");
    }
    return true;
}

bool TransitionReader::lower_shorthand() {
    split(ComponentRange{0U, value_.components.size()}, parts_);
    const std::vector<ComponentRange> parts = parts_;
    for (const ComponentRange part : parts) {
        SingleTransition single{};
        if (!read_single(part, single)) return false;
        if (single.none) {
            if (parts.size() != 1U) {
                return fail(DiagnosticCode::value_syntax, value_.components[part.first].token, "none alone");
            }
            none_ = true;
        } else {
            properties_.push_back(single.target);
        }
        durations_.push_back(single.duration);
        TimingFunction timing = single.timing;
        if (!single.timed) timing = TimingFunction{};
        timings_.push_back(timing);
        delays_.push_back(single.delay);
    }
    return true;
}

bool TransitionReader::lower_properties() {
    split(ComponentRange{0U, value_.components.size()}, parts_);
    const std::vector<ComponentRange> parts = parts_;
    for (const ComponentRange part : parts) {
        if (part.first == part.end || next(part.first) != part.end) {
            const std::uint32_t token = part.first < value_.components.size() ? value_.components[part.first].token
                                                                               : start_;
            return fail(DiagnosticCode::value_syntax, token, "one property");
        }
        if (keyword(part.first, "none")) {
            if (parts.size() != 1U) {
                return fail(DiagnosticCode::value_syntax, value_.components[part.first].token, "none alone");
            }
            none_ = true;
            continue;
        }
        TransitionTarget target{};
        if (!read_target(part.first, target)) return false;
        properties_.push_back(std::move(target));
    }
    return true;
}

bool TransitionReader::lower_times(bool delays) {
    split(ComponentRange{0U, value_.components.size()}, parts_);
    const std::vector<ComponentRange> parts = parts_;
    for (const ComponentRange part : parts) {
        if (part.first == part.end || next(part.first) != part.end) {
            const std::uint32_t token = part.first < value_.components.size() ? value_.components[part.first].token
                                                                               : start_;
            return fail(DiagnosticCode::value_syntax, token, "one time");
        }
        double seconds = 0.0;
        if (!read_time(part.first, delays, seconds)) return false;
        if (delays) {
            delays_.push_back(interval_of(seconds));
        } else {
            durations_.push_back(interval_of(seconds));
        }
    }
    return true;
}

bool TransitionReader::lower_timings() {
    split(ComponentRange{0U, value_.components.size()}, parts_);
    const std::vector<ComponentRange> parts = parts_;
    for (const ComponentRange part : parts) {
        if (part.first == part.end || next(part.first) != part.end) {
            const std::uint32_t token = part.first < value_.components.size() ? value_.components[part.first].token
                                                                               : start_;
            return fail(DiagnosticCode::value_syntax, token, "one timing function");
        }
        TimingFunction timing{};
        if (!read_timing(part.first, timing)) return false;
        timings_.push_back(timing);
    }
    return true;
}

MotionLoweringOutcome TransitionReader::lower(TransitionProperty property, WrittenTransitions& written) {
    if (value_.components.empty()) {
        fail(DiagnosticCode::value_syntax, start_, "a value");
        return MotionLoweringOutcome::refused;
    }
    bool read = false;
    switch (property) {
    case TransitionProperty::transition: read = lower_shorthand(); break;
    case TransitionProperty::property: read = lower_properties(); break;
    case TransitionProperty::duration: read = lower_times(false); break;
    case TransitionProperty::timing_function: read = lower_timings(); break;
    case TransitionProperty::delay: read = lower_times(true); break;
    }
    if (!read) return dropped_ ? MotionLoweringOutcome::dropped : MotionLoweringOutcome::refused;
    written.token = start_;
    const bool shorthand = property == TransitionProperty::transition;
    if (shorthand || property == TransitionProperty::property) {
        written.properties_set = true;
        written.properties = std::move(properties_);
        if (none_) written.properties.clear();
    }
    if (shorthand || property == TransitionProperty::duration) {
        written.durations_set = true;
        written.durations = std::move(durations_);
    }
    if (shorthand || property == TransitionProperty::timing_function) {
        written.timings_set = true;
        written.timings = std::move(timings_);
    }
    if (shorthand || property == TransitionProperty::delay) {
        written.delays_set = true;
        written.delays = std::move(delays_);
    }
    return MotionLoweringOutcome::lowered;
}

// The curve of one transition: its timing function with its duration and
// delay. The physical curves keep their own time, so they take no
// duration.
bool curve_of(const TimingFunction& timing, FrameInterval duration, FrameInterval delay, MotionCurve& curve,
              MotionLoweringProblem& problem) {
    switch (timing.kind) {
    case MotionKind::timed:
        if (timing.easing == MotionEasing::cubic_bezier) {
            curve = MotionCurve::cubic_bezier(duration, timing.bezier[0], timing.bezier[1], timing.bezier[2],
                                              timing.bezier[3]);
        } else if (timing.easing == MotionEasing::steps) {
            curve = MotionCurve::stepped(duration, timing.steps, timing.step_position);
        } else {
            curve = MotionCurve::timed(duration, timing.easing);
        }
        break;
    case MotionKind::follow:
    case MotionKind::spring:
    case MotionKind::decay:
        if (duration != FrameInterval{}) {
            problem.code = DiagnosticCode::value_form_not_admitted;
            problem.token = timing.token;
            problem.detail = "a duration with follow(), spring() or decay(), whose own time sets the pace";
            return false;
        }
        if (timing.kind == MotionKind::follow) curve = MotionCurve::follow(timing.constant);
        if (timing.kind == MotionKind::decay) curve = MotionCurve::decay(timing.constant);
        if (timing.kind == MotionKind::spring) curve = MotionCurve::spring(timing.constant, timing.overshoot);
        break;
    }
    if (delay != FrameInterval{}) curve = curve.delayed(delay);
    return true;
}

void declare(std::string_view property, const MotionCurve& curve, std::uint32_t token,
             std::vector<DeclaredTransition>& declared) {
    for (std::size_t index = 0U; index < declared.size(); ++index) {
        if (declared[index].property != property) continue;
        declared.erase(declared.begin() + static_cast<std::ptrdiff_t>(index));
        break;
    }
    DeclaredTransition transition{};
    transition.property = std::string(property);
    transition.curve = curve;
    transition.token = token;
    declared.push_back(std::move(transition));
}

// Whether `refining` names the same control and part as `host`, with
// every state of `host`'s and more.
bool refines(const ThemeRule& refining, const ThemeRule& host) noexcept {
    const RuleSelector& left = refining.selector;
    const RuleSelector& right = host.selector;
    if (refining.level != host.level || refining.specificity != host.specificity) return false;
    if (left.control_type != right.control_type || left.stable_id != right.stable_id ||
        left.classes != right.classes || left.part != right.part || left.role != right.role) {
        return false;
    }
    if (left.states.size() <= right.states.size()) return false;
    for (const AnatomyState state : right.states) {
        if (std::find(left.states.begin(), left.states.end(), state) == left.states.end()) return false;
    }
    return true;
}

// A refinement found for one host, by its rule's precedence: a static
// value's machine (origin `origin`) or a parameter's declaration (at
// `position` of the rule's declarations), and the value it sets.
struct Refinement final {
    RuleSpecificity specificity{};
    std::size_t rule{};
    std::size_t origin{};
    bool declaration{};
    std::size_t position{};
    double target{};
};

// A declaration of a rule, to remove once absorbed.
struct DeclarationPosition final {
    std::size_t rule{};
    std::size_t declaration{};

    friend bool operator==(DeclarationPosition left, DeclarationPosition right) noexcept {
        return left.rule == right.rule && left.declaration == right.declaration;
    }
};

// The later position first, so that removing one keeps the earlier ones'.
struct LaterPosition final {
    bool operator()(DeclarationPosition left, DeclarationPosition right) const noexcept {
        if (left.rule != right.rule) return left.rule > right.rule;
        return left.declaration > right.declaration;
    }
};

// The primitive a rule selects by name, or empty when it selects none.
std::string_view selected_primitive(const ThemeRule& rule) noexcept {
    for (const RuleDeclaration& declaration : rule.declarations) {
        if (declaration.property == primitive_property) return declaration.name;
    }
    return {};
}

// The more precedent refinement first: the higher specificity, then the
// later rule.
struct MorePrecedent final {
    bool operator()(const Refinement& left, const Refinement& right) const noexcept {
        if (!(left.specificity == right.specificity)) return right.specificity < left.specificity;
        return left.rule > right.rule;
    }
};

// Whether `rule` declares nothing.
bool rule_empty(const ThemeRule& rule) noexcept {
    const bool empty = rule.declarations.empty() && rule.transitions.empty() && !rule.ambient.has_value() &&
                       rule.materials.empty() && rule.recipe_fields.empty();
    return empty;
}

} // namespace

std::optional<TransitionProperty> transition_property(std::string_view name) noexcept {
    for (const NamedTransitionProperty& named : transition_properties) {
        if (named.name == name) return named.property;
    }
    return std::nullopt;
}

void clear_written_transitions(WrittenTransitions& written) noexcept {
    written.properties_set = false;
    written.properties.clear();
    written.durations_set = false;
    written.durations.clear();
    written.timings_set = false;
    written.timings.clear();
    written.delays_set = false;
    written.delays.clear();
    written.token = 0U;
}

bool transitions_written(const WrittenTransitions& written) noexcept {
    const bool any = written.properties_set || written.durations_set || written.timings_set || written.delays_set;
    return any;
}

MotionLoweringOutcome lower_transition_value(TransitionProperty property, const ThemeValue& value, std::uint32_t start,
                                             WrittenTransitions& written, MotionLoweringProblem& problem) {
    TransitionReader reader(value, start, problem);
    const MotionLoweringOutcome outcome = reader.lower(property, written);
    return outcome;
}

void reset_transition_lists(TransitionProperty property, WrittenTransitions& written) noexcept {
    const bool shorthand = property == TransitionProperty::transition;
    if (shorthand || property == TransitionProperty::property) {
        written.properties_set = true;
        written.properties.clear();
    }
    if (shorthand || property == TransitionProperty::duration) {
        written.durations_set = false;
        written.durations.clear();
    }
    if (shorthand || property == TransitionProperty::timing_function) {
        written.timings_set = false;
        written.timings.clear();
    }
    if (shorthand || property == TransitionProperty::delay) {
        written.delays_set = false;
        written.delays.clear();
    }
}

MotionLoweringOutcome build_declared_transitions(const WrittenTransitions& written, const PrimitiveDescriptor* primitive,
                                                 std::vector<DeclaredTransition>& declared,
                                                 MotionLoweringProblem& problem) {
    declared.clear();
    if (!transitions_written(written)) return MotionLoweringOutcome::lowered;
    if (!written.properties_set) {
        // CSS's initial property is `all`, which the engine cannot move.
        problem.code = DiagnosticCode::value_form_not_admitted;
        problem.token = written.token;
        problem.detail = "a transition names its properties (transition-property)";
        return MotionLoweringOutcome::refused;
    }
    for (std::size_t index = 0U; index < written.properties.size(); ++index) {
        const TransitionTarget& target = written.properties[index];
        FrameInterval duration{};
        if (!written.durations.empty()) duration = written.durations[index % written.durations.size()];
        TimingFunction timing{};
        if (!written.timings.empty()) timing = written.timings[index % written.timings.size()];
        FrameInterval delay{};
        if (!written.delays.empty()) delay = written.delays[index % written.delays.size()];
        if (target.kind == TransitionTargetKind::parameter) {
            const std::optional<std::size_t> found =
                primitive == nullptr ? std::optional<std::size_t>{} : find_parameter(*primitive, target.name);
            if (!found) {
                problem.code = DiagnosticCode::value_form_not_admitted;
                problem.token = target.token;
                problem.detail = target.name + ": transitions move opacity, scale, translate, face and the animated "
                                               "parameters of the rule's primitive";
                return MotionLoweringOutcome::refused;
            }
            const PrimitiveParameterDeclaration& parameter = (*primitive).parameters[*found];
            if (parameter.kind != PrimitiveParameterKind::number || !parameter.animated) {
                problem.code = DiagnosticCode::value_wrong_kind;
                problem.token = target.token;
                problem.detail = "an animated parameter";
                return MotionLoweringOutcome::refused;
            }
        }
        MotionCurve curve{};
        if (!curve_of(timing, duration, delay, curve, problem)) return MotionLoweringOutcome::refused;
        if (target.kind == TransitionTargetKind::translate) {
            declare(translate_x_property, curve, target.token, declared);
            declare(translate_y_property, curve, target.token, declared);
        } else {
            declare(target.name, curve, target.token, declared);
        }
    }
    return MotionLoweringOutcome::lowered;
}

MotionLoweringOutcome group_transitions(ThemeRuleSheet& sheet, std::span<const MachineOrigin> origins,
                                        MotionLoweringProblem& problem) {
    std::vector<bool> absorbed(origins.size(), false);
    std::vector<DeclarationPosition> absorbed_declarations{};
    std::vector<Refinement> refinements{};
    for (const MachineOrigin& host : origins) {
        if (!host.curved) continue;
        const ThemeRule& host_rule = sheet.rules[host.rule];
        const std::string& property = host_rule.transitions[host.machine].property;
        const std::string_view primitive = selected_primitive(host_rule);
        refinements.clear();
        for (std::size_t index = 0U; index < origins.size(); ++index) {
            const MachineOrigin& candidate = origins[index];
            if (candidate.curved || candidate.rule == host.rule) continue;
            const ThemeRule& rule = sheet.rules[candidate.rule];
            if (rule.transitions[candidate.machine].property != property || !refines(rule, host_rule)) continue;
            Refinement refinement{};
            refinement.specificity = rule.specificity ? *rule.specificity : rule_specificity(rule.selector);
            refinement.rule = candidate.rule;
            refinement.origin = index;
            refinement.target = rule.transitions[candidate.machine].states[0].target;
            refinements.push_back(refinement);
        }
        // A parameter's static values are declarations of rules selecting
        // the same primitive.
        for (std::size_t index = 0U; index < sheet.rules.size() && !primitive.empty(); ++index) {
            const ThemeRule& rule = sheet.rules[index];
            if (index == host.rule || selected_primitive(rule) != primitive || !refines(rule, host_rule)) continue;
            for (std::size_t position = 0U; position < rule.declarations.size(); ++position) {
                const RuleDeclaration& declaration = rule.declarations[position];
                if (declaration.property != property || declaration.kind != RuleValueKind::number) continue;
                Refinement refinement{};
                refinement.specificity = rule.specificity ? *rule.specificity : rule_specificity(rule.selector);
                refinement.rule = index;
                refinement.declaration = true;
                refinement.position = position;
                refinement.target = declaration.number;
                refinements.push_back(refinement);
            }
        }
        PropertyTransition& machine = sheet.rules[host.rule].transitions[host.machine];
        if (refinements.size() + machine.states.size() > maximum_presentation_states) {
            problem.code = DiagnosticCode::too_many_layers;
            problem.token = host.token;
            problem.detail = std::to_string(maximum_presentation_states) + " states of one transition";
            return MotionLoweringOutcome::refused;
        }
        std::sort(refinements.begin(), refinements.end(), MorePrecedent{});
        std::vector<PresentationState> states{};
        states.reserve(refinements.size() + machine.states.size());
        for (const Refinement& refinement : refinements) {
            PresentationState state{};
            state.when = sheet.rules[refinement.rule].selector.states;
            state.target = refinement.target;
            states.push_back(std::move(state));
            if (refinement.declaration) {
                absorbed_declarations.push_back(DeclarationPosition{refinement.rule, refinement.position});
            } else {
                absorbed[refinement.origin] = true;
            }
        }
        for (PresentationState& own : machine.states) states.push_back(std::move(own));
        machine.states = std::move(states);
    }
    // Remove the absorbed machines and the curved ones left with nothing to
    // move, from the last machine of each rule, so that the positions of
    // the ones still to remove hold; then the absorbed declarations, the
    // same way.
    for (std::size_t index = origins.size(); index > 0U; --index) {
        const MachineOrigin& origin = origins[index - 1U];
        std::vector<PropertyTransition>& machines = sheet.rules[origin.rule].transitions;
        const PropertyTransition& machine = machines[origin.machine];
        const bool idle = origin.curved && machine.states.empty() && machine.property != face_property;
        if (!absorbed[index - 1U] && !idle) continue;
        machines.erase(machines.begin() + static_cast<std::ptrdiff_t>(origin.machine));
    }
    std::sort(absorbed_declarations.begin(), absorbed_declarations.end(), LaterPosition{});
    const std::vector<DeclarationPosition>::iterator unique_end =
        std::unique(absorbed_declarations.begin(), absorbed_declarations.end());
    absorbed_declarations.erase(unique_end, absorbed_declarations.end());
    for (const DeclarationPosition& position : absorbed_declarations) {
        std::vector<RuleDeclaration>& declarations = sheet.rules[position.rule].declarations;
        declarations.erase(declarations.begin() + static_cast<std::ptrdiff_t>(position.declaration));
    }
    std::vector<ThemeRule> kept{};
    kept.reserve(sheet.rules.size());
    for (ThemeRule& rule : sheet.rules) {
        if (!rule_empty(rule)) kept.push_back(std::move(rule));
    }
    sheet.rules = std::move(kept);
    return MotionLoweringOutcome::lowered;
}

} // namespace gui_forms::theme_language
