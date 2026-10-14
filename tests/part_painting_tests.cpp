// The by-role paint path (owner ruling, 2026-10-14): a part described by its
// role and states is drawn from the theme, and parts whose role draws nothing
// of its own stay untouched.

#include "gui_forms/control.hpp"
#include "gui_forms/theme.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

// One drawing operation as the recording painter saw it.
struct Operation final {
    std::string kind;
    Rect bounds{};
    Color color{};
};

class RecordingPainter final : public Painter {
public:
    void save() override { operations.push_back({"save", {}, {}}); }
    void restore() override { operations.push_back({"restore", {}, {}}); }
    void translate(Point) override {}
    void clip_rect(Rect rect) override { operations.push_back({"clip", rect, {}}); }
    void fill_rect(Rect rect, Color color) override { operations.push_back({"fill", rect, color}); }
    void fill_rounded_rect(Rect rect, double, Color color) override {
        operations.push_back({"fill_rounded", rect, color});
    }
    void stroke_rect(Rect rect, Color color, double) override {
        operations.push_back({"stroke", rect, color});
    }
    void stroke_rounded_rect(Rect rect, double, Color color, double) override {
        operations.push_back({"stroke_rounded", rect, color});
    }
    void draw_line(Point from, Point to, Color color, double) override {
        const Rect extent{std::min(from.x, to.x), std::min(from.y, to.y),
                          std::abs(to.x - from.x), std::abs(to.y - from.y)};
        operations.push_back({"line", extent, color});
    }
    void draw_text_utf8(Point origin, std::string_view, FontSpec, Color color) override {
        operations.push_back({"text", {origin.x, origin.y, 0.0, 0.0}, color});
    }
    void draw_image(ImageId, Rect destination, double) override {
        operations.push_back({"image", destination, {}});
    }

    [[nodiscard]] std::size_t count(const char* kind) const {
        std::size_t total = 0U;
        for (const Operation& operation : operations) {
            if (operation.kind == kind) ++total;
        }
        return total;
    }

    std::vector<Operation> operations;
};

AnatomyStateSet states_of(AnatomyState state) {
    AnatomyStateSet states;
    states.insert(state);
    return states;
}

void silent_roles(const Control& control) {
    const AnatomyState none = AnatomyState::normal;
    const PartRole silent[] = {PartRole::region, PartRole::shadow, PartRole::edge,
                               PartRole::perimeter, PartRole::sheen};
    for (const PartRole role : silent) {
        RecordingPainter painter;
        PartPaint part;
        part.role = role;
        part.bounds = {0.0, 0.0, 40.0, 20.0};
        part.states = states_of(none);
        paint_part(painter, control, part);
        require(painter.operations.empty(), "a role the surface material draws paints nothing itself");
    }
    RecordingPainter painter;
    PartPaint empty;
    empty.role = PartRole::face;
    paint_part(painter, control, empty);
    require(painter.operations.empty(), "an empty part paints nothing");
}

void surfaces_and_cues(const Control& control) {
    RecordingPainter face_painter;
    PartPaint face;
    face.role = PartRole::face;
    face.bounds = {0.0, 0.0, 80.0, 24.0};
    face.family = ControlVisualRole::button;
    paint_part(face_painter, control, face);
    require(!face_painter.operations.empty(), "a face paints its recipe's material");

    const Theme& theme = control.effective_theme();
    PartPaint focus;
    focus.role = PartRole::focus;
    focus.bounds = {0.0, 0.0, 80.0, 24.0};
    focus.family = ControlVisualRole::button;
    RecordingPainter unfocused;
    paint_part(unfocused, control, focus);
    require(unfocused.operations.empty(), "a focus part without focus_visible paints nothing");
    focus.states = states_of(AnatomyState::focus_visible);
    RecordingPainter focused;
    paint_part(focused, control, focus);
    const ControlVisualRecipe& recipe =
        theme.resolve(ControlVisualRole::button, part_visual_context(control, focus.states));
    require(focused.count("stroke_rounded") == 1U && focused.operations[0].color == recipe.focus_ring,
            "a visible focus cue strokes one ring in the recipe's focus colour");

    PartPaint preselect = focus;
    preselect.role = PartRole::preselect;
    preselect.states = states_of(AnatomyState::preselect);
    RecordingPainter defaulted;
    paint_part(defaulted, control, preselect);
    const ControlVisualRecipe& default_recipe =
        theme.resolve(ControlVisualRole::button, part_visual_context(control, preselect.states));
    require(defaulted.count("stroke_rounded") == 1U &&
                defaulted.operations[0].color == default_recipe.default_ring,
            "a preselect cue strokes the default ring");
}

