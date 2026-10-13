#include "gui_forms/anatomy.hpp"
#include "gui_forms/basic_controls.hpp"
#include "gui_forms/container_controls.hpp"
#include "gui_forms/material.hpp"
#include "gui_forms/range_controls.hpp"
#include "gui_forms/window.hpp"

#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

using namespace gui_forms;

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

bool near(double left, double right) {
    return std::abs(left - right) <= 1.0e-9;
}

// Every published standard state must be declared by the control's anatomy.
void require_declared_states(const Control& control) {
    const ControlAnatomy& anatomy = control.anatomy();
    const AnatomyStateSet active = control.anatomy_states();
    for (std::size_t index = 0; index < anatomy_state_count; ++index) {
        const AnatomyState state = static_cast<AnatomyState>(index);
        if (active.contains(state)) {
            require(declares_state(anatomy, state),
                    std::string(anatomy.control_id) + " publishes undeclared state " +
                        std::string(anatomy_state_name(state)));
        }
    }
    const unsigned int declared_custom_bits =
        (1U << static_cast<unsigned int>(anatomy.custom_states.size())) - 1U;
    const unsigned int active_custom_bits = active.custom_bits();
    require((active_custom_bits & ~declared_custom_bits) == 0U,
            std::string(anatomy.control_id) + " publishes an undeclared custom state");
}

// Every present part lies inside the control's region, except for parts
// that are allowed to reach past it (none of the range controls today).
void require_parts_within_region(const Control& control) {
    const ControlAnatomy& anatomy = control.anatomy();
    std::vector<PartGeometry> geometry(anatomy.parts.size());
    control.fill_part_geometry(geometry);
    const Rect region = control.client_rectangle();
    for (std::size_t index = 0; index < geometry.size(); ++index) {
        if (!geometry[index].present) {
            require(geometry[index].bounds.empty(),
                    "an absent part must publish empty bounds");
            continue;
        }
        const Rect bounds = geometry[index].bounds;
        const bool inside = bounds.x >= region.x - 0.5 &&
                            bounds.y >= region.y - 0.5 &&
                            bounds.right() <= region.right() + 0.5 &&
                            bounds.bottom() <= region.bottom() + 0.5;
        require(inside, std::string(anatomy.control_id) + " part " +
                            std::string(anatomy.parts[index].name) +
                            " must lie within the control region");
    }
}

void test_names_are_kebab_case_and_round_trip() {
    for (std::size_t index = 0; index < part_role_count; ++index) {
        const PartRole role = static_cast<PartRole>(index);
        const std::string_view name = part_role_name(role);
        require(is_kebab_case_name(name), "part role names must be kebab-case");
        const std::optional<PartRole> parsed = part_role_from_name(name);
        require(parsed.has_value() && *parsed == role,
                "part role names must round-trip");
    }
    for (std::size_t index = 0; index < anatomy_state_count; ++index) {
        const AnatomyState state = static_cast<AnatomyState>(index);
        const std::string_view name = anatomy_state_name(state);
        require(is_kebab_case_name(name), "state names must be kebab-case");
        const std::optional<AnatomyState> parsed = anatomy_state_from_name(name);
        require(parsed.has_value() && *parsed == state,
                "state names must round-trip");
    }
    require(part_role_name(PartRole::count).empty() &&
                anatomy_state_name(AnatomyState::count).empty(),
            "out-of-range enumerators must have no name");
    require(anatomy_state_name(AnatomyState::drop_accepted) == "drop-accepted" &&
                part_role_name(PartRole::preselect) == "preselect" &&
                anatomy_state_name(AnatomyState::preselect) == "preselect",
            "snake_case enumerators must map to kebab-case names");
    require(!part_role_from_name("Thumb").has_value() &&
                !anatomy_state_from_name("drop_accepted").has_value(),
            "lookup must be exact and case-sensitive");

    require(is_kebab_case_name("scroll-thumb") && is_kebab_case_name("a1-b2"),
            "kebab-case names must be accepted");
    require(!is_kebab_case_name("") && !is_kebab_case_name("-thumb") &&
                !is_kebab_case_name("thumb-") && !is_kebab_case_name("scroll--thumb") &&
                !is_kebab_case_name("Scroll") && !is_kebab_case_name("scroll_thumb") &&
                !is_kebab_case_name("1st") && !is_kebab_case_name("scroll thumb"),
            "malformed names must be rejected");
}

