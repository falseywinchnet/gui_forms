// Control-owned motion values (T2 plan, phase E3): the curves, their closed
// forms, the settled conditions and the moves. The clock that advances them
// is motion_clock.cpp.

#include "gui_forms/engine/motion_value/motion_value.hpp"

#include "gui_forms/control.hpp"
#include "gui_forms/engine/engine_math/engine_math.hpp"
#include "gui_forms/engine/motion_clock/motion_clock.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <cmath>

// The same sampling times give the same values on every host: the closed
// forms use only + − × ÷, sqrt and the engine's own exponential, logarithm,
// sine and cosine, without fused multiply-add (T2 plan, "Numerics").
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace gui_forms {
namespace {

// The back-out constant whose single overshoot is exactly 8 %: the
// overshoot of 1 + (c + 1) u³ + c u² is 4c³ / (27 (c + 1)²), which is 0.08
// for c = 1.5.
constexpr double overshoot_back = 1.5;
// The exponential approach easing's rate over the duration.
constexpr double approach_rate = 5.0;
constexpr double pi = 3.14159265358979323846;
// Targets and velocities beyond this are refused, so that no gap or
// product of a velocity and a time constant overflows.
constexpr double largest_motion_magnitude = 1e12;

double seconds_of(FrameInterval interval) noexcept {
    const double seconds = std::chrono::duration<double>(interval).count();
    return seconds;
}

MotionCurve normalized(MotionCurve curve) noexcept {
    if (curve.duration < FrameInterval::zero()) curve.duration = FrameInterval::zero();
    if (curve.duration > motion_duration_ceiling) curve.duration = motion_duration_ceiling;
    if (!(curve.overshoot > 0.0)) curve.overshoot = 0.0;
    if (curve.overshoot > maximum_motion_overshoot) curve.overshoot = maximum_motion_overshoot;
    return curve;
}

bool admissible(double value) noexcept {
    const bool fits = std::isfinite(value) && std::fabs(value) <= largest_motion_magnitude;
    return fits;
}

// d/dp of motion_easing, for a timed move's velocity.
double easing_slope(MotionEasing easing, double p) noexcept {
    switch (easing) {
    case MotionEasing::smoothstep: {
        const double slope = 6.0 * p * (1.0 - p);
        return slope;
    }
    case MotionEasing::exponential_approach: {
        const double normalization = 1.0 - engine_exponential(-approach_rate);
        const double slope = approach_rate * engine_exponential(-approach_rate * p) / normalization;
        return slope;
    }
    case MotionEasing::linear:
        return 1.0;
    case MotionEasing::ease: {
        if (p < 0.5) {
            const double slope = 12.0 * p * p;
            return slope;
        }
        const double u = 2.0 - 2.0 * p;
        const double slope = 3.0 * u * u;
        return slope;
    }
    case MotionEasing::overshoot: {
        const double u = p - 1.0;
        const double slope = 3.0 * (overshoot_back + 1.0) * u * u + 2.0 * overshoot_back * u;
        return slope;
    }
    }
    return 1.0;
}

} // namespace

MotionCurve MotionCurve::timed(FrameInterval duration, MotionEasing easing) noexcept {
    MotionCurve curve{};
    curve.kind = MotionKind::timed;
    curve.duration = duration;
    curve.easing = easing;
    const MotionCurve result = normalized(curve);
    return result;
}

MotionCurve MotionCurve::follow(FrameInterval time_constant) noexcept {
    MotionCurve curve{};
    curve.kind = MotionKind::follow;
    curve.duration = time_constant;
    const MotionCurve result = normalized(curve);
    return result;
}

MotionCurve MotionCurve::spring(FrameInterval response, double overshoot) noexcept {
    MotionCurve curve{};
    curve.kind = MotionKind::spring;
    curve.duration = response;
    curve.overshoot = overshoot;
    const MotionCurve result = normalized(curve);
    return result;
}

MotionCurve MotionCurve::decay(FrameInterval time_constant) noexcept {
    MotionCurve curve{};
    curve.kind = MotionKind::decay;
    curve.duration = time_constant;
    const MotionCurve result = normalized(curve);
    return result;
}

double motion_easing(MotionEasing easing, double progress) noexcept {
    if (!(progress > 0.0)) return 0.0;
    if (progress >= 1.0) return 1.0;
    const double p = progress;
    switch (easing) {
    case MotionEasing::smoothstep: {
        const double eased = p * p * (3.0 - 2.0 * p);
        return eased;
    }
    case MotionEasing::exponential_approach: {
        const double normalization = 1.0 - engine_exponential(-approach_rate);
        const double eased = (1.0 - engine_exponential(-approach_rate * p)) / normalization;
        return eased;
    }
    case MotionEasing::linear:
        return p;
    case MotionEasing::ease: {
        if (p < 0.5) {
            const double eased = 4.0 * p * p * p;
            return eased;
        }
        const double u = 2.0 - 2.0 * p;
        const double eased = 1.0 - u * u * u * 0.5;
        return eased;
    }
    case MotionEasing::overshoot: {
        const double u = p - 1.0;
        const double eased = 1.0 + (overshoot_back + 1.0) * u * u * u + overshoot_back * u * u;
        return eased;
    }
    }
    return p;
}

// -- segments --------------------------------------------------------------------

MotionValue::Segment MotionValue::make_segment(const MotionCurve& curve, FrameTime start, double origin,
                                               double velocity, double target) noexcept {
    Segment segment{};
    segment.start = start;
    segment.origin = origin;
    segment.origin_velocity = velocity;
    segment.target = target;
    segment.curve = curve;
    segment.seconds = seconds_of(curve.duration);
    if (segment.seconds <= 0.0) return segment;
    if (curve.kind == MotionKind::follow || curve.kind == MotionKind::decay) {
        segment.rate = 1.0 / segment.seconds;
    } else if (curve.kind == MotionKind::spring) {
        segment.natural = 1.0 / segment.seconds;
        if (curve.overshoot > 0.0) {
            // The damping ratio whose step response from rest overshoots by
            // M: M = e^(−πζ/√(1 − ζ²)), so ζ = −ln M / √(π² + ln² M). For
            // M = 8 % it is about 0.627.
            const double logarithm = engine_logarithm(curve.overshoot);
            const double zeta = -logarithm / std::sqrt(pi * pi + logarithm * logarithm);
            segment.rate = zeta * segment.natural;
            segment.damped = segment.natural * std::sqrt(1.0 - zeta * zeta);
        } else {
            segment.rate = segment.natural;
        }
    }
    return segment;
}

// The closed forms, with the gap g = position − target and A = origin −
// target:
// - follow and decay: g = A e^(−rt), the velocity −rA e^(−rt);
// - spring, critically damped: g = (A + Bt) e^(−ωt), B = v₀ + ωA;
// - spring, under-damped: g = e^(−at)(A cos ω_d t + C sin ω_d t), a = ζω,
//   C = (v₀ + aA) / ω_d;
// - timed: origin + (target − origin) · easing(t / duration).
MotionValue::Sample MotionValue::evaluate(const Segment& segment, double elapsed) noexcept {
    Sample sample{};
    sample.position = segment.target;
    if (segment.seconds <= 0.0) {
        sample.elapsed_all = true;
        return sample;
    }
    const double gap = segment.origin - segment.target;
    switch (segment.curve.kind) {
    case MotionKind::timed: {
        if (elapsed >= segment.seconds) {
            sample.elapsed_all = true;
            return sample;
        }
        const double progress = elapsed / segment.seconds;
        const double travel = segment.target - segment.origin;
        sample.position = segment.origin + travel * motion_easing(segment.curve.easing, progress);
        sample.velocity = travel * easing_slope(segment.curve.easing, progress) / segment.seconds;
        return sample;
    }
    case MotionKind::follow:
    case MotionKind::decay: {
        const double factor = engine_exponential(-segment.rate * elapsed);
        sample.position = segment.target + gap * factor;
        sample.velocity = -segment.rate * gap * factor;
        return sample;
    }
    case MotionKind::spring: {
        const double envelope = engine_exponential(-segment.rate * elapsed);
        // Beyond e^-745 the envelope is zero: the spring stands at its target.
        if (envelope == 0.0) return sample;
        if (segment.damped == 0.0) {
            const double omega = segment.natural;
            const double b = segment.origin_velocity + omega * gap;
            const double linear = gap + b * elapsed;
            sample.position = segment.target + linear * envelope;
            sample.velocity = (b - omega * linear) * envelope;
            return sample;
        }
        const double a = segment.rate;
        const double w = segment.damped;
        const double c = (segment.origin_velocity + a * gap) / w;
        const EngineSineCosine phase = engine_sine_cosine(w * elapsed);
        const double oscillation = gap * phase.cosine + c * phase.sine;
        sample.position = segment.target + envelope * oscillation;
        const double cosine_part = (c * w - a * gap) * phase.cosine;
        const double sine_part = (a * c + gap * w) * phase.sine;
        sample.velocity = envelope * (cosine_part - sine_part);
        return sample;
    }
    }
    return sample;
}

// -- the value ------------------------------------------------------------------

MotionValue::MotionValue(Control& owner, MotionCurve curve, double initial) noexcept
    : owner_(&owner), curve_(normalized(curve)) {
    const double start = admissible(initial) ? initial : 0.0;
    current_ = start;
    segment_.origin = start;
    segment_.target = start;
}

MotionValue::~MotionValue() {
    if (clock_ != nullptr) (*clock_).release(*this);
}

void MotionValue::set_curve(MotionCurve curve) noexcept {
    curve_ = normalized(curve);
}

bool MotionValue::set_extent(double logical_units) noexcept {
    if (!std::isfinite(logical_units) || logical_units < minimum_motion_extent ||
        logical_units > maximum_motion_extent) {
        return false;
    }
    extent_ = logical_units;
    return true;
}

MotionValue::Sample MotionValue::state_at(FrameTime now) const noexcept {
    if (!moving_) {
        Sample rest{};
        rest.position = current_;
        rest.elapsed_all = true;
        return rest;
    }
    const double since = seconds_of(now - segment_.start);
    const double elapsed = std::max(0.0, since);
    const Sample sample = evaluate(segment_, elapsed);
    return sample;
}

MotionStart MotionValue::begin(const Segment& next, double position, double velocity) {
    Window* const window = (*owner_).attached_window();
    if (window == nullptr) {
        if (clock_ != nullptr) (*clock_).release(*this);
        segment_ = next;
        moving_ = true;
        stop();
        return MotionStart::detached;
    }
    MotionClock& clock = (*window).motion_clock();
    if (clock.suppressed() || next.seconds <= 0.0) {
        if (clock_ != nullptr) (*clock_).release(*this);
        segment_ = next;
        moving_ = true;
        stop();
        (*window).metrics().record_motion_finished();
        return MotionStart::finished;
    }
    // Enlisting may throw; nothing has changed yet.
    clock.enlist(*this);
    segment_ = next;
    current_ = position;
    velocity_ = velocity;
    moving_ = true;
    return MotionStart::started;
}

MotionStart MotionValue::animate_to(double target) {
    if (!admissible(target)) return MotionStart::refused;
    if (moving_ && segment_.target == target) return MotionStart::unchanged;
    if (!moving_ && current_ == target) return MotionStart::unchanged;
    const Window* const window = (*owner_).attached_window();
    const FrameTime now = window != nullptr ? (*window).motion_clock().now() : FrameTime{};
    const Sample here = state_at(now);
    const Segment next = make_segment(curve_, now, here.position, here.velocity, target);
    const MotionStart started = begin(next, here.position, here.velocity);
    return started;
}

MotionStart MotionValue::fling(double velocity) {
    if (curve_.kind != MotionKind::decay || !admissible(velocity)) return MotionStart::refused;
    if (!moving_ && velocity == 0.0) return MotionStart::unchanged;
    const Window* const window = (*owner_).attached_window();
    const FrameTime now = window != nullptr ? (*window).motion_clock().now() : FrameTime{};
    const Sample here = state_at(now);
    // Friction stops it where the velocity carries it: the integral of
    // v e^(−t/τ) is vτ.
    const double travel = velocity * seconds_of(curve_.duration);
    const double rest = here.position + travel;
    const Segment next = make_segment(curve_, now, here.position, velocity, rest);
    const MotionStart started = begin(next, here.position, velocity);
    return started;
}

MotionStart MotionValue::adopt(const MotionValue& source) {
    if (&source == this) return MotionStart::unchanged;
    if (!source.moving_) {
        jump_to(source.current_);
        return MotionStart::finished;
    }
    const Window* const window = (*owner_).attached_window();
    const FrameTime now = window != nullptr ? (*window).motion_clock().now() : FrameTime{};
    const Sample here = source.state_at(now);
    const MotionStart started = begin(source.segment_, here.position, here.velocity);
    // After begin, which may throw and then leaves this value unchanged.
    curve_ = source.curve_;
    return started;
}

void MotionValue::finish() noexcept {
    if (clock_ != nullptr) (*clock_).release(*this);
    stop();
}

void MotionValue::jump_to(double value) noexcept {
    if (!admissible(value)) return;
    finish();
    current_ = value;
    segment_.origin = value;
    segment_.target = value;
}

bool MotionValue::advance(FrameTime now, double scale) noexcept {
    if (!moving_) return true;
    const Sample sample = state_at(now);
    // The tolerance in value units: half a device pixel over the extent at
    // the window's scale.
    const double device_units = extent_ * scale;
    const double tolerance = motion_settle_tolerance / device_units;
    const double gap = std::fabs(sample.position - segment_.target);
    bool settled = false;
    switch (segment_.curve.kind) {
    case MotionKind::timed:
        settled = sample.elapsed_all;
        break;
    case MotionKind::follow:
    case MotionKind::decay:
        settled = gap <= tolerance;
        break;
    case MotionKind::spring: {
        const double frame_travel = std::fabs(sample.velocity) * motion_settle_frame_seconds;
        settled = gap <= tolerance && frame_travel <= tolerance;
        break;
    }
    }
    if (!std::isfinite(sample.position) || !std::isfinite(sample.velocity)) settled = true;
    if (settled) {
        stop();
        return true;
    }
    current_ = sample.position;
    velocity_ = sample.velocity;
    return false;
}

void MotionValue::stop() noexcept {
    if (!moving_) return;
    moving_ = false;
    current_ = segment_.target;
    velocity_ = 0.0;
}

} // namespace gui_forms
