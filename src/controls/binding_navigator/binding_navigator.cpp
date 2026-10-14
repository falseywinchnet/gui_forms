#include "gui_forms/controls/binding_navigator/binding_navigator.hpp"

#include "gui_forms/delegate.hpp"
#include "gui_forms/window.hpp"

#include <array>
#include <charconv>
#include <cstddef>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace gui_forms {
namespace {

constexpr std::string_view position_item = "position";
constexpr std::string_view count_item = "count";
constexpr double position_width = 50.0;

std::shared_ptr<Command> make_action(std::string_view navigator_id, std::string_view action,
                                     std::string text) {
    std::string identity(navigator_id);
    identity.append(".");
    identity.append(action);
    std::shared_ptr<Command> command = std::make_shared<Command>(std::move(identity), std::move(text));
    return command;
}

bool has_item(const std::vector<ToolStripItemSpec>& items, std::string_view item_id) noexcept {
    for (const ToolStripItemSpec& item : items) {
        if (item.stable_id == item_id) return true;
    }
    return false;
}

bool has_text_box(const std::vector<ToolStripItemSpec>& items, std::string_view item_id) noexcept {
    for (const ToolStripItemSpec& item : items) {
        if (item.stable_id == item_id) return item.kind == ToolStripItemKind::text_box;
    }
    return false;
}

// A one-based position typed into the position box, or none when the text
// is not a whole number in [1, count].
std::optional<std::size_t> typed_position(std::string_view text, std::size_t count) noexcept {
    std::size_t value{};
    const char* const first = text.data();
    const char* const last = text.data() + text.size();
    const std::from_chars_result parsed = std::from_chars(first, last, value);
    if (text.empty() || parsed.ec != std::errc{} || parsed.ptr != last || value == 0U || value > count) {
        return std::nullopt;
    }
    return value;
}

} // namespace

BindingNavigator::BindingNavigator(StableId navigator_id) : ToolStrip(std::move(navigator_id)) {
    const std::string identity(stable_id().value());
    move_first_ = make_action(identity, "move_first", "First");
    move_previous_ = make_action(identity, "move_previous", "Previous");
    move_next_ = make_action(identity, "move_next", "Next");
    move_last_ = make_action(identity, "move_last", "Last");
    add_new_ = make_action(identity, "add_new", "Add");
    delete_ = make_action(identity, "delete", "Delete");
}

void BindingNavigator::initialize_control_tree() {
    std::vector<ToolStripItemSpec> items;
    items.push_back({.stable_id = "move_first", .command = move_first_});
    items.push_back({.stable_id = "move_previous", .command = move_previous_});
    items.push_back({.stable_id = "separator_position", .kind = ToolStripItemKind::separator});
    items.push_back({.stable_id = std::string(position_item), .kind = ToolStripItemKind::text_box,
                     .text = "Position", .width = position_width});
    items.push_back({.stable_id = std::string(count_item), .kind = ToolStripItemKind::label, .text = "of 0"});
    items.push_back({.stable_id = "separator_move", .kind = ToolStripItemKind::separator});
    items.push_back({.stable_id = "move_next", .command = move_next_});
    items.push_back({.stable_id = "move_last", .command = move_last_});
    items.push_back({.stable_id = "separator_edit", .kind = ToolStripItemKind::separator});
    items.push_back({.stable_id = "add_new", .command = add_new_});
    items.push_back({.stable_id = "delete", .command = delete_});
    set_items(std::move(items));
    action_tokens_.push_back((*move_first_).invoked().subscribe(
        *this, Delegate<const CommandInvocation&>::bind<BindingNavigator, &BindingNavigator::on_move_first>(*this)));
    action_tokens_.push_back((*move_previous_).invoked().subscribe(
        *this,
        Delegate<const CommandInvocation&>::bind<BindingNavigator, &BindingNavigator::on_move_previous>(*this)));
    action_tokens_.push_back((*move_next_).invoked().subscribe(
        *this, Delegate<const CommandInvocation&>::bind<BindingNavigator, &BindingNavigator::on_move_next>(*this)));
    action_tokens_.push_back((*move_last_).invoked().subscribe(
        *this, Delegate<const CommandInvocation&>::bind<BindingNavigator, &BindingNavigator::on_move_last>(*this)));
    action_tokens_.push_back((*add_new_).invoked().subscribe(
        *this, Delegate<const CommandInvocation&>::bind<BindingNavigator, &BindingNavigator::on_add_new>(*this)));
    action_tokens_.push_back((*delete_).invoked().subscribe(
        *this, Delegate<const CommandInvocation&>::bind<BindingNavigator, &BindingNavigator::on_delete>(*this)));
    refresh();
}

void BindingNavigator::set_binding_source(std::shared_ptr<BindingSource> source) {
    require_mutable();
    if (source_ == source) return;
    list_changed_.disconnect();
    position_changed_.disconnect();
    source_disposed_.disconnect();
    source_ = std::move(source);
    if (source_) {
        BindingSource& bound = *source_;
        list_changed_ = bound.list_changed().subscribe(
            *this, Delegate<const BindingListChange&>::bind<BindingNavigator, &BindingNavigator::on_list_changed>(*this));
        position_changed_ = bound.position_changed().subscribe(
            *this, Delegate<std::ptrdiff_t>::bind<BindingNavigator, &BindingNavigator::on_position_changed>(*this));
        source_disposed_ = bound.disposed_event().subscribe(
            *this, Delegate<>::bind<BindingNavigator, &BindingNavigator::on_source_disposed>(*this));
    }
    refresh();
}

