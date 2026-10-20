// The ribbon's file area (stage RB, phase 6; owner decisions 3, 23, 26): the
// application menu's recent pane, shown as a two-pane menu below the File
// tab, and Backstage, a BackstageView over everything below the tab row.

#include "gui_forms/controls/ribbon/ribbon.hpp"

#include "ribbon_application_menu.hpp"

#include "gui_forms/delegate.hpp"
#include "gui_forms/text.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <utility>

namespace gui_forms {

bool Ribbon::file_area_open() const noexcept {
    const bool result = application_open_ || application_popup_open_ || backstage_open_;
    return result;
}

// -- recent items --------------------------------------------------------------

// A two-pane menu takes command and separator items only (no checks, radio
// items or submenus beside the recent pane).
void Ribbon::validate_two_pane(const std::vector<MenuItemSpec>& items, std::size_t recent) const {
    if (recent == 0U) return;
    for (const MenuItemSpec& item : items) {
        if (item.kind != MenuItemKind::command && item.kind != MenuItemKind::separator) {
            throw std::invalid_argument("Ribbon application menus beside recent items take commands and separators only");
        }
    }
}

void Ribbon::set_recent_items(std::vector<RibbonRecentItem> items) {
    require_mutable();
    if (items.size() > maximum_recent_items) {
        throw std::invalid_argument("Ribbon has more recent items than maximum_recent_items");
    }
    std::unordered_set<std::string> identities{};
    for (const RibbonRecentItem& item : items) {
        const bool single_line = item.text.find('\n') == std::string::npos &&
                                 item.description.find('\n') == std::string::npos;
        if (item.stable_id.empty() || !validate_utf8(item.stable_id).valid() || !validate_utf8(item.text).valid() ||
            !validate_utf8(item.description).valid() || !single_line || !identities.insert(item.stable_id).second) {
            throw std::invalid_argument("Ribbon recent items require unique IDs and single-line UTF-8");
        }
    }
    validate_two_pane(application_menu_, items.size());
    close_application_popup();
    recent_items_ = std::move(items);
    invalidate(Dirty::semantics);
}

void Ribbon::set_recent_caption(std::string caption) {
    require_mutable();
    if (!validate_utf8(caption).valid()) throw std::invalid_argument("Ribbon recent caption must be valid UTF-8");
    recent_caption_ = std::move(caption);
    if (application_popup_) (*application_popup_).invalidate(Dirty::paint);
}

Control::Ptr Ribbon::application_menu_popup() const noexcept {
    return application_popup_;
}

// Opens the two-pane menu as a popup covering the window client, its panel
// below the File tab, over the ribbon (owner decision 23).
bool Ribbon::show_application_popup(MenuOpenMode mode) {
    Window* const window = attached_window();
    if (window == nullptr) return false;
    if (application_popup_open_) return true;
    close_menus();
    close_group_popup();
    const Control::Ptr self = shared_from_this();
    if (!application_popup_) {
        application_popup_ =
            make_control<RibbonApplicationMenu>(StableId(std::string(stable_id().value()) + ".application_menu"), *this);
    }
    const Layout placed = layout();
    const Rect absolute = absolute_bounds();
    const Size client = (*window).client_size();
    (*application_popup_).set_requested_bounds({0.0, 0.0, client.width, client.height});
    (*application_popup_).set_origin(
        {absolute.x + placed.application.x, absolute.y + placed.application.y + placed.application.height});
    (*application_popup_).reset(mode == MenuOpenMode::keyboard);
    application_popup_token_ = (*window).open_popup(self, application_popup_);
    application_popup_open_ = true;
    if (Event<>* closed = application_popup_token_.closed_event()) {
        application_popup_revoked_ =
            (*closed).subscribe(*this, Delegate<>::bind<Ribbon, &Ribbon::on_application_popup_revoked>(*this));
    }
    application_scope_ = (*window).begin_focus_scope(application_popup_, application_popup_);
    invalidate(Dirty::paint | Dirty::semantics);
    return true;
}

void Ribbon::close_application_popup() noexcept {
    if (!application_popup_open_) return;
    application_popup_open_ = false;
    Window* const window = attached_window();
    if (window != nullptr && application_scope_) {
        try {
            static_cast<void>((*window).end_focus_scope(application_scope_));
        } catch (...) {
        }
    }
    application_scope_ = {};
    application_popup_revoked_.disconnect();
    application_popup_token_.disconnect();
    if (!is_alive()) return;
    try {
        invalidate(Dirty::paint | Dirty::semantics);
    } catch (...) {
    }
}

void Ribbon::on_application_popup_revoked() {
    if (!application_popup_open_) return;
    application_popup_open_ = false;
    application_scope_ = {};
    application_popup_revoked_.disconnect();
    invalidate(Dirty::paint | Dirty::semantics);
}

// -- Backstage -----------------------------------------------------------------

void Ribbon::set_backstage(std::shared_ptr<BackstageView> view) {
    require_mutable();
    if (backstage_ == view) return;
    if (view && (*view).parent()) throw std::invalid_argument("Ribbon Backstage view already has a parent");
    close_backstage();
    backstage_close_.disconnect();
    backstage_ = std::move(view);
    if (backstage_) {
        backstage_close_ = (*backstage_).close_requested().subscribe(
            *this, Delegate<>::bind<Ribbon, &Ribbon::on_backstage_close_requested>(*this));
    }
    invalidate(Dirty::semantics);
}

// Backstage covers the window from the bottom of the ribbon's tab row to the
// client's bottom edge, the full width; the tab row stays usable above it.
void Ribbon::place_backstage() {
    Window* const window = attached_window();
    if (window == nullptr || !backstage_) return;
    const Rect absolute = absolute_bounds();
    const Size client = (*window).client_size();
    const double top = std::min(client.height, absolute.y + tab_row_height());
    (*backstage_).set_requested_bounds({0.0, top, client.width, std::max(0.0, client.height - top)});
}

bool Ribbon::show_backstage() {
    require_mutable();
    Window* const window = attached_window();
    if (!backstage_ || window == nullptr || !effectively_enabled()) return false;
    if (backstage_open_) return true;
    close_menus();
    close_group_popup();
    place_backstage();
    backstage_token_ = (*window).open_popup(shared_from_this(), backstage_);
    backstage_open_ = true;
    if (Event<>* closed = backstage_token_.closed_event()) {
        backstage_revoked_ = (*closed).subscribe(*this, Delegate<>::bind<Ribbon, &Ribbon::on_backstage_revoked>(*this));
    }
    backstage_scope_ = (*window).begin_focus_scope(backstage_, backstage_);
    invalidate(Dirty::paint | Dirty::semantics);
    backstage_open_changed_.emit(true);
    return true;
}

void Ribbon::close_backstage() noexcept {
    if (!backstage_open_) return;
    backstage_open_ = false;
    Window* const window = attached_window();
    if (window != nullptr && backstage_scope_) {
        try {
            static_cast<void>((*window).end_focus_scope(backstage_scope_));
        } catch (...) {
        }
    }
    backstage_scope_ = {};
    backstage_revoked_.disconnect();
    backstage_token_.disconnect();
    if (!is_alive()) return;
    try {
        invalidate(Dirty::paint | Dirty::semantics);
        backstage_open_changed_.emit(false);
    } catch (...) {
    }
}

void Ribbon::on_backstage_close_requested() {
    close_backstage();
}

void Ribbon::on_backstage_revoked() {
    if (!backstage_open_) return;
    backstage_open_ = false;
    backstage_scope_ = {};
    backstage_revoked_.disconnect();
    invalidate(Dirty::paint | Dirty::semantics);
    backstage_open_changed_.emit(false);
}

} // namespace gui_forms
