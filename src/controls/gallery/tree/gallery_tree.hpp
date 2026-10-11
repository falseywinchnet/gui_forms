#pragma once

#include "../context/gallery_context.hpp"
#include "gallery_dml.hpp"
#include "gui_forms/gui_forms.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace gui_forms::gallery {

// One direct child of the declared `gallery.root`: the registry-driven shell
// mounts each of them (command strip, compiled-DML pages, diagnostics overlay).
struct GalleryMount final {
    const dml::NodeSpec* specification{};
    Control::Ptr control;
};

struct GalleryTree final {
    std::vector<GalleryMount> mounts;
    std::shared_ptr<GalleryContext> context;

    [[nodiscard]] Control::Ptr mount(std::string_view id) const;
};

// Instantiates every compiled-DML node except the declared boundary
// `gallery.root`; the shell owns placement of the returned mounts.
[[nodiscard]] GalleryTree build_gallery_tree();

} // namespace gui_forms::gallery
