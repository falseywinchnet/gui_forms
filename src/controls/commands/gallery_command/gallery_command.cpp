#include "gui_forms/commands/gallery_command/gallery_command.hpp"
#include "gui_forms/window.hpp"

#include "gui_forms/text.hpp"

#include <array>
#include <stdexcept>
#include <utility>

namespace gui_forms {
namespace {

bool valid_item_id(std::string_view item_id) {
    const bool result = !item_id.empty() && validate_utf8(item_id).valid();
    return result;
}

} // namespace

GalleryCommand::GalleryCommand(std::shared_ptr<Command> command) : command_(std::move(command)) {
    if (!command_) throw std::invalid_argument("GalleryCommand requires a Command");
}

void GalleryCommand::set_value(std::optional<std::string> item_id) {
    if (item_id && !valid_item_id(*item_id)) {
        throw std::invalid_argument("GalleryCommand values are non-empty, valid UTF-8 item IDs");
    }
    if (value_ == item_id) return;
    value_ = std::move(item_id);
    value_changed_.emit(value_);
}

bool GalleryCommand::commit(std::string item_id, std::string_view source_id) {
    const bool executed = commit_into(std::move(item_id), nullptr, source_id);
    return executed;
}

bool GalleryCommand::commit_routed(std::string item_id, Window& window, std::string_view source_id) {
    const bool executed = commit_into(std::move(item_id), &window, source_id);
    return executed;
}

bool GalleryCommand::commit_into(std::string item_id, Window* window, std::string_view source_id) {
    if (!is_alive() || !(*command_).state().enabled || !valid_item_id(item_id)) return false;
    // The committed value replaces the preview: no cancellation is raised.
    previewing_.reset();
    if (!(value_ && *value_ == item_id)) {
        value_ = std::move(item_id);
        value_changed_.emit(value_);
    }
    // The Command is shared and may outlive a handler's changes; hold it.
    const std::shared_ptr<Command> command = command_;
    if (window != nullptr) {
        const bool routed = (*command).execute_routed(*window, source_id);
        return routed;
    }
    const bool executed = (*command).execute(source_id);
    return executed;
}

bool GalleryCommand::preview(std::string item_id, std::string_view source_id) {
    if (!is_alive() || !(*command_).state().enabled || !valid_item_id(item_id)) return false;
    if (previewing_ && *previewing_ == item_id) return false;
    previewing_ = item_id;
    const GalleryPreview requested{std::move(item_id), std::string(source_id)};
    preview_requested_.emit(requested);
    return true;
}

void GalleryCommand::cancel_preview(std::string_view source_id) {
    if (!previewing_) return;
    const GalleryPreview cancelled{std::move(*previewing_), std::string(source_id)};
    previewing_.reset();
    preview_cancelled_.emit(cancelled);
}

void GalleryCommand::on_dispose() noexcept {
    previewing_.reset();
    value_changed_.disconnect_all();
    preview_requested_.disconnect_all();
    preview_cancelled_.disconnect_all();
}

namespace {

// The events GalleryCommand adds to those of Component, in declaration order.
constexpr std::array<InteractionDeclaration, 3> gallery_command_interactions{{
    {.name = "value_changed", .payload = "std::optional<std::string>", .mutable_payload = false},
    {.name = "preview_requested", .payload = "GalleryPreview", .mutable_payload = false},
    {.name = "preview_cancelled", .payload = "GalleryPreview", .mutable_payload = false},
}};

} // namespace

void GalleryCommand::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    Component::append_interactions(interactions);
    interactions.insert(interactions.end(), gallery_command_interactions.begin(),
                        gallery_command_interactions.end());
}

} // namespace gui_forms
