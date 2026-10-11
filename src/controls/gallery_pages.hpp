#pragma once

#include "gallery/tree/gallery_tree.hpp"
#include "gui_forms/gui_forms.hpp"

#include <memory>
#include <string_view>

namespace gui_forms::gallery {

struct GalleryShell final {
    Control::Ptr root;
};

// Builds the registry-driven Gallery shell from public GUI.Forms controls only:
// the compiled-DML command strip, navigation grouped by catalog category, one
// retained surface per page (compiled-DML pages and the pages migrated from
// the retired Complete Showcase), the status strip, and the diagnostics
// overlay. `compiled` supplies the DML mounts; the shell places them.
[[nodiscard]] GalleryShell build_gallery_shell(const GalleryTree& compiled);

// Attaches the window-owned components (UI Timer, ToolTip, providers, binding
// source) that pages declared while building. Idempotent per window.
void initialize_gallery_runtime(Window& window);

} // namespace gui_forms::gallery
