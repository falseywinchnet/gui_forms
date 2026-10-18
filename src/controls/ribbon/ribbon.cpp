#include "gui_forms/controls/ribbon/ribbon.hpp"

#include "../menu/menu_utilities.hpp"

#include "gui_forms/delegate.hpp"
#include "gui_forms/text.hpp"
#include "gui_forms/theme/part_painting/part_painting.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace gui_forms {
namespace {

// Ribbon geometry in logical units at text scale 1. The command plane holds
// three 22-unit rows of small commands (or one 66-unit large command) over
// a 17-unit caption row.
constexpr double tab_row_extent = 24.0;
constexpr double tab_padding = 12.0;
constexpr double tab_gap = 2.0;
constexpr double application_minimum_width = 56.0;
constexpr double plane_top_padding = 3.0;
constexpr double row_extent = 22.0;
constexpr std::size_t rows_per_column = 3U;
constexpr double caption_extent = 17.0;
constexpr double plane_bottom_padding = 2.0;
constexpr double group_padding = 4.0;
constexpr double column_gap = 2.0;
constexpr double divider_width = 1.0;
constexpr double launcher_size = 13.0;
constexpr double launcher_glyph_inset = 3.0;
constexpr double large_icon = 32.0;
constexpr double small_icon = 16.0;
constexpr double large_minimum_width = 42.0;
constexpr double label_extent = 16.0;
constexpr double icon_gap = 3.0;
constexpr double small_label_padding = 6.0;
constexpr double arrow_zone = 12.0;
constexpr double arrow_glyph = 7.0;
constexpr double default_combo_box_width = 110.0;
constexpr double default_numeric_width = 64.0;
constexpr double default_text_box_width = 100.0;
constexpr double check_box_mark_width = 24.0;
constexpr double maximum_hosted_width = 1024.0;

bool hosted(RibbonCommandKind kind) noexcept {
    const bool result = kind == RibbonCommandKind::combo_box || kind == RibbonCommandKind::numeric_up_down ||
                        kind == RibbonCommandKind::check_box || kind == RibbonCommandKind::text_box;
    return result;
}

bool has_menu(RibbonCommandKind kind) noexcept {
    const bool result = kind == RibbonCommandKind::split || kind == RibbonCommandKind::drop_down;
    return result;
}

bool needs_command(RibbonCommandKind kind) noexcept {
    const bool result = kind == RibbonCommandKind::button || kind == RibbonCommandKind::toggle ||
                        kind == RibbonCommandKind::split;
    return result;
}

// A square glyph of `side` centred in `area`, on whole units.
Rect centred_square(Rect area, double side) noexcept {
    if (area.width < side || area.height < side) return {};
    const Rect square{std::floor(area.x + (area.width - side) * 0.5),
                      std::floor(area.y + (area.height - side) * 0.5), side, side};
    return square;
}

std::string node_identity(std::string_view ribbon_id, std::string_view kind, std::string_view id) {
    std::string identity(ribbon_id);
    identity.append(".");
    identity.append(kind);
    identity.append(".");
    identity.append(id);
    return identity;
}

// The part of `identity` after "<ribbon>.<kind>.", or empty.
std::string_view node_suffix(std::string_view identity, std::string_view ribbon_id, std::string_view kind) {
    const std::size_t prefix = ribbon_id.size() + kind.size() + 2U;
    if (identity.size() <= prefix || !identity.starts_with(ribbon_id) || identity[ribbon_id.size()] != '.') {
        return {};
    }
    const std::string_view rest = identity.substr(ribbon_id.size() + 1U);
    if (!rest.starts_with(kind) || rest[kind.size()] != '.') return {};
    return identity.substr(prefix);
}

Rect to_window(Rect absolute, Rect local) noexcept {
    const Rect result{absolute.x + local.x, absolute.y + local.y, local.width, local.height};
    return result;
}

} // namespace

// -- construction --------------------------------------------------------------

Ribbon::Ribbon(StableId ribbon_id) : Control(std::move(ribbon_id)) {
    set_focusable(true);
    set_paint_plane(PaintPlane::control);
    popup_ = std::make_unique<ContextMenu>(std::string(stable_id().value()) + ".menu");
    popup_invoked_ = (*popup_).item_invoked().subscribe(
        *this, Delegate<const MenuItemInvocation&>::bind<Ribbon, &Ribbon::on_popup_item_invoked>(*this));
    popup_changed_ = (*popup_).open_changed().subscribe(
        *this, Delegate<bool>::bind<Ribbon, &Ribbon::on_popup_open_changed>(*this));
}

Ribbon::~Ribbon() {
    close_menus();
}

void Ribbon::on_dispose() noexcept {
    close_menus();
    if (popup_) (*popup_).dispose();
}

void Ribbon::on_attachment_committed() noexcept {
    Control::on_attachment_committed();
    Window* const host = attached_window();
    if (!focus_region_ || host == nullptr) return;
    // A ribbon nested in another region, or already registered, stays out.
    try {
        static_cast<void>((*host).add_focus_region(shared_from_this()));
    } catch (...) {
    }
}

void Ribbon::set_focus_region(bool region) {
    require_mutable();
    if (focus_region_ == region) return;
    focus_region_ = region;
    Window* const host = attached_window();
    if (host == nullptr) return;
    const Control::Ptr self = shared_from_this();
    if (region) {
        static_cast<void>((*host).add_focus_region(self));
        return;
    }
    std::vector<Control::Ptr> remaining = (*host).focus_regions();
    remaining.erase(std::remove(remaining.begin(), remaining.end(), self), remaining.end());
    (*host).set_focus_regions(std::move(remaining));
}

// -- model ---------------------------------------------------------------------

void Ribbon::set_tabs(std::vector<RibbonTabSpec> tabs) {
    require_mutable();
    if (tabs.size() > maximum_tabs) throw std::invalid_argument("Ribbon has more tabs than maximum_tabs");
    std::unordered_set<std::string> identities{};
    std::unordered_set<std::string> menu_ids{};
    for (const RibbonTabSpec& tab : tabs) {
        if (tab.stable_id.empty() || !validate_utf8(tab.stable_id).valid() || !validate_utf8(tab.text).valid() ||
            !identities.insert(tab.stable_id).second) {
            throw std::invalid_argument("Ribbon tabs require unique stable IDs and valid UTF-8");
        }
        if (tab.groups.size() > maximum_groups_per_tab) {
            throw std::invalid_argument("Ribbon tab has more groups than maximum_groups_per_tab");
        }
        for (const RibbonGroupSpec& group : tab.groups) {
            if (group.stable_id.empty() || !validate_utf8(group.stable_id).valid() ||
                !validate_utf8(group.caption).valid() || !identities.insert(group.stable_id).second) {
                throw std::invalid_argument("Ribbon groups require unique stable IDs and valid UTF-8");
            }
            if (group.commands.size() > maximum_commands_per_group) {
                throw std::invalid_argument("Ribbon group has more commands than maximum_commands_per_group");
            }
            for (const RibbonCommandSpec& command : group.commands) {
                if (command.stable_id.empty() || !validate_utf8(command.stable_id).valid() ||
                    !validate_utf8(command.text).valid() || !identities.insert(command.stable_id).second) {
                    throw std::invalid_argument("Ribbon commands require unique stable IDs and valid UTF-8");
                }
                if (needs_command(command.kind) && !command.command) {
                    throw std::invalid_argument("Ribbon buttons, toggles and splits require a Command");
                }
                if (hosted(command.kind) && command.command) {
                    throw std::invalid_argument("Ribbon hosted commands take no Command");
                }
                if (has_menu(command.kind) != !command.drop_down.empty()) {
                    throw std::invalid_argument("Ribbon split and drop-down commands require a menu, and others have none");
                }
                if (!command.show_label && (command.size != RibbonCommandSize::small || command.image.value == 0U ||
                                            hosted(command.kind))) {
                    throw std::invalid_argument("Ribbon icon-only commands must be small and have an image");
                }
                if (command.kind != RibbonCommandKind::combo_box && !command.choices.empty()) {
                    throw std::invalid_argument("Ribbon choices belong to combo box commands only");
                }
                for (const std::string& choice : command.choices) {
                    if (!validate_utf8(choice).valid()) throw std::invalid_argument("Ribbon choices must be valid UTF-8");
                }
                if (!std::isfinite(command.width) || command.width < 0.0 || command.width > maximum_hosted_width) {
                    throw std::invalid_argument("Ribbon command width must be finite and between 0 and 1024");
                }
                if (!command.drop_down.empty()) {
                    std::unordered_set<std::string> menu_identities{};
                    std::size_t count{};
                    menu_detail::validate_specs(command.drop_down, menu_identities, 1U, count);
                }
            }
        }
    }
    close_menus();
    // Hosted controls carry over to a command of the same ID and kind.
    std::vector<std::pair<std::string, CommandRecord>> previous;
    for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
        for (std::size_t group = 0; group < tabs_[tab].groups.size(); ++group) {
            for (std::size_t command = 0; command < tabs_[tab].groups[group].commands.size(); ++command) {
                CommandRecord& record = records_[tab][group][command];
                if (!record.hosted) continue;
                const RibbonCommandSpec& spec = tabs_[tab].groups[group].commands[command];
                std::string key = spec.stable_id;
                key.push_back('/');
                key.push_back(static_cast<char>('0' + static_cast<int>(spec.kind)));
                previous.emplace_back(std::move(key), std::move(record));
            }
        }
    }
    const std::optional<std::size_t> old_selection = selected_tab_;
    std::string selected_id;
    if (old_selection) selected_id = tabs_[*old_selection].stable_id;
    tabs_ = std::move(tabs);
    records_.clear();
    records_.resize(tabs_.size());
    for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
        records_[tab].resize(tabs_[tab].groups.size());
        for (std::size_t group = 0; group < tabs_[tab].groups.size(); ++group) {
            const std::vector<RibbonCommandSpec>& commands = tabs_[tab].groups[group].commands;
            records_[tab][group].resize(commands.size());
            for (std::size_t command = 0; command < commands.size(); ++command) {
                const RibbonCommandSpec& spec = commands[command];
                CommandRecord& record = records_[tab][group][command];
                if (hosted(spec.kind)) {
                    std::string key = spec.stable_id;
                    key.push_back('/');
                    key.push_back(static_cast<char>('0' + static_cast<int>(spec.kind)));
                    for (std::pair<std::string, CommandRecord>& old : previous) {
                        if (old.first != key || !old.second.hosted) continue;
                        record.hosted = std::move(old.second.hosted);
                        break;
                    }
                    if (!record.hosted) {
                        record.hosted = make_hosted(spec);
                        add_child(record.hosted);
                    }
                    configure_hosted(spec, *record.hosted);
                }
                if (spec.command) {
                    const CommandWatch watch{this, tab, group, command};
                    record.watch = (*spec.command).state_changed().subscribe(*this, watch);
                    record.text = (*spec.command).state().text;
                    record.visible = (*spec.command).state().visible;
                }
            }
        }
    }
    for (std::pair<std::string, CommandRecord>& old : previous) {
        if (old.second.hosted) static_cast<void>(remove_child((*old.second.hosted).runtime_id()));
    }
    selected_tab_.reset();
    for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
        if (tabs_[tab].stable_id == selected_id) selected_tab_ = tab;
    }
    if (!selected_tab_ && !tabs_.empty()) selected_tab_ = 0U;
    hot_ = Hit{};
    pressed_command_.reset();
    pressed_launcher_.reset();
    stop_.reset();
    invalidate(Dirty::measure | Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

Control::Ptr Ribbon::make_hosted(const RibbonCommandSpec& command) const {
    const StableId identity(std::string(stable_id().value()) + "." + command.stable_id);
    if (command.kind == RibbonCommandKind::combo_box) return make_control<ComboBox>(identity);
    if (command.kind == RibbonCommandKind::numeric_up_down) return make_control<NumericUpDown>(identity);
    if (command.kind == RibbonCommandKind::check_box) return make_control<CheckBox>(identity, command.text);
    return make_control<TextBox>(identity);
}

void Ribbon::configure_hosted(const RibbonCommandSpec& command, Control& control) const {
    control.set_accessible_name(parse_mnemonic_text(command.text).display_text);
    if (command.kind == RibbonCommandKind::combo_box) {
        ComboBox& combo = static_cast<ComboBox&>(control);
        const std::span<const std::string> current = combo.items();
        const bool same = current.size() == command.choices.size() &&
                          std::equal(current.begin(), current.end(), command.choices.begin());
        if (!same) combo.set_items(command.choices);
    }
    if (command.kind == RibbonCommandKind::check_box) {
        CheckBox& box = static_cast<CheckBox&>(control);
        if (box.text() != command.text) box.set_text(command.text);
    }
}

Control::Ptr Ribbon::hosted_control(std::string_view command_id, RibbonCommandKind kind) const {
    for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
        for (std::size_t group = 0; group < tabs_[tab].groups.size(); ++group) {
            const std::vector<RibbonCommandSpec>& commands = tabs_[tab].groups[group].commands;
            for (std::size_t command = 0; command < commands.size(); ++command) {
                if (commands[command].stable_id != command_id) continue;
                if (commands[command].kind != kind) break;
                return records_[tab][group][command].hosted;
            }
        }
    }
    throw std::out_of_range("Ribbon has no hosted command of this kind with this ID");
}

