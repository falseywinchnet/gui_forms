#include "gui_forms/controls/scrollable_control/container_control/user_control/user_control.hpp"
#include "../../scrollable_anatomy.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>

namespace gui_forms {

UserControl::UserControl(StableId stable_id)
    : ContainerControl(std::move(stable_id)) {}

void UserControl::on_attached_to_window() {
    attached_ = true;
    if (!loaded_) {
        loaded_ = true;
        publish_change(loaded_event_);
    }
}

void UserControl::on_attachment_committed() noexcept {
    ++attachment_count_;
}

void UserControl::on_detached_from_window() noexcept {
    attached_ = false;
}


namespace {

constexpr ControlAnatomy user_control_anatomy{
    .control_id = "user-control",
    .parts = scrollable_detail::parts,
    .states = scrollable_detail::states,
    .custom_states = {},
};

} // namespace

const ControlAnatomy& UserControl::anatomy() const noexcept {
    return user_control_anatomy;
}

} // namespace gui_forms