// Shows the source's position and count and enables each action that can
// act: moves away from the ends, add while the source allows additions and
// delete while it has a current record and allows removals. Without a
// source every action is disabled.
void BindingNavigator::refresh() {
    std::size_t count = 0U;
    std::ptrdiff_t position = -1;
    bool allow_new = false;
    bool allow_remove = false;
    if (source_ && (*source_).is_alive()) {
        count = (*source_).count();
        position = (*source_).position();
        allow_new = (*source_).allow_new();
        allow_remove = (*source_).allow_remove();
    }
    const bool current = count != 0U && position >= 0;
    const std::ptrdiff_t last = static_cast<std::ptrdiff_t>(count) - 1;
    (*move_first_).set_enabled(current && position > 0);
    (*move_previous_).set_enabled(current && position > 0);
    (*move_next_).set_enabled(current && position < last);
    (*move_last_).set_enabled(current && position < last);
    (*add_new_).set_enabled(source_ != nullptr && allow_new);
    (*delete_).set_enabled(current && allow_remove);
    const std::vector<ToolStripItemSpec>& shown = items();
    if (has_item(shown, count_item)) set_item_text(count_item, "of " + std::to_string(count));
    if (has_text_box(shown, position_item)) {
        TextBox& box = *text_box(position_item);
        box.set_text(current ? std::to_string(position + 1) : std::string("0"));
        box.set_enabled(count != 0U);
    }
}

void BindingNavigator::commit_position() {
    if (!has_text_box(items(), position_item)) return;
    const std::shared_ptr<TextBox> box = text_box(position_item);
    if (source_ && (*source_).is_alive()) {
        const std::optional<std::size_t> typed = typed_position((*box).text(), (*source_).count());
        if (typed) static_cast<void>((*source_).set_position(static_cast<std::ptrdiff_t>(*typed - 1U)));
    }
    // A rejected entry, or one equal to the current position, shows the
    // position again.
    refresh();
}

void BindingNavigator::on_key_preview(KeyEvent& event) {
    ToolStrip::on_key_preview(event);
    if (event.handled || event.action != KeyAction::down) return;
    if (event.physical_key != PhysicalKey::enter && event.physical_key != PhysicalKey::escape) return;
    if (!has_text_box(items(), position_item) || window() == nullptr) return;
    const std::shared_ptr<TextBox> box = text_box(position_item);
    if ((*window()).focused_control() != box) return;
    if (event.physical_key == PhysicalKey::enter) {
        commit_position();
    } else {
        refresh();
    }
    event.handled = true;
}

void BindingNavigator::on_move_first(const CommandInvocation&) {
    if (source_ && (*source_).is_alive()) static_cast<void>((*source_).move_first());
}

void BindingNavigator::on_move_previous(const CommandInvocation&) {
    if (source_ && (*source_).is_alive()) static_cast<void>((*source_).move_previous());
}

void BindingNavigator::on_move_next(const CommandInvocation&) {
    if (source_ && (*source_).is_alive()) static_cast<void>((*source_).move_next());
}

void BindingNavigator::on_move_last(const CommandInvocation&) {
    if (source_ && (*source_).is_alive()) static_cast<void>((*source_).move_last());
}

void BindingNavigator::on_add_new(const CommandInvocation&) {
    if (!source_ || !(*source_).is_alive() || !(*source_).allow_new()) return;
    // The source is held across the handler, which may replace it.
    const std::shared_ptr<BindingSource> source = source_;
    BindingNavigatorAddNew request{std::string(stable_id().value()), std::nullopt};
    adding_new_.emit(request);
    if (!request.record || !is_alive() || !(*source).is_alive() || !(*source).allow_new()) return;
    const std::size_t index = (*source).add(std::move(*request.record));
    static_cast<void>((*source).set_position(static_cast<std::ptrdiff_t>(index)));
}

void BindingNavigator::on_delete(const CommandInvocation&) {
    if (!source_ || !(*source_).is_alive() || !(*source_).allow_remove()) return;
    static_cast<void>((*source_).remove_current());
}

void BindingNavigator::on_list_changed(const BindingListChange&) {
    if (is_alive()) refresh();
}

void BindingNavigator::on_position_changed(std::ptrdiff_t) {
    if (is_alive()) refresh();
}

void BindingNavigator::on_source_disposed() {
    if (!is_alive()) return;
    list_changed_.disconnect();
    position_changed_.disconnect();
    source_disposed_.disconnect();
    source_.reset();
    refresh();
}

SemanticDescriptor BindingNavigator::semantic_descriptor() const {
    SemanticDescriptor descriptor = ToolStrip::semantic_descriptor();
    descriptor.name = accessible_name().empty() ? "Binding navigator" : accessible_name();
    return descriptor;
}

const ControlAnatomy& BindingNavigator::anatomy() const noexcept {
    // A navigator is a ToolStrip: the same parts, states and item system.
    static const ControlAnatomy binding_navigator_anatomy{
        .control_id = "binding_navigator",
        .parts = ToolStrip::anatomy().parts,
        .states = ToolStrip::anatomy().states,
        .custom_states = {},
        .item_systems = ToolStrip::anatomy().item_systems,
    };
    return binding_navigator_anatomy;
}

namespace {

// The event BindingNavigator adds to those of ToolStrip.
constexpr std::array<InteractionDeclaration, 1> binding_navigator_interactions{{
    {.name = "adding_new", .payload = "BindingNavigatorAddNew", .mutable_payload = true},
}};

} // namespace

void BindingNavigator::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    ToolStrip::append_interactions(interactions);
    interactions.insert(interactions.end(), binding_navigator_interactions.begin(),
                        binding_navigator_interactions.end());
}

} // namespace gui_forms
