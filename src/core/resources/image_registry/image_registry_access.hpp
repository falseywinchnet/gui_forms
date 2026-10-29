#pragma once

#include "gui_forms/resources/image_registry/image_registry.hpp"

#include <memory>

// Internal: a registry's identity for owners of its images that may
// outlive it (a compiled theme, which removes its images when released).

namespace gui_forms::detail {

// Names the registry holding a set of images: it follows the images when
// the registry is moved and is released when the registry is destroyed or
// a move assignment replaces its images.
struct ImageRegistryLiveness final {
    ImageRegistry* registry{};
};

struct ImageRegistryAccess final {
    // A weak handle that locks to `registry`'s liveness while its images
    // live. Like the registry, single-threaded.
    [[nodiscard]] static std::weak_ptr<ImageRegistryLiveness> liveness(ImageRegistry& registry);
};

} // namespace gui_forms::detail
