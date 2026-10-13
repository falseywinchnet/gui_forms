#include "support.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <span>
#include <string_view>

namespace gallery_pages_tests {
namespace {

// Registry ids whose controls do not yet declare their own anatomy (stage
// T1). The coverage test fails when this list and the Gallery disagree in
// either direction, so a control that gains an anatomy must leave the list
// in the same change. The T1 gate is this list empty.
constexpr std::array<std::string_view, 33> anatomy_pending{
    "command-bar",
    "text-box",
    "review-card",
    "list-box",
    "checked-list-box",
    "combo-box",
    "tree-view",
    "object-view",
    "correspondence-view",
    "choice-group",
    "breadcrumb-trail",
    "range-control",
    "numeric-up-down",
    "color-value-editor",
    "flags-value-editor",
    "property-grid",
    "property-list",
    "instrument-rack",
    "date-time-picker",
    "expandable-sections",
    "tab-control",
    "split-container",
    "command-overflow-panel",
    "master-detail-view",
    "menu-strip",
    "context-menu",
    "drawing-surface",
    "raster-canvas",
    "easing-preview",
    "metrics-view",
    "typography-specimen",
    "visual-inspector-view",
    "visual-inspector-overlay",
};

// Registry entries that are not controls: nonvisual components and the
// LiveSurface service type. They have no parts; a component that shows
// something does so through an internal control.
constexpr std::array<std::string_view, 11> nonvisual_entries{
    "command",
    "tool-tip",
    "timer",
    "image-list",
    "error-provider",
    "help-provider",
    "semantic-feedback",
    "binding",
    "binding-source",
    "binding-context",
    "live-surface",
};

bool finite_rect(Rect rect) {
    return std::isfinite(rect.x) && std::isfinite(rect.y) &&
           std::isfinite(rect.width) && std::isfinite(rect.height);
}

bool contains_id(std::span<const std::string> ids, std::string_view id) {
    for (std::size_t index = 0; index < ids.size(); ++index) {
        if (ids[index] == id) return true;
    }
    return false;
}

bool pending(std::string_view id) {
    for (std::size_t index = 0; index < anatomy_pending.size(); ++index) {
        if (anatomy_pending[index] == id) return true;
    }
    return false;
}

bool nonvisual(std::string_view id) {
    for (std::size_t index = 0; index < nonvisual_entries.size(); ++index) {
        if (nonvisual_entries[index] == id) return true;
    }
    return false;
}

// Checks one control's published anatomy and records its declaring id.
void inspect_control(const Control& control, std::vector<std::string>& seen_ids) {
    const ControlAnatomy& anatomy = control.anatomy();
    const AnatomyValidation validation = validate_control_anatomy(anatomy);
    if (validation.defect != AnatomyDefect::none) {
        throw std::runtime_error(std::string(anatomy.control_id) +
                                 " anatomy is malformed: " +
                                 std::string(anatomy_defect_name(validation.defect)));
    }
    std::vector<PartGeometry> geometry(anatomy.parts.size());
    control.fill_part_geometry(geometry);
    for (std::size_t index = 0; index < geometry.size(); ++index) {
        const PartGeometry& part = geometry[index];
        if (!finite_rect(part.bounds) || (!part.present && !part.bounds.empty())) {
            throw std::runtime_error(std::string(anatomy.control_id) + " part " +
                                     std::string(anatomy.parts[index].name) +
                                     " publishes invalid geometry");
        }
    }
    const AnatomyStateSet active = control.anatomy_states();
    for (std::size_t index = 0; index < anatomy_state_count; ++index) {
        const AnatomyState state = static_cast<AnatomyState>(index);
        if (active.contains(state) && !declares_state(anatomy, state)) {
            throw std::runtime_error(std::string(anatomy.control_id) +
                                     " publishes undeclared state " +
                                     std::string(anatomy_state_name(state)));
        }
    }
    const std::string id(anatomy.control_id);
    if (!contains_id(seen_ids, id)) seen_ids.push_back(id);
}

void inspect_tree(const Control::Ptr& root, std::vector<std::string>& seen_ids,
                  std::size_t& inspected) {
    std::vector<Control::Ptr> pending_controls{root};
    while (!pending_controls.empty()) {
        const Control::Ptr control = pending_controls.back();
        pending_controls.pop_back();
        inspect_control(*control, seen_ids);
        ++inspected;
        const std::span<const Control::Ptr> children = (*control).children();
        for (std::size_t index = 0; index < children.size(); ++index) {
            pending_controls.push_back(children[index]);
        }
    }
}

} // namespace

// T1 gate test: lay out every Gallery page, read every control's part
// geometry and states, and compare the registry against the controls that
// publish their own anatomy.
void test_anatomy_coverage() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    std::vector<std::string> seen_ids;
    std::size_t inspected = 0;
    for (const gallery::GalleryPageSpec& page : gallery::gallery_pages) {
        require(gallery::select_gallery_page(window, page.id),
                "every Gallery page must be selectable");
        window.perform_layout();
        inspect_tree(window.root(), seen_ids, inspected);
    }
    // Registry types the Gallery does not attach to its root tree: base
    // types reached only through subclasses, and popups.
    const std::shared_ptr<ButtonBase> button_base =
        make_control<ButtonBase>(StableId("anatomy.coverage.button-base"), "Base");
    const std::shared_ptr<ScrollBar> scroll_bar =
        make_control<ScrollBar>(StableId("anatomy.coverage.scroll-bar"));
    // Popups open only on demand and live outside the root tree.
    const std::shared_ptr<AnchoredPopupLayer> popup_layer = make_control<AnchoredPopupLayer>(
        StableId("anatomy.coverage.anchored-popup-layer"), button_base,
        AnchoredPopupPlacement{});
    inspect_control(*button_base, seen_ids);
    inspect_control(*scroll_bar, seen_ids);
    inspect_control(*popup_layer, seen_ids);
    inspected += 3U;

    std::string unexpected_gaps;
    std::string stale_pending;
    std::size_t covered = 0;
    std::size_t controls = 0;
    for (const gallery::CatalogEntry& entry : gallery::control_catalog) {
        if (nonvisual(entry.id)) continue;
        ++controls;
        const bool declared = contains_id(seen_ids, entry.id);
        if (declared) ++covered;
        if (!declared && !pending(entry.id)) {
            unexpected_gaps += " " + std::string(entry.id);
        }
        if (declared && pending(entry.id)) {
            stale_pending += " " + std::string(entry.id);
        }
    }
    std::cout << "anatomy coverage: " << covered << " of " << controls
              << " registry controls (" << nonvisual_entries.size()
              << " nonvisual entries excluded); "
              << inspected << " control instances inspected\n";
    if (!unexpected_gaps.empty()) {
        throw std::runtime_error("registry controls without their own anatomy on a "
                                 "laid-out Gallery page:" + unexpected_gaps);
    }
    if (!stale_pending.empty()) {
        throw std::runtime_error("anatomy_pending lists controls that now declare "
                                 "an anatomy; remove them:" + stale_pending);
    }
}

} // namespace gallery_pages_tests
