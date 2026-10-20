#pragma once

// One window's semantic tree as the Windows accessibility publishers see it:
// how a native object names its node, how the node is found again in a
// fresh snapshot, and how an action reaches the retained control. Shared by
// the MSAA objects (windows_accessibility.cpp). Win32 types stay inside
// src/host/windows/.
//
// Execution context: every member is used on the window's UI thread, the
// one owner of the semantic tree. MSAA calls from a client in another
// process arrive through WM_GETOBJECT and COM's apartment marshalling, which
// delivers them on this thread; in-process calls from this thread arrive
// directly.

#include "gui_forms/window.hpp"

#include <windows.h>

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace gui_forms::host {

// How a native object names its semantic node: the runtime ID of the
// nearest control-backed node at or above it (its scope) and its own
// runtime ID. A control's runtime ID is unique within the process and never
// reused, so the identity of a disposed control resolves to nothing, and a
// stale object is refused rather than reaching another control. A virtual
// child's runtime ID comes from its stable ID, which is unique within the
// control that publishes it but not within the window, so it is looked for
// only below its scope. The window's client object is {0, 0}; a virtual
// node with no control-backed node above it has scope 0.
struct SemanticElementId final {
    std::uint64_t scope{};
    std::uint64_t node{};
};

[[nodiscard]] bool operator==(SemanticElementId left, SemanticElementId right) noexcept;
[[nodiscard]] bool operator!=(SemanticElementId left, SemanticElementId right) noexcept;

// A node found in a snapshot. The pointers borrow the snapshot passed to
// the function that filled the location and are valid only while it lives
// unchanged. `parent` is null for the client node; `scope` is the nearest
// control-backed node at or above `node`, or null when there is none.
struct SemanticLocation final {
    const SemanticNode* node{};
    const SemanticNode* parent{};
    const SemanticNode* scope{};
    // The node's position among its parent's children; zero for the client.
    std::size_t index{};
    SemanticElementId id{};
};

// True for a node a control publishes about itself (not a virtual child
// and not the client node).
[[nodiscard]] bool control_backed(const SemanticNode& node) noexcept;

// The client node's location in `client`.
[[nodiscard]] SemanticLocation client_location(const SemanticNode& client) noexcept;
SemanticLocation client_location(SemanticNode&&) = delete;

// The location of child `index` of the node at `parent`; `index` must be
// below its child count.
[[nodiscard]] SemanticLocation child_location(const SemanticLocation& parent,
                                              std::size_t index) noexcept;

// Finds the node `id` names in `client`. Returns false, leaving `location`
// unchanged, when no node of the tree has that identity any more.
[[nodiscard]] bool locate_semantic_element(const SemanticNode& client, SemanticElementId id,
                                           SemanticLocation& location) noexcept;
bool locate_semantic_element(SemanticNode&&, SemanticElementId, SemanticLocation&) = delete;

// The location of `location`'s parent. Returns false for the client node.
[[nodiscard]] bool parent_semantic_location(const SemanticNode& client,
                                            const SemanticLocation& location,
                                            SemanticLocation& parent) noexcept;
bool parent_semantic_location(SemanticNode&&, const SemanticLocation&,
                              SemanticLocation&) = delete;

// The publishers' link to one window. Shared by WindowsAccessibility and
// every native object it hands out; `model` and `hwnd` are cleared when the
// window goes, after which every object resolves to nothing and reports its
// disconnected error. Objects observe the window and never extend its life.
struct SemanticSource final {
    HWND hwnd{};
    Window* model{};
    // Device pixels per logical unit.
    double scale{1.0};
    // The semantic generation last announced to clients.
    std::uint64_t generation{};
    // The runtime ID of the focused node last announced, or zero.
    std::uint64_t focus{};
    // True once a client has asked for an object; events are raised only
    // after that.
    bool observed{};

    // Fills `client` with the window's client node: the snapshot's roots
    // under one node named by the window title and covering the client
    // area. Returns false, with `client` unspecified, when the window has
    // gone or the snapshot failed.
    [[nodiscard]] bool snapshot(SemanticNode& client) noexcept;
    // Performs `action` on the located node through the window, scoped to
    // the node's control so a virtual child reaches its own owner. Returns
    // true when the control took it.
    [[nodiscard]] bool perform(const SemanticLocation& location, SemanticAction action,
                               std::string_view value) noexcept;
};

} // namespace gui_forms::host