constexpr std::array<PartDeclaration, 2> duplicate_parts{{
    {.name = "thumb", .role = PartRole::thumb, .composed_from = {}},
    {.name = "thumb", .role = PartRole::face, .composed_from = {}},
}};
constexpr std::array<PartDeclaration, 1> single_region{{
    {.name = "region", .role = PartRole::region, .composed_from = {}},
}};
constexpr std::array<AnatomyState, 2> duplicate_states{
    AnatomyState::hot, AnatomyState::hot,
};
constexpr std::array<CustomStateDeclaration, 1> shadowing_custom_state{{
    {.name = "pressed"},
}};
constexpr std::array<CustomStateDeclaration, 2> custom_states{{
    {.name = "armed"},
    {.name = "recording"},
}};

AnatomyDefect defect_of(const ControlAnatomy& anatomy) {
    const AnatomyValidation validation = validate_control_anatomy(anatomy);
    return validation.defect;
}

void test_validation_rejects_malformed_anatomy() {
    require(defect_of(base_control_anatomy()) == AnatomyDefect::none,
            "the base anatomy must validate");

    const ControlAnatomy no_parts{
        .control_id = "empty", .parts = {}, .states = {}, .custom_states = {}};
    require(defect_of(no_parts) == AnatomyDefect::no_parts,
            "an anatomy without parts must be rejected");

    const ControlAnatomy bad_id{
        .control_id = "TrackBar", .parts = single_region, .states = {},
        .custom_states = {}};
    require(defect_of(bad_id) == AnatomyDefect::control_id_not_kebab_case,
            "a control id must be kebab-case");

    const ControlAnatomy duplicated{
        .control_id = "dial", .parts = duplicate_parts, .states = {},
        .custom_states = {}};
    const AnatomyValidation duplicate_result = validate_control_anatomy(duplicated);
    require(duplicate_result.defect == AnatomyDefect::duplicate_part_name &&
                duplicate_result.index == 1U,
            "a duplicated part name must be rejected at its second declaration");

    const ControlAnatomy repeated_state{
        .control_id = "dial", .parts = single_region, .states = duplicate_states,
        .custom_states = {}};
    require(defect_of(repeated_state) == AnatomyDefect::duplicate_state,
            "a repeated state must be rejected");

    const ControlAnatomy shadowing{
        .control_id = "dial", .parts = single_region, .states = {},
        .custom_states = shadowing_custom_state};
    require(defect_of(shadowing) == AnatomyDefect::custom_state_shadows_standard_state,
            "a custom state may not reuse a standard state's name");

    const ControlAnatomy with_custom{
        .control_id = "dial", .parts = single_region, .states = {},
        .custom_states = custom_states};
    require(defect_of(with_custom) == AnatomyDefect::none,
            "declared custom states must validate");
    const std::optional<CustomStateIndex> recording =
        find_custom_state(with_custom, "recording");
    require(recording.has_value() && (*recording).value == 1U &&
                !find_custom_state(with_custom, "idle").has_value(),
            "custom states must be found by declared name");
    AnatomyStateSet states;
    states.insert(*recording);
    require(states.contains(*recording) && !states.contains(CustomStateIndex{0U}) &&
                states.standard_bits() == 0U,
            "custom state bits must be independent of standard states");
}

