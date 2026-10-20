#include "gui_forms/engine/theme_engine/theme_engine.hpp"

#include "gui_forms/control/control/control.hpp"
#include "gui_forms/theme/part_painting/part_painting.hpp"
#include "gui_forms/window/window.hpp"
#include "../../theme/part_painting/built_in_recipe.hpp"

namespace gui_forms {

ThemeEngine::ThemeEngine() : registry_(&builtin_primitive_registry()) {}

void ThemeEngine::paint(Painter& painter, const Control& control, const PartPaint& part) const {
    if (part.key && part_bounds_paintable(part.bounds)) {
        Window* const window = control.attached_window();
        if (window != nullptr) (*window).note_engine_part(control, *part.key, part.states);
    }
    // No theme has rules yet (T3), so nothing selects a primitive.
    paint_part_with_built_in_recipe(painter, control, part);
}

} // namespace gui_forms
