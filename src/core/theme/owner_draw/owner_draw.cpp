#include "gui_forms/theme/owner_draw/owner_draw.hpp"

#include "gui_forms/control/control/control.hpp"
#include "gui_forms/theme/part_painting/part_painting.hpp"
#include "gui_forms/theme/theme/theme.hpp"
#include "gui_forms/types/painter/painter.hpp"

namespace gui_forms {
namespace {

// Saves a painter's state for one owner-draw call and restores it when the
// call ends, normally or by an exception from a handler.
class PainterStateScope final {
public:
    explicit PainterStateScope(Painter& painter) : painter_(painter) { painter_.save(); }
    ~PainterStateScope() { painter_.restore(); }
    PainterStateScope(const PainterStateScope&) = delete;
    PainterStateScope& operator=(const PainterStateScope&) = delete;

private:
    Painter& painter_;
};

} // namespace

Color item_content_text(const Control& control, ControlVisualRole family, AnatomyStateSet states) noexcept {
    const ControlVisualContext context = part_visual_context(control, states);
    const Color text = control.effective_theme().resolve(family, context).text;
    return text;
}

bool raise_item_content_paint(Event<ItemContentPaint&>& event, ItemContentPaint& item) {
    if (item.content.empty() || !item.content.finite()) return false;
    item.draw_default = false;
    {
        const PainterStateScope scope(item.painter);
        item.painter.clip_rect(item.content);
        event.emit(item);
    }
    const bool draw_default = item.draw_default;
    return draw_default;
}

} // namespace gui_forms
