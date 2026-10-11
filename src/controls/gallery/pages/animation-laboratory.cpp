#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_animation(const std::shared_ptr<Surface>& page,
                   const std::shared_ptr<PageContext>& context) {
    class ToggleMotionPauseClick final {
    public:
        explicit ToggleMotionPauseClick(
            std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<PageContext> context = context_.lock();
            if (!context || !(*context).live_motion) return;
            (*context).user_paused = !(*context).user_paused;
            (*context).apply_motion_policy();
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    class SetReducedMotion final {
    public:
        explicit SetReducedMotion(std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()(bool enabled) const
        {
            if (const std::shared_ptr<PageContext> context = context_.lock()) {
                (*context).reduced_motion = enabled;
                (*context).apply_motion_policy();
            }
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    (*page).add_at(label("gallery.animation.heading", "ANIMATION LABORATORY", 22.0, 700,
                       shell_blue_dark), {24.0, 18.0, 520.0, 34.0});
    (*page).add_at(label("gallery.animation.lead",
                       "Eight public easing curves, alternate direction, retained frame scheduling, hidden-page suspension.",
                       12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});
    std::shared_ptr<gui_forms::EasingPreview> easing = make_control<EasingBoard>(StableId("gallery.animation.easing"));
    (*easing).set_title("TIMELINE AND EASING LAB");
    (*easing).set_style(page_style());
    (*easing).set_tracks({
        {EasingCurve::linear, "Linear", accent},
        {EasingCurve::ease_in, "Ease in", accent},
        {EasingCurve::ease_out, "Ease out", accent},
        {EasingCurve::ease_in_out, "Ease in/out", violet},
        {EasingCurve::smooth_step, "Smooth step", violet},
        {EasingCurve::back_out, "Back out", violet},
        {EasingCurve::bounce_out, "Bounce", orange},
        {EasingCurve::elastic_out, "Elastic", orange},
    });
    (*page).add_at(easing, {24.0, 92.0, 930.0, 340.0});
    (*context).easing = easing;

    std::shared_ptr<LayoutGroup> controls = group("gallery.animation.controls",
                          "Animation controls and motion policy", {930.0, 168.0});
    (*page).add_at(controls, {24.0, 454.0, 930.0, 168.0});
    std::shared_ptr<gui_forms::Button> pause = make_control<Button>(StableId("gallery.animation.pause"), "Pause motion");
    (*pause).set_visual_style(ButtonVisualStyle::accent);
    (*controls).add_at(pause, {24.0, 42.0, 160.0, 36.0});
    (*context).motion_pause = pause;
    const std::weak_ptr<PageContext> weak_context = context;
    (*context).subscriptions.push_back((*pause).clicked().subscribe(
        *easing, ToggleMotionPauseClick(weak_context)));
    std::shared_ptr<Label> copy = label("gallery.animation.policy",
                      "Animation is deadline-driven and quiescent when hidden or paused. No perpetual redraw loop.",
                      12.0, 400, ink);
    (*controls).add_at(copy, {210.0, 46.0, 650.0, 28.0});
    std::shared_ptr<gui_forms::CheckBox> reduced = make_control<CheckBox>(StableId("gallery.animation.reduced"),
                                          "Reduced-motion substitution");
    (*reduced).set_indicator_style(ChoiceIndicatorStyle::toggle);
    (*controls).add_at(reduced, {24.0, 96.0, 310.0, 28.0});
    (*context).subscriptions.push_back((*reduced).checked_changed().subscribe(
        *easing, SetReducedMotion(weak_context)));
}

} // namespace gui_forms::gallery::pages