std::shared_ptr<ComboBox> Ribbon::combo_box(std::string_view command_id) const {
    return std::static_pointer_cast<ComboBox>(hosted_control(command_id, RibbonCommandKind::combo_box));
}

std::shared_ptr<NumericUpDown> Ribbon::numeric_up_down(std::string_view command_id) const {
    return std::static_pointer_cast<NumericUpDown>(hosted_control(command_id, RibbonCommandKind::numeric_up_down));
}

std::shared_ptr<CheckBox> Ribbon::check_box(std::string_view command_id) const {
    return std::static_pointer_cast<CheckBox>(hosted_control(command_id, RibbonCommandKind::check_box));
}

std::shared_ptr<TextBox> Ribbon::text_box(std::string_view command_id) const {
    return std::static_pointer_cast<TextBox>(hosted_control(command_id, RibbonCommandKind::text_box));
}

void Ribbon::set_selected_tab(std::size_t tab) {
    require_mutable();
    if (tab >= tabs_.size()) throw std::out_of_range("Ribbon tab index is past the end");
    select_tab(tab);
}

void Ribbon::select_tab(std::size_t tab) {
    if (selected_tab_ == tab) return;
    close_menus();
    selected_tab_ = tab;
    hot_ = Hit{};
    pressed_command_.reset();
    pressed_launcher_.reset();
    if (stop_ && (*stop_).kind != StopKind::tab && (*stop_).kind != StopKind::application) {
        stop_ = Stop{StopKind::tab, tab, {}};
    }
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
    const RibbonTabChange change{std::string(stable_id().value()), tab, tabs_[tab].stable_id};
    publish_change(selected_tab_changed_, change);
}

