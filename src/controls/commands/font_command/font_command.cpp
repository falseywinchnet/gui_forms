#include "gui_forms/commands/font_command/font_command.hpp"

#include "gui_forms/text.hpp"

#include <array>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace gui_forms {

bool FontCommand::valid_value(const FontChoice& value) noexcept {
    const bool family = validate_utf8(value.family).valid() && value.family.find('\n') == std::string::npos;
    const bool size = std::isfinite(value.size) && value.size >= 0.0 && value.size <= maximum_size;
    return family && size;
}

FontCommand::FontCommand(std::shared_ptr<Command> command) : command_(std::move(command)) {
    if (!command_) throw std::invalid_argument("FontCommand requires a Command");
}

void FontCommand::set_value(FontChoice value) {
    if (!valid_value(value)) throw std::invalid_argument("FontCommand value is not a valid font choice");
    if (value_ == value) return;
    value_ = std::move(value);
    value_changed_.emit(value_);
}

bool FontCommand::commit(FontChoice value, std::string_view source_id) {
    if (!is_alive() || !(*command_).state().enabled || !valid_value(value)) return false;
    if (value.family.empty() && value.size == 0.0) return false;
    if (!(value_ == value)) {
        value_ = std::move(value);
        value_changed_.emit(value_);
    }
    // The Command is shared and may outlive a handler's changes; hold it.
    const std::shared_ptr<Command> command = command_;
    const bool executed = (*command).execute(source_id);
    return executed;
}

void FontCommand::on_dispose() noexcept {
    value_changed_.disconnect_all();
}

namespace {

// The events FontCommand adds to those of Component, in declaration order.
constexpr std::array<InteractionDeclaration, 1> font_command_interactions{{
    {.name = "value_changed", .payload = "FontChoice", .mutable_payload = false},
}};

} // namespace

void FontCommand::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    Component::append_interactions(interactions);
    interactions.insert(interactions.end(), font_command_interactions.begin(), font_command_interactions.end());
}

} // namespace gui_forms
