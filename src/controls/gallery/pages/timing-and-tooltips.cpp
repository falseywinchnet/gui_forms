#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_timing_and_tooltips(const std::shared_ptr<Surface>& page,
                             const std::shared_ptr<PageContext>& context) {
    (*page).add_at(label("gallery.timing.heading", "TIMING AND TOOLTIPS", 22.0,
                       700, shell_blue_dark), {24.0, 18.0, 620.0, 34.0});
    (*page).add_at(label("gallery.timing.lead",
                       "UI-thread component deadlines, coalesced cadence, focus/hover help, owner revocation, and retained overlays.",
                       12.0, 400, muted), {26.0, 52.0, 920.0, 24.0});

    std::shared_ptr<LayoutGroup> timer_group = group("gallery.timing.timer.group",
                             "Timer · deterministic UI-loop component", {930.0, 248.0});
    (*page).add_at(timer_group, {24.0, 92.0, 930.0, 248.0});
    (*context).timer_status = label("gallery.timing.tick.status",
                                  "Idle · enter this page to start", 12.0, 700, green);
    (*timer_group).add_at((*context).timer_status, {22.0, 38.0, 510.0, 28.0});
    (*context).timer_start = make_control<Button>(StableId("gallery.timing.start"),
                                                "Start timer");
    (*(*context).timer_start).set_visual_style(ButtonVisualStyle::accent);
    (*context).timer_stop = make_control<Button>(StableId("gallery.timing.stop"),
                                               "Stop timer");
    (*timer_group).add_at((*context).timer_start, {22.0, 78.0, 132.0, 34.0});
    (*timer_group).add_at((*context).timer_stop, {166.0, 78.0, 132.0, 34.0});
    (*timer_group).add_at(label("gallery.timing.interval.label", "Interval", 10.0,
                              700, shell_blue_dark), {326.0, 76.0, 80.0, 20.0});
    (*context).timer_interval = make_control<TrackBar>(
        StableId("gallery.timing.interval"));
    (*(*context).timer_interval).set_range(80.0, 600.0);
    (*(*context).timer_interval).set_value(250.0);
    (*(*context).timer_interval).set_tick_frequency(4U);
    (*(*context).timer_interval).set_accessible_name("UI timer interval in milliseconds");
    (*timer_group).add_at((*context).timer_interval, {326.0, 98.0, 260.0, 44.0});
    (*context).timer_progress = make_control<ProgressBar>(
        StableId("gallery.timing.progress"));
    (*(*context).timer_progress).set_range(0.0, 100.0);
    (*(*context).timer_progress).set_value(0.0);
    (*(*context).timer_progress).set_accessible_name("Timer cycle");
    (*timer_group).add_at((*context).timer_progress, {22.0, 164.0, 564.0, 24.0});
    (*context).timer_motion = make_control<ScaledPanel>(
        StableId("gallery.timing.motion"), Size{294.0, 170.0});
    (*(*context).timer_motion).set_background(Color::rgba(247, 249, 251));
    (*(*context).timer_motion).set_border_style(BorderStyle::line);
    (*(*context).timer_motion).add_at(
        label("gallery.timing.motion.caption", "WINDOW-QUEUE MOTION · 4 Hz",
              10.0, 700, muted),
        {14.0, 8.0, 266.0, 24.0});
    std::shared_ptr<gui_forms::Panel> motion_rule = make_control<Panel>(
        StableId("gallery.timing.motion.rule"));
    (*motion_rule).set_background(rule);
    (*(*context).timer_motion).add_at(motion_rule, {14.0, 84.0, 266.0, 1.0});
    (*context).timer_motion_target = make_control<Button>(
        StableId("gallery.timing.motion.target"), "Moving tooltip target");
    (*(*context).timer_motion_target).set_visual_style(ButtonVisualStyle::accent);
    (*(*context).timer_motion_target).set_style(page_style());
    (*(*context).timer_motion).add_at(
        (*context).timer_motion_target, {14.0, 39.0, 168.0, 34.0});
    (*timer_group).add_at((*context).timer_motion, {612.0, 38.0, 294.0, 170.0});
    std::shared_ptr<Label> cadence = label("gallery.timing.cadence.note",
                         "Late deadlines coalesce to one ordered tick; no catch-up burst and no hidden paint control.",
                         10.0, 400, muted);
    (*cadence).set_text_wrapping(TextWrapping::word);
    (*cadence).set_vertical_alignment(VerticalAlignment::near);
    (*timer_group).add_at(cadence, {22.0, 200.0, 564.0, 36.0});

    std::shared_ptr<LayoutGroup> tips_group = group("gallery.timing.tooltip.group",
                            "ToolTip · attached provider and retained overlay", {930.0, 264.0});
    (*page).add_at(tips_group, {24.0, 362.0, 930.0, 264.0});
    std::shared_ptr<gui_forms::Button> hover = make_control<Button>(StableId("gallery.timing.tooltip.hover"),
                                      "Hover for detail");
    std::shared_ptr<gui_forms::Button> keyboard = make_control<Button>(StableId("gallery.timing.tooltip.focus"),
                                         "Tab here for help");
    std::shared_ptr<gui_forms::Button> persistent = make_control<Button>(
        StableId("gallery.timing.tooltip.persistent"), "Show persistent help");
    (*persistent).set_visual_style(ButtonVisualStyle::accent);
    (*keyboard).set_visual_style(ButtonVisualStyle::command);
    (*tips_group).add_at(hover, {22.0, 44.0, 176.0, 36.0});
    (*tips_group).add_at(keyboard, {214.0, 44.0, 176.0, 36.0});
    (*tips_group).add_at(persistent, {406.0, 44.0, 190.0, 36.0});
    (*context).tooltip_disabled_target = make_control<Button>(
        StableId("gallery.timing.tooltip.disabled"), "Disabled target");
    (*(*context).tooltip_disabled_target).set_enabled(false);
    (*context).tooltip_show_disabled = make_control<Button>(
        StableId("gallery.timing.tooltip.show-disabled"), "Explain disabled control");
    (*tips_group).add_at((*context).tooltip_disabled_target,
                       {22.0, 102.0, 176.0, 36.0});
    (*tips_group).add_at((*context).tooltip_show_disabled,
                       {214.0, 102.0, 206.0, 36.0});
    std::shared_ptr<Label> policy = label("gallery.timing.tooltip.policy",
                        "Hover: 420 ms · Reshow: 90 ms · Auto-pop: 4 s\nKeyboard focus uses the same provider. Popups follow moving targets and disappear with their owner.",
                        10.0, 400, muted);
    (*policy).set_text_wrapping(TextWrapping::word);
    (*policy).set_vertical_alignment(VerticalAlignment::near);
    (*tips_group).add_at(policy, {22.0, 166.0, 574.0, 70.0});
    std::shared_ptr<Label> proof = label("gallery.timing.tooltip.proof",
                       "TRY IT\nHover each target, tab through the controls, adjust the interval, and watch the anchored moving tooltip.",
                       11.0, 700, shell_blue_dark);
    (*proof).set_text_wrapping(TextWrapping::word);
    (*proof).set_vertical_alignment(VerticalAlignment::near);
    (*tips_group).add_at(proof, {628.0, 44.0, 270.0, 126.0});
    (*context).provider_error_target = make_control<TextBox>(
        StableId("gallery.timing.provider.field"), "192.168.1.96:5555");
    (*(*context).provider_error_target).set_accessible_name("Remote endpoint");
    (*context).provider_error_toggle = make_control<Button>(
        StableId("gallery.timing.provider.toggle"), "Clear");
    (*(*context).provider_error_toggle).set_visual_style(ButtonVisualStyle::command);
    (*tips_group).add_at((*context).provider_error_target,
                       {628.0, 174.0, 204.0, 32.0});
    (*tips_group).add_at((*context).provider_error_toggle,
                       {840.0, 174.0, 66.0, 32.0});
    std::shared_ptr<Label> provider_note = label(
        "gallery.timing.provider.note",
        "ErrorProvider adornment · focus the field and press F1",
        9.0, 400, muted);
    (*tips_group).add_at(provider_note, {628.0, 210.0, 278.0, 22.0});
}

} // namespace gui_forms::gallery::pages
