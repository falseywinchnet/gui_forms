// The by-role paint path (owner ruling, 2026-10-14): a part described by its
// role and states is drawn from the theme, and parts whose role draws nothing
// of its own stay untouched. The shadow role paints the theme's overlay
// shadow (owner decision RB-37).

#include "gui_forms/control.hpp"
#include "gui_forms/theme.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
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
    Point offset{};
    double radius{};
    double blur{};
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
    void draw_box_shadow(Rect rect, double corner_radius, Point offset, double blur_radius, double,
                         Color color) override {
        operations.push_back({"shadow", rect, color, offset, corner_radius, blur_radius});
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
    const PartRole silent[] = {PartRole::region, PartRole::edge, PartRole::perimeter,
                               PartRole::sheen};
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

// The shadow role casts the theme's overlay shadow around the casting
// surface's rect, layer by layer, in the family's corner radius; it reaches
// as far as overlay_shadow_outsets says; paint_part_from_recipe leaves it to
// paint_part.
void overlay_shadow(const Control& control) {
    const Theme& theme = control.effective_theme();
    const std::vector<MaterialShadow>& layers = theme.overlay_shadow(false);
    require(layers.size() == 2U && theme.overlay_shadow(true).empty(),
            "the default theme casts a two-layer overlay shadow, and none in high contrast");
    for (const MaterialShadow& layer : layers) {
        require(!layer.inset && layer.offset.y > 0.0 && layer.offset.x > 0.0 &&
                    layer.offset.x <= layer.offset.y * 0.5,
                "each layer falls down and right from the upper-left light, right by at most half its drop");
    }
    PartPaint shadow;
    shadow.role = PartRole::shadow;
    shadow.family = ControlVisualRole::tool_tip;
    shadow.bounds = {10.0, 12.0, 120.0, 40.0};
    RecordingPainter painter;
    paint_part(painter, control, shadow);
    require(painter.operations.size() == layers.size() && painter.count("shadow") == layers.size(),
            "a shadow part paints one box shadow per layer and nothing else");
    for (std::size_t index = 0; index < layers.size(); ++index) {
        const Operation& operation = painter.operations[index];
        require(operation.bounds == shadow.bounds && operation.offset == layers[index].offset &&
                    operation.blur == layers[index].blur_radius && operation.color == layers[index].color &&
                    operation.radius == 0.0,
                "each layer is cast from the surface's rect with the theme's offset, blur and colour");
    }
    shadow.corner_radius = 5.0;
    RecordingPainter rounded;
    paint_part(rounded, control, shadow);
    require(rounded.operations.size() == layers.size() && rounded.operations[0].radius == 5.0,
            "a part's own corner radius shapes its shadow");

    const Insets reach = overlay_shadow_outsets(control);
    // The wide layer reaches furthest: three blur radii (30) shifted by its
    // offset (2, 4).
    require(reach.left == 28.0 && reach.top == 26.0 && reach.right == 32.0 && reach.bottom == 34.0,
            "the overlay shadow reaches three blur radii past the surface, shifted down and right");

    RecordingPainter from_recipe;
    const ControlVisualContext context = part_visual_context(control, shadow.states);
    paint_part_from_recipe(from_recipe, shadow, theme.resolve(shadow.family, context), context);
    require(from_recipe.operations.empty(), "the recipe path paints no overlay shadow");
}

// A theme that casts no overlay shadow paints nothing for a shadow part and
// reports no reach; Theme::create refuses inset or too many layers.
void theme_without_overlay_shadow() {
    ThemeDefinition definition = windows_professional_theme_definition();
    definition.id = "no-overlay-shadow";
    definition.overlay_shadow.clear();
    const std::shared_ptr<Control> control = make_control<Control>(StableId("part.flat"));
    (*control).set_theme_override(Theme::create(definition));
    PartPaint shadow;
    shadow.role = PartRole::shadow;
    shadow.bounds = {0.0, 0.0, 60.0, 20.0};
    RecordingPainter painter;
    paint_part(painter, *control, shadow);
    const Insets reach = overlay_shadow_outsets(*control);
    require(painter.operations.empty() && reach == Insets{},
            "a theme without an overlay shadow casts none");

    ThemeDefinition inset = windows_professional_theme_definition();
    inset.overlay_shadow[0].inset = true;
    bool refused_inset = false;
    try {
        static_cast<void>(Theme::create(inset));
    } catch (const std::invalid_argument&) {
        refused_inset = true;
    }
    ThemeDefinition crowded = windows_professional_theme_definition();
    crowded.high_contrast_overlay_shadow.assign(SurfaceMaterial::maximum_shadows + 1U, MaterialShadow{});
    bool refused_crowded = false;
    try {
        static_cast<void>(Theme::create(crowded));
    } catch (const std::invalid_argument&) {
        refused_crowded = true;
    }
    require(refused_inset && refused_crowded,
            "a theme's overlay shadow must hold at most four outer layers");
}

// Every family has a registered name, and the tooltip family has its own
// recipe (owner decision RB-38) apart from the card it borrowed before.
void families() {
    for (std::size_t index = 0; index < control_visual_role_count; ++index) {
        require(!control_visual_role_name(static_cast<ControlVisualRole>(index)).empty(),
                "every recipe family has a registered name");
    }
    require(control_visual_role_name(ControlVisualRole::tool_tip) == "tool_tip" &&
                control_visual_role_name(ControlVisualRole::count).empty(),
            "the tooltip family is registered as tool_tip");
    const std::shared_ptr<const Theme> theme = default_theme();
    const ControlVisualContext normal{};
    const ControlVisualRecipe& tool_tip = (*theme).resolve(ControlVisualRole::tool_tip, normal);
    const ControlVisualRecipe& card = (*theme).resolve(ControlVisualRole::card, normal);
    require(tool_tip.material.fills.front().color == Color::rgba(255, 255, 225) &&
                tool_tip.material.border && (*tool_tip.material.border).color == Color::rgba(104, 111, 118) &&
                tool_tip.material.corner_radius == 0.0 && tool_tip.text == Color::rgba(24, 31, 38) &&
                !(tool_tip.material == card.material),
            "the default theme's tooltip family keeps the plain tooltip's face, edge and text");
}

} // namespace

int main() {
    const std::shared_ptr<Control> control = make_control<Control>(StableId("part.painting"));
    silent_roles(*control);
    surfaces_and_cues(*control);
    marks_and_text(*control);
    state_context(*control);
    overlay_shadow(*control);
    theme_without_overlay_shadow();
    families();
    std::cout << "by-role paint path: silent roles, surfaces, cues, marks, text, states, overlay shadow "
                 "and families passed\n";
    return 0;
}
