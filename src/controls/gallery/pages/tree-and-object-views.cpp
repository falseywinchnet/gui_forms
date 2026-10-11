#include "pages.hpp"

namespace gui_forms::gallery::pages {

// Page under construction: TreeView, ObjectView, CorrespondenceView, BreadcrumbTrail.
void add_tree_and_object_views(const std::shared_ptr<Surface>& page,
                               const std::shared_ptr<PageContext>& context) {
    static_cast<void>(context);
    (*page).add_at(label("gallery.tree-and-object-views.heading", "TREES AND OBJECT VIEWS", 22.0, 700, shell_blue_dark),
                   {24.0, 18.0, 620.0, 34.0});
    (*page).add_at(label("gallery.tree-and-object-views.pending", "TreeView, ObjectView, CorrespondenceView and BreadcrumbTrail over one fixture. This page is being written.",
                         12.0, 400, muted),
                   {26.0, 52.0, 920.0, 24.0});
}

}  // namespace gui_forms::gallery::pages
