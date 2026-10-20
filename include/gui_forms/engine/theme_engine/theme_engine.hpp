#pragma once

#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"

namespace gui_forms {

class Control;
class Painter;
struct PartPaint;

// The theme engine's entry point (T2 plan, ThemeEngine). paint_part hands it
// every part; controls never call it directly.
//
// Ownership: a Theme owns its engine, and every window showing the theme
// shares it. An application's windows share one theme (ApplicationResources),
// so they share one engine. The engine is immutable once its theme is made:
// it holds the theme's rules (from T3) and borrows the built-in primitive
// registry. What changes from frame to frame belongs to each window's
// EnginePartTable.
//
// The engine draws a part with a primitive only when a rule of its theme
// selects a registered primitive for the part, the parameters validate and
// the cost is within budget. In every other case the built-in recipe draws
// it: the body paint_part had before the engine existed. No theme has rules
// before T3, so today every part is drawn by the built-in recipe and every
// recorded command is what it was.
class ThemeEngine final {
public:
    ThemeEngine();

    // The primitives the engine's rules may select.
    [[nodiscard]] const PrimitiveRegistry& registry() const noexcept { return *registry_; }

    // Paints one part of `control` (see paint_part). A part with a key
    // (PartPaint::key) and paintable bounds, painted by a control attached
    // to a window, has its record in that window's part table made or
    // updated first. A part without a key, or of a detached control, is
    // drawn statically and leaves no record.
    void paint(Painter& painter, const Control& control, const PartPaint& part) const;

private:
    // Borrowed: the built-in registry has static storage.
    const PrimitiveRegistry* registry_;
};

} // namespace gui_forms
