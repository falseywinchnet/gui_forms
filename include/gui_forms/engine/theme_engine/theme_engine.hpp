#pragma once

#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"
#include "gui_forms/engine/theme_rules/theme_rules.hpp"

#include <cstdint>

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
// it holds the theme's compiled rules (the cascade, E4) and borrows the
// built-in primitive registry. What changes from frame to frame belongs to
// each window's EnginePartTable.
//
// The engine draws a part with a primitive only when the cascade selects a
// registered primitive for the part, its parameters validate, the cost is
// within budget (GIVEN F1-1) and the presentation is not forced high
// contrast. In every other case the built-in recipe draws it: the body
// paint_part had before the engine existed. A theme with no rules draws
// every part with the built-in recipe, as before E4.
class ThemeEngine final {
public:
    // An engine with no rules.
    ThemeEngine();
    // An engine with `sheet` compiled against the built-in registry.
    // Invalid rules and declarations are dropped with diagnostics; nothing
    // throws for them.
    explicit ThemeEngine(const ThemeRuleSheet& sheet);

    // The primitives the engine's rules may select.
    [[nodiscard]] const PrimitiveRegistry& registry() const noexcept { return *registry_; }
    [[nodiscard]] const CompiledThemeRules& rules() const noexcept { return rules_; }

    // How the engine draws one part, and why.
    enum class PartDrawing : std::uint8_t {
        // The cascade's primitive draws it.
        primitive,
        // No rule selects a primitive, or the winning rule selects built_in.
        built_in_recipe,
        // Forced high contrast (CascadeLevel::forced_high_contrast): the
        // engine's own pass draws every part with the built-in recipe, whose
        // high-contrast records keep geometry and state composition and draw
        // no gradient, texture, glow or cast shadow.
        forced_high_contrast,
        // The selected primitive does not paint the part's role.
        role_mismatch,
        // The part's primitive has an animated parameter and would cost more
        // than maximum_animated_operations_per_frame at its device size.
        over_frame_budget,
        // The part's bounds are empty, or the control is in no window.
        not_paintable,
    };

    struct PartPlan final {
        PartDrawing drawing{PartDrawing::built_in_recipe};
        CompiledThemeRules::Resolution resolution{};
        // The raster's size for a primitive, in device pixels.
        DeviceExtent extent{};
        DeviceScale scale{};
    };

    // The cascade's decision for one part of `control`, without painting.
    [[nodiscard]] PartPlan plan(const Control& control, const PartPaint& part) const noexcept;

    // Paints one part of `control` (see paint_part). A part with a key
    // (PartPaint::key) and paintable bounds, painted by a control attached
    // to a window, has its record in that window's part table made or
    // updated first; a primitive then draws into the record's cached raster.
    // A part without a key leaves no record; a primitive draws it into a
    // raster of its own for this paint. If the primitive refuses the part,
    // the built-in recipe draws it.
    void paint(Painter& painter, const Control& control, const PartPaint& part) const;

private:
    // Borrowed: the built-in registry has static storage.
    const PrimitiveRegistry* registry_;
    CompiledThemeRules rules_{};
};

} // namespace gui_forms