void test_range_control_anatomies_validate() {
    std::shared_ptr<TrackBar> track = make_control<TrackBar>(StableId("anatomy.track"));
    std::shared_ptr<HScrollBar> scroll = make_control<HScrollBar>(StableId("anatomy.scroll"));
    std::shared_ptr<ProgressBar> progress = make_control<ProgressBar>(StableId("anatomy.progress"));
    const std::array<const Control*, 3> controls{track.get(), scroll.get(), progress.get()};
    for (std::size_t index = 0; index < controls.size(); ++index) {
        const ControlAnatomy& anatomy = (*controls[index]).anatomy();
        const AnatomyValidation validation = validate_control_anatomy(anatomy);
        require(validation.defect == AnatomyDefect::none,
                std::string(anatomy.control_id) + " anatomy is malformed: " +
                    std::string(anatomy_defect_name(validation.defect)));
        require(find_part(anatomy, "region").has_value(),
                std::string(anatomy.control_id) + " must declare its region");
    }
    require((*track).anatomy().control_id == "track-bar" &&
                (*scroll).anatomy().control_id == "h-scroll-bar" &&
                (*progress).anatomy().control_id == "progress-bar",
            "range controls must publish their registry ids");
    require(&(*track).anatomy() == &(*make_control<TrackBar>(StableId("anatomy.track.2"))).anatomy(),
            "an anatomy belongs to the type, not the instance");

    std::vector<PartGeometry> wrong_size(1U);
    bool rejected = false;
    try {
        (*track).fill_part_geometry(wrong_size);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    require(rejected, "geometry storage of the wrong size must be rejected");
}

// The PlaySuite workaround copied "ten points at each end" from TrackBar's
// private pointer mapping. The published value span must give the same
// mapping, so a consumer can read it instead.
void test_track_bar_geometry_matches_pointer_mapping() {
    std::shared_ptr<TrackBar> slider = make_control<TrackBar>(StableId("anatomy.slider"));
    (*slider).set_requested_bounds({10.0, 10.0, 200.0, 28.0});
    (*slider).set_value(25.0);
    Window window(slider, {240.0, 60.0});
    window.perform_layout();

    const Rect span = (*slider).value_span_bounds();
    const PartGeometry track = (*slider).parts().track;
    const PartGeometry thumb = (*slider).parts().thumb;
    const PartGeometry fill = (*slider).parts().fill;
    const PartGeometry ticks = (*slider).parts().ticks;
    require(track.present && thumb.present && ticks.present,
            "classic track bar must publish track, thumb and ticks");
    require(!fill.present, "classic track bar has no fill");
    const Rect region = (*slider).client_rectangle();
    require(track.bounds == span && near(span.x, 10.0) &&
                near(span.width, region.width - 20.0),
            "classic track must be the value span inset by ten units");
    require(near(thumb.bounds.x + thumb.bounds.width * 0.5, span.x + span.width * 0.25),
            "thumb centre must sit at the value's position along the span");

    const Rect absolute = (*slider).absolute_bounds();
    PointerEvent down;
    down.action = PointerAction::down;
    down.button = PointerButton::primary;
    down.position = {absolute.x + span.x + span.width * 0.75, absolute.y + 14.0};
    down.pointer_id = 3;
    require(window.dispatch_pointer(down) && near((*slider).value(), 75.0),
            "pointer at three quarters of the published span must select 75");
    const TrackBar::States dragging = (*slider).states();
    require(dragging.pressed && dragging.captured && !dragging.normal,
            "dragging must publish pressed and captured");
    require_declared_states(*slider);
    PointerEvent up = down;
    up.action = PointerAction::up;
    require(window.dispatch_pointer(up), "pointer up must be handled");
    require((*slider).states().normal,
            "release must return to normal");

    (*slider).set_visual_style(TrackBarVisualStyle::filled);
    const PartGeometry filled = (*slider).parts().fill;
    const PartGeometry groove = (*slider).parts().track;
    require(filled.present && near(filled.bounds.x, span.x) &&
                near(filled.bounds.width, span.width * 0.75) &&
                near(groove.bounds.width, span.width),
            "filled style must publish the value portion of the groove");
    (*slider).set_visual_style(TrackBarVisualStyle::compact);
    require(!(*slider).parts().ticks.present,
            "compact style draws no ticks");
    require_parts_within_region(*slider);
}

void test_scroll_bar_geometry_and_states() {
    std::shared_ptr<Panel> root = make_control<Panel>(StableId("anatomy.scroll.root"));
    std::shared_ptr<HScrollBar> horizontal = make_control<HScrollBar>(StableId("anatomy.scroll.h"));
    (*horizontal).set_requested_bounds({10.0, 10.0, 240.0, 18.0});
    (*horizontal).set_range(0.0, 100.0);
    (*horizontal).set_large_change(20.0);
    (*horizontal).set_value(50.0);
    std::shared_ptr<VScrollBar> vertical = make_control<VScrollBar>(StableId("anatomy.scroll.v"));
    (*vertical).set_requested_bounds({260.0, 10.0, 18.0, 180.0});
    (*root).add_child(horizontal);
    (*root).add_child(vertical);
    Window window(root, {300.0, 210.0});
    window.perform_layout();

    const std::array<std::shared_ptr<ScrollBar>, 2> bars{horizontal, vertical};
    for (std::size_t index = 0; index < bars.size(); ++index) {
        const ScrollBar& bar = *bars[index];
        require(bar.parts().decrement_arrow.bounds == bar.decrement_button_bounds() &&
                    bar.parts().increment_arrow.bounds == bar.increment_button_bounds() &&
                    bar.parts().track.bounds == bar.track_bounds() &&
                    bar.parts().thumb.bounds == bar.thumb_bounds(),
                "scroll bar parts must equal its public geometry");
        require_parts_within_region(bar);
        require_declared_states(bar);
    }
    const ControlAnatomy& anatomy = (*horizontal).anatomy();
    const std::optional<PartIndex> arrow = find_part(anatomy, "decrement-arrow");
    require(arrow.has_value() &&
                anatomy.parts[(*arrow).value].role == PartRole::actuator &&
                anatomy.parts[(*arrow).value].composed_from == "button",
            "scroll arrows must be actuators composed from the button system");

    const Rect absolute = (*horizontal).absolute_bounds();
    const Rect thumb = (*horizontal).thumb_bounds();
    PointerEvent down;
    down.action = PointerAction::down;
    down.button = PointerButton::primary;
    down.position = {absolute.x + thumb.x + thumb.width * 0.5,
                     absolute.y + thumb.y + thumb.height * 0.5};
    require(window.dispatch_pointer(down), "thumb press must be handled");
    const ScrollBar::States dragging = (*horizontal).states();
    require(dragging.pressed && dragging.captured,
            "thumb drag must publish pressed and captured");
    PointerEvent up = down;
    up.action = PointerAction::up;
    require(window.dispatch_pointer(up), "thumb release must be handled");

    (*horizontal).set_enabled(false);
    const ScrollBar::States disabled = (*horizontal).states();
    require(disabled.disabled && !disabled.enabled,
            "a disabled control must publish disabled, not enabled");
}

void test_progress_bar_geometry_and_states() {
    std::shared_ptr<ProgressBar> progress = make_control<ProgressBar>(StableId("anatomy.bar"));
    (*progress).set_requested_bounds({0.0, 0.0, 204.0, 20.0});
    (*progress).set_value(50.0);
    Window window(progress, {204.0, 20.0});
    window.perform_layout();

    const PartGeometry fill = (*progress).parts().fill;
    const PartGeometry activity = (*progress).parts().activity;
    require(fill.present && !activity.present,
            "a determinate bar publishes its fill and no activity");
    require(near(fill.bounds.x, 2.0) && near(fill.bounds.width, 100.0),
            "fill must cover the value's share of the interior");
    require(!(*progress).states().indeterminate,
            "a determinate bar is not indeterminate");

    (*progress).set_visual_style(ProgressBarVisualStyle::marquee);
    require(!(*progress).parts().fill.present &&
                (*progress).parts().activity.present,
            "a marquee bar publishes its activity band instead of a fill");
    require((*progress).states().indeterminate,
            "a marquee bar is indeterminate");
    require_parts_within_region(*progress);
    require_declared_states(*progress);
}

// A control type that declares no anatomy of its own.
class UndeclaredControl final : public Control {
public:
    explicit UndeclaredControl(StableId stable_id) : Control(std::move(stable_id)) {}
};

void test_base_anatomy_for_undeclared_controls() {
    std::shared_ptr<UndeclaredControl> plain =
        make_control<UndeclaredControl>(StableId("anatomy.plain"));
    (*plain).set_requested_bounds({0.0, 0.0, 80.0, 40.0});
    Window window(plain, {80.0, 40.0});
    window.perform_layout();
    require(&(*plain).anatomy() == &base_control_anatomy(),
            "a type that declares nothing inherits the base anatomy");
    const PartGeometry region = (*plain).parts().region;
    require(region.present && region.bounds == (*plain).client_rectangle(),
            "the base region is the client rectangle");
    require(!find_part((*plain).anatomy(), "thumb").has_value(),
            "an undeclared part name must not resolve");
    require_declared_states(*plain);
}

// Records where text is drawn, in window coordinates, and measures text with
// Painter's default metrics so it can serve as the window's text metrics.
class TextOriginPainter final : public Painter {
public:
    void save() override { saved_offsets.push_back(offset); }
    void restore() override {
        if (!saved_offsets.empty()) {
            offset = saved_offsets.back();
            saved_offsets.pop_back();
        }
    }
    void translate(Point delta) override {
        offset.x += delta.x;
        offset.y += delta.y;
    }
    void clip_rect(Rect) override {}
    void fill_rect(Rect, Color) override {}
    void stroke_rect(Rect, Color, double) override {}
    void draw_line(Point, Point, Color, double) override {}
    void draw_text_utf8(Point origin, std::string_view, FontSpec, Color) override {
        const Point placed{origin.x + offset.x, origin.y + offset.y};
        text_origins.push_back(placed);
    }
    void draw_image(ImageId, Rect, double) override {}

    Point offset{};
    std::vector<Point> saved_offsets;
    std::vector<Point> text_origins;
};

void test_button_label_matches_painted_text() {
    std::shared_ptr<Panel> root = make_control<Panel>(StableId("anatomy.buttons.root"));
    (*root).set_requested_bounds({0.0, 0.0, 300.0, 120.0});
    std::shared_ptr<Button> button = make_control<Button>(StableId("anatomy.button"), "Apply");
    (*button).set_requested_bounds({20.0, 30.0, 120.0, 30.0});
    (*root).add_child(button);
    Window window(root, {300.0, 120.0});
    TextOriginPainter painter;
    window.set_text_metrics_provider(&painter);
    window.perform_layout();

    const ControlAnatomy& anatomy = (*button).anatomy();
    require(validate_control_anatomy(anatomy).defect == AnatomyDefect::none &&
                anatomy.control_id == "button",
            "Button must publish a valid anatomy");
    const PartGeometry face = (*button).parts().face;
    const PartGeometry label = (*button).parts().label;
    require(face.present && face.bounds == (*button).client_rectangle(),
            "the button face is its whole client rectangle");
    require(label.present && label.bounds.width > 0.0 && label.bounds.height > 0.0,
            "a button with text publishes its label");
    require(!(*button).parts().icon.present,
            "a button without an image publishes no icon");
    require(near(label.bounds.x + label.bounds.width * 0.5, face.bounds.width * 0.5),
            "a centred label is centred in the face");

    static_cast<void>(window.paint(painter, {0.0, 0.0, 300.0, 120.0}));
    require(painter.text_origins.size() == 1U, "the button must draw one line of text");
    const Rect absolute = (*button).absolute_bounds();
    require(near(painter.text_origins[0].x, absolute.x + label.bounds.x),
            "the published label must start where paint draws the text");
    require(painter.text_origins[0].y > absolute.y + label.bounds.y &&
                painter.text_origins[0].y <= absolute.y + label.bounds.bottom(),
            "the painted baseline must fall inside the published label");
    window.set_text_metrics_provider(nullptr);

    require(!(*button).states().preselect,
            "a button is not preselected until the form makes it the accept target");
    window.set_accept_button(button);
    require((*button).states().preselect,
            "the accept button publishes preselect");
    require((*button).parts().preselect.present,
            "the preselect cue part is present whether or not it is shown");

    const PointerEvent down{PointerAction::down, PointerButton::primary,
                            {absolute.x + 10.0, absolute.y + 10.0}};
    require(window.dispatch_pointer(down), "button press must be handled");
    const Button::States pressed = (*button).states();
    require(pressed.pressed && !pressed.normal,
            "a pressed button publishes pressed, not normal");
    require_declared_states(*button);
    require_parts_within_region(*button);
}

void test_check_box_and_radio_button_parts() {
    std::shared_ptr<Panel> root = make_control<Panel>(StableId("anatomy.choices.root"));
    (*root).set_requested_bounds({0.0, 0.0, 300.0, 120.0});
    std::shared_ptr<CheckBox> check = make_control<CheckBox>(StableId("anatomy.check"), "Wrap lines");
    (*check).set_requested_bounds({10.0, 10.0, 160.0, 24.0});
    std::shared_ptr<RadioButton> radio = make_control<RadioButton>(StableId("anatomy.radio"), "Metric");
    (*radio).set_requested_bounds({10.0, 50.0, 160.0, 24.0});
    (*root).add_child(check);
    (*root).add_child(radio);
    Window window(root, {300.0, 120.0});
    window.perform_layout();

    require(validate_control_anatomy((*check).anatomy()).defect == AnatomyDefect::none &&
                validate_control_anatomy((*radio).anatomy()).defect == AnatomyDefect::none,
            "CheckBox and RadioButton must publish valid anatomies");
    const CheckBox::Parts check_parts = (*check).parts();
    const RadioButton::Parts radio_parts = (*radio).parts();
    const std::array<PartGeometry, 2> wells{check_parts.well, radio_parts.well};
    const std::array<PartGeometry, 2> labels{check_parts.label, radio_parts.label};
    const std::array<PartGeometry, 2> marks{check_parts.indicator, radio_parts.indicator};
    for (std::size_t index = 0; index < wells.size(); ++index) {
        const PartGeometry well = wells[index];
        const PartGeometry label = labels[index];
        require(well.present && near(well.bounds.x, 1.0) &&
                    near(well.bounds.width, 15.0) && near(well.bounds.height, 15.0) &&
                    near(well.bounds.y, (24.0 - 15.0) * 0.5),
                "a choice publishes its 15-unit well, centred vertically");
        require(label.present && label.bounds.x >= well.bounds.right(),
                "the label follows the well");
        require(!marks[index].present, "an unchecked choice shows no mark");
    }
    require_declared_states(*check);
    require_declared_states(*radio);
    require_parts_within_region(*check);
    require_parts_within_region(*radio);
    require(!(*check).parts().face.present,
            "a normal-appearance check box has no button face");

    (*check).set_checked(true);
    (*radio).set_checked(true);
    const PartGeometry check_mark = (*check).parts().indicator;
    const PartGeometry radio_dot = (*radio).parts().indicator;
    const Rect check_well = (*check).parts().well.bounds;
    const Rect radio_well = (*radio).parts().well.bounds;
    require(check_mark.present && check_well.contains(check_mark.bounds),
            "a checked box publishes its mark inside the well");
    require(radio_dot.present && radio_well.contains(radio_dot.bounds) &&
                near(radio_dot.bounds.width, 7.0),
            "a checked radio publishes its 7-unit dot inside the well");
    require((*check).states().checked &&
                (*radio).states().checked,
            "checked choices publish checked");

    (*check).set_three_state(true);
    (*check).set_check_state(CheckState::indeterminate);
    const CheckBox::States mixed = (*check).states();
    require(mixed.indeterminate && !mixed.checked &&
                (*check).parts().indicator.present,
            "an indeterminate box publishes indeterminate and its bar");

    (*check).set_indicator_style(ChoiceIndicatorStyle::toggle);
    (*check).set_check_state(CheckState::unchecked);
    const PartGeometry knob_off = (*check).parts().indicator;
    (*check).set_check_state(CheckState::checked);
    const PartGeometry knob_on = (*check).parts().indicator;
    require(knob_off.present && knob_on.present &&
                knob_on.bounds.x > knob_off.bounds.x &&
                near((*check).parts().well.bounds.width, 30.0),
            "a toggle publishes its knob, which moves when checked");

    (*check).set_appearance(CheckBoxAppearance::button);
    require((*check).parts().face.present &&
                !(*check).parts().well.present &&
                !(*check).parts().indicator.present,
            "the button appearance publishes a face instead of a well");
    require_parts_within_region(*check);
}

void test_link_label_custom_visited_state() {
    std::shared_ptr<LinkLabel> link = make_control<LinkLabel>(StableId("anatomy.link"), "Learn more");
    (*link).set_requested_bounds({0.0, 0.0, 160.0, 22.0});
    Window window(link, {160.0, 22.0});
    window.perform_layout();
    const ControlAnatomy& anatomy = (*link).anatomy();
    require(validate_control_anatomy(anatomy).defect == AnatomyDefect::none,
            "LinkLabel must publish a valid anatomy");
    const std::optional<CustomStateIndex> visited = find_custom_state(anatomy, "visited");
    require(visited.has_value(), "LinkLabel declares a visited state");
    require(!(*link).states().visited, "a new link is not visited");
    (*link).set_visited(true);
    require((*link).states().visited && (*link).anatomy_states().contains(*visited),
            "a visited link publishes its visited state");
    const PartGeometry label = (*link).parts().label;
    const PartGeometry underline = (*link).parts().underline;
    require(label.present && underline.present &&
                near(label.bounds.x, underline.bounds.x) &&
                near(label.bounds.width, underline.bounds.width) &&
                underline.bounds.y > label.bounds.bottom(),
            "the underline runs beneath the label across its width");
    require_parts_within_region(*link);
}

void test_drop_down_button_disclosure() {
    std::shared_ptr<DropDownButton> button = make_control<DropDownButton>(
        StableId("anatomy.drop"), "Export", DropDownButtonMode::split);
    (*button).set_requested_bounds({0.0, 0.0, 140.0, 28.0});
    Window window(button, {140.0, 28.0});
    window.perform_layout();
    require(validate_control_anatomy((*button).anatomy()).defect == AnatomyDefect::none &&
                (*button).anatomy().control_id == "drop-down-button",
            "DropDownButton must publish its own valid anatomy");
    const PartGeometry segment = (*button).parts().drop_down;
    const PartGeometry arrow = (*button).parts().disclosure;
    const Rect region = (*button).client_rectangle();
    require(segment.present && segment.bounds == (*button).drop_down_bounds() &&
                near(segment.bounds.right(), region.right()) &&
                near(segment.bounds.width, (*button).drop_down_width()),
            "the drop-down segment sits at the trailing edge");
    require(arrow.present && segment.bounds.contains(arrow.bounds),
            "the disclosure arrow lies inside its segment");
    require((*button).states().closed,
            "a shut drop-down publishes closed");
    (*button).set_drop_down_open(true);
    const DropDownButton::States open = (*button).states();
    require(open.open && !open.closed,
            "an open drop-down publishes open");
    require_declared_states(*button);
    require_parts_within_region(*button);
}

void test_panel_and_scrollable_parts() {
    std::shared_ptr<Panel> panel = make_control<Panel>(StableId("anatomy.panel"));
    (*panel).set_requested_bounds({0.0, 0.0, 200.0, 120.0});
    std::shared_ptr<Label> tall = make_control<Label>(StableId("anatomy.panel.tall"), "Tall");
    (*tall).set_requested_bounds({0.0, 0.0, 120.0, 400.0});
    (*panel).add_child(tall);
    Window window(panel, {200.0, 120.0});
    window.perform_layout();
    require(validate_control_anatomy((*panel).anatomy()).defect == AnatomyDefect::none &&
                (*panel).anatomy().control_id == "panel",
            "Panel must publish its own valid anatomy");
    require((*panel).parts().face.present && !(*panel).parts().edge.present,
            "a borderless panel publishes a face and no edge");
    (*panel).set_border_style(BorderStyle::line);
    require((*panel).parts().edge.present,
            "a bordered panel publishes its edge");
    require(!(*panel).parts().scrollable.vertical_thumb.present,
            "without auto-scroll no scroll axis is published");

    (*panel).set_auto_scroll(true);
    window.perform_layout();
    const PartGeometry track = (*panel).parts().scrollable.vertical_track;
    const PartGeometry thumb = (*panel).parts().scrollable.vertical_thumb;
    const PartGeometry viewport = (*panel).parts().scrollable.viewport;
    require(track.present && thumb.present && track.bounds.contains(thumb.bounds),
            "an overflowing auto-scroll panel publishes its vertical thumb inside its track");
    require(viewport.present && viewport.bounds.right() <= track.bounds.x,
            "the viewport ends where the vertical scroll axis begins");
    const ControlAnatomy& anatomy = (*panel).anatomy();
    const std::optional<PartIndex> thumb_index = find_part(anatomy, "vertical-thumb");
    require(thumb_index.has_value() &&
                anatomy.parts[(*thumb_index).value].role == PartRole::thumb,
            "a container's scroll thumb shares the thumb role with ScrollBar");
    require_declared_states(*panel);
    require_parts_within_region(*panel);
}

void test_group_box_and_label_parts() {
    std::shared_ptr<Panel> root = make_control<Panel>(StableId("anatomy.groups.root"));
    (*root).set_requested_bounds({0.0, 0.0, 320.0, 200.0});
    std::shared_ptr<GroupBox> group = make_control<GroupBox>(StableId("anatomy.group"), "Output");
    (*group).set_requested_bounds({10.0, 10.0, 200.0, 100.0});
    std::shared_ptr<Label> label = make_control<Label>(StableId("anatomy.label"), "Ready");
    (*label).set_requested_bounds({10.0, 130.0, 160.0, 24.0});
    (*root).add_child(group);
    (*root).add_child(label);
    Window window(root, {320.0, 200.0});
    TextOriginPainter painter;
    window.set_text_metrics_provider(&painter);
    window.perform_layout();

    const PartGeometry caption = (*group).parts().caption;
    const PartGeometry edge = (*group).parts().edge;
    require(caption.present && edge.present && caption.bounds.y < edge.bounds.y &&
                caption.bounds.bottom() > edge.bounds.y,
            "the group caption sits across the top of its frame");
    require((*group).anatomy().control_id == "group-box", "GroupBox publishes its own id");

    const PartGeometry text = (*label).parts().label;
    require(text.present && text.bounds.width > 0.0 && near(text.bounds.x, 2.0),
            "a near-aligned label publishes its text block from the left inset");
    static_cast<void>(window.paint(painter, {0.0, 0.0, 320.0, 200.0}));
    const Rect absolute = (*label).absolute_bounds();
    bool found = false;
    for (std::size_t index = 0; index < painter.text_origins.size(); ++index) {
        const Point origin = painter.text_origins[index];
        if (near(origin.x, absolute.x + text.bounds.x) &&
            origin.y > absolute.y + text.bounds.y &&
            origin.y <= absolute.y + text.bounds.bottom()) {
            found = true;
        }
    }
    window.set_text_metrics_provider(nullptr);
    require(found, "the label's published block must contain where paint draws its text");
    require_parts_within_region(*group);
    require_parts_within_region(*label);
}

// A record read through a base type agrees with what the dynamic type
// publishes: MaterialPanel paints no border, so even with a border style set
// its Panel view has no edge.
void test_base_record_follows_dynamic_type() {
    std::shared_ptr<MaterialPanel> material =
        make_control<MaterialPanel>(StableId("anatomy.material"));
    (*material).set_requested_bounds({0.0, 0.0, 120.0, 60.0});
    (*material).set_border_style(BorderStyle::line);
    Window window(material, {120.0, 60.0});
    window.perform_layout();
    const Panel& as_panel = *material;
    const Panel::Parts parts = as_panel.parts();
    require(parts.face.present && !parts.edge.present,
            "a base-type record must report the dynamic type's parts");
    require(parts.scrollable.region.bounds == (*material).client_rectangle(),
            "composed records carry the scrollable parts");
    const Control& as_control = *material;
    const Control::States states = as_control.states();
    require(states.normal && states.enabled && !states.disabled,
            "the base states record reads the dynamic type's states");
}

} // namespace

int main() {
    try {
        test_names_are_kebab_case_and_round_trip();
        test_validation_rejects_malformed_anatomy();
        test_range_control_anatomies_validate();
        test_track_bar_geometry_matches_pointer_mapping();
        test_scroll_bar_geometry_and_states();
        test_progress_bar_geometry_and_states();
        test_base_anatomy_for_undeclared_controls();
        test_button_label_matches_painted_text();
        test_check_box_and_radio_button_parts();
        test_link_label_custom_visited_state();
        test_drop_down_button_disclosure();
        test_panel_and_scrollable_parts();
        test_group_box_and_label_parts();
        test_base_record_follows_dynamic_type();
        std::cout << "gui_forms_anatomy_tests: all tests passed\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "gui_forms_anatomy_tests: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
