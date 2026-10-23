#pragma once

#include "gui_forms/theme/theme/theme.hpp"

#include <memory>

namespace gui_forms::test_support {

// The professional theme definition, held on the heap. Tests hold their
// definitions through it: before the role recipes moved out of line
// (ControlRoleTable) a definition was some 105 KB, and the sanitizer build,
// which gives every local of a test's inlined functions its own slot, ran
// out of a 1 MB stack with a few in one main. `new` from the returned value
// builds the definition in place.
[[nodiscard]] inline std::unique_ptr<ThemeDefinition> professional_definition() {
    std::unique_ptr<ThemeDefinition> definition(new ThemeDefinition(windows_professional_theme_definition()));
    return definition;
}

} // namespace gui_forms::test_support
