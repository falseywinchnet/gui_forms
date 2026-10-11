#include "pages.hpp"

namespace gui_forms::gallery::pages {

// Owner of the Containers and Focus page: one SplitContainer whose seam is
// dragged, keyed and collapsed, a base ContainerControl with descendant focus
// authority, and a UserControl whose one-shot Loaded lifecycle is observed.
// Every handler is a named member connected with gui_forms::on; the owner
// lives in the Gallery's ComponentContainer for exactly the Gallery lifetime.
class ContainersPage final : public gui_forms::Component {
public:
    ContainersPage(const std::shared_ptr<Surface>& page,
                   const std::shared_ptr<PageContext>& context) {
        static_cast<void>(context);
        (*page).add_at(label("gallery.containers.heading", "CONTAINERS AND FOCUS",
                             22.0, 700, shell_blue_dark),
                       {24.0, 18.0, 500.0, 34.0});
        (*page).add_at(label("gallery.containers.lead",
                             "Real retained composition: split allocation, wide hit target, collapse, focus transfer, nested scope.",
                             12.0, 400, muted),
                       {26.0, 52.0, 900.0, 24.0});
        build_split(page);
        build_base_container(page);
        build_user_control(page);
        on((*collapse_).clicked(), *this, &ContainersPage::on_collapse);
        on((*focus_splitter_).clicked(), *this, &ContainersPage::on_focus_splitter);
        on((*base_focus_).clicked(), *this, &ContainersPage::on_focus_descendant);
        on((*user_control_).loaded(), *this, &ContainersPage::on_user_loaded);
    }

private:
    void build_split(const std::shared_ptr<Surface>& page) {
        split_ = make_control<SplitContainer>(StableId("gallery.containers.split"));
        (*split_).set_splitter_distance(310.0);
        (*split_).set_first_minimum(180.0);
        (*split_).set_second_minimum(280.0);
        (*page).add_at(split_, {24.0, 100.0, 930.0, 430.0});
        const std::shared_ptr<SplitterPanel> first = (*split_).first_panel();
        const std::shared_ptr<SplitterPanel> second = (*split_).second_panel();
        (*first).set_background(Color::rgba(225, 235, 243));
        (*second).set_background(paper);

        std::shared_ptr<Label> first_title = label(
            "gallery.containers.first.title", "NAVIGATION PANE", 13.0, 700,
            shell_blue_dark);
        (*first_title).set_requested_bounds({20.0, 22.0, 240.0, 28.0});
        (*first).add_child(first_title);
        for (std::size_t index = 0; index < 6U; ++index) {
            std::shared_ptr<Button> item = make_control<Button>(
                StableId("gallery.containers.item." + std::to_string(index)),
                "Retained item " + std::to_string(index + 1U));
            (*item).set_visual_style(ButtonVisualStyle::command);
            (*item).set_requested_bounds(
                {20.0, 62.0 + static_cast<double>(index) * 48.0, 255.0, 36.0});
            (*first).add_child(item);
        }

        std::shared_ptr<Label> second_title = label(
            "gallery.containers.second.title", "CONTENT PANE", 13.0, 700,
            shell_blue_dark);
        (*second_title).set_requested_bounds({24.0, 22.0, 260.0, 28.0});
        (*second).add_child(second_title);
        std::shared_ptr<Label> explanation = label(
            "gallery.containers.second.copy",
            "Drag the seam continuously. Focus it and use arrow keys.\nCollapse and restore preserve allocation.",
            12.0, 400, ink);
        (*explanation).set_text_wrapping(TextWrapping::word);
        (*explanation).set_vertical_alignment(VerticalAlignment::near);
        (*explanation).set_requested_bounds({24.0, 62.0, 520.0, 52.0});
        (*second).add_child(explanation);

        collapse_ = make_control<Button>(StableId("gallery.containers.collapse"),
                                         "Collapse navigation");
        (*collapse_).set_requested_bounds({24.0, 132.0, 190.0, 34.0});
        (*second).add_child(collapse_);
        focus_splitter_ = make_control<Button>(StableId("gallery.containers.focus"),
                                               "Focus the splitter");
        (*focus_splitter_).set_visual_style(ButtonVisualStyle::accent);
        (*focus_splitter_).set_requested_bounds({228.0, 132.0, 170.0, 34.0});
        (*second).add_child(focus_splitter_);
    }

