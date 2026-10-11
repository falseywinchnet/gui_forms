#pragma once

#include "gui_forms/window.hpp"

#include <memory>

namespace gui_forms::correspondence {

// Builds the fixture-backed Correspondence mail-client demonstration. The
// retained shell owns its CorrespondenceModel; callers only own the native
// Window boundary, exactly like gallery::make_gallery().
[[nodiscard]] std::unique_ptr<Window> make_correspondence();

}  // namespace gui_forms::correspondence
