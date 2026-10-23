#pragma once

#include "gui_forms/theme/theme/theme.hpp"

#include <memory>

namespace gui_forms::test_support {

// The professional theme definition, held on the heap. A ThemeDefinition is
// some 105 KB and is never kept on a test's stack (theme_types.hpp): the
// sanitizer build gives every local of a test's inlined functions its own
// slot, and a few definitions in one main exhaust a 1 MB thread stack.
// `new` from the returned value builds the definition on the heap directly;
// std::make_unique would first materialize it on the caller's frame.
[[nodiscard]] inline std::unique_ptr<ThemeDefinition> professional_definition() {
    std::unique_ptr<ThemeDefinition> definition(new ThemeDefinition(windows_professional_theme_definition()));
    return definition;
}

} // namespace gui_forms::test_support
