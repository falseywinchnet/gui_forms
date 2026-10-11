#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_containers(const std::shared_ptr<Surface>& page,
                    const std::shared_ptr<PageContext>& context) {
    class CollapseNavigationClick final {
    public:
        CollapseNavigationClick(std::weak_ptr<SplitContainer> split,
                                std::weak_ptr<Button> collapse) noexcept
            : split_(std::move(split)), collapse_(std::move(collapse)) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<SplitContainer> split = split_.lock();
            const std::shared_ptr<Button> collapse = collapse_.lock();
            if (!split || !collapse) return;
            const bool next = !(*split).first_collapsed();
            (*split).set_first_collapsed(next);
            (*collapse).set_text(next ? "Restore navigation" : "Collapse navigation");
        }

    private:
        std::weak_ptr<SplitContainer> split_;
        std::weak_ptr<Button> collapse_;
    };

    class FocusSplitterClick final {
    public:
        explicit FocusSplitterClick(std::weak_ptr<SplitContainer> split) noexcept
            : split_(std::move(split)) {}

        void operator()(ButtonBase&) const
        {
            if (const std::shared_ptr<SplitContainer> split = split_.lock()) {
                static_cast<void>(
                    (*split).request_active_control((*split).splitter_control()));
            }
        }

    private:
        std::weak_ptr<SplitContainer> split_;
    };

    class FocusContainerDescendantClick final {
    public:
        FocusContainerDescendantClick(
            std::weak_ptr<ContainerControl> container,
            std::weak_ptr<Button> focus) noexcept
            : container_(std::move(container)), focus_(std::move(focus)) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<ContainerControl> container = container_.lock();
            const std::shared_ptr<Button> focus = focus_.lock();
            if (container && focus) {
                static_cast<void>((*container).request_active_control(focus));
            }
        }

    private:
        std::weak_ptr<ContainerControl> container_;
        std::weak_ptr<Button> focus_;
    };

    class UserControlLoaded final {
    public:
        explicit UserControlLoaded(std::weak_ptr<Label> status) noexcept
            : status_(std::move(status)) {}

        void operator()() const
        {
            if (const std::shared_ptr<Label> status = status_.lock()) {
                (*status).set_text(
                    "UserControl · Loaded fired once · attachment committed");
            }
        }

    private:
        std::weak_ptr<Label> status_;
    };

    (*page).add_at(label("gallery.containers.heading", "CONTAINERS AND FOCUS", 22.0, 700,
                       shell_blue_dark), {24.0, 18.0, 500.0, 34.0});
    (*page).add_at(label("gallery.containers.lead",
                       "Real retained composition: split allocation, wide hit target, collapse, focus transfer, nested scope.",
                       12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});

    std::shared_ptr<gui_forms::SplitContainer> split = make_control<SplitContainer>(StableId("gallery.containers.split"));
    (*split).set_splitter_distance(310.0);
    (*split).set_first_minimum(180.0);
    (*split).set_second_minimum(280.0);
    (*page).add_at(split, {24.0, 100.0, 930.0, 430.0});
    (*(*split).first_panel()).set_background(Color::rgba(225, 235, 243));
    (*(*split).second_panel()).set_background(paper);

    std::shared_ptr<Label> first_title = label("gallery.containers.first.title", "NAVIGATION PANE", 13.0, 700,
                             shell_blue_dark);
    (*first_title).set_requested_bounds({20.0, 22.0, 240.0, 28.0});
    (*(*split).first_panel()).add_child(first_title);
    for (std::size_t index = 0; index < 6U; ++index) {
        std::shared_ptr<gui_forms::Button> item = make_control<Button>(
            StableId("gallery.containers.item." + std::to_string(index)),
            "Retained item " + std::to_string(index + 1U));
        (*item).set_visual_style(ButtonVisualStyle::command);
        (*item).set_requested_bounds({20.0, 62.0 + static_cast<double>(index) * 48.0,
                                    255.0, 36.0});
        (*(*split).first_panel()).add_child(item);
    }
    std::shared_ptr<Label> second_title = label("gallery.containers.second.title", "CONTENT PANE", 13.0, 700,
                              shell_blue_dark);
    (*second_title).set_requested_bounds({24.0, 22.0, 260.0, 28.0});
    (*(*split).second_panel()).add_child(second_title);
    std::shared_ptr<Label> explanation = label("gallery.containers.second.copy",
                             "Drag the seam continuously. Focus it and use arrow keys.\nCollapse and restore preserve allocation.",
                             12.0, 400, ink);
    (*explanation).set_text_wrapping(TextWrapping::word);
    (*explanation).set_vertical_alignment(VerticalAlignment::near);
    (*explanation).set_requested_bounds({24.0, 62.0, 520.0, 52.0});
    (*(*split).second_panel()).add_child(explanation);
    std::shared_ptr<gui_forms::Button> collapse = make_control<Button>(StableId("gallery.containers.collapse"),
                                         "Collapse navigation");
    (*collapse).set_requested_bounds({24.0, 132.0, 190.0, 34.0});
    (*(*split).second_panel()).add_child(collapse);
    const std::weak_ptr<SplitContainer> weak_split = split;
    const std::weak_ptr<Button> weak_collapse = collapse;
    (*context).subscriptions.push_back((*collapse).clicked().subscribe(
        *split, CollapseNavigationClick(weak_split, weak_collapse)));
    std::shared_ptr<gui_forms::Button> focus_button = make_control<Button>(StableId("gallery.containers.focus"),
                                             "Focus the splitter");
    (*focus_button).set_visual_style(ButtonVisualStyle::accent);
    (*focus_button).set_requested_bounds({228.0, 132.0, 170.0, 34.0});
    (*(*split).second_panel()).add_child(focus_button);
    (*context).subscriptions.push_back((*focus_button).clicked().subscribe(
        *split, FocusSplitterClick(weak_split)));

    std::shared_ptr<gui_forms::ContainerControl> base_container = make_control<ContainerControl>(
        StableId("gallery.containers.base"));
    (*base_container).set_accessible_name("Base ContainerControl specimen");
    (*page).add_at(base_container, {24.0, 548.0, 450.0, 82.0});
    std::shared_ptr<gui_forms::Panel> base_face = make_control<Panel>(
        StableId("gallery.containers.base.face"));
    (*base_face).set_background(Color::rgba(232, 239, 245));
    (*base_face).set_border_style(BorderStyle::line);
    (*base_face).set_dock(DockStyle::fill);
    (*base_container).add_child(base_face);
    std::shared_ptr<Label> base_copy = label(
        "gallery.containers.base.copy",
        "ContainerControl · active descendant + retained child composition",
        11.0, 600, shell_blue_dark);
    (*base_copy).set_requested_bounds({14.0, 12.0, 390.0, 24.0});
    (*base_face).add_child(base_copy);
    std::shared_ptr<gui_forms::Button> base_focus = make_control<Button>(
        StableId("gallery.containers.base.focus"), "Focus descendant");
    (*base_focus).set_requested_bounds({14.0, 42.0, 150.0, 28.0});
    (*base_face).add_child(base_focus);
    const std::weak_ptr<ContainerControl> weak_base = base_container;
    const std::weak_ptr<Button> weak_base_focus = base_focus;
    (*context).subscriptions.push_back((*base_focus).clicked().subscribe(
        *base_container,
        FocusContainerDescendantClick(weak_base, weak_base_focus)));

    std::shared_ptr<gui_forms::UserControl> user_control = make_control<UserControl>(
        StableId("gallery.containers.user"));
    (*user_control).set_accessible_name("UserControl lifecycle specimen");
    (*page).add_at(user_control, {488.0, 548.0, 466.0, 82.0});
    std::shared_ptr<gui_forms::Panel> user_face = make_control<Panel>(
        StableId("gallery.containers.user.face"));
    (*user_face).set_background(Color::rgba(238, 244, 236));
    (*user_face).set_border_style(BorderStyle::line);
    (*user_face).set_dock(DockStyle::fill);
    (*user_control).add_child(user_face);
    std::shared_ptr<Label> user_status = label(
        "gallery.containers.user.status",
        "UserControl · awaiting one-shot Loaded lifecycle",
        11.0, 600, green);
    (*user_status).set_requested_bounds({14.0, 12.0, 420.0, 24.0});
    (*user_face).add_child(user_status);
    std::shared_ptr<gui_forms::Button> user_action = make_control<Button>(
        StableId("gallery.containers.user.action"), "Reusable child action");
    (*user_action).set_visual_style(ButtonVisualStyle::command);
    (*user_action).set_requested_bounds({14.0, 42.0, 178.0, 28.0});
    (*user_face).add_child(user_action);
    const std::weak_ptr<Label> weak_user_status = user_status;
    (*context).subscriptions.push_back((*user_control).loaded().subscribe(
        *user_control, UserControlLoaded(weak_user_status)));
}

} // namespace gui_forms::gallery::pages