void Ribbon::set_minimized(bool minimized) {
    require_mutable();
    if (minimized_ == minimized) return;
    close_menus();
    minimized_ = minimized;
    hot_ = Hit{};
    if (minimized && stop_ && (*stop_).kind != StopKind::tab && (*stop_).kind != StopKind::application) {
        stop_ = Stop{StopKind::tab, selected_tab_.value_or(0U), {}};
    }
    invalidate(Dirty::measure | Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
    publish_change(minimized_changed_, minimized_);
}

void Ribbon::set_application_text(std::string text) {
    require_mutable();
    if (!validate_utf8(text).valid()) throw std::invalid_argument("Ribbon application text must be valid UTF-8");
    if (application_text_ == text) return;
    application_text_ = std::move(text);
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void Ribbon::set_application_menu(std::vector<MenuItemSpec> items) {
    require_mutable();
    std::unordered_set<std::string> identities{};
    std::size_t count{};
    menu_detail::validate_specs(items, identities, 1U, count);
    close_menus();
    application_menu_ = std::move(items);
    invalidate(Dirty::semantics);
}

void Ribbon::set_font(FontSpec font) {
    require_mutable();
    if (!valid_font_spec(font)) throw std::invalid_argument("Ribbon font specification is invalid");
    if (font_ == font) return;
    font_ = font;
    invalidate(Dirty::measure | Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

const RibbonGroupSpec* Ribbon::selected_group(std::size_t group) const noexcept {
    if (!selected_tab_ || group >= tabs_[*selected_tab_].groups.size()) return nullptr;
    return &tabs_[*selected_tab_].groups[group];
}

const RibbonCommandSpec* Ribbon::selected_command(CommandAddress address) const noexcept {
    const RibbonGroupSpec* const group = selected_group(address.group);
    if (group == nullptr || address.command >= (*group).commands.size()) return nullptr;
    return &(*group).commands[address.command];
}

std::optional<Ribbon::CommandAddress> Ribbon::find_command(std::string_view command_id) const {
    if (!selected_tab_) return std::nullopt;
    const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
    for (std::size_t group = 0; group < groups.size(); ++group) {
        for (std::size_t command = 0; command < groups[group].commands.size(); ++command) {
            if (groups[group].commands[command].stable_id == command_id) return CommandAddress{group, command};
        }
    }
    return std::nullopt;
}

// The command's text as authored, mnemonic marker included: its own, or its
// Command's when it has none.
const std::string& Ribbon::authored_text(const RibbonCommandSpec& command) const noexcept {
    if (command.text.empty() && command.command) return (*command.command).state().text;
    return command.text;
}

std::string Ribbon::command_text(const RibbonCommandSpec& command) const {
    return parse_mnemonic_text(authored_text(command)).display_text;
}

bool Ribbon::command_enabled(const RibbonCommandSpec& command) const {
    const bool own = !command.command || (*command.command).state().enabled;
    return own && effectively_enabled();
}

bool Ribbon::command_visible(const RibbonCommandSpec& command) const {
    const bool result = !command.command || (*command.command).state().visible;
    return result;
}

bool Ribbon::command_checked(const RibbonCommandSpec& command) const {
    const bool result = command.kind == RibbonCommandKind::toggle && command.command &&
                        (*command.command).state().checked;
    return result;
}

void Ribbon::CommandWatch::operator()(const CommandState&) const {
    if (ribbon != nullptr && (*ribbon).is_alive()) (*ribbon).command_state_changed(tab, group, command);
}

// A Command's state changed. Text and visibility change the layout; enabled
// and checked change only the command's own pixels, so only its bounds are
// repainted.
void Ribbon::command_state_changed(std::size_t tab, std::size_t group, std::size_t command) {
    if (tab >= tabs_.size() || group >= tabs_[tab].groups.size() || command >= tabs_[tab].groups[group].commands.size()) {
        return;
    }
    const RibbonCommandSpec& spec = tabs_[tab].groups[group].commands[command];
    CommandRecord& record = records_[tab][group][command];
    const gui_forms::CommandState& state = (*spec.command).state();
    if (state.text != record.text || state.visible != record.visible) {
        record.text = state.text;
        record.visible = state.visible;
        invalidate(Dirty::measure | Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
        return;
    }
    invalidate(Dirty::semantics);
    if (selected_tab_ != tab || minimized_) return;
    const Layout placed = layout();
    const Rect bounds = placed.commands[group][command];
    if (!bounds.empty()) invalidate(bounds);
}

// -- layout --------------------------------------------------------------------

double Ribbon::tab_row_height() const noexcept {
    const double result = tab_row_extent * effective_text_scale();
    return result;
}

double Ribbon::plane_height() const noexcept {
    const double rows = row_extent * static_cast<double>(rows_per_column);
    const double result = (plane_top_padding + rows + caption_extent + plane_bottom_padding) * effective_text_scale();
    return result;
}

double Ribbon::text_width(std::string_view text) const {
    if (text.empty()) return 0.0;
    const ResolvedTextLayout metrics = resolve_text_layout_utf8(text, effective_font(font_));
    return metrics.logical_size.width;
}

// A command's size at its authored size: large commands fill the rows'
// height, small and hosted commands take one row.
Size Ribbon::command_size(const RibbonCommandSpec& command) const {
    const double scale = effective_text_scale();
    const std::string label = command_text(command);
    const double arrow = has_menu(command.kind) ? arrow_zone * scale : 0.0;
    if (hosted(command.kind)) {
        double width = command.width;
        if (width <= 0.0 && command.kind == RibbonCommandKind::combo_box) width = default_combo_box_width;
        if (width <= 0.0 && command.kind == RibbonCommandKind::numeric_up_down) width = default_numeric_width;
        if (width <= 0.0 && command.kind == RibbonCommandKind::text_box) width = default_text_box_width;
        double measured = width * scale;
        if (width <= 0.0) measured = check_box_mark_width * scale + text_width(label);
        return {measured, row_extent * scale};
    }
    if (command.size == RibbonCommandSize::large) {
        const double width = std::max(large_minimum_width * scale, text_width(label) + 10.0 * scale);
        return {width, row_extent * scale * static_cast<double>(rows_per_column)};
    }
    double width = icon_gap * scale;
    if (command.image.value != 0U) width += (small_icon + icon_gap) * scale;
    if (command.show_label) width += text_width(label) + small_label_padding * scale;
    if (command.image.value == 0U) width += icon_gap * scale;
    width += arrow;
    return {width, row_extent * scale};
}

Ribbon::Layout Ribbon::layout() const {
    return layout_for(client_rectangle());
}

Ribbon::Layout Ribbon::layout_for(Rect client) const {
    const double scale = effective_text_scale();
    Layout placed;
    const double row = tab_row_height();
    placed.tab_rail = {client.x, client.y, client.width, row};
    const double application_width =
        std::max(application_minimum_width * scale,
                 text_width(parse_mnemonic_text(application_text_).display_text) + tab_padding * scale * 2.0);
    placed.application = {client.x, client.y, application_width, row};
    double x = client.x + application_width + tab_gap * scale;
    placed.tabs.reserve(tabs_.size());
    for (const RibbonTabSpec& tab : tabs_) {
        const double width = text_width(parse_mnemonic_text(tab.text).display_text) + tab_padding * scale * 2.0;
        placed.tabs.push_back({x, client.y, width, row});
        x += width + tab_gap * scale;
    }
    if (!selected_tab_) return placed;
    const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
    placed.groups.assign(groups.size(), Rect{});
    placed.commands.resize(groups.size());
    for (std::size_t group = 0; group < groups.size(); ++group) {
        placed.commands[group].assign(groups[group].commands.size(), Rect{});
    }
    if (minimized_) return placed;
    placed.plane = {client.x, client.y + row, client.width, plane_height()};
    const double top = placed.plane.y + plane_top_padding * scale;
    const double rows_height = row_extent * scale * static_cast<double>(rows_per_column);
    double group_x = placed.plane.x;
    for (std::size_t group = 0; group < groups.size(); ++group) {
        // Columns: a large command is a column of its own; small and hosted
        // commands stack three to a column, a column as wide as its widest.
        double column_x = group_x + group_padding * scale;
        double column_width = 0.0;
        std::size_t column_rows = 0U;
        for (std::size_t command = 0; command < groups[group].commands.size(); ++command) {
            const RibbonCommandSpec& spec = groups[group].commands[command];
            if (!command_visible(spec)) continue;
            const Size size = command_size(spec);
            const bool large = !hosted(spec.kind) && spec.size == RibbonCommandSize::large;
            if (large || column_rows == rows_per_column) {
                if (column_rows != 0U) {
                    column_x += column_width + column_gap * scale;
                    column_width = 0.0;
                    column_rows = 0U;
                }
            }
            if (large) {
                placed.commands[group][command] = {column_x, top, size.width, rows_height};
                column_x += size.width + column_gap * scale;
                continue;
            }
            const double y = top + static_cast<double>(column_rows) * row_extent * scale;
            placed.commands[group][command] = {column_x, y, size.width, size.height};
            column_width = std::max(column_width, size.width);
            ++column_rows;
        }
        if (column_rows != 0U) column_x += column_width + column_gap * scale;
        const double content = column_x - column_gap * scale - (group_x + group_padding * scale);
        double caption = text_width(groups[group].caption) + 8.0 * scale;
        if (groups[group].dialog_launcher) caption += (launcher_size + 4.0) * scale;
        const double inner = std::max(std::max(0.0, content), caption);
        const double width = inner + group_padding * scale * 2.0 + divider_width;
        placed.groups[group] = {group_x, placed.plane.y, width, placed.plane.height};
        group_x += width;
    }
    return placed;
}

Size Ribbon::measure(Size available) {
    const double height = tab_row_height() + (minimized_ ? 0.0 : plane_height());
    const Size result{available.width, std::min(available.height, height)};
    return result;
}

void Ribbon::arrange(Rect final_bounds) {
    arrange_self(final_bounds);
    const Layout placed = layout_for({0.0, 0.0, final_bounds.width, final_bounds.height});
    for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
        for (std::size_t group = 0; group < tabs_[tab].groups.size(); ++group) {
            for (std::size_t command = 0; command < tabs_[tab].groups[group].commands.size(); ++command) {
                const Control::Ptr& control = records_[tab][group][command].hosted;
                if (!control || !is_current_layout_child(control)) continue;
                Rect bounds{};
                if (selected_tab_ == tab) bounds = Rect::intersection(placed.commands[group][command], placed.plane);
                set_child_layout_collapsed(control, bounds.empty());
                if (!bounds.empty()) set_child_layout(control, bounds);
            }
        }
    }
}

// -- parts -------------------------------------------------------------------

Ribbon::TabParts Ribbon::tab_parts(const Layout& placed, std::size_t tab) const {
    const Rect bounds = placed.tabs[tab];
    const double padding = tab_padding * effective_text_scale();
    TabParts parts;
    parts.tab = PartGeometry::from_bounds(bounds);
    parts.tab_label = PartGeometry::from_bounds(
        {bounds.x + padding, bounds.y, std::max(0.0, bounds.width - padding * 2.0), bounds.height});
    parts.tab_focus = PartGeometry::from_bounds(bounds);
    return parts;
}

// A group's caption runs along its bottom, the launcher in its corner, and
// the divider down its right edge.
Ribbon::GroupParts Ribbon::group_parts(const Layout& placed, std::size_t group) const {
    const double scale = effective_text_scale();
    const Rect bounds = placed.groups[group];
    const Rect visible = Rect::intersection(bounds, placed.plane);
    const double caption_height = caption_extent * scale;
    const double bottom = bounds.y + bounds.height - plane_bottom_padding * scale;
    const Rect caption_row{bounds.x + group_padding * scale, bottom - caption_height,
                           std::max(0.0, bounds.width - group_padding * scale * 2.0 - divider_width), caption_height};
    Rect caption = caption_row;
    Rect launcher{};
    if ((*selected_group(group)).dialog_launcher) {
        const double side = launcher_size * scale;
        launcher = {caption_row.x + caption_row.width - side, caption_row.y + std::floor((caption_height - side) * 0.5),
                    side, side};
        caption.width = std::max(0.0, caption.width - side - 4.0 * scale);
    }
    const double inset = launcher_glyph_inset * scale;
    const Rect glyph{launcher.x + inset, launcher.y + inset, std::max(0.0, launcher.width - inset * 2.0),
                     std::max(0.0, launcher.height - inset * 2.0)};
    const Rect divider{bounds.x + bounds.width - divider_width, bounds.y + 3.0 * scale, divider_width,
                       std::max(0.0, bounds.height - 6.0 * scale)};
    GroupParts parts;
    parts.group_region = PartGeometry::from_bounds(visible);
    parts.group_caption = PartGeometry::from_bounds(Rect::intersection(caption, visible));
    parts.group_divider = PartGeometry::from_bounds(Rect::intersection(divider, visible));
    parts.group_launcher = PartGeometry::from_bounds(Rect::intersection(launcher, visible));
    parts.group_launcher_glyph = PartGeometry::from_bounds(Rect::intersection(glyph, visible));
    parts.group_launcher_focus = parts.group_launcher;
    if (parts.group_launcher.present) {
        AnatomyStateSet states;
        if (hot_.kind == Hit::Kind::launcher && hot_.index == group) states.insert(AnatomyState::hot);
        if (pressed_launcher_ == group) states.insert(AnatomyState::pressed);
        const Window* const host = attached_window();
        if (focused_ && stop_ && *stop_ == Stop{StopKind::launcher, group, {}} && host != nullptr &&
            (*host).focus_cue_visible()) {
            states.insert(AnatomyState::focus_visible);
        }
        parts.group_launcher.states = states;
        parts.group_launcher_glyph.states = states;
        parts.group_launcher_focus.states = states;
    }
    return parts;
}

// A command's parts within its bounds. A large command shows its icon over
// its label; a large split's face is the icon, its arrow the label and the
// glyph beneath. A small command shows its icon, label and, for a split or
// drop-down, an arrow zone at its right end. A hosted command is its
// control.
Ribbon::CommandParts Ribbon::command_parts(const Layout& placed, CommandAddress address) const {
    const double scale = effective_text_scale();
    const RibbonCommandSpec& spec = *selected_command(address);
    const Rect bounds = placed.commands[address.group][address.command];
    CommandParts parts;
    if (bounds.empty()) return parts;
    const Rect clip = Rect::intersection(placed.groups[address.group], placed.plane);
    const Rect visible = Rect::intersection(bounds, clip);
    if (hosted(spec.kind)) {
        const PartGeometry control = PartGeometry::from_bounds(visible);
        if (spec.kind == RibbonCommandKind::combo_box) parts.command_combo_box = control;
        if (spec.kind == RibbonCommandKind::numeric_up_down) parts.command_numeric_up_down = control;
        if (spec.kind == RibbonCommandKind::check_box) parts.command_check_box = control;
        if (spec.kind == RibbonCommandKind::text_box) parts.command_text_box = control;
        return parts;
    }
    Rect face = bounds;
    Rect icon{};
    Rect label{};
    Rect arrow{};
    Rect glyph{};
    if (spec.size == RibbonCommandSize::large) {
        const double side = large_icon * scale;
        const double icon_top = bounds.y + icon_gap * scale;
        if (spec.image.value != 0U) icon = {std::floor(bounds.x + (bounds.width - side) * 0.5), icon_top, side, side};
        label = {bounds.x + 2.0 * scale, icon_top + side + icon_gap * scale, std::max(0.0, bounds.width - 4.0 * scale),
                 label_extent * scale};
        if (has_menu(spec.kind)) {
            glyph = centred_square({bounds.x, label.y + label.height, bounds.width, arrow_glyph * scale + 2.0 * scale},
                                   arrow_glyph * scale);
        }
        if (spec.kind == RibbonCommandKind::split) {
            const double split_line = icon_top + side + icon_gap * scale * 0.5;
            face = {bounds.x, bounds.y, bounds.width, split_line - bounds.y};
            arrow = {bounds.x, split_line, bounds.width, bounds.y + bounds.height - split_line};
        }
    } else {
        double x = bounds.x + icon_gap * scale;
        const double middle = bounds.y + bounds.height * 0.5;
        const double arrow_width = has_menu(spec.kind) ? arrow_zone * scale : 0.0;
        if (spec.image.value != 0U) {
            const double side = small_icon * scale;
            icon = {x, std::floor(middle - side * 0.5), side, side};
            x += side + icon_gap * scale;
        } else {
            x += icon_gap * scale;
        }
        if (spec.show_label) {
            label = {x, bounds.y, std::max(0.0, bounds.x + bounds.width - arrow_width - x), bounds.height};
        }
        if (has_menu(spec.kind)) {
            const Rect zone{bounds.x + bounds.width - arrow_width, bounds.y, arrow_width, bounds.height};
            glyph = centred_square(zone, arrow_glyph * scale);
            if (spec.kind == RibbonCommandKind::split) {
                face = {bounds.x, bounds.y, std::max(0.0, bounds.width - arrow_width), bounds.height};
                arrow = zone;
            }
        }
    }
    parts.command_face = PartGeometry::from_bounds(Rect::intersection(face, visible));
    parts.command_icon = PartGeometry::from_bounds(Rect::intersection(icon, visible));
    parts.command_label = PartGeometry::from_bounds(Rect::intersection(label, visible));
    parts.command_arrow = PartGeometry::from_bounds(Rect::intersection(arrow, visible));
    parts.command_arrow_glyph = PartGeometry::from_bounds(Rect::intersection(glyph, visible));
    parts.command_focus = PartGeometry::from_bounds(visible);
    if (parts.command_arrow.present) {
        const AnatomyStateSet states = command_states(address);
        if (states.contains(AnatomyState::hot)) parts.command_arrow.states.insert(AnatomyState::hot);
        if (pressed_command_ == address && pressed_zone_ == CommandZone::arrow) {
            parts.command_arrow.states.insert(AnatomyState::pressed);
        }
    }
    return parts;
}

AnatomyStateSet Ribbon::tab_states(std::size_t tab) const {
    AnatomyStateSet states;
    if (hot_.kind == Hit::Kind::tab && hot_.index == tab) states.insert(AnatomyState::hot);
    if (selected_tab_ == tab) states.insert(AnatomyState::selected);
    const Window* const host = attached_window();
    if (focused_ && stop_ && *stop_ == Stop{StopKind::tab, tab, {}} && host != nullptr &&
        (*host).focus_cue_visible()) {
        states.insert(AnatomyState::focus_visible);
    }
    return states;
}

AnatomyStateSet Ribbon::command_states(CommandAddress address) const {
    AnatomyStateSet states;
    const RibbonCommandSpec* const spec = selected_command(address);
    if (spec == nullptr) return states;
    const bool enabled = command_enabled(*spec);
    if (!enabled) states.insert(AnatomyState::disabled);
    if (enabled && !hosted((*spec).kind) && hot_.kind == Hit::Kind::command && hot_.command == address) {
        states.insert(AnatomyState::hot);
    }
    if (pressed_command_ == address) states.insert(AnatomyState::pressed);
    if (command_checked(*spec)) states.insert(AnatomyState::checked);
    if (open_command_ == address) states.insert(AnatomyState::open);
    const Window* const host = attached_window();
    if (focused_ && stop_ && *stop_ == Stop{StopKind::command, 0U, address} && host != nullptr &&
        (*host).focus_cue_visible()) {
        states.insert(AnatomyState::focus_visible);
    }
    return states;
}

// -- paint -------------------------------------------------------------------

void Ribbon::on_paint(Painter& painter, Rect) {
    const Layout placed = layout();
    const FontSpec font = effective_font(font_);
    PartPaint part;
    part.font = font;
    part.family = ControlVisualRole::panel;
    part.role = PartRole::face;
    part.bounds = placed.tab_rail;
    paint_part(painter, *this, part);
    // The File tab: a solid accent face, as the design language's File tab.
    AnatomyStateSet application;
    if (hot_.kind == Hit::Kind::application) application.insert(AnatomyState::hot);
    if (application_pressed_) application.insert(AnatomyState::pressed);
    if (application_open_) application.insert(AnatomyState::open);
    const Window* const host = attached_window();
    const bool cue = focused_ && host != nullptr && (*host).focus_cue_visible();
    if (cue && stop_ && (*stop_).kind == StopKind::application) application.insert(AnatomyState::focus_visible);
    const std::string application_label = parse_mnemonic_text(application_text_).display_text;
    part.family = ControlVisualRole::accent_button;
    part.states = application;
    part.bounds = placed.application;
    paint_part(painter, *this, part);
    part.role = PartRole::label;
    part.text = application_label;
    part.alignment = PartTextAlignment::center;
    paint_part(painter, *this, part);
    part.text = {};
    part.role = PartRole::focus;
    paint_part(painter, *this, part);
    // Tabs: flat at rest, a plate when hot or selected (the menu-item
    // family) until T4 draws the tab stack.
    part.family = ControlVisualRole::menu_item;
    for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
        const TabParts parts = tab_parts(placed, tab);
        part.states = tab_states(tab);
        part.role = PartRole::face;
        part.bounds = parts.tab.bounds;
        paint_part(painter, *this, part);
        const std::string label = parse_mnemonic_text(tabs_[tab].text).display_text;
        part.role = PartRole::label;
        part.bounds = parts.tab_label.bounds;
        part.text = label;
        paint_part(painter, *this, part);
        part.text = {};
        part.role = PartRole::focus;
        part.bounds = parts.tab_focus.bounds;
        paint_part(painter, *this, part);
    }
    part.alignment = PartTextAlignment::start;
    if (placed.plane.empty() || !selected_tab_) return;
    part.states = {};
    part.family = ControlVisualRole::panel;
    part.role = PartRole::face;
    part.bounds = placed.plane;
    paint_part(painter, *this, part);
    painter.save();
    painter.clip_rect(placed.plane);
    const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
    for (std::size_t group = 0; group < groups.size(); ++group) {
        if (Rect::intersection(placed.groups[group], placed.plane).empty()) continue;
        const GroupParts parts = group_parts(placed, group);
        part.family = ControlVisualRole::panel;
        part.states = {};
        part.role = PartRole::caption;
        part.bounds = parts.group_caption.bounds;
        part.alignment = PartTextAlignment::center;
        part.text = groups[group].caption;
        paint_part(painter, *this, part);
        part.text = {};
        part.alignment = PartTextAlignment::start;
        part.role = PartRole::divider;
        part.bounds = parts.group_divider.bounds;
        paint_part(painter, *this, part);
        if (parts.group_launcher.present) {
            part.family = ControlVisualRole::menu_item;
            part.states = parts.group_launcher.states;
            part.role = PartRole::actuator;
            part.bounds = parts.group_launcher.bounds;
            paint_part(painter, *this, part);
            part.role = PartRole::glyph;
            part.bounds = parts.group_launcher_glyph.bounds;
            part.mark = PartMark::dialog_launcher;
            paint_part(painter, *this, part);
            part.mark = PartMark::none;
            part.role = PartRole::focus;
            part.bounds = parts.group_launcher_focus.bounds;
            paint_part(painter, *this, part);
        }
        for (std::size_t command = 0; command < groups[group].commands.size(); ++command) {
            const RibbonCommandSpec& spec = groups[group].commands[command];
            const CommandAddress address{group, command};
            const CommandParts command_part = command_parts(placed, address);
            if (hosted(spec.kind) || !command_part.command_face.present) continue;
            const AnatomyStateSet states = command_states(address);
            // A split's face and arrow press apart, as ToolStrip's do.
            AnatomyStateSet face_states = states;
            if (spec.kind == RibbonCommandKind::split && pressed_zone_ != CommandZone::main) {
                face_states.erase(AnatomyState::pressed);
            }
            part.family = ControlVisualRole::menu_item;
            part.role = PartRole::face;
            part.states = face_states;
            part.bounds = command_part.command_face.bounds;
            paint_part(painter, *this, part);
            if (command_part.command_arrow.present) {
                part.role = PartRole::actuator;
                part.states = command_part.command_arrow.states;
                part.bounds = command_part.command_arrow.bounds;
                paint_part(painter, *this, part);
            }
            part.states = states;
            if (command_part.command_icon.present) {
                part.role = PartRole::icon;
                part.bounds = command_part.command_icon.bounds;
                part.image = spec.image;
                paint_part(painter, *this, part);
                part.image = {};
            }
            if (command_part.command_label.present) {
                const std::string label = command_text(spec);
                part.role = PartRole::label;
                part.bounds = command_part.command_label.bounds;
                part.text = label;
                part.alignment = spec.size == RibbonCommandSize::large ? PartTextAlignment::center
                                                                       : PartTextAlignment::start;
                paint_part(painter, *this, part);
                part.text = {};
                part.alignment = PartTextAlignment::start;
            }
            if (command_part.command_arrow_glyph.present) {
                part.role = PartRole::glyph;
                part.bounds = command_part.command_arrow_glyph.bounds;
                part.mark = PartMark::arrow_down;
                paint_part(painter, *this, part);
                part.mark = PartMark::none;
            }
            part.role = PartRole::focus;
            part.bounds = command_part.command_focus.bounds;
            paint_part(painter, *this, part);
        }
    }
    painter.restore();
}

// -- input -------------------------------------------------------------------

Ribbon::Hit Ribbon::hit_test(const Layout& placed, Point local) const {
    Hit hit;
    if (placed.application.contains(local)) {
        hit.kind = Hit::Kind::application;
        return hit;
    }
    for (std::size_t tab = 0; tab < placed.tabs.size(); ++tab) {
        if (!placed.tabs[tab].contains(local)) continue;
        hit.kind = Hit::Kind::tab;
        hit.index = tab;
        return hit;
    }
    if (placed.plane.empty() || !placed.plane.contains(local) || !selected_tab_) return hit;
    const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
    for (std::size_t group = 0; group < groups.size(); ++group) {
        if (!placed.groups[group].contains(local)) continue;
        const GroupParts parts = group_parts(placed, group);
        if (parts.group_launcher.present && parts.group_launcher.bounds.contains(local)) {
            hit.kind = Hit::Kind::launcher;
            hit.index = group;
            return hit;
        }
        for (std::size_t command = 0; command < groups[group].commands.size(); ++command) {
            if (!placed.commands[group][command].contains(local)) continue;
            const CommandAddress address{group, command};
            const CommandParts command_part = command_parts(placed, address);
            hit.kind = Hit::Kind::command;
            hit.command = address;
            hit.zone = command_part.command_arrow.present && command_part.command_arrow.bounds.contains(local)
                           ? CommandZone::arrow
                           : CommandZone::main;
            return hit;
        }
    }
    return hit;
}

void Ribbon::set_hot(const Hit& hit) {
    if (hot_.kind == hit.kind && hot_.index == hit.index && hot_.command == hit.command && hot_.zone == hit.zone) {
        return;
    }
    hot_ = hit;
    invalidate(Dirty::paint);
}

void Ribbon::click_tab(std::size_t tab) {
    if (tab >= tabs_.size()) return;
    // The selected tab minimizes the command plane or shows it again
    // (owner decision 19); another tab is selected and shows the plane.
    if (selected_tab_ == tab) {
        set_minimized(!minimized_);
        return;
    }
    select_tab(tab);
    if (minimized_) set_minimized(false);
}

// The one path every activation of a command takes: pointer, keyboard,
// mnemonic and accessibility.
void Ribbon::click_command(CommandAddress address) {
    const RibbonCommandSpec* const found = selected_command(address);
    if (found == nullptr || hosted((*found).kind) || !command_enabled(*found)) return;
    const RibbonCommandSpec spec = *found;
    if (spec.kind == RibbonCommandKind::drop_down) {
        static_cast<void>(show_drop_down(spec.stable_id));
        return;
    }
    if (spec.kind == RibbonCommandKind::toggle) {
        (*spec.command).set_checked(!(*spec.command).state().checked);
    }
    const Control::Ptr lifetime = shared_from_this();
    if (window() != nullptr) {
        static_cast<void>((*spec.command).execute_routed(*window(), stable_id().value()));
    } else {
        static_cast<void>((*spec.command).execute(stable_id().value()));
    }
    if (!is_alive()) return;
    const RibbonCommandClick click{std::string(stable_id().value()), spec.stable_id};
    command_clicked_.emit(click);
}

void Ribbon::launch(std::size_t group) {
    const RibbonGroupSpec* const found = selected_group(group);
    if (found == nullptr || !(*found).dialog_launcher || !effectively_enabled()) return;
    const RibbonDialogLaunch launched{std::string(stable_id().value()), (*found).stable_id};
    dialog_launcher_clicked_.emit(launched);
}

void Ribbon::on_pointer(PointerEvent& event) {
    if (!eligible_for_input()) return;
    if (event.action == PointerAction::leave) {
        set_hot(Hit{});
        return;
    }
    const Rect absolute = absolute_bounds();
    const Point local{event.position.x - absolute.x, event.position.y - absolute.y};
    const Layout placed = layout();
    const Hit hit = hit_test(placed, local);
    if (event.action == PointerAction::move || event.action == PointerAction::enter) {
        set_hot(hit);
        return;
    }
    if (event.button != PointerButton::primary) return;
    const Control::Ptr lifetime = shared_from_this();
    if (event.action == PointerAction::down) {
        if (hit.kind == Hit::Kind::none) return;
        event.handled = true;
        if (hit.kind == Hit::Kind::application) {
            if (application_open_) {
                close_menus();
            } else {
                static_cast<void>(show_application_menu(MenuOpenMode::pointer));
            }
        } else if (hit.kind == Hit::Kind::tab) {
            click_tab(hit.index);
        } else if (hit.kind == Hit::Kind::launcher) {
            pressed_launcher_ = hit.index;
            invalidate(Dirty::paint);
        } else {
            const RibbonCommandSpec* const spec = selected_command(hit.command);
            if (spec == nullptr || hosted((*spec).kind) || !command_enabled(*spec)) return;
            const bool opens = (*spec).kind == RibbonCommandKind::drop_down ||
                               ((*spec).kind == RibbonCommandKind::split && hit.zone == CommandZone::arrow);
            if (opens) {
                if (open_command_ == hit.command) {
                    close_menus();
                } else {
                    static_cast<void>(show_drop_down((*spec).stable_id, MenuOpenMode::pointer));
                }
                return;
            }
            pressed_command_ = hit.command;
            pressed_zone_ = hit.zone;
            invalidate(Dirty::paint);
        }
        return;
    }
    if (event.action != PointerAction::up) return;
    const std::optional<CommandAddress> pressed = pressed_command_;
    const CommandZone zone = pressed_zone_;
    const std::optional<std::size_t> launcher = pressed_launcher_;
    if (!pressed && !launcher) return;
    pressed_command_.reset();
    pressed_zone_ = CommandZone::none;
    pressed_launcher_.reset();
    invalidate(Dirty::paint);
    event.handled = true;
    // A click is a press and release on the same part.
    if (pressed && hit.kind == Hit::Kind::command && hit.command == *pressed && hit.zone == zone) {
        click_command(*pressed);
    } else if (launcher && hit.kind == Hit::Kind::launcher && hit.index == *launcher) {
        launch(*launcher);
    }
}

// The keyboard stops in Tab order: the File tab, the tabs, then (while the
// plane shows) each group's visible button-family commands and launcher.
std::vector<Ribbon::Stop> Ribbon::stops() const {
    std::vector<Stop> result;
    result.push_back(Stop{StopKind::application, 0U, {}});
    for (std::size_t tab = 0; tab < tabs_.size(); ++tab) result.push_back(Stop{StopKind::tab, tab, {}});
    if (minimized_ || !selected_tab_) return result;
    const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
    for (std::size_t group = 0; group < groups.size(); ++group) {
        for (std::size_t command = 0; command < groups[group].commands.size(); ++command) {
            const RibbonCommandSpec& spec = groups[group].commands[command];
            if (hosted(spec.kind) || !command_visible(spec)) continue;
            result.push_back(Stop{StopKind::command, 0U, CommandAddress{group, command}});
        }
        if (groups[group].dialog_launcher) result.push_back(Stop{StopKind::launcher, group, {}});
    }
    return result;
}

void Ribbon::set_stop(std::optional<Stop> stop) {
    if (stop_ == stop) return;
    stop_ = stop;
    invalidate(Dirty::paint | Dirty::semantics);
}

void Ribbon::activate_stop(const Stop& stop) {
    if (stop.kind == StopKind::application) {
        static_cast<void>(show_application_menu());
    } else if (stop.kind == StopKind::tab) {
        click_tab(stop.index);
    } else if (stop.kind == StopKind::launcher) {
        launch(stop.index);
    } else {
        click_command(stop.command);
    }
}

void Ribbon::on_key(KeyEvent& event) {
    if (!focused_ || event.action != KeyAction::down || !effectively_enabled()) return;
    const std::vector<Stop> all = stops();
    if (!stop_) stop_ = Stop{StopKind::tab, selected_tab_.value_or(0U), {}};
    std::size_t position = 0U;
    for (std::size_t index = 0; index < all.size(); ++index) {
        if (all[index] == *stop_) position = index;
    }
    const Stop current = all[position];
    const bool in_row = current.kind == StopKind::application || current.kind == StopKind::tab;
    const std::size_t row_count = tabs_.size() + 1U;
    const std::uint32_t key = event.physical_key;
    const bool shift = has_modifier(event.modifiers, Modifier::shift);
    if (key == PhysicalKey::tab) {
        // Tab moves through every stop and leaves the ribbon after the last
        // (or, with Shift, before the first).
        if (!shift && position + 1U < all.size()) {
            set_stop(all[position + 1U]);
        } else if (shift && position > 0U) {
            set_stop(all[position - 1U]);
        } else {
            return;
        }
    } else if (in_row && (key == PhysicalKey::left || key == PhysicalKey::right)) {
        // Along the tab row; selection follows the focused tab.
        const std::size_t next = key == PhysicalKey::right ? std::min(position + 1U, row_count - 1U)
                                                           : (position == 0U ? 0U : position - 1U);
        set_stop(all[next]);
        if (all[next].kind == StopKind::tab) select_tab(all[next].index);
    } else if (in_row && key == PhysicalKey::down) {
        if (all.size() <= row_count) return;
        set_stop(all[row_count]);
    } else if (!in_row && (key == PhysicalKey::right || key == PhysicalKey::left || key == PhysicalKey::up ||
                           key == PhysicalKey::down)) {
        const RibbonCommandSpec* const spec =
            current.kind == StopKind::command ? selected_command(current.command) : nullptr;
        if (key == PhysicalKey::down && spec != nullptr && has_menu((*spec).kind)) {
            static_cast<void>(show_drop_down((*spec).stable_id));
        } else if (key == PhysicalKey::right || key == PhysicalKey::down) {
            if (position + 1U < all.size()) set_stop(all[position + 1U]);
        } else if (position > row_count) {
            set_stop(all[position - 1U]);
        } else {
            set_stop(Stop{StopKind::tab, selected_tab_.value_or(0U), {}});
        }
    } else if (key == PhysicalKey::enter || key == PhysicalKey::space) {
        activate_stop(current);
    } else if (key == PhysicalKey::escape) {
        if (menu_open()) {
            close_menus();
        } else if (!in_row) {
            set_stop(Stop{StopKind::tab, selected_tab_.value_or(0U), {}});
        } else {
            return;
        }
    } else {
        return;
    }
    event.handled = true;
}

void Ribbon::on_focus_changed(bool focused) {
    focused_ = focused;
    if (focused_ && !stop_) stop_ = Stop{StopKind::tab, selected_tab_.value_or(0U), {}};
    invalidate(Dirty::paint | Dirty::semantics);
}

bool Ribbon::mnemonic_matches(char32_t character) const noexcept {
    if (is_mnemonic(character, application_text_)) return true;
    for (const RibbonTabSpec& tab : tabs_) {
        if (is_mnemonic(character, tab.text)) return true;
    }
    if (minimized_ || !selected_tab_) return false;
    for (const RibbonGroupSpec& group : tabs_[*selected_tab_].groups) {
        for (const RibbonCommandSpec& command : group.commands) {
            if (!hosted(command.kind) && is_mnemonic(character, authored_text(command))) return true;
        }
    }
    return false;
}

// A mnemonic opens the File menu, selects (and shows) a tab, or activates a
// command of the selected tab, in that order.
bool Ribbon::process_mnemonic_self(char32_t character) {
    if (is_mnemonic(character, application_text_)) {
        static_cast<void>(show_application_menu());
        return true;
    }
    for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
        if (!is_mnemonic(character, tabs_[tab].text)) continue;
        select_tab(tab);
        if (minimized_) set_minimized(false);
        return true;
    }
    if (minimized_ || !selected_tab_) return false;
    const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
    for (std::size_t group = 0; group < groups.size(); ++group) {
        for (std::size_t command = 0; command < groups[group].commands.size(); ++command) {
            const RibbonCommandSpec& spec = groups[group].commands[command];
            if (hosted(spec.kind) || !is_mnemonic(character, authored_text(spec))) continue;
            click_command(CommandAddress{group, command});
            return true;
        }
    }
    return false;
}

// -- menus -------------------------------------------------------------------

bool Ribbon::show_application_menu(MenuOpenMode mode) {
    require_mutable();
    if (application_menu_.empty() || attached_window() == nullptr || !effectively_enabled()) return false;
    if (application_open_ && (*popup_).is_open()) return true;
    close_menus();
    const Layout placed = layout();
    (*popup_).set_items(application_menu_);
    const Rect absolute = absolute_bounds();
    // Over the ribbon, below the File tab (owner decision 23).
    (*popup_).show(shared_from_this(), {absolute.x + placed.application.x, absolute.y + placed.application.y +
                                                                               placed.application.height},
                   mode);
    application_open_ = true;
    invalidate(Dirty::paint | Dirty::semantics);
    return true;
}

bool Ribbon::show_drop_down(std::string_view command_id, MenuOpenMode mode) {
    require_mutable();
    const std::optional<CommandAddress> address = find_command(command_id);
    if (!address || minimized_ || attached_window() == nullptr) return false;
    const RibbonCommandSpec& spec = *selected_command(*address);
    if (!has_menu(spec.kind) || !command_enabled(spec)) return false;
    const Layout placed = layout();
    const Rect bounds = Rect::intersection(placed.commands[(*address).group][(*address).command], placed.plane);
    if (bounds.empty()) return false;
    if (open_command_ == address && (*popup_).is_open()) return true;
    close_menus();
    (*popup_).set_items(spec.drop_down);
    const Rect absolute = absolute_bounds();
    (*popup_).show(shared_from_this(), {absolute.x + bounds.x, absolute.y + bounds.y + bounds.height}, mode);
    open_command_ = address;
    invalidate(Dirty::paint | Dirty::semantics);
    return true;
}

void Ribbon::close_menus() noexcept {
    if (popup_) (*popup_).close();
}

bool Ribbon::menu_open() const noexcept {
    const bool result = popup_ && (*popup_).is_open();
    return result;
}

void Ribbon::on_popup_open_changed(bool open) {
    if (open) return;
    application_open_ = false;
    open_command_.reset();
    invalidate(Dirty::paint | Dirty::semantics);
}

void Ribbon::on_popup_item_invoked(const MenuItemInvocation& invocation) {
    if (application_open_) {
        application_menu_item_invoked_.emit(invocation);
        return;
    }
    if (!open_command_) return;
    const RibbonCommandSpec* const spec = selected_command(*open_command_);
    if (spec == nullptr) return;
    const RibbonDropDownInvocation chosen{std::string(stable_id().value()), (*spec).stable_id, invocation};
    drop_down_item_invoked_.emit(chosen);
}

// -- semantics -----------------------------------------------------------------

SemanticDescriptor Ribbon::semantic_descriptor() const {
    SemanticDescriptor descriptor{};
    descriptor.role = SemanticRole::group;
    descriptor.name = accessible_name().empty() ? "Ribbon" : accessible_name();
    descriptor.description = accessible_description();
    descriptor.actions = {SemanticAction::focus};
    descriptor.exposed = true;
    return descriptor;
}

std::vector<SemanticNode> Ribbon::semantic_virtual_children() const {
    std::vector<SemanticNode> nodes{};
    const std::string ribbon_id(stable_id().value());
    const Rect absolute = absolute_bounds();
    const Layout placed = layout();
    const bool enabled = effectively_enabled();
    SemanticState base = SemanticState::visible;
    if (enabled) base |= SemanticState::enabled;

    SemanticNode application{};
    application.stable_id = ribbon_id + ".application";
    application.runtime_id = 0U;
    application.role = SemanticRole::menu_button;
    application.name = parse_mnemonic_text(application_text_).display_text;
    application.has_popup = true;
    application.bounds = to_window(absolute, placed.application);
    application.states = base | SemanticState::focusable;
    if (application_open_) application.states |= SemanticState::expanded;
    application.actions = {SemanticAction::press, SemanticAction::show_menu};
    nodes.push_back(std::move(application));

    SemanticNode row{};
    row.stable_id = ribbon_id + ".tab_row";
    row.role = SemanticRole::tab_group;
    row.name = "Ribbon tabs";
    row.orientation = SemanticOrientation::horizontal;
    row.bounds = to_window(absolute, placed.tab_rail);
    row.states = base;
    for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
        SemanticNode node{};
        node.stable_id = node_identity(ribbon_id, "tab", tabs_[tab].stable_id);
        node.role = SemanticRole::tab;
        node.name = parse_mnemonic_text(tabs_[tab].text).display_text;
        node.position_in_set = tab + 1U;
        node.set_size = tabs_.size();
        node.bounds = to_window(absolute, placed.tabs[tab]);
        node.states = base | SemanticState::focusable;
        if (selected_tab_ == tab) node.states |= SemanticState::selected;
        if (focused_ && stop_ && *stop_ == Stop{StopKind::tab, tab, {}}) node.states |= SemanticState::focused;
        // The selected tab shows the command plane, its panel (owner
        // decision DK-40). Tabs publish select, not press (DK-35); a press
        // sent anyway still clicks the tab (on_semantic_child_action).
        if (selected_tab_ == tab && !placed.plane.empty()) node.controlled_panel = ribbon_id + ".command_plane";
        node.actions = {SemanticAction::select};
        row.children.push_back(std::move(node));
    }
    nodes.push_back(std::move(row));
    if (placed.plane.empty() || !selected_tab_) return nodes;

    SemanticNode plane{};
    plane.stable_id = ribbon_id + ".command_plane";
    plane.role = SemanticRole::tab_panel;
    plane.name = parse_mnemonic_text(tabs_[*selected_tab_].text).display_text;
    plane.bounds = to_window(absolute, placed.plane);
    plane.states = base;
    const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
    for (std::size_t group = 0; group < groups.size(); ++group) {
        const Rect visible = Rect::intersection(placed.groups[group], placed.plane);
        if (visible.empty()) continue;
        SemanticNode group_node{};
        group_node.stable_id = node_identity(ribbon_id, "group", groups[group].stable_id);
        group_node.role = SemanticRole::group;
        group_node.name = groups[group].caption;
        group_node.bounds = to_window(absolute, visible);
        group_node.states = base;
        // Commands count their place among the group's ribbon-drawn
        // commands; hosted controls describe themselves as children of the
        // ribbon.
        std::size_t set_size = 0U;
        for (std::size_t command = 0; command < groups[group].commands.size(); ++command) {
            const RibbonCommandSpec& spec = groups[group].commands[command];
            if (!hosted(spec.kind) && !placed.commands[group][command].empty()) ++set_size;
        }
        std::size_t position = 0U;
        for (std::size_t command = 0; command < groups[group].commands.size(); ++command) {
            const RibbonCommandSpec& spec = groups[group].commands[command];
            const Rect bounds = Rect::intersection(placed.commands[group][command], visible);
            if (hosted(spec.kind) || placed.commands[group][command].empty()) continue;
            ++position;
            const CommandAddress address{group, command};
            SemanticNode node{};
            node.stable_id = node_identity(ribbon_id, "command", spec.stable_id);
            node.role = SemanticRole::button;
            if (spec.kind == RibbonCommandKind::split) node.role = SemanticRole::split_button;
            if (spec.kind == RibbonCommandKind::drop_down) node.role = SemanticRole::menu_button;
            node.has_popup = has_menu(spec.kind);
            node.name = command_text(spec);
            node.position_in_set = position;
            node.set_size = set_size;
            node.bounds = to_window(absolute, bounds);
            node.states = SemanticState::visible | SemanticState::focusable;
            if (command_enabled(spec)) node.states |= SemanticState::enabled;
            if (command_checked(spec)) node.states |= SemanticState::checked;
            if (open_command_ == address) node.states |= SemanticState::expanded;
            if (focused_ && stop_ && *stop_ == Stop{StopKind::command, 0U, address}) {
                node.states |= SemanticState::focused;
            }
            node.actions = {SemanticAction::press};
            if (has_menu(spec.kind)) node.actions.push_back(SemanticAction::show_menu);
            group_node.children.push_back(std::move(node));
        }
        if (groups[group].dialog_launcher) {
            const GroupParts parts = group_parts(placed, group);
            SemanticNode launcher{};
            launcher.stable_id = node_identity(ribbon_id, "launcher", groups[group].stable_id);
            launcher.role = SemanticRole::button;
            launcher.name = groups[group].caption + " dialog launcher";
            launcher.bounds = to_window(absolute, parts.group_launcher.bounds);
            launcher.states = base | SemanticState::focusable;
            launcher.actions = {SemanticAction::press};
            group_node.children.push_back(std::move(launcher));
        }
        plane.children.push_back(std::move(group_node));
    }
    nodes.push_back(std::move(plane));
    return nodes;
}

bool Ribbon::on_semantic_child_action(std::string_view id, SemanticAction action, std::string_view) {
    if (!effectively_enabled()) return false;
    const std::string ribbon_id(stable_id().value());
    if (id == ribbon_id + ".application") {
        if (action == SemanticAction::collapse) {
            close_menus();
            return true;
        }
        if (action != SemanticAction::press && action != SemanticAction::show_menu &&
            action != SemanticAction::expand) {
            return false;
        }
        return show_application_menu();
    }
    const std::string_view tab_id = node_suffix(id, ribbon_id, "tab");
    for (std::size_t tab = 0; tab < tabs_.size() && !tab_id.empty(); ++tab) {
        if (tabs_[tab].stable_id != tab_id) continue;
        if (action == SemanticAction::press) {
            click_tab(tab);
            return true;
        }
        if (action != SemanticAction::select) return false;
        select_tab(tab);
        if (minimized_) set_minimized(false);
        return true;
    }
    const std::string_view command_id = node_suffix(id, ribbon_id, "command");
    if (!command_id.empty()) {
        const std::optional<CommandAddress> address = find_command(command_id);
        if (!address || minimized_) return false;
        if (action == SemanticAction::press) {
            click_command(*address);
            return true;
        }
        if (action == SemanticAction::show_menu || action == SemanticAction::expand) {
            return show_drop_down(command_id);
        }
        return false;
    }
    const std::string_view launcher_id = node_suffix(id, ribbon_id, "launcher");
    if (!launcher_id.empty() && action == SemanticAction::press && selected_tab_ && !minimized_) {
        const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
        for (std::size_t group = 0; group < groups.size(); ++group) {
            if (groups[group].stable_id != launcher_id) continue;
            launch(group);
            return true;
        }
    }
    return false;
}

// -- anatomy -----------------------------------------------------------------

namespace {

constexpr std::array<AnatomyState, 4> application_states{
    {AnatomyState::hot, AnatomyState::pressed, AnatomyState::open, AnatomyState::focus_visible}};
constexpr std::array<AnatomyState, 3> launcher_states{
    {AnatomyState::hot, AnatomyState::pressed, AnatomyState::focus_visible}};
constexpr std::array<AnatomyState, 2> arrow_states{{AnatomyState::hot, AnatomyState::pressed}};

constexpr std::array<PartField<Ribbon::Parts>, 6> ribbon_part_fields{{
    {.declaration = {.name = "region", .role = PartRole::region, .composed_from = {}},
     .member = &Ribbon::Parts::region},
    {.declaration = {.name = "tab_rail", .role = PartRole::face, .composed_from = {}},
     .member = &Ribbon::Parts::tab_rail},
    {.declaration = {.name = "application_button", .role = PartRole::face, .composed_from = "button",
                     .states = application_states},
     .member = &Ribbon::Parts::application_button},
    {.declaration = {.name = "application_label", .role = PartRole::label, .composed_from = {},
                     .states = application_states},
     .member = &Ribbon::Parts::application_label},
    {.declaration = {.name = "command_plane", .role = PartRole::face, .composed_from = {}},
     .member = &Ribbon::Parts::command_plane},
    {.declaration = {.name = "focus", .role = PartRole::focus, .composed_from = {}},
     .member = &Ribbon::Parts::focus},
}};

constexpr std::array<PartField<Ribbon::TabParts>, 3> ribbon_tab_part_fields{{
    {.declaration = {.name = "tab", .role = PartRole::face, .composed_from = {}},
     .member = &Ribbon::TabParts::tab},
    {.declaration = {.name = "tab_label", .role = PartRole::label, .composed_from = {}},
     .member = &Ribbon::TabParts::tab_label},
    {.declaration = {.name = "tab_focus", .role = PartRole::focus, .composed_from = {}},
     .member = &Ribbon::TabParts::tab_focus},
}};

constexpr std::array<StateField<Ribbon::TabStates>, 3> ribbon_tab_state_fields{{
    {.state = AnatomyState::hot, .member = &Ribbon::TabStates::hot},
    {.state = AnatomyState::selected, .member = &Ribbon::TabStates::selected},
    {.state = AnatomyState::focus_visible, .member = &Ribbon::TabStates::focus_visible},
}};

constexpr std::array<PartField<Ribbon::GroupParts>, 6> ribbon_group_part_fields{{
    {.declaration = {.name = "group_region", .role = PartRole::region, .composed_from = {}},
     .member = &Ribbon::GroupParts::group_region},
    {.declaration = {.name = "group_caption", .role = PartRole::caption, .composed_from = {}},
     .member = &Ribbon::GroupParts::group_caption},
    {.declaration = {.name = "group_divider", .role = PartRole::divider, .composed_from = {}},
     .member = &Ribbon::GroupParts::group_divider},
    {.declaration = {.name = "group_launcher", .role = PartRole::actuator, .composed_from = "button",
                     .states = launcher_states},
     .member = &Ribbon::GroupParts::group_launcher},
    {.declaration = {.name = "group_launcher_glyph", .role = PartRole::glyph, .composed_from = {},
                     .states = launcher_states},
     .member = &Ribbon::GroupParts::group_launcher_glyph},
    {.declaration = {.name = "group_launcher_focus", .role = PartRole::focus, .composed_from = {},
                     .states = launcher_states},
     .member = &Ribbon::GroupParts::group_launcher_focus},
}};

constexpr std::array<PartField<Ribbon::CommandParts>, 10> ribbon_command_part_fields{{
    {.declaration = {.name = "command_face", .role = PartRole::face, .composed_from = {}},
     .member = &Ribbon::CommandParts::command_face},
    {.declaration = {.name = "command_icon", .role = PartRole::icon, .composed_from = {}},
     .member = &Ribbon::CommandParts::command_icon},
    {.declaration = {.name = "command_label", .role = PartRole::label, .composed_from = {}},
     .member = &Ribbon::CommandParts::command_label},
    {.declaration = {.name = "command_arrow", .role = PartRole::actuator, .composed_from = "button",
                     .states = arrow_states},
     .member = &Ribbon::CommandParts::command_arrow},
    {.declaration = {.name = "command_arrow_glyph", .role = PartRole::glyph, .composed_from = {}},
     .member = &Ribbon::CommandParts::command_arrow_glyph},
    {.declaration = {.name = "command_focus", .role = PartRole::focus, .composed_from = {}},
     .member = &Ribbon::CommandParts::command_focus},
    {.declaration = {.name = "command_combo_box", .role = PartRole::well, .composed_from = "combo_box"},
     .member = &Ribbon::CommandParts::command_combo_box},
    {.declaration = {.name = "command_numeric_up_down", .role = PartRole::well,
                     .composed_from = "numeric_up_down"},
     .member = &Ribbon::CommandParts::command_numeric_up_down},
    {.declaration = {.name = "command_check_box", .role = PartRole::indicator, .composed_from = "check_box"},
     .member = &Ribbon::CommandParts::command_check_box},
    {.declaration = {.name = "command_text_box", .role = PartRole::well, .composed_from = "text_box"},
     .member = &Ribbon::CommandParts::command_text_box},
}};

constexpr std::array<StateField<Ribbon::CommandStates>, 6> ribbon_command_state_fields{{
    {.state = AnatomyState::hot, .member = &Ribbon::CommandStates::hot},
    {.state = AnatomyState::pressed, .member = &Ribbon::CommandStates::pressed},
    {.state = AnatomyState::checked, .member = &Ribbon::CommandStates::checked},
    {.state = AnatomyState::open, .member = &Ribbon::CommandStates::open},
    {.state = AnatomyState::disabled, .member = &Ribbon::CommandStates::disabled},
    {.state = AnatomyState::focus_visible, .member = &Ribbon::CommandStates::focus_visible},
}};

constexpr std::array<PartDeclaration, 6> ribbon_parts = part_declarations(ribbon_part_fields);
constexpr std::array<PartDeclaration, 3> ribbon_tab_parts = part_declarations(ribbon_tab_part_fields);
constexpr std::array<AnatomyState, 3> ribbon_tab_states = state_declarations(ribbon_tab_state_fields);
constexpr std::array<PartDeclaration, 6> ribbon_group_parts = part_declarations(ribbon_group_part_fields);
constexpr std::array<PartDeclaration, 10> ribbon_command_parts = part_declarations(ribbon_command_part_fields);
constexpr std::array<AnatomyState, 6> ribbon_command_states = state_declarations(ribbon_command_state_fields);
constexpr std::array<AnatomyState, 5> ribbon_states{
    AnatomyState::normal, AnatomyState::enabled, AnatomyState::disabled,
    AnatomyState::focused, AnatomyState::focus_visible,
};

constexpr ItemSystemIndex tab_system{0U};
constexpr ItemSystemIndex group_system{1U};
constexpr ItemSystemIndex command_system{2U};

constexpr std::array<ItemSystemDeclaration, 3> ribbon_item_systems{{
    {.name = "tab", .parts = ribbon_tab_parts, .states = ribbon_tab_states, .custom_states = {}},
    {.name = "group", .parts = ribbon_group_parts, .states = {}, .custom_states = {}},
    {.name = "command", .parts = ribbon_command_parts, .states = ribbon_command_states, .custom_states = {}},
}};

constexpr ControlAnatomy ribbon_anatomy{
    .control_id = "ribbon",
    .parts = ribbon_parts,
    .states = ribbon_states,
    .custom_states = {},
    .item_systems = ribbon_item_systems,
};

} // namespace

const ControlAnatomy& Ribbon::anatomy() const noexcept {
    return ribbon_anatomy;
}

void Ribbon::publish_part_geometry(std::span<PartGeometry> geometry) const {
    const Layout placed = layout();
    Parts record;
    record.region = PartGeometry::from_bounds(client_rectangle());
    record.tab_rail = PartGeometry::from_bounds(placed.tab_rail);
    record.application_button = PartGeometry::from_bounds(placed.application);
    const double padding = tab_padding * effective_text_scale();
    record.application_label = PartGeometry::from_bounds(
        {placed.application.x + padding, placed.application.y, std::max(0.0, placed.application.width - padding * 2.0),
         placed.application.height});
    AnatomyStateSet application;
    if (hot_.kind == Hit::Kind::application) application.insert(AnatomyState::hot);
    if (application_pressed_) application.insert(AnatomyState::pressed);
    if (application_open_) application.insert(AnatomyState::open);
    const Window* const host = attached_window();
    if (focused_ && stop_ && (*stop_).kind == StopKind::application && host != nullptr &&
        (*host).focus_cue_visible()) {
        application.insert(AnatomyState::focus_visible);
    }
    record.application_button.states = application;
    record.application_label.states = application;
    record.command_plane = PartGeometry::from_bounds(placed.plane);
    record.focus = PartGeometry::from_bounds(client_rectangle());
    write_parts(anatomy(), ribbon_part_fields, record, geometry);
}

Ribbon::Parts Ribbon::parts() const {
    const PartGeometryBuffer published = published_part_geometry();
    Parts record;
    read_parts(anatomy(), ribbon_part_fields, published.parts(), record);
    return record;
}

// The realized items in paint order: the tabs, then the selected tab's
// visible groups, then their visible commands.
Ribbon::RealizedItem Ribbon::locate_realized(const Layout& placed, std::size_t item) const {
    RealizedItem located;
    if (item < tabs_.size()) {
        located.kind = RealizedItem::Kind::tab;
        located.index = item;
        return located;
    }
    if (placed.plane.empty() || !selected_tab_) return located;
    std::size_t seen = tabs_.size();
    const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
    for (std::size_t group = 0; group < groups.size(); ++group) {
        if (Rect::intersection(placed.groups[group], placed.plane).empty()) continue;
        if (seen == item) {
            located.kind = RealizedItem::Kind::group;
            located.index = group;
            return located;
        }
        ++seen;
    }
    for (std::size_t group = 0; group < groups.size(); ++group) {
        if (Rect::intersection(placed.groups[group], placed.plane).empty()) continue;
        for (std::size_t command = 0; command < groups[group].commands.size(); ++command) {
            if (Rect::intersection(placed.commands[group][command], placed.plane).empty()) continue;
            if (seen == item) {
                located.kind = RealizedItem::Kind::command;
                located.command = CommandAddress{group, command};
                return located;
            }
            ++seen;
        }
    }
    return located;
}

std::size_t Ribbon::realized_item_count() const {
    const Layout placed = layout();
    std::size_t count = tabs_.size();
    if (placed.plane.empty() || !selected_tab_) return count;
    for (std::size_t group = 0; group < placed.groups.size(); ++group) {
        if (Rect::intersection(placed.groups[group], placed.plane).empty()) continue;
        ++count;
        for (std::size_t command = 0; command < placed.commands[group].size(); ++command) {
            if (!Rect::intersection(placed.commands[group][command], placed.plane).empty()) ++count;
        }
    }
    return count;
}

std::string Ribbon::realized_item_stable_id(std::size_t item) const {
    const RealizedItem located = locate_realized(layout(), item);
    const std::string ribbon_id(stable_id().value());
    if (located.kind == RealizedItem::Kind::tab) return node_identity(ribbon_id, "tab", tabs_[located.index].stable_id);
    if (located.kind == RealizedItem::Kind::group) {
        return node_identity(ribbon_id, "group", (*selected_group(located.index)).stable_id);
    }
    if (located.kind == RealizedItem::Kind::command) {
        return node_identity(ribbon_id, "command", (*selected_command(located.command)).stable_id);
    }
    return {};
}

ItemSystemIndex Ribbon::publish_realized_item_system(std::size_t item) const {
    const RealizedItem located = locate_realized(layout(), item);
    if (located.kind == RealizedItem::Kind::group) return group_system;
    if (located.kind == RealizedItem::Kind::command) return command_system;
    return tab_system;
}

void Ribbon::publish_realized_item_geometry(std::size_t item, std::span<PartGeometry> geometry) const {
    const Layout placed = layout();
    const RealizedItem located = locate_realized(placed, item);
    if (located.kind == RealizedItem::Kind::tab) {
        const TabParts record = tab_parts(placed, located.index);
        write_item_parts(anatomy().item_systems[tab_system.value], ribbon_tab_part_fields, record, geometry);
    } else if (located.kind == RealizedItem::Kind::group) {
        const GroupParts record = group_parts(placed, located.index);
        write_item_parts(anatomy().item_systems[group_system.value], ribbon_group_part_fields, record, geometry);
    } else if (located.kind == RealizedItem::Kind::command) {
        const CommandParts record = command_parts(placed, located.command);
        write_item_parts(anatomy().item_systems[command_system.value], ribbon_command_part_fields, record, geometry);
    }
}

AnatomyStateSet Ribbon::realized_item_anatomy_states(std::size_t item) const {
    const RealizedItem located = locate_realized(layout(), item);
    if (located.kind == RealizedItem::Kind::tab) return tab_states(located.index);
    if (located.kind == RealizedItem::Kind::command) return command_states(located.command);
    return AnatomyStateSet{};
}

std::vector<Ribbon::Tab> Ribbon::realized_tabs() const {
    const Layout placed = layout();
    std::vector<Tab> records;
    records.reserve(tabs_.size());
    for (std::size_t tab = 0; tab < tabs_.size(); ++tab) {
        Tab record;
        record.index = tab;
        record.stable_id = tabs_[tab].stable_id;
        record.parts = tab_parts(placed, tab);
        read_states(ribbon_tab_state_fields, tab_states(tab), record.states);
        records.push_back(record);
    }
    return records;
}

std::vector<Ribbon::Group> Ribbon::realized_groups() const {
    const Layout placed = layout();
    std::vector<Group> records;
    if (placed.plane.empty() || !selected_tab_) return records;
    const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
    for (std::size_t group = 0; group < groups.size(); ++group) {
        if (Rect::intersection(placed.groups[group], placed.plane).empty()) continue;
        Group record;
        record.index = group;
        record.stable_id = groups[group].stable_id;
        record.parts = group_parts(placed, group);
        records.push_back(record);
    }
    return records;
}

std::vector<Ribbon::CommandItem> Ribbon::realized_commands() const {
    const Layout placed = layout();
    std::vector<CommandItem> records;
    if (placed.plane.empty() || !selected_tab_) return records;
    const std::vector<RibbonGroupSpec>& groups = tabs_[*selected_tab_].groups;
    for (std::size_t group = 0; group < groups.size(); ++group) {
        if (Rect::intersection(placed.groups[group], placed.plane).empty()) continue;
        for (std::size_t command = 0; command < groups[group].commands.size(); ++command) {
            if (Rect::intersection(placed.commands[group][command], placed.plane).empty()) continue;
            const CommandAddress address{group, command};
            CommandItem record;
            record.group = group;
            record.index = command;
            record.stable_id = groups[group].commands[command].stable_id;
            record.parts = command_parts(placed, address);
            read_states(ribbon_command_state_fields, command_states(address), record.states);
            records.push_back(record);
        }
    }
    return records;
}

namespace {

// The events Ribbon adds to those of Control, in declaration order.
constexpr std::array<InteractionDeclaration, 6> ribbon_interactions{{
    {.name = "command_clicked", .payload = "RibbonCommandClick", .mutable_payload = false},
    {.name = "drop_down_item_invoked", .payload = "RibbonDropDownInvocation", .mutable_payload = false},
    {.name = "dialog_launcher_clicked", .payload = "RibbonDialogLaunch", .mutable_payload = false},
    {.name = "selected_tab_changed", .payload = "RibbonTabChange", .mutable_payload = false},
    {.name = "minimized_changed", .payload = "bool", .mutable_payload = false},
    {.name = "application_menu_item_invoked", .payload = "MenuItemInvocation", .mutable_payload = false},
}};

} // namespace

void Ribbon::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    Control::append_interactions(interactions);
    interactions.insert(interactions.end(), ribbon_interactions.begin(), ribbon_interactions.end());
}

} // namespace gui_forms
