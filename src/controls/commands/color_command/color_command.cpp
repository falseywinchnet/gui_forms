#include "gui_forms/commands/color_command/color_command.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <utility>

namespace gui_forms {

ColorCommand::ColorCommand(std::shared_ptr<Command> command) : command_(std::move(command)) {
    if (!command_) throw std::invalid_argument("ColorCommand requires a Command");
    recent_.reserve(maximum_recent_colors);
}

void ColorCommand::set_value(std::optional<Color> value) {
    if (value_ == value) return;
    value_ = value;
    value_changed_.emit(value_);
}

bool ColorCommand::commit(std::optional<Color> value, std::string_view source_id) {
    if (!is_alive() || !(*command_).state().enabled) return false;
    if (value_ != value) {
        value_ = value;
        value_changed_.emit(value_);
    }
    // The colour moves to the front of the recent colours; the oldest falls
    // off past maximum_recent_colors.
    if (value) {
        recent_.erase(std::remove(recent_.begin(), recent_.end(), *value), recent_.end());
        recent_.insert(recent_.begin(), *value);
        if (recent_.size() > maximum_recent_colors) recent_.resize(maximum_recent_colors);
    }
    // The Command is shared and may outlive a handler's changes; hold it.
    const std::shared_ptr<Command> command = command_;
    const bool executed = (*command).execute(source_id);
    return executed;
}

void ColorCommand::on_dispose() noexcept {
    value_changed_.disconnect_all();
}

namespace {

// The events ColorCommand adds to those of Component, in declaration order.
constexpr std::array<InteractionDeclaration, 1> color_command_interactions{{
    {.name = "value_changed", .payload = "std::optional<Color>", .mutable_payload = false},
}};

} // namespace

void ColorCommand::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    Component::append_interactions(interactions);
    interactions.insert(interactions.end(), color_command_interactions.begin(), color_command_interactions.end());
}

} // namespace gui_forms
