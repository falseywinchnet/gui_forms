#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_dock_and_anchor(const std::shared_ptr<Surface>& page,
                         const std::shared_ptr<PageContext>& context) {
    class ToggleDockEdgeClick final {
    public:
        ToggleDockEdgeClick(std::weak_ptr<Button> edge,
                            std::weak_ptr<Button> toggle,
                            std::weak_ptr<PageContext> context) noexcept
            : edge_(std::move(edge)), toggle_(std::move(toggle)),
              context_(std::move(context)) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<Button> edge = edge_.lock();
            const std::shared_ptr<Button> toggle = toggle_.lock();
            if (!edge || !toggle) return;
            (*edge).set_visible(!(*edge).visible());
            (*toggle).set_text((*edge).visible() ? "Hide left edge"
                                                 : "Restore left edge");
            if (const std::shared_ptr<PageContext> context = context_.lock()) {
                (*(*context).status).set_text(
                    (*edge).visible()
                        ? "Dock · left edge restored · fill contracted"
                        : "Dock · hidden edge released · fill expanded");
            }
        }

    private:
        std::weak_ptr<Button> edge_;
        std::weak_ptr<Button> toggle_;
        std::weak_ptr<PageContext> context_;
    };

    class SwapDockOrderClick final {
    public:
        SwapDockOrderClick(std::weak_ptr<Panel> canvas,
                           std::weak_ptr<Button> first,
                           std::weak_ptr<Button> second,
                           std::weak_ptr<Button> swap,
                           std::weak_ptr<PageContext> context,
                           std::shared_ptr<bool> top_b_front) noexcept
            : canvas_(std::move(canvas)), first_(std::move(first)),
              second_(std::move(second)), swap_(std::move(swap)),
              context_(std::move(context)), top_b_front_(std::move(top_b_front)) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<Panel> canvas = canvas_.lock();
            const std::shared_ptr<Button> first = first_.lock();
            const std::shared_ptr<Button> second = second_.lock();
            const std::shared_ptr<Button> swap = swap_.lock();
            if (!canvas || !first || !second || !swap) return;
            *top_b_front_ = !*top_b_front_;
            const std::shared_ptr<Button>& old_first = *top_b_front_ ? first : second;
            const std::shared_ptr<Button>& new_first = *top_b_front_ ? second : first;
            const std::optional<std::size_t> destination =
                (*canvas).child_index((*old_first).runtime_id());
            if (!destination ||
                !(*canvas).set_child_index((*new_first).runtime_id(), *destination)) {
                return;
            }
            (*swap).set_text(*top_b_front_ ? "Move Top A behind Top B"
                                           : "Move Top B behind Top A");
            if (const std::shared_ptr<PageContext> context = context_.lock()) {
                (*(*context).status).set_text(
                    *top_b_front_ ? "Dock · Top B now consumes the edge first"
                                  : "Dock · Top A now consumes the edge first");
            }
        }

    private:
        std::weak_ptr<Panel> canvas_;
        std::weak_ptr<Button> first_;
        std::weak_ptr<Button> second_;
        std::weak_ptr<Button> swap_;
        std::weak_ptr<PageContext> context_;
        std::shared_ptr<bool> top_b_front_;
    };

    class ResizeAnchorSpecimenClick final {
    public:
        ResizeAnchorSpecimenClick(
            std::weak_ptr<ScaledGroupBox> group,
            std::weak_ptr<Panel> canvas,
            std::weak_ptr<Button> resize,
            std::weak_ptr<PageContext> context,
            std::shared_ptr<bool> expanded) noexcept
            : group_(std::move(group)), canvas_(std::move(canvas)),
              resize_(std::move(resize)), context_(std::move(context)),
              expanded_(std::move(expanded)) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<ScaledGroupBox> group = group_.lock();
            const std::shared_ptr<Panel> canvas = canvas_.lock();
            const std::shared_ptr<Button> resize = resize_.lock();
            if (!group || !canvas || !resize) return;
            *expanded_ = !*expanded_;
            (*group).set_design_bounds(
                *canvas, *expanded_ ? Rect{18.0, 38.0, 730.0, 190.0}
                                    : Rect{18.0, 38.0, 600.0, 170.0});
            (*resize).set_text(*expanded_ ? "Restore specimen"
                                          : "Expand specimen");
            if (const std::shared_ptr<PageContext> context = context_.lock()) {
                (*(*context).status).set_text(
                    *expanded_
                        ? "Anchor · parent expanded · stretch/right/center updated"
                        : "Anchor · parent restored from retained design slot");
            }
        }

    private:
        std::weak_ptr<ScaledGroupBox> group_;
        std::weak_ptr<Panel> canvas_;
        std::weak_ptr<Button> resize_;
        std::weak_ptr<PageContext> context_;
        std::shared_ptr<bool> expanded_;
    };

    (*page).add_at(label("gallery.dock.heading", "DOCK AND ANCHOR", 22.0, 700,
                       shell_blue_dark), {24.0, 18.0, 620.0, 34.0});
    (*page).add_at(label(
                     "gallery.dock.lead",
                     "Base Control layout owns z-ordered docking, padded client consumption, compound edge anchors, and runtime rebasing.",
                     12.0, 400, muted),
                 {26.0, 52.0, 920.0, 24.0});

    std::shared_ptr<LayoutGroup> dock_group = group(
        "gallery.dock.group",
        "Dock · Left + Top + Top + Right + Bottom + Fill · z-order commands",
        {930.0, 260.0});
    (*page).add_at(dock_group, {24.0, 92.0, 930.0, 260.0});
    std::shared_ptr<gui_forms::Panel> dock_canvas = make_control<Panel>(StableId("gallery.dock.canvas"));
    (*dock_canvas).set_accessible_name("Dock layout specimen");
    (*dock_canvas).set_background(Color::rgba(238, 243, 247));
    (*dock_canvas).set_border_style(BorderStyle::sunken);
    (*dock_canvas).set_padding({8.0, 8.0, 8.0, 8.0});
    (*dock_group).add_at(dock_canvas, {18.0, 38.0, 610.0, 190.0});

    std::shared_ptr<gui_forms::Button> fill = make_control<Button>(StableId("gallery.dock.fill"),
                                     "Fill · remaining client");
    (*fill).set_visual_style(ButtonVisualStyle::accent);
    (*fill).set_requested_bounds({0.0, 0.0, 80.0, 30.0});
    (*fill).set_dock(DockStyle::fill);
    std::shared_ptr<gui_forms::Button> bottom = make_control<Button>(StableId("gallery.dock.bottom"),
                                       "Bottom · 26 px");
    (*bottom).set_requested_bounds({0.0, 0.0, 100.0, 26.0});
    (*bottom).set_dock(DockStyle::bottom);
    std::shared_ptr<gui_forms::Button> right = make_control<Button>(StableId("gallery.dock.right"),
                                      "Right");
    (*right).set_requested_bounds({0.0, 0.0, 76.0, 30.0});
    (*right).set_dock(DockStyle::right);
    std::shared_ptr<gui_forms::Button> top_second = make_control<Button>(
        StableId("gallery.dock.top.second"), "Top B · z-order");
    (*top_second).set_visual_style(ButtonVisualStyle::command);
    (*top_second).set_requested_bounds({0.0, 0.0, 100.0, 25.0});
    (*top_second).set_dock(DockStyle::top);
    std::shared_ptr<gui_forms::Button> top_first = make_control<Button>(
        StableId("gallery.dock.top.first"), "Top A · first edge");
    (*top_first).set_visual_style(ButtonVisualStyle::command);
    (*top_first).set_requested_bounds({0.0, 0.0, 100.0, 25.0});
    (*top_first).set_dock(DockStyle::top);
    std::shared_ptr<gui_forms::Button> left = make_control<Button>(StableId("gallery.dock.left"), "Left");
    (*left).set_requested_bounds({0.0, 0.0, 82.0, 30.0});
    (*left).set_dock(DockStyle::left);
    (*dock_canvas).add_child(left);
    (*dock_canvas).add_child(top_first);
    (*dock_canvas).add_child(top_second);
    (*dock_canvas).add_child(right);
    (*dock_canvas).add_child(bottom);
    (*dock_canvas).add_child(fill);

    std::shared_ptr<gui_forms::Button> toggle_left = make_control<Button>(
        StableId("gallery.dock.toggle-left"), "Hide left edge");
    (*toggle_left).set_visual_style(ButtonVisualStyle::accent);
    (*dock_group).add_at(toggle_left, {652.0, 52.0, 236.0, 34.0});
    std::shared_ptr<gui_forms::Button> swap_top = make_control<Button>(
        StableId("gallery.dock.swap-top"), "Move Top B behind Top A");
    (*dock_group).add_at(swap_top, {652.0, 98.0, 236.0, 34.0});
    std::shared_ptr<Label> dock_note = label(
        "gallery.dock.note",
        "Hidden docked controls consume zero extent. SetChildIndex changes edge order without rewriting preferred bounds.",
        11.0, 600, green);
    (*dock_note).set_text_wrapping(TextWrapping::word);
    (*dock_note).set_vertical_alignment(VerticalAlignment::near);
    (*dock_group).add_at(dock_note, {652.0, 148.0, 238.0, 70.0});

    const std::weak_ptr<Button> weak_left = left;
    const std::weak_ptr<Button> weak_toggle = toggle_left;
    const std::weak_ptr<PageContext> weak_context = context;
    (*context).subscriptions.push_back((*toggle_left).clicked().subscribe(
        *page, ToggleDockEdgeClick(weak_left, weak_toggle, weak_context)));
    const std::weak_ptr<Panel> weak_dock_canvas = dock_canvas;
    const std::weak_ptr<Button> weak_top_first = top_first;
    const std::weak_ptr<Button> weak_top_second = top_second;
    const std::weak_ptr<Button> weak_swap = swap_top;
    std::shared_ptr<bool> top_b_front = std::make_shared<bool>(false);
    (*context).subscriptions.push_back((*swap_top).clicked().subscribe(
        *page, SwapDockOrderClick(
            weak_dock_canvas, weak_top_first, weak_top_second, weak_swap,
            weak_context, top_b_front)));

    std::shared_ptr<LayoutGroup> anchor_group = group(
        "gallery.anchor.group",
        "Anchor · fixed · stretch · centered · bottom-right · live parent resize",
        {930.0, 250.0});
    (*page).add_at(anchor_group, {24.0, 374.0, 930.0, 250.0});
    std::shared_ptr<gui_forms::Panel> anchor_canvas = make_control<Panel>(StableId("gallery.anchor.canvas"));
    (*anchor_canvas).set_accessible_name("Compound anchor specimen");
    (*anchor_canvas).set_background(Color::rgba(247, 249, 251));
    (*anchor_canvas).set_border_style(BorderStyle::line);
    (*anchor_canvas).set_padding({8.0, 8.0, 8.0, 8.0});
    (*anchor_group).add_at(anchor_canvas, {18.0, 38.0, 600.0, 170.0});

    std::shared_ptr<gui_forms::Button> fixed = make_control<Button>(StableId("gallery.anchor.fixed"),
                                      "Left + Top");
    (*fixed).set_requested_bounds({18.0, 16.0, 126.0, 28.0});
    std::shared_ptr<gui_forms::Button> stretch = make_control<Button>(StableId("gallery.anchor.stretch"),
                                        "Left + Right · stretches");
    (*stretch).set_visual_style(ButtonVisualStyle::accent);
    (*stretch).set_requested_bounds({18.0, 56.0, 280.0, 28.0});
    (*stretch).set_anchor(AnchorStyles::left | AnchorStyles::right |
                        AnchorStyles::top);
    std::shared_ptr<gui_forms::Button> centered = make_control<Button>(StableId("gallery.anchor.centered"),
                                         "No edges · centered");
    (*centered).set_requested_bounds({190.0, 106.0, 152.0, 28.0});
    (*centered).set_anchor(AnchorStyles::none);
    std::shared_ptr<gui_forms::Button> bottom_right = make_control<Button>(
        StableId("gallery.anchor.bottom-right"), "Right + Bottom");
    (*bottom_right).set_visual_style(ButtonVisualStyle::command);
    (*bottom_right).set_requested_bounds({424.0, 126.0, 142.0, 28.0});
    (*bottom_right).set_anchor(AnchorStyles::right | AnchorStyles::bottom);
    (*anchor_canvas).add_child(fixed);
    (*anchor_canvas).add_child(stretch);
    (*anchor_canvas).add_child(centered);
    (*anchor_canvas).add_child(bottom_right);

    std::shared_ptr<gui_forms::Button> resize = make_control<Button>(StableId("gallery.anchor.resize"),
                                       "Expand specimen");
    (*resize).set_visual_style(ButtonVisualStyle::accent);
    (*anchor_group).add_at(resize, {758.0, 54.0, 150.0, 34.0});
    std::shared_ptr<Label> anchor_note = label(
        "gallery.anchor.note",
        "Runtime size and Anchor changes rebase from committed geometry. Authored bounds remain queryable and unchanged.",
        11.0, 600, green);
    (*anchor_note).set_text_wrapping(TextWrapping::word);
    (*anchor_note).set_vertical_alignment(VerticalAlignment::near);
    (*anchor_group).add_at(anchor_note, {758.0, 104.0, 150.0, 104.0});

    const std::weak_ptr<ScaledGroupBox> weak_anchor_group = anchor_group;
    const std::weak_ptr<Panel> weak_anchor_canvas = anchor_canvas;
    const std::weak_ptr<Button> weak_resize = resize;
    std::shared_ptr<bool> expanded = std::make_shared<bool>(false);
    (*context).subscriptions.push_back((*resize).clicked().subscribe(
        *page, ResizeAnchorSpecimenClick(
            weak_anchor_group, weak_anchor_canvas, weak_resize,
            weak_context, expanded)));
}

} // namespace gui_forms::gallery::pages
