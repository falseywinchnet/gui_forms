#include "correspondence.hpp"

#include "correspondence_controls.hpp"

#include <utility>

namespace gui_forms::correspondence {

std::unique_ptr<Window> make_correspondence()
{
    CorrespondenceTree tree = build_correspondence_tree();
    std::unique_ptr<Window> window =
        std::make_unique<Window>(tree.root, Size{1120.0, 720.0});
    // The shell projected its model once while detached; repeat inside the
    // window so the first layout sees the attached state, then install the
    // hover tooltips, which need a live window.
    (*tree.root).synchronize();
    (*tree.root).attach_tooltips();
    (*window).perform_layout();
    return window;
}

}  // namespace gui_forms::correspondence
