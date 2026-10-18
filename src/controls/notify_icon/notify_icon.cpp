#include "gui_forms/components/notify_icon/notify_icon.hpp"

#include "gui_forms/delegate.hpp"
#include "gui_forms/text.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <utility>

namespace gui_forms {
namespace {

bool valid_text(std::string_view text) {
    const bool result = validate_utf8(text).valid();
    return result;
}

// The source name a menu row's command sees when the icon's menu runs it.
constexpr std::string_view notify_icon_source = "notify_icon";

} // namespace

NotifyIcon::NotifyIcon(Window& window) : window_lifetime_(window.lifetime_) {
    window.verify_access("NotifyIcon construction");
}

NotifyIcon::~NotifyIcon() {
    forget_icon();
}

Window* NotifyIcon::bound_window() const noexcept {
    const std::shared_ptr<detail::WindowLifetime> lifetime = window_lifetime_.lock();
    return lifetime ? (*lifetime).window : nullptr;
}

HostServices* NotifyIcon::current_services() const noexcept {
    Window* const window = bound_window();
    return window != nullptr ? (*window).host_services() : nullptr;
}

void NotifyIcon::require_ui(std::string_view operation) const {
    if (!is_alive()) throw std::logic_error("GUI.Forms cannot use a disposed NotifyIcon");
    Window* const window = bound_window();
    if (window == nullptr) throw std::logic_error("GUI.Forms cannot use a NotifyIcon after Window shutdown");
    (*window).verify_access(operation);
}

bool NotifyIcon::supported() const noexcept {
    HostServices* const services = current_services();
    if (services == nullptr) return false;
    const bool result = (*services).snapshot().capabilities.supports(HostCapability::notification_area);
    return result;
}

bool NotifyIcon::shown() const noexcept {
    // The icon is shown while the window still has the services that showed
    // it and they still hold its identity; a session that ended took the
    // icon with it.
    if (services_ == nullptr || current_services() != services_ || !tray_events_.connected()) return false;
    const bool result = (*services_).tray_icon_live(icon_id_);
    return result;
}

HostTrayIconRequest NotifyIcon::request() const {
    HostTrayIconRequest built;
    built.images = images_;
    built.tooltip = text_;
    return built;
}

void NotifyIcon::set_icon(std::vector<HostImage> images) {
    require_ui("NotifyIcon icon mutation");
    if (images.size() > HostServices::maximum_tray_icon_images) {
        throw std::invalid_argument("NotifyIcon takes at most 8 icon images");
    }
    for (const HostImage& image : images) {
        if (validate_host_image(image.view()) != HostImageError::none ||
            image.width > HostServices::maximum_tray_icon_side || image.height > HostServices::maximum_tray_icon_side) {
            throw std::invalid_argument("NotifyIcon images must be valid and at most 256 pixels a side");
        }
    }
    images_ = std::move(images);
    status_ = synchronize(true);
}

void NotifyIcon::set_text(std::string text) {
    require_ui("NotifyIcon text mutation");
    if (text.size() > HostServices::maximum_tray_tooltip_bytes || !valid_text(text)) {
        throw std::invalid_argument("NotifyIcon text must be valid UTF-8 of at most 127 bytes");
    }
    if (text_ == text) return;
    text_ = std::move(text);
    status_ = synchronize(true);
}

HostServiceStatus NotifyIcon::set_visible(bool visible) {
    require_ui("NotifyIcon visibility");
    visible_ = visible;
    status_ = synchronize(false);
    return status_;
}

void NotifyIcon::set_context_menu(std::shared_ptr<ContextMenu> menu) {
    require_ui("NotifyIcon menu mutation");
    context_menu_ = std::move(menu);
}

// Leaves the icon's host state, removing a shown icon. Never throws.
void NotifyIcon::forget_icon() noexcept {
    if (shown()) {
        const HostServiceStatus removed = (*services_).remove_tray_icon(icon_id_);
        static_cast<void>(removed);
    }
    tray_events_.disconnect();
    services_ = nullptr;
    icon_id_ = 0U;
}

HostServiceStatus NotifyIcon::synchronize(bool content_changed) {
    const bool live = shown();
    if (!live && services_ != nullptr) forget_icon();
    if (!visible_) {
        forget_icon();
        return {};
    }
    HostServices* const services = current_services();
    if (services == nullptr) return {HostServiceError::unsupported};
    if (images_.empty()) {
        forget_icon();
        return {HostServiceError::invalid_argument};
    }
    if (live) {
        if (!content_changed) return {};
        const HostServiceStatus updated = (*services).update_tray_icon(icon_id_, request());
        return updated;
    }
    const HostTrayIconResult added = (*services).add_tray_icon(request());
    if (!added.status.accepted()) return added.status;
    services_ = services;
    icon_id_ = added.icon_id;
    // The token, not this component's disposal, owns the connection: it
    // stays connected until forget_icon, so a disposing icon can still tell
    // that its services hold it. Disconnection also reports that the
    // services' event (and the services) are gone.
    tray_events_ = (*services).tray_icon_events().subscribe(
        Delegate<const HostTrayIconEvent&>::bind<NotifyIcon, &NotifyIcon::on_tray_event>(*this));
    return added.status;
}

HostServiceStatus NotifyIcon::show_balloon_tip(std::string title, std::string text, HostTrayNotificationIcon icon,
                                               std::chrono::milliseconds timeout) {
    require_ui("NotifyIcon notification");
    HostServices* const services = current_services();
    if (services == nullptr) {
        status_ = {HostServiceError::unsupported};
        return status_;
    }
    if (!(*services).snapshot().capabilities.supports(HostCapability::notification_area_messages)) {
        status_ = {HostServiceError::unsupported};
        return status_;
    }
    if (!shown()) {
        status_ = {HostServiceError::invalid_argument};
        return status_;
    }
    HostTrayNotificationRequest notification;
    notification.title = std::move(title);
    notification.text = std::move(text);
    notification.icon = icon;
    // At most ten minutes; zero or less is the host's default.
    const long long milliseconds = timeout.count();
    const long long bounded = std::clamp<long long>(milliseconds, 0, 600000);
    notification.timeout_milliseconds = static_cast<std::uint32_t>(bounded);
    status_ = (*services).show_tray_notification(icon_id_, notification);
    return status_;
}

void NotifyIcon::on_tray_event(const HostTrayIconEvent& event) {
    if (!is_alive() || event.icon_id != icon_id_ || current_services() != services_) return;
    if (event.action == HostTrayIconAction::click) {
        const NotifyIconClick clicked{PointerButton::primary};
        click_.emit(clicked);
    } else if (event.action == HostTrayIconAction::double_click) {
        double_click_.emit();
    } else if (event.action == HostTrayIconAction::context) {
        const NotifyIconClick clicked{PointerButton::secondary};
        click_.emit(clicked);
        if (is_alive()) show_menu();
    } else if (event.action == HostTrayIconAction::notification_clicked) {
        balloon_tip_clicked_.emit();
    } else if (event.action == HostTrayIconAction::notification_closed) {
        balloon_tip_closed_.emit();
    }
}

// Shows the context menu's rows natively, then runs the chosen row's command.
void NotifyIcon::show_menu() {
    if (!context_menu_ || !shown()) return;
    HostServices& services = *services_;
    if (!services.snapshot().capabilities.supports(HostCapability::notification_area_menus)) return;
    const std::vector<MenuItemSpec>& specs = (*context_menu_).items();
    HostTrayMenuRequest menu;
    menu.items.reserve(specs.size());
    for (const MenuItemSpec& spec : specs) {
        HostTrayMenuItem item;
        if (spec.kind == MenuItemKind::separator) {
            item.separator = true;
        } else if (spec.kind == MenuItemKind::submenu || !spec.command) {
            continue;
        } else {
            const CommandState& state = (*spec.command).state();
            if (!state.visible) continue;
            item.id = spec.stable_id;
            item.text = spec.text.empty() ? state.text : spec.text;
            item.enabled = state.enabled;
            item.checked = spec.kind != MenuItemKind::command && state.checked;
        }
        menu.items.push_back(std::move(item));
        if (menu.items.size() == HostServices::maximum_tray_menu_items) break;
    }
    if (menu.items.empty()) return;
    const HostTrayMenuResult chosen = services.show_tray_menu(icon_id_, menu);
    status_ = chosen.status;
    if (!chosen.status.accepted() || chosen.chosen_id.empty() || !is_alive() || !context_menu_) return;
    for (const MenuItemSpec& spec : (*context_menu_).items()) {
        if (spec.stable_id != chosen.chosen_id || !spec.command) continue;
        const std::shared_ptr<Command> command = spec.command;
        // Through the bound window's command chain (owner decision DK-47).
        Window* const routing = bound_window();
        bool executed = false;
        if (routing != nullptr) {
            executed = (*command).execute_routed(*routing, notify_icon_source);
        } else {
            executed = (*command).execute(notify_icon_source);
        }
        if (!executed || !is_alive()) return;
        const MenuItemInvocation invocation{(*context_menu_).stable_id(), spec.stable_id, (*command).stable_id(),
                                            std::string(notify_icon_source)};
        menu_item_invoked_.emit(invocation);
        return;
    }
}

void NotifyIcon::verify_dispose_thread() {
    if (Window* const window = bound_window()) (*window).verify_access("NotifyIcon disposal");
}

void NotifyIcon::on_dispose() noexcept {
    forget_icon();
    visible_ = false;
    window_lifetime_.reset();
}

namespace {

// The events NotifyIcon adds to those of Component, in declaration order.
constexpr std::array<InteractionDeclaration, 5> notify_icon_interactions{{
    {.name = "click", .payload = "NotifyIconClick", .mutable_payload = false},
    {.name = "double_click", .payload = "", .mutable_payload = false},
    {.name = "balloon_tip_clicked", .payload = "", .mutable_payload = false},
    {.name = "balloon_tip_closed", .payload = "", .mutable_payload = false},
    {.name = "menu_item_invoked", .payload = "MenuItemInvocation", .mutable_payload = false},
}};

} // namespace

void NotifyIcon::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    Component::append_interactions(interactions);
    interactions.insert(interactions.end(), notify_icon_interactions.begin(), notify_icon_interactions.end());
}

} // namespace gui_forms
