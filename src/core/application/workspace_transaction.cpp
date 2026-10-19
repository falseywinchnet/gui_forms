#include "gui_forms/application/workspace_transaction/workspace_transaction.hpp"
#include "application_state.hpp"

#include <stdexcept>
#include <string>
#include <utility>

namespace gui_forms {
namespace {
// Shows a window through the run's windowing: synchronous on every host and
// without activation, unlike ApplicationWindowHandle::show, which may show
// asynchronously and activate (macOS).
HostServiceStatus show_quietly(const ApplicationWindowHandle& window) {
    const std::shared_ptr<detail::ApplicationWindowState> state =
        detail::ApplicationHandleAccess::state(window);
    if (!state || (*state).closed) return {HostServiceError::after_shutdown};
    const std::shared_ptr<detail::ApplicationRunState> run = (*state).run.lock();
    if (!run || (*run).closed || (*run).windowing == nullptr) return {HostServiceError::after_shutdown};
    const HostServiceStatus status = (*(*run).windowing).set_visible((*state).id, true);
    return status;
}

void keep_first_failure(HostServiceStatus& first, HostServiceStatus status) noexcept {
    if (first.accepted() && !status.accepted()) first = status;
}
} // namespace

WorkspaceTransaction::WorkspaceTransaction(ApplicationWindows windows)
    : windows_(std::move(windows)) {
    // A transaction holds at most one scope per window of the run.
    included_.reserve(HostWindowing::maximum_windows);
    scopes_.reserve(HostWindowing::maximum_windows);
}

WorkspaceTransaction::~WorkspaceTransaction() {
    if (!open_) return;
    try {
        static_cast<void>(commit());
    } catch (...) {
        // A destructor cannot report; the scopes still close as the vector
        // is destroyed, so no window is left suspended.
    }
}

void WorkspaceTransaction::require_open(const char* operation) const {
    if (open_) return;
    std::string message = "GUI.Forms workspace transaction: ";
    message += operation;
    message += " after commit";
    throw std::logic_error(message);
}

void WorkspaceTransaction::include(Window& window) {
    require_open("include");
    for (std::size_t index = 0U; index < included_.size(); ++index) {
        if (included_[index] == &window) return;
    }
    UpdateScope scope = window.begin_update();
    included_.push_back(&window);
    scopes_.push_back(std::move(scope));
}

ApplicationWindowOpenResult WorkspaceTransaction::open_window(ApplicationWindow&& entry) {
    require_open("open_window");
    Window* model = entry.model.get();
    const bool visible = entry.options.initially_visible;
    // Included before the host attaches it, so its first layout is the
    // commit's.
    const std::size_t included_before = included_.size();
    if (model != nullptr && (*model).check_access()) include(*model);
    // Created hidden: nothing appears before the commit has laid it out.
    entry.options.initially_visible = false;
    ApplicationWindowOpenResult result = windows_.open_window(std::move(entry));
    if (!result.status.accepted()) {
        // A refusal before the native request leaves the entry, and its
        // model, with the caller: the scope is closed now, so the
        // transaction holds no model it does not share with a host. A
        // refusal after the host took the entry (a native creation that
        // failed) may already have destroyed the model: that scope is
        // abandoned, never closed, so nothing touches the model again.
        const bool consumed = !entry.model;
        if (!consumed) entry.options.initially_visible = visible;
        if (included_.size() > included_before) {
            if (consumed) {
                scopes_.back().abandon();
            } else {
                scopes_.back().close();
            }
            scopes_.pop_back();
            included_.pop_back();
        }
        return result;
    }
    // The host owns the model now and keeps it until the window closes.
    if (visible) to_show_.push_back(result.window);
    return result;
}

HostServiceStatus WorkspaceTransaction::show_window(const ApplicationWindowHandle& window) {
    require_open("show_window");
    if (!window.active()) return {HostServiceError::after_shutdown};
    to_show_.push_back(window);
    return {};
}

HostServiceStatus WorkspaceTransaction::destroy_window(const ApplicationWindowHandle& window) {
    require_open("destroy_window");
    if (!window.active()) return {HostServiceError::after_shutdown};
    to_destroy_.push_back(window);
    return {};
}

void WorkspaceTransaction::defer_notification(std::function<void()> notification) {
    require_open("defer_notification");
    if (!notification) throw std::invalid_argument("workspace transaction notification is empty");
    notifications_.push_back(std::move(notification));
}

WorkspaceTransactionResult WorkspaceTransaction::commit() {
    WorkspaceTransactionResult result;
    if (!open_) return result;
    open_ = false;
    // 1. Each included window lays out once, as its scope closes.
    for (std::size_t index = 0U; index < scopes_.size(); ++index) {
        scopes_[index].close();
        ++result.windows_laid_out;
    }
    scopes_.clear();
    included_.clear();
    // 2. New and requested windows appear, before anything is destroyed.
    for (std::size_t index = 0U; index < to_show_.size(); ++index) {
        const HostServiceStatus status = show_quietly(to_show_[index]);
        keep_first_failure(result.status, status);
        if (status.accepted()) ++result.windows_shown;
    }
    to_show_.clear();
    // 3. Emptied windows go.
    for (std::size_t index = 0U; index < to_destroy_.size(); ++index) {
        const HostServiceStatus status = windows_.destroy_window(to_destroy_[index]);
        keep_first_failure(result.status, status);
        if (status.accepted()) ++result.windows_destroyed;
    }
    to_destroy_.clear();
    // 4. One batch of notifications. Taken first: a notification may start
    // another transaction, which must not see this queue.
    std::vector<std::function<void()>> notifications = std::move(notifications_);
    notifications_.clear();
    for (std::size_t index = 0U; index < notifications.size(); ++index) {
        notifications[index]();
        ++result.notifications;
    }
    return result;
}

} // namespace gui_forms
