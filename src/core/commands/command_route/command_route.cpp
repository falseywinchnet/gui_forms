#include "gui_forms/commands/command_route/command_route.hpp"

#include <stdexcept>
#include <utility>

namespace gui_forms {

CommandTarget::CommandTarget(std::string name) : name_(std::move(name)) {}

void CommandTarget::set_handler(std::string command_id, Delegate<const CommandInvocation&> handler) {
    if (command_id.empty() || !handler) {
        throw std::invalid_argument("a command handler needs a command identity and a bound delegate");
    }
    for (Handler& existing : handlers_) {
        if (existing.command_id != command_id) continue;
        existing.run = handler;
        return;
    }
    Handler added{std::move(command_id), handler};
    handlers_.push_back(std::move(added));
}

bool CommandTarget::remove_handler(std::string_view command_id) {
    for (std::size_t index = 0; index < handlers_.size(); ++index) {
        if (handlers_[index].command_id != command_id) continue;
        const std::vector<Handler>::difference_type offset =
            static_cast<std::vector<Handler>::difference_type>(index);
        handlers_.erase(handlers_.begin() + offset);
        return true;
    }
    return false;
}

bool CommandTarget::handles(std::string_view command_id) const noexcept {
    for (const Handler& handler : handlers_) {
        if (handler.command_id == command_id) return true;
    }
    return false;
}

bool CommandTarget::invoke(const CommandInvocation& invocation) const {
    Delegate<const CommandInvocation&> run{};
    for (const Handler& handler : handlers_) {
        if (handler.command_id != invocation.command_id) continue;
        run = handler.run;
        break;
    }
    if (!run) return false;
    run(invocation);
    return true;
}

} // namespace gui_forms
