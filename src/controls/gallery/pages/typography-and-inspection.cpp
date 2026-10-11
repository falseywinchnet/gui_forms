#include "pages.hpp"

#include "gui_forms/controls/typography_specimen/typography_specimen.hpp"
#include "gui_forms/controls/visual_inspector_view/visual_inspector_view.hpp"

namespace gui_forms::gallery::pages {

// Owns the typography page: four TypographySpecimen runs, the
// VisualInspectorView over one of them, and the VisualInspectorOverlay whose
// visibility is one checkable Command shared by a CheckBox and a Button. The
// inspection target is a Value<int> bound to a ChoiceGroup; the diagnostics
// adornments and the live specimen's size are bound Values too.
class TypographyPage final : public gui_forms::Component {
public:
    explicit TypographyPage(const std::shared_ptr<Surface>& body);

private:
    static constexpr std::array<std::string_view, 4> specimen_ids{
        "gallery.typography.specimen.0", "gallery.typography.specimen.1",
        "gallery.typography.specimen.2", "gallery.typography.specimen.3"};
    static constexpr std::string_view toggle_button_id =
        "gallery.typography.overlay.toggle";

    void build_specimens(const std::shared_ptr<Surface>& body);
    void build_policy(const std::shared_ptr<Surface>& body);
    void build_inspector(const std::shared_ptr<Surface>& body);
    void on_overlay_invoked(const CommandInvocation& invocation);
    void on_overlay_state(const CommandState& state);
    void on_target_changed(int index);
    void on_diagnostics_changed(bool visible);
    void on_size_changed(double size);

