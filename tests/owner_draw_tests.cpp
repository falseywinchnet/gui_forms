// The content-only owner-draw seam (owner decision W-Q6): an item's content
// event is raised with the painter clipped to the content rectangle and
// restored afterwards, the default content is drawn only on request, and
// the reported text colour is the family recipe's for the item's states.

#include "gui_forms/control.hpp"
#include "gui_forms/theme.hpp"
#include "gui_forms/theme/owner_draw/owner_draw.hpp"
#include "gui_forms/types/painter/painter.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

// One painter call as the recording painter saw it.
struct Operation final {
    std::string kind;
    Rect bounds{};
};

class RecordingPainter final : public Painter {
public:
    void save() override { operations.push_back({"save", {}}); }
    void restore() override { operations.push_back({"restore", {}}); }
    void translate(Point) override {}
    void clip_rect(Rect rect) override { operations.push_back({"clip", rect}); }
    void fill_rect(Rect rect, Color) override { operations.push_back({"fill", rect}); }
    void fill_rounded_rect(Rect rect, double, Color) override { operations.push_back({"fill", rect}); }
    void stroke_rect(Rect rect, Color, double) override { operations.push_back({"stroke", rect}); }
    void stroke_rounded_rect(Rect rect, double, Color, double) override {
        operations.push_back({"stroke", rect});
    }
    void draw_line(Point, Point, Color, double) override { operations.push_back({"line", {}}); }
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override {
        operations.push_back({"text", {}});
    }
    void draw_image(ImageId, Rect destination, double) override { operations.push_back({"image", destination}); }

    std::vector<Operation> operations;
};

// A handler that fills the content it is given and counts its calls.
class FillContent final {
public:
    FillContent(std::size_t& calls, bool draw_default) noexcept : calls_(calls), draw_default_(draw_default) {}

    void operator()(ItemContentPaint& item) const {
        ++calls_;
        item.painter.fill_rect(item.content, item.text);
        if (draw_default_) item.draw_default = true;
    }

private:
    std::size_t& calls_;
    bool draw_default_{};
};

// A handler that fails after drawing.
class ThrowAfterDrawing final {
public:
    void operator()(ItemContentPaint& item) const {
        item.painter.fill_rect(item.content, item.text);
        throw std::runtime_error("handler failed");
    }
};

void clips_and_restores() {
    RecordingPainter painter;
    Event<ItemContentPaint&> event;
    std::size_t calls = 0U;
    SubscriptionToken token = event.subscribe(FillContent(calls, false));
    const Rect content{10.0, 4.0, 80.0, 16.0};
    ItemContentPaint item{.painter = painter, .index = 2U, .stable_id = "item.2", .content = content};
    const bool draw_default = raise_item_content_paint(event, item);
    require(calls == 1U && !draw_default, "one raise calls the handler once and draws no default content");
    require(painter.operations.size() == 4U && painter.operations[0].kind == "save" &&
                painter.operations[1].kind == "clip" && painter.operations[1].bounds == content &&
                painter.operations[2].kind == "fill" && painter.operations[3].kind == "restore",
            "the handler draws between a save, a clip to the content and a restore");
    token.disconnect();
}

void default_content_on_request() {
    RecordingPainter painter;
    Event<ItemContentPaint&> event;
    std::size_t calls = 0U;
    SubscriptionToken token = event.subscribe(FillContent(calls, true));
    ItemContentPaint item{.painter = painter, .content = {0.0, 0.0, 40.0, 12.0}, .draw_default = true};
    require(raise_item_content_paint(event, item) && calls == 1U,
            "a handler that sets draw_default asks for the default content");
    token.disconnect();

    Event<ItemContentPaint&> unobserved;
    ItemContentPaint stale{.painter = painter, .content = {0.0, 0.0, 40.0, 12.0}, .draw_default = true};
    require(!raise_item_content_paint(unobserved, stale),
            "draw_default starts false on every raise, whatever the caller left in it");
}

void empty_content_raises_nothing() {
    RecordingPainter painter;
    Event<ItemContentPaint&> event;
    std::size_t calls = 0U;
    SubscriptionToken token = event.subscribe(FillContent(calls, true));
    ItemContentPaint empty{.painter = painter, .content = {4.0, 4.0, 0.0, 12.0}};
    require(!raise_item_content_paint(event, empty) && calls == 0U && painter.operations.empty(),
            "an empty content rectangle raises nothing and touches no painter state");
    token.disconnect();
}

void restores_when_a_handler_throws() {
    RecordingPainter painter;
    Event<ItemContentPaint&> event;
    SubscriptionToken token = event.subscribe(ThrowAfterDrawing{});
    ItemContentPaint item{.painter = painter, .content = {0.0, 0.0, 20.0, 10.0}};
    bool thrown = false;
    try {
        static_cast<void>(raise_item_content_paint(event, item));
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    require(thrown && !painter.operations.empty() && painter.operations.back().kind == "restore",
            "a throwing handler's exception passes through with the painter restored");
    token.disconnect();
}

void reports_the_recipe_text() {
    const std::shared_ptr<Control> control = make_control<Control>(StableId("owner.draw"));
    const Theme& theme = (*control).effective_theme();
    AnatomyStateSet selected;
    selected.insert(AnatomyState::selected);
    const ControlVisualContext selected_context = part_visual_context(*control, selected);
    require(item_content_text(*control, ControlVisualRole::selection, selected) ==
                theme.resolve(ControlVisualRole::selection, selected_context).text,
            "a selected item's text is the selection recipe's selected text");
    AnatomyStateSet disabled;
    disabled.insert(AnatomyState::disabled);
    const ControlVisualContext disabled_context = part_visual_context(*control, disabled);
    const Color disabled_text = item_content_text(*control, ControlVisualRole::selection, disabled);
    require(disabled_context.surface == ControlSurfaceState::disabled &&
                disabled_text == theme.resolve(ControlVisualRole::selection, disabled_context).text &&
                !(disabled_text == item_content_text(*control, ControlVisualRole::selection, AnatomyStateSet{})),
            "a disabled item's text is the selection recipe's disabled text, apart from its normal text");
}

} // namespace

int main() {
    clips_and_restores();
    default_content_on_request();
    empty_content_raises_nothing();
    restores_when_a_handler_throws();
    reports_the_recipe_text();
    std::cout << "owner draw: clip and restore, default content on request, empty content, "
                 "throwing handlers and recipe text passed\n";
    return 0;
}
