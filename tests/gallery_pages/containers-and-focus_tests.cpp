#include "support.hpp"

namespace gallery_pages_tests {

void test_containers_and_focus(gui_forms::Window& window) {
    select_page(window, "containers-and-focus");
    std::shared_ptr<gui_forms::SplitContainer> split = std::dynamic_pointer_cast<SplitContainer>(
        window.find("gallery.containers.split"));
    std::shared_ptr<gui_forms::ContainerControl> base_container = std::dynamic_pointer_cast<ContainerControl>(
        window.find("gallery.containers.base"));
    std::shared_ptr<gui_forms::UserControl> user_control = std::dynamic_pointer_cast<UserControl>(
        window.find("gallery.containers.user"));
    std::shared_ptr<gui_forms::Label> user_status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.containers.user.status"));
    require(base_container && user_control && user_status &&
                (*user_control).is_loaded() && (*user_control).is_attached() &&
                (*user_control).attachment_count() == 1U &&
                (*user_status).text().find("Loaded fired once") != std::string::npos,
            "gallery must directly dogfood public ContainerControl and UserControl lifecycle");
    click(window, window.find("gallery.containers.base.focus"));
    require((*base_container).active_control() ==
                window.find("gallery.containers.base.focus") &&
                window.semantic_snapshot().to_json().find(
                    "UserControl lifecycle specimen") != std::string::npos,
            "public container focus and named group semantics must work in the gallery");
    click(window, window.find("gallery.containers.collapse"));
    require(split && (*split).first_collapsed(),
            "gallery collapse command must mutate the public SplitContainer");
    click(window, window.find("gallery.containers.collapse"));
    require(!(*split).first_collapsed(),
            "gallery SplitContainer must restore its retained allocation");
}

} // namespace gallery_pages_tests