    Command overlay_{};
    Value<int> target_{0};
    Value<bool> diagnostics_{true};
    Value<double> size_{14.0};
    std::array<std::shared_ptr<TypographySpecimen>, 4> specimens_{};
    std::shared_ptr<VisualInspectorView> inspector_;
    std::shared_ptr<VisualInspectorOverlay> overlay_view_;
    std::shared_ptr<Label> readout_;
};

TypographyPage::TypographyPage(const std::shared_ptr<Surface>& body) {
    (*body).add_at(label("gallery.typography-and-inspection.heading",
                         "TYPOGRAPHY AND INSPECTION", 22.0, 700, shell_blue_dark),
                   {24.0, 18.0, 620.0, 34.0});
    (*body).add_at(label("gallery.typography-and-inspection.lead",
                         "Committed text runs with their logical line box and resolved baseline, and the public visual-inspection snapshot of any retained control.",
                         12.0, 400, muted), {26.0, 52.0, 920.0, 24.0});
    build_specimens(body);
    build_policy(body);
    build_inspector(body);
    // The transparent overlay covers the whole body and never hit-tests.
    overlay_view_ = make_control<VisualInspectorOverlay>(
        StableId("gallery.typography.inspector.overlay"), std::string(specimen_ids[0]));
    (*overlay_view_).set_visible(false);
    (*body).add_at(overlay_view_, {0.0, 0.0, 1000.0, 660.0});

    on(overlay_.invoked(), *this, &TypographyPage::on_overlay_invoked);
    on(overlay_.state_changed(), *this, &TypographyPage::on_overlay_state);
    on(target_.changed(), *this, &TypographyPage::on_target_changed);
    on(diagnostics_.changed(), *this, &TypographyPage::on_diagnostics_changed);
    on(size_.changed(), *this, &TypographyPage::on_size_changed);
}

void TypographyPage::build_specimens(const std::shared_ptr<Surface>& body) {
    std::shared_ptr<LayoutGroup> specimens = group(
        "gallery.typography.specimens",
        "TypographySpecimen · committed runs, line boxes, baselines", {560.0, 330.0});
    (*body).add_at(specimens, {24.0, 92.0, 560.0, 330.0});

    specimens_[0] = make_control<TypographySpecimen>(
        StableId(std::string(specimen_ids[0])), "CONTROL FACE · GALLERY HEADING");
    (*specimens_[0]).set_font({FontRole::control, 12.0, 700, false, 0.24});
    (*specimens_[0]).set_caption("FontRole::control · 12 / 700 · tracking 0.24");
    (*specimens).add_at(specimens_[0], {22.0, 40.0, 516.0, 56.0});

    specimens_[1] = make_control<TypographySpecimen>(
        StableId(std::string(specimen_ids[1])),
        "The quick brown fox jumps over the lazy dog · Ω ß ñ ø");
    (*specimens_[1]).set_font({FontRole::content, 12.0, 400, false});
    (*specimens_[1]).set_caption("FontRole::content · 12 / 400 · UTF-8 run");
    (*specimens).add_at(specimens_[1], {22.0, 104.0, 516.0, 56.0});

    specimens_[2] = make_control<TypographySpecimen>(
        StableId(std::string(specimen_ids[2])), "Resolved baseline at display size");
    (*specimens_[2]).set_font({FontRole::content, 20.0, 400, true});
    (*specimens_[2]).set_caption("FontRole::content · 20 / 400 · italic");
    (*specimens).add_at(specimens_[2], {22.0, 168.0, 516.0, 70.0});

    specimens_[3] = make_control<TypographySpecimen>(
        StableId(std::string(specimen_ids[3])),
        "Wrapped specimen: the size slider below rewrites this run's FontSpec, and the committed layout reports the resulting line boxes.");
    (*specimens_[3]).set_font({FontRole::content, size_.get(), 400, false});
    (*specimens_[3]).set_wrap_text(true);
    (*specimens_[3]).set_caption("FontRole::content · bound size · wrapped");
    (*specimens).add_at(specimens_[3], {22.0, 246.0, 516.0, 72.0});
    for (const std::shared_ptr<TypographySpecimen>& specimen : specimens_) {
        (*specimen).set_diagnostics_visible(diagnostics_.get());
    }
}

void TypographyPage::build_policy(const std::shared_ptr<Surface>& body) {
    std::shared_ptr<LayoutGroup> policy = group(
        "gallery.typography.policy", "Inspection target and review policy",
        {560.0, 180.0});
    (*body).add_at(policy, {24.0, 442.0, 560.0, 180.0});

    std::shared_ptr<ChoiceGroup> target =
        make_control<ChoiceGroup>(StableId("gallery.typography.target"));
    (*target).set_items({
        {0, "Heading", {}, {}, "Inspect the heading specimen"},
        {1, "Body", {}, {}, "Inspect the body specimen"},
        {2, "Display", {}, {}, "Inspect the display specimen"},
        {3, "Wrapped", {}, {}, "Inspect the wrapped specimen"},
    });
    (*target).set_accessible_name("Inspection target");
    (*target).bind(target_);
    (*policy).add_at(target, {22.0, 40.0, 516.0, 34.0});

    std::shared_ptr<CheckBox> diagnostics = make_control<CheckBox>(
        StableId("gallery.typography.diagnostics"), "Line box and baseline adornments");
    (*diagnostics).bind(diagnostics_);
    (*policy).add_at(diagnostics, {22.0, 88.0, 250.0, 28.0});

    overlay_.set_text("Show inspector overlay");
    overlay_.set_description("Draw the target's arranged, visual and clipped bounds");
    overlay_.set_checked(false);
    std::shared_ptr<CheckBox> overlay = make_control<CheckBox>(
        StableId("gallery.typography.overlay"), "Show inspector overlay");
    (*overlay).set_indicator_style(ChoiceIndicatorStyle::toggle);
    (*overlay).bind(overlay_);
    (*policy).add_at(overlay, {290.0, 88.0, 248.0, 28.0});
    std::shared_ptr<Button> toggle = make_control<Button>(
        StableId(std::string(toggle_button_id)), "Toggle overlay");
    (*toggle).set_visual_style(ButtonVisualStyle::command);
    (*toggle).bind(overlay_);
    (*policy).add_at(toggle, {22.0, 128.0, 180.0, 32.0});

    (*policy).add_at(label("gallery.typography.size.label", "Wrapped specimen size · 10–28",
                           10.0, 700, shell_blue_dark),
                     {290.0, 120.0, 248.0, 18.0});
    std::shared_ptr<TrackBar> size =
        make_control<TrackBar>(StableId("gallery.typography.size"));
    (*size).set_range(10.0, 28.0);
    (*size).set_tick_frequency(2U);
    (*size).set_accessible_name("Wrapped specimen font size");
    (*size).bind(size_);
    (*policy).add_at(size, {290.0, 136.0, 248.0, 36.0});
}

void TypographyPage::build_inspector(const std::shared_ptr<Surface>& body) {
    std::shared_ptr<LayoutGroup> inspection = group(
        "gallery.typography.inspection",
        "VisualInspectorView · public inspection snapshot", {350.0, 530.0});
    (*body).add_at(inspection, {604.0, 92.0, 350.0, 530.0});
    inspector_ = make_control<VisualInspectorView>(
        StableId("gallery.typography.inspector"), std::string(specimen_ids[0]));
    (*inspector_).set_accessible_name("Visual inspection of the selected specimen");
    (*inspection).add_at(inspector_, {20.0, 38.0, 310.0, 440.0});
    readout_ = label("gallery.typography.readout",
                     "Inspecting " + std::string(specimen_ids[0]), 10.0, 400, muted);
    (*readout_).set_text_wrapping(TextWrapping::word);
    (*readout_).set_vertical_alignment(VerticalAlignment::near);
    (*inspection).add_at(readout_, {20.0, 486.0, 310.0, 36.0});
}

void TypographyPage::on_overlay_invoked(const CommandInvocation& invocation) {
    // The CheckBox already moved the command's checked state before invoking
    // it; only the plain Button needs the invocation to flip the state.
    if (invocation.source_id != toggle_button_id) return;
    overlay_.set_checked(!overlay_.state().checked);
}

void TypographyPage::on_overlay_state(const CommandState& state) {
    const bool visible = state.checkable && state.checked;
    if ((*overlay_view_).visible() == visible) return;
    (*overlay_view_).set_visible(visible);
    if (visible) (*overlay_view_).refresh();
}

void TypographyPage::on_target_changed(const int index) {
    if (index < 0 || static_cast<std::size_t>(index) >= specimen_ids.size()) return;
    const std::string target(specimen_ids[static_cast<std::size_t>(index)]);
    (*inspector_).set_target_stable_id(target);
    (*overlay_view_).set_target_stable_id(target);
    (*inspector_).refresh();
    (*overlay_view_).refresh();
    (*readout_).set_text("Inspecting " + target);
}

void TypographyPage::on_diagnostics_changed(const bool visible) {
    for (const std::shared_ptr<TypographySpecimen>& specimen : specimens_) {
        (*specimen).set_diagnostics_visible(visible);
    }
}

void TypographyPage::on_size_changed(const double size) {
    FontSpec font = (*specimens_[3]).font();
    font.size = size;
    (*specimens_[3]).set_font(font);
    (*inspector_).refresh();
}

void add_typography_and_inspection(const std::shared_ptr<Surface>& page,
                                   const std::shared_ptr<PageContext>& context) {
    (*context).components.add(std::make_shared<TypographyPage>(page));
}

}  // namespace gui_forms::gallery::pages