    void build_base_container(const std::shared_ptr<Surface>& page) {
        base_container_ = make_control<ContainerControl>(
            StableId("gallery.containers.base"));
        (*base_container_).set_accessible_name("Base ContainerControl specimen");
        (*page).add_at(base_container_, {24.0, 548.0, 450.0, 82.0});
        std::shared_ptr<Panel> face = make_control<Panel>(
            StableId("gallery.containers.base.face"));
        (*face).set_background(Color::rgba(232, 239, 245));
        (*face).set_border_style(BorderStyle::line);
        (*face).set_dock(DockStyle::fill);
        (*base_container_).add_child(face);
        std::shared_ptr<Label> copy = label(
            "gallery.containers.base.copy",
            "ContainerControl · active descendant + retained child composition",
            11.0, 600, shell_blue_dark);
        (*copy).set_requested_bounds({14.0, 12.0, 390.0, 24.0});
        (*face).add_child(copy);
        base_focus_ = make_control<Button>(
            StableId("gallery.containers.base.focus"), "Focus descendant");
        (*base_focus_).set_requested_bounds({14.0, 42.0, 150.0, 28.0});
        (*face).add_child(base_focus_);
    }

    void build_user_control(const std::shared_ptr<Surface>& page) {
        user_control_ = make_control<UserControl>(
            StableId("gallery.containers.user"));
        (*user_control_).set_accessible_name("UserControl lifecycle specimen");
        (*page).add_at(user_control_, {488.0, 548.0, 466.0, 82.0});
        std::shared_ptr<Panel> face = make_control<Panel>(
            StableId("gallery.containers.user.face"));
        (*face).set_background(Color::rgba(238, 244, 236));
        (*face).set_border_style(BorderStyle::line);
        (*face).set_dock(DockStyle::fill);
        (*user_control_).add_child(face);
        user_status_ = label("gallery.containers.user.status",
                             "UserControl · awaiting one-shot Loaded lifecycle",
                             11.0, 600, green);
        (*user_status_).set_requested_bounds({14.0, 12.0, 420.0, 24.0});
        (*face).add_child(user_status_);
        std::shared_ptr<Button> action = make_control<Button>(
            StableId("gallery.containers.user.action"), "Reusable child action");
        (*action).set_visual_style(ButtonVisualStyle::command);
        (*action).set_requested_bounds({14.0, 42.0, 178.0, 28.0});
        (*face).add_child(action);
    }

    // Input-only: the user clicked. The collapse state itself lives in the
    // public SplitContainer, so the button text follows the container.
    void on_collapse() {
        const bool next = !(*split_).first_collapsed();
        (*split_).set_first_collapsed(next);
        (*collapse_).set_text(next ? "Restore navigation" : "Collapse navigation");
    }

    void on_focus_splitter() {
        static_cast<void>(
            (*split_).request_active_control((*split_).splitter_control()));
    }

    void on_focus_descendant() {
        static_cast<void>((*base_container_).request_active_control(base_focus_));
    }

    void on_user_loaded() {
        (*user_status_).set_text(
            "UserControl · Loaded fired once · attachment committed");
    }

    std::shared_ptr<SplitContainer> split_;
    std::shared_ptr<Button> collapse_;
    std::shared_ptr<Button> focus_splitter_;
    std::shared_ptr<ContainerControl> base_container_;
    std::shared_ptr<Button> base_focus_;
    std::shared_ptr<UserControl> user_control_;
    std::shared_ptr<Label> user_status_;
};

void add_containers(const std::shared_ptr<Surface>& page,
                    const std::shared_ptr<PageContext>& context) {
    (*context).components.add(std::make_shared<ContainersPage>(page, context));
}

} // namespace gui_forms::gallery::pages
