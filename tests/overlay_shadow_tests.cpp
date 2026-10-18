// The overlay shadow (owner decision RB-37) and the tooltip family (RB-38)
// on the surfaces that really overlap content: ToolTip's and RibbonToolTip's
// bubbles and ContextMenu's panels cast the theme's overlay shadow from
// their own rect, report its reach as visual outsets so that damage and
// replay cover it, and cast none in high contrast. Both bubbles paint their
// face from the `tool_tip` family.

#include "gui_forms/gui_forms.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

class NullPainter final : public Painter {
public:
    void save() override {}
    void restore() override {}
    void translate(Point) override {}
    void clip_rect(Rect) override {}
    void clip_rounded_rect(Rect, double) override {}
    void fill_rect(Rect, Color) override {}
    void fill_rounded_rect(Rect, double, Color) override {}
    void stroke_rect(Rect, Color, double) override {}
    void stroke_rounded_rect(Rect, double, Color, double) override {}
    void fill_linear_gradient(Rect, Point, Point, std::span<const GradientStop>) override {}
    void fill_radial_gradient(Rect, Point, Size, std::span<const GradientStop>) override {}
    void draw_box_shadow(Rect, double, Point, double, double, Color) override {}
    void draw_line(Point, Point, Color, double) override {}
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override {}
    void draw_image(ImageId, Rect, double) override {}
};

// A face colour no shipped family uses, so a bubble's face is recognisable
// as drawn from the tooltip family.
constexpr Color marker_face = Color::rgba(201, 222, 243);

std::shared_ptr<const Theme> marked_theme() {
    ThemeDefinition definition = windows_professional_theme_definition();
    definition.id = "overlay-shadow-marked";
    ControlRoleRecipes& tool_tip = definition.roles[static_cast<std::size_t>(ControlVisualRole::tool_tip)];
    for (ControlVisualRecipe& recipe : tool_tip.ordinary) {
        recipe.material.fills = {MaterialFillLayer::solid(marker_face)};
    }
    const std::shared_ptr<const Theme> theme = Theme::create(definition);
    return theme;
}

void paint_all(Window& window) {
    window.perform_layout();
    NullPainter painter;
    const Size client = window.client_size();
    static_cast<void>(window.paint(painter, Rect{0.0, 0.0, client.width, client.height}));
}

// The first popup-tree control whose stable id ends with `suffix`.
const VisualControlInspection* find_overlay(const VisualInspectionSnapshot& snapshot, std::string_view suffix) {
    for (const VisualControlInspection& control : snapshot.controls) {
        const std::string_view id = control.stable_id;
        if (control.popup_tree && id.size() >= suffix.size() &&
            id.substr(id.size() - suffix.size()) == suffix) {
            return &control;
        }
    }
    return nullptr;
}

std::vector<const VisualPaintOperationSnapshot*> operations_of(const VisualControlInspection& control,
                                                               VisualPaintOperation kind) {
    std::vector<const VisualPaintOperationSnapshot*> found;
    for (const VisualPaintOperationSnapshot& operation : control.display_operations) {
        if (operation.operation == kind) found.push_back(&operation);
    }
    return found;
}

bool painted_colour(const VisualControlInspection& control, Color color) {
    for (const VisualPaintOperationSnapshot& operation : control.display_operations) {
        if (operation.operation != VisualPaintOperation::draw_box_shadow && operation.color == color) return true;
    }
    return false;
}

// `overlay` cast the window theme's overlay shadow from its own rect, before
// anything else it painted, and reports its reach unclipped by its layer.
void require_cast_shadow(const VisualControlInspection& overlay, const Theme& theme, const char* message) {
    const std::vector<MaterialShadow>& layers = theme.overlay_shadow(false);
    const std::vector<const VisualPaintOperationSnapshot*> shadows =
        operations_of(overlay, VisualPaintOperation::draw_box_shadow);
    const Rect local{0.0, 0.0, overlay.layout.absolute_bounds.width, overlay.layout.absolute_bounds.height};
    bool cast = shadows.size() == layers.size();
    for (std::size_t index = 0; cast && index < shadows.size(); ++index) {
        const VisualPaintOperationSnapshot& shadow = *shadows[index];
        cast = shadow.rect == local && shadow.color == layers[index].color &&
               shadow.first == layers[index].offset && shadow.command_index == index;
    }
    const Insets reach = box_shadow_visual_outsets(layers);
    const bool reported = overlay.layout.visual_outsets == reach && !overlay.layout.clipped_by_ancestor &&
                          overlay.layout.effective_clip == overlay.layout.visual_bounds;
    require(cast && reported, message);
}

void require_no_shadow(const VisualControlInspection& overlay, const char* message) {
    const bool none = operations_of(overlay, VisualPaintOperation::draw_box_shadow).empty() &&
                      overlay.layout.visual_outsets == Insets{};
    require(none, message);
}

void set_high_contrast(Window& window) {
    PresentationSettings settings = window.presentation_settings();
    settings.high_contrast = true;
    window.set_presentation_settings(settings);
}

struct Fixture final {
    Fixture() : window(root, {480.0, 300.0}) {
        (*root).set_requested_bounds({0.0, 0.0, 480.0, 300.0});
        (*button).set_requested_bounds({60.0, 60.0, 140.0, 30.0});
        (*root).add_child(button);
        window.set_theme(theme);
        window.perform_layout();
    }

