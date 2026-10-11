#include "pages.hpp"

namespace gui_forms::gallery::pages {

// Owns the animation laboratory: the EasingPreview board, the pause Command
// its button binds, and the reduced-motion Value its check box binds. Both
// feed the shell-wide MotionPolicy retained in PageContext, which the status
// strip's "Live animation" gate also drives.
class AnimationPage final : public gui_forms::Component {
public:
    AnimationPage(const std::shared_ptr<Surface>& body,
                  const std::shared_ptr<PageContext>& context);

private:
    void on_pause();
    void on_reduced_changed(bool reduced);

    std::weak_ptr<PageContext> context_;
    Command pause_{};
    Value<bool> reduced_{false};
};

AnimationPage::AnimationPage(const std::shared_ptr<Surface>& body,
                             const std::shared_ptr<PageContext>& context)
    : context_(context) {
    (*body).add_at(label("gallery.animation.heading", "ANIMATION LABORATORY", 22.0, 700,
                         shell_blue_dark), {24.0, 18.0, 520.0, 34.0});
    (*body).add_at(label("gallery.animation.lead",
                         "Eight public easing curves, alternate direction, retained frame scheduling, hidden-page suspension.",
                         12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});
    std::shared_ptr<EasingBoard> easing =
        make_control<EasingBoard>(StableId("gallery.animation.easing"));
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
    (*body).add_at(easing, {24.0, 92.0, 930.0, 340.0});
    (*context).easing = easing;

    std::shared_ptr<LayoutGroup> controls = group(
        "gallery.animation.controls", "Animation controls and motion policy",
        {930.0, 168.0});
    (*body).add_at(controls, {24.0, 454.0, 930.0, 168.0});

    pause_.set_text("Pause motion");
    pause_.set_description("Pause or resume the retained animation surfaces");
    std::shared_ptr<Button> pause =
        make_control<Button>(StableId("gallery.animation.pause"), "Pause motion");
    (*pause).set_visual_style(ButtonVisualStyle::accent);
    (*pause).bind(pause_);
    (*controls).add_at(pause, {24.0, 42.0, 160.0, 36.0});
    (*context).motion_pause = pause;
    on(pause_.invoked(), *this, &AnimationPage::on_pause);

    std::shared_ptr<Label> copy = label(
        "gallery.animation.policy",
        "Animation is deadline-driven and quiescent when hidden or paused. No perpetual redraw loop.",
        12.0, 400, ink);
    (*controls).add_at(copy, {210.0, 46.0, 650.0, 28.0});

    std::shared_ptr<CheckBox> reduced = make_control<CheckBox>(
        StableId("gallery.animation.reduced"), "Reduced-motion substitution");
    (*reduced).set_indicator_style(ChoiceIndicatorStyle::toggle);
    (*reduced).bind(reduced_);
    (*controls).add_at(reduced, {24.0, 96.0, 310.0, 28.0});
    on(reduced_.changed(), *this, &AnimationPage::on_reduced_changed);
}

void AnimationPage::on_pause() {
    const std::shared_ptr<PageContext> context = context_.lock();
    if (!context || !(*context).live_motion) return;
    (*context).user_paused = !(*context).user_paused;
    (*context).apply_motion_policy();
}

void AnimationPage::on_reduced_changed(const bool reduced) {
    const std::shared_ptr<PageContext> context = context_.lock();
    if (!context) return;
    (*context).reduced_motion = reduced;
    (*context).apply_motion_policy();
}

void add_animation(const std::shared_ptr<Surface>& page,
                   const std::shared_ptr<PageContext>& context) {
    (*context).components.add(std::make_shared<AnimationPage>(page, context));
}

} // namespace gui_forms::gallery::pages
