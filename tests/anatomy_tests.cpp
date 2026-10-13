#include "gui_forms/anatomy.hpp"
#include "gui_forms/container_controls.hpp"
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

PartGeometry required_part(const Control& control, std::string_view part_name) {
    const std::optional<PartGeometry> geometry = control.part_geometry(part_name);
    require(geometry.has_value(),
            std::string(control.anatomy().control_id) + " must declare part " +
                std::string(part_name));
    return *geometry;
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
                (*scroll).anatomy().control_id == "scroll-bar" &&
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
    const PartGeometry track = required_part(*slider, "track");
    const PartGeometry thumb = required_part(*slider, "thumb");
    const PartGeometry fill = required_part(*slider, "fill");
    const PartGeometry ticks = required_part(*slider, "ticks");
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
    const AnatomyStateSet dragging = (*slider).anatomy_states();
    require(dragging.contains(AnatomyState::pressed) &&
                dragging.contains(AnatomyState::captured) &&
                !dragging.contains(AnatomyState::normal),
            "dragging must publish pressed and captured");
    require_declared_states(*slider);
    PointerEvent up = down;
    up.action = PointerAction::up;
    require(window.dispatch_pointer(up), "pointer up must be handled");
    require((*slider).anatomy_states().contains(AnatomyState::normal),
            "release must return to normal");

    (*slider).set_visual_style(TrackBarVisualStyle::filled);
    const PartGeometry filled = required_part(*slider, "fill");
    const PartGeometry groove = required_part(*slider, "track");
    require(filled.present && near(filled.bounds.x, span.x) &&
                near(filled.bounds.width, span.width * 0.75) &&
                near(groove.bounds.width, span.width),
            "filled style must publish the value portion of the groove");
    (*slider).set_visual_style(TrackBarVisualStyle::compact);
    require(!required_part(*slider, "ticks").present,
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
        require(required_part(bar, "decrement-arrow").bounds == bar.decrement_button_bounds() &&
                    required_part(bar, "increment-arrow").bounds == bar.increment_button_bounds() &&
                    required_part(bar, "track").bounds == bar.track_bounds() &&
                    required_part(bar, "thumb").bounds == bar.thumb_bounds(),
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
    const AnatomyStateSet dragging = (*horizontal).anatomy_states();
    require(dragging.contains(AnatomyState::pressed) &&
                dragging.contains(AnatomyState::captured),
            "thumb drag must publish pressed and captured");
    PointerEvent up = down;
    up.action = PointerAction::up;
    require(window.dispatch_pointer(up), "thumb release must be handled");

    (*horizontal).set_enabled(false);
    const AnatomyStateSet disabled = (*horizontal).anatomy_states();
    require(disabled.contains(AnatomyState::disabled) &&
                !disabled.contains(AnatomyState::enabled),
            "a disabled control must publish disabled, not enabled");
}

void test_progress_bar_geometry_and_states() {
    std::shared_ptr<ProgressBar> progress = make_control<ProgressBar>(StableId("anatomy.bar"));
    (*progress).set_requested_bounds({0.0, 0.0, 204.0, 20.0});
    (*progress).set_value(50.0);
    Window window(progress, {204.0, 20.0});
    window.perform_layout();

    const PartGeometry fill = required_part(*progress, "fill");
    const PartGeometry activity = required_part(*progress, "activity");
    require(fill.present && !activity.present,
            "a determinate bar publishes its fill and no activity");
    require(near(fill.bounds.x, 2.0) && near(fill.bounds.width, 100.0),
            "fill must cover the value's share of the interior");
    require(!(*progress).anatomy_states().contains(AnatomyState::indeterminate),
            "a determinate bar is not indeterminate");

    (*progress).set_visual_style(ProgressBarVisualStyle::marquee);
    require(!required_part(*progress, "fill").present &&
                required_part(*progress, "activity").present,
            "a marquee bar publishes its activity band instead of a fill");
    require((*progress).anatomy_states().contains(AnatomyState::indeterminate),
            "a marquee bar is indeterminate");
    require_parts_within_region(*progress);
    require_declared_states(*progress);
}

void test_base_anatomy_for_undeclared_controls() {
    std::shared_ptr<Panel> panel = make_control<Panel>(StableId("anatomy.panel"));
    (*panel).set_requested_bounds({0.0, 0.0, 80.0, 40.0});
    Window window(panel, {80.0, 40.0});
    window.perform_layout();
    require(&(*panel).anatomy() == &base_control_anatomy(),
            "a type that declares nothing inherits the base anatomy");
    const PartGeometry region = required_part(*panel, "region");
    require(region.present && region.bounds == (*panel).client_rectangle(),
            "the base region is the client rectangle");
    require(!(*panel).part_geometry("thumb").has_value(),
            "an undeclared part name must not resolve");
    require_declared_states(*panel);
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
        std::cout << "gui_forms_anatomy_tests: all tests passed\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "gui_forms_anatomy_tests: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
