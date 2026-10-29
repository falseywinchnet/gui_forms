#pragma once

#include "gui_forms/resources/image_registry/image_registry.hpp"

#include "../../resources/image_registry/image_registry_access.hpp"

#include <memory>
#include <utility>

// Internal: an image a compiled theme package loaded (theme_compiler.cpp).

namespace gui_forms::detail {

// An image owned by the themes holding it: the last one released removes
// it from its registry, unless the registry is gone.
class ThemeImageLease final {
public:
    ThemeImageLease(std::weak_ptr<ImageRegistryLiveness> registry, ImageId image, Size pixel_size) noexcept
        : registry_(std::move(registry)), image_(image), pixel_size_(pixel_size) {}
    ~ThemeImageLease() {
        const std::shared_ptr<ImageRegistryLiveness> live = registry_.lock();
        if (live != nullptr && (*live).registry != nullptr) static_cast<void>((*(*live).registry).remove(image_));
    }
    ThemeImageLease(const ThemeImageLease&) = delete;
    ThemeImageLease& operator=(const ThemeImageLease&) = delete;

    [[nodiscard]] ImageId image() const noexcept { return image_; }
    [[nodiscard]] Size pixel_size() const noexcept { return pixel_size_; }
    [[nodiscard]] const std::weak_ptr<ImageRegistryLiveness>& registry() const noexcept { return registry_; }

private:
    std::weak_ptr<ImageRegistryLiveness> registry_;
    ImageId image_{};
    Size pixel_size_{};
};

} // namespace gui_forms::detail