    std::shared_ptr<const Theme> theme = marked_theme();
    Control::Ptr root = make_control<Control>(StableId("overlay.root"));
    std::shared_ptr<Button> button = make_control<Button>(StableId("overlay.target"), "Target");
    Window window;
};

void plain_tool_tip() {
    Fixture fixture;
    ToolTip tips(fixture.window);
    tips.set_tool_tip(fixture.button, "Opens the calibrated input selector");
    tips.show(fixture.button);
    paint_all(fixture.window);
    const VisualInspectionSnapshot snapshot = fixture.window.visual_inspection_snapshot();
    const VisualControlInspection* bubble = find_overlay(snapshot, ".bubble");
    require(bubble != nullptr, "the plain tooltip shows its bubble");
    require_cast_shadow(*bubble, *fixture.theme, "the plain tooltip's bubble casts the overlay shadow");
    require(painted_colour(*bubble, marker_face), "the plain tooltip's face comes from the tooltip family");
    const VisualControlInspection* text = find_overlay(snapshot, ".bubble.text");
    const ControlVisualContext ordinary{};
    require(text != nullptr &&
                painted_colour(*text, (*fixture.theme).resolve(ControlVisualRole::tool_tip, ordinary).text),
            "the plain tooltip's text takes the tooltip family's colour");

    // Hiding damages the whole shadow, not only the bubble's rect.
    const Rect reach = (*bubble).layout.visual_bounds;
    static_cast<void>(fixture.window.take_damage());
    tips.hide();
    const Rect damaged = fixture.window.take_damage().bounds();
    require(Rect::united(damaged, reach) == damaged, "hiding the bubble damages its shadow's reach");

    set_high_contrast(fixture.window);
    tips.show(fixture.button);
    paint_all(fixture.window);
    const VisualInspectionSnapshot contrast = fixture.window.visual_inspection_snapshot();
    const VisualControlInspection* plain = find_overlay(contrast, ".bubble");
    require(plain != nullptr, "the plain tooltip shows in high contrast");
    require_no_shadow(*plain, "the plain tooltip casts no shadow in high contrast");
    const VisualControlInspection* contrast_text = find_overlay(contrast, ".bubble.text");
    ControlVisualContext high{};
    high.high_contrast = true;
    require(contrast_text != nullptr &&
                painted_colour(*contrast_text, (*fixture.theme).resolve(ControlVisualRole::tool_tip, high).text),
            "the plain tooltip's text follows the tooltip family into high contrast");
}

void enhanced_tool_tip() {
    Fixture fixture;
    RibbonToolTip tips(fixture.window);
    RibbonToolTipContent content;
    content.title = "Paste";
    content.description = "Add content from the Clipboard.";
    tips.set_tool_tip(fixture.button, content);
    tips.show(fixture.button);
    paint_all(fixture.window);
    const VisualInspectionSnapshot snapshot = fixture.window.visual_inspection_snapshot();
    const VisualControlInspection* bubble = find_overlay(snapshot, ".bubble");
    require(bubble != nullptr && (*bubble).stable_id.starts_with("ribbon-tooltip."),
            "the enhanced tooltip shows its bubble");
    require_cast_shadow(*bubble, *fixture.theme, "the enhanced tooltip's bubble casts the overlay shadow");
    require(painted_colour(*bubble, marker_face), "the enhanced tooltip's face comes from the tooltip family");
    require(tips.overlay_bounds().width == (*bubble).layout.absolute_bounds.width,
            "the enhanced tooltip's bounds are its face; the shadow falls outside them");

    set_high_contrast(fixture.window);
    tips.show(fixture.button);
    paint_all(fixture.window);
    const VisualInspectionSnapshot contrast = fixture.window.visual_inspection_snapshot();
    const VisualControlInspection* plain = find_overlay(contrast, ".bubble");
    require(plain != nullptr, "the enhanced tooltip shows in high contrast");
    require_no_shadow(*plain, "the enhanced tooltip casts no shadow in high contrast");
}

void context_menu() {
    Fixture fixture;
    std::shared_ptr<Command> open = std::make_shared<Command>("open", "Open");
    std::shared_ptr<Command> copy = std::make_shared<Command>("copy", "Copy");
    ContextMenu menu("overlay.context");
    menu.set_items({
        {"open", MenuItemKind::command, open},
        {"copy", MenuItemKind::command, copy},
    });
    menu.show(fixture.button, {100.0, 100.0}, MenuOpenMode::pointer);
    paint_all(fixture.window);
    const VisualInspectionSnapshot snapshot = fixture.window.visual_inspection_snapshot();
    const VisualControlInspection* panel = snapshot.find("overlay.context.popup.panel.0");
    require(panel != nullptr && (*panel).popup_tree, "the context menu shows its panel");
    require_cast_shadow(*panel, *fixture.theme, "the context menu's panel casts the overlay shadow");
    menu.close();

    set_high_contrast(fixture.window);
    menu.show(fixture.button, {100.0, 100.0}, MenuOpenMode::pointer);
    paint_all(fixture.window);
    const VisualInspectionSnapshot contrast = fixture.window.visual_inspection_snapshot();
    const VisualControlInspection* plain = contrast.find("overlay.context.popup.panel.0");
    require(plain != nullptr, "the context menu shows in high contrast");
    require_no_shadow(*plain, "the context menu casts no shadow in high contrast");
    menu.close();
}

} // namespace

int main() {
    plain_tool_tip();
    enhanced_tool_tip();
    context_menu();
    std::cout << "Overlay shadow and tooltip family: plain and enhanced tooltips and the context menu passed\n";
    return 0;
}
