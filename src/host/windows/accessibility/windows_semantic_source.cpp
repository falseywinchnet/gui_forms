#include "windows_semantic_source.hpp"

#include <exception>
#include <string>
#include <utility>
#include <vector>

namespace gui_forms::host {

namespace {

// Which nodes a search passes through: control-backed nodes only (looking
// for a scope; a control-backed node never lies below a virtual one), or
// virtual nodes only (looking for a virtual child below its scope; a
// control-backed node there has its own scope).
enum class SearchKind : std::uint8_t { controls, virtual_children };

bool search_children(const SemanticLocation& at, std::uint64_t runtime_id, SearchKind kind,
                     SemanticLocation& found) noexcept {
    const std::vector<SemanticNode>& children = (*at.node).children;
    for (std::size_t index = 0; index < children.size(); ++index) {
        const SemanticNode& child = children[index];
        const bool backed = control_backed(child);
        if (kind == SearchKind::controls && !backed) continue;
        if (kind == SearchKind::virtual_children && backed) continue;
        const SemanticLocation child_at = child_location(at, index);
        if (child.runtime_id == runtime_id) {
            found = child_at;
            return true;
        }
        if (search_children(child_at, runtime_id, kind, found)) return true;
    }
    return false;
}

// The window title as UTF-8, or empty.
std::string window_title(HWND hwnd) {
    const int length = GetWindowTextLengthW(hwnd);
    if (length <= 0) return {};
    std::wstring wide(static_cast<std::size_t>(length) + 1U, L'\0');
    const int copied = GetWindowTextW(hwnd, wide.data(), length + 1);
    if (copied <= 0) return {};
    const int bytes = WideCharToMultiByte(CP_UTF8, 0, wide.data(), copied, nullptr, 0,
                                          nullptr, nullptr);
    if (bytes <= 0) return {};
    std::string title(static_cast<std::size_t>(bytes), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wide.data(), copied, title.data(), bytes, nullptr, nullptr);
    return title;
}

} // namespace

bool operator==(SemanticElementId left, SemanticElementId right) noexcept {
    const bool equal = left.scope == right.scope && left.node == right.node;
    return equal;
}

bool operator!=(SemanticElementId left, SemanticElementId right) noexcept {
    const bool different = !(left == right);
    return different;
}

bool control_backed(const SemanticNode& node) noexcept {
    const bool backed = node.runtime_id != 0U && !is_virtual_semantic_runtime_id(node.runtime_id);
    return backed;
}

SemanticLocation client_location(const SemanticNode& client) noexcept {
    SemanticLocation location;
    location.node = &client;
    return location;
}

SemanticLocation child_location(const SemanticLocation& parent, std::size_t index) noexcept {
    const SemanticNode& child = (*parent.node).children[index];
    SemanticLocation location;
    location.node = &child;
    location.parent = parent.node;
    location.index = index;
    if (control_backed(child)) {
        location.scope = &child;
        location.id = SemanticElementId{child.runtime_id, child.runtime_id};
    } else {
        location.scope = parent.scope;
        const std::uint64_t scope = parent.scope == nullptr ? 0U : (*parent.scope).runtime_id;
        location.id = SemanticElementId{scope, child.runtime_id};
    }
    return location;
}

bool locate_semantic_element(const SemanticNode& client, SemanticElementId id,
                             SemanticLocation& location) noexcept {
    const SemanticLocation client_at = client_location(client);
    if (id.node == 0U) {
        if (id.scope != 0U) return false;
        location = client_at;
        return true;
    }
    SemanticLocation scope_at = client_at;
    if (id.scope != 0U && !search_children(client_at, id.scope, SearchKind::controls, scope_at)) {
        return false;
    }
    if (id.node == id.scope) {
        location = scope_at;
        return true;
    }
    SemanticLocation found;
    if (!search_children(scope_at, id.node, SearchKind::virtual_children, found)) return false;
    location = found;
    return true;
}

bool parent_semantic_location(const SemanticNode& client, const SemanticLocation& location,
                              SemanticLocation& parent) noexcept {
    if (location.parent == nullptr) return false;
    const SemanticNode& node = *location.parent;
    SemanticElementId id;
    if (location.parent == &client) {
        id = SemanticElementId{};
    } else if (control_backed(node)) {
        id = SemanticElementId{node.runtime_id, node.runtime_id};
    } else {
        // A virtual parent shares its child's scope: no control-backed node
        // lies between them.
        const std::uint64_t scope = location.scope == nullptr ? 0U : (*location.scope).runtime_id;
        id = SemanticElementId{scope, node.runtime_id};
    }
    const bool found = locate_semantic_element(client, id, parent);
    return found;
}

bool SemanticSource::snapshot(SemanticNode& client) noexcept {
    if (model == nullptr || hwnd == nullptr) return false;
    try {
        SemanticSnapshot tree = (*model).semantic_snapshot();
        client.runtime_id = 0U;
        client.stable_id.clear();
        client.role = SemanticRole::group;
        client.children = std::move(tree.roots);
        client.states = SemanticState::enabled | SemanticState::visible;
        client.name = window_title(hwnd);
        RECT bounds{};
        GetClientRect(hwnd, &bounds);
        client.bounds = Rect{0.0, 0.0, bounds.right / scale, bounds.bottom / scale};
        return true;
    } catch (...) {
        return false;
    }
}

bool SemanticSource::perform(const SemanticLocation& location, SemanticAction action,
                             std::string_view value) noexcept {
    // The client node takes no action of its own.
    if (model == nullptr || (*location.node).runtime_id == 0U) return false;
    try {
        if (action == SemanticAction::focus) SetFocus(hwnd);
        const SemanticNode& node = *location.node;
        bool handled = false;
        if (location.scope == nullptr) {
            // A virtual child with no control-backed node above it: no
            // control names it, so the window looks for it by stable ID
            // under its root and popups.
            handled = (*model).perform_semantic_action(node.stable_id, action, value);
        } else {
            // The window refuses a scope whose control was disposed or
            // replaced since the snapshot (interview GF017).
            const RuntimeId scope{(*location.scope).runtime_id};
            std::string_view virtual_child;
            if (location.scope != location.node) virtual_child = node.stable_id;
            handled = (*model).perform_semantic_action(scope, virtual_child, action, value);
        }
        if (handled) NotifyWinEvent(EVENT_OBJECT_VALUECHANGE, hwnd, OBJID_CLIENT, CHILDID_SELF);
        return handled;
    } catch (...) {
        return false;
    }
}

} // namespace gui_forms::host
