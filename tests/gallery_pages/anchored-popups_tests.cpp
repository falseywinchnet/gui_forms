#include "page_tests.hpp"

namespace gallery_pages_tests {
namespace {

std::shared_ptr<AnchoredPopupLayer> open_layer(Window& window) {
    window.perform_layout();
    return std::dynamic_pointer_cast<AnchoredPopupLayer>(window.find("gallery.popups.layer"));
}

} // namespace

void test_anchored_popups(gui_forms::Window& window) {
    select_page(window, "anchored-popups");
    require(window.find("gallery.anchored-popups.heading") != nullptr,
            "anchored-popups page shows its heading");
    const std::shared_ptr<Label> status =
        std::dynamic_pointer_cast<Label>(window.find("gallery.popups.status"));
    const std::shared_ptr<Button> top_left =
        std::dynamic_pointer_cast<Button>(window.find("gallery.popups.anchor.0"));
    const std::shared_ptr<Button> bottom_right =
        std::dynamic_pointer_cast<Button>(window.find("gallery.popups.anchor.2"));
    const std::shared_ptr<CheckBox> above =
        std::dynamic_pointer_cast<CheckBox>(window.find("gallery.popups.above"));
    const std::shared_ptr<CheckBox> flip =
        std::dynamic_pointer_cast<CheckBox>(window.find("gallery.popups.flip"));
    require(status && top_left && bottom_right && above && flip &&
                window.find("gallery.popups.layer") == nullptr,
            "the page must publish its anchors and options, and no popup before one opens");

    // Opening: the layer becomes a popup root owned by the anchor, focus is
    // contained in the content field, and the anchor reports expanded state.
    click(window, top_left);
    std::shared_ptr<AnchoredPopupLayer> layer = open_layer(window);
    require(layer && window.focus_scope_depth() == 1U &&
                window.focused_control() == window.find("gallery.popups.field") &&
                (*top_left).expanded_state() == std::optional<bool>(true) &&
                !(*layer).resolved_placement().placed_above &&
                (*status).text().find("Popup open from gallery.popups.anchor.0") == 0U,
            "an anchor click must open the layer as a focus-contained popup placed below");
    const Rect anchor_bounds = (*top_left).absolute_bounds();
    const Rect content_bounds = (*window.find("gallery.popups.content")).absolute_bounds();
    require(content_bounds.y >= anchor_bounds.y + anchor_bounds.height &&
                content_bounds.x == anchor_bounds.x && content_bounds.height == 200.0,
            "near alignment below must place the content under the anchor's leading edge");
    require(window.semantic_snapshot().to_json().find("gallery.popups.layer") != std::string::npos,
            "an open layer must publish its dialog boundary in the semantic snapshot");

    // A Value change re-resolves the open popup in place.
    (*above).set_checked(true);
    window.perform_layout();
    require((*layer).resolved_placement().placed_above &&
                (*status).text().find("Placement re-resolved while open · placed above") == 0U,
            "the bound Prefer-above Value must re-place the open popup above its anchor");

    // Click-away: the press is reported, the owner closes the popup, and the
    // press does not fall through to the page.
    const Point away = center(window.find("gallery.popups.options.note"));
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary, away}),
            "a press outside the popup must be consumed by the layer");
    static_cast<void>(window.dispatch_pointer({PointerAction::up, PointerButton::primary, away}));
    require(window.find("gallery.popups.layer") == nullptr && window.focus_scope_depth() == 0U &&
                (*top_left).expanded_state() == std::optional<bool>(false) &&
                (*status).text().find("Dismissed by click-away") == 0U,
            "click-away must close the popup through the owner and restore the page");

    // Flipping: the bottom-right anchor cannot fit the popup below.
    (*above).set_checked(false);
    click(window, bottom_right);
    layer = open_layer(window);
    require(layer && (*layer).resolved_placement().placed_above &&
                !(*layer).resolved_placement().height_clamped,
            "a bottom anchor must flip the popup above when below cannot fit");
    require(window.dispatch_key({KeyAction::down, PhysicalKey::escape}) &&
                window.find("gallery.popups.layer") == nullptr &&
                window.focused_control() == bottom_right &&
                (*status).text().find("Dismissed by Escape") == 0U,
            "Escape must be intercepted by the layer and close the popup with focus restored");

    // Without flipping the popup stays on the preferred side at its requested
    // height and its position is clamped inside the client margin, so it
    // slides up over the anchor; height is clamped only to the client itself.
    click(window, flip);
    click(window, bottom_right);
    layer = open_layer(window);
    const AnchoredPopupPlacementResult clamped = (*layer).resolved_placement();
    const Rect anchor_bottom_right = (*bottom_right).absolute_bounds();
    require(layer && !clamped.placed_above && !clamped.height_clamped &&
                clamped.bounds.height == 200.0 &&
                clamped.bounds.y + clamped.bounds.height <= window.client_size().height - 4.0 &&
                clamped.bounds.y < anchor_bottom_right.y + anchor_bottom_right.height,
            "with vertical flipping disabled the popup must keep its side and height and be clamped into the client");
    click(window, window.find("gallery.popups.close"));
    require(window.find("gallery.popups.layer") == nullptr && window.focus_scope_depth() == 0U &&
                window.focused_control() == bottom_right &&
                (*status).text().find("Closed by its button") == 0U,
            "the content's Close button must end the scope and release the popup");
    click(window, flip);
}

}  // namespace gallery_pages_tests