void marks_and_text(const Control& control) {
    const Theme& theme = control.effective_theme();
    PartPaint check;
    check.role = PartRole::indicator;
    check.bounds = {2.0, 2.0, 12.0, 9.0};
    check.family = ControlVisualRole::choice;
    check.mark = PartMark::check;
    check.states = states_of(AnatomyState::checked);
    RecordingPainter checked;
    paint_part(checked, control, check);
    const ControlVisualRecipe& choice =
        theme.resolve(ControlVisualRole::choice, part_visual_context(control, check.states));
    require(checked.count("line") == 2U && checked.operations[0].color == choice.glyph,
            "a check mark is two strokes in the recipe's glyph colour");

    PartPaint arrow;
    arrow.role = PartRole::glyph;
    arrow.bounds = {0.0, 0.0, 9.0, 9.0};
    arrow.mark = PartMark::arrow_down;
    RecordingPainter arrows;
    paint_part(arrows, control, arrow);
    require(arrows.count("fill") == 4U, "a 9-unit arrow fills four rows");
    for (const Operation& row : arrows.operations) {
        require(row.bounds.x >= arrow.bounds.x && row.bounds.right() <= arrow.bounds.right() &&
                    row.bounds.y >= arrow.bounds.y && row.bounds.bottom() <= arrow.bounds.bottom(),
                "an arrow stays inside its part");
    }

    PartPaint label;
    label.role = PartRole::label;
    label.bounds = {0.0, 0.0, 100.0, 20.0};
    label.family = ControlVisualRole::button;
    label.text = "Apply";
    RecordingPainter enabled;
    paint_part(enabled, control, label);
    const ControlVisualRecipe& button =
        theme.resolve(ControlVisualRole::button, part_visual_context(control, label.states));
    require(enabled.count("text") == 1U && enabled.count("clip") == 1U,
            "a label draws its text once, clipped to the part");
    for (const Operation& operation : enabled.operations) {
        if (operation.kind == "text") {
            require(operation.color == button.text, "an enabled label uses the recipe's text colour");
        }
    }
    label.states = states_of(AnatomyState::unavailable);
    RecordingPainter unavailable;
    paint_part(unavailable, control, label);
    const ControlVisualRecipe& disabled =
        theme.resolve(ControlVisualRole::button, part_visual_context(control, label.states));
    for (const Operation& operation : unavailable.operations) {
        if (operation.kind == "text") {
            require(operation.color == disabled.muted_text, "an unavailable label uses the muted colour");
        }
    }
}

void state_context(const Control& control) {
    require(part_visual_context(control, states_of(AnatomyState::hot)).surface ==
                ControlSurfaceState::hot,
            "hot selects the hot surface");
    require(part_visual_context(control, states_of(AnatomyState::captured)).surface ==
                ControlSurfaceState::pressed,
            "a captured part is drawn pressed");
    require(part_visual_context(control, states_of(AnatomyState::checked)).selected,
            "checked selects the selected recipes");
    require(part_visual_context(control, states_of(AnatomyState::unavailable)).surface ==
                ControlSurfaceState::disabled,
            "an unavailable part is drawn disabled");
    require(!part_visual_context(control, states_of(AnatomyState::focused)).focused,
            "focus without focus_visible draws no cue");
}

} // namespace

int main() {
    const std::shared_ptr<Control> control = make_control<Control>(StableId("part.painting"));
    silent_roles(*control);
    surfaces_and_cues(*control);
    marks_and_text(*control);
    state_context(*control);
    std::cout << "by-role paint path: silent roles, surfaces, cues, marks, text and states passed\n";
    return 0;
}
