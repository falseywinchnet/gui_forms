#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace gui_forms::gallery::dml {

enum class NodeKind {
    form,
    command_strip,
    panel,
    user_control,
    backplane,
    group,
    label,
    link_label,
    button,
    text_input,
    check_box,
    radio_button,
    slider,
    progress,
    list,
    row,
    instrument,
    diagnostics,
};

enum class LayoutKind {
    none,
    flex_row,
    flex_column,
    stack,
    form_grid,
};

enum NodeFlags : unsigned {
    no_flags = 0U,
    enabled = 1U << 0U,
    visible = 1U << 1U,
    focusable = 1U << 2U,
    checked = 1U << 3U,
    selected = 1U << 4U,
    default_action = 1U << 5U,
    transparent = 1U << 6U,
};

struct NodeSpec final {
    std::string_view id;
    std::string_view parent_id;
    NodeKind kind;
    LayoutKind layout;
    std::string_view text;
    unsigned flags;
    int order;
    int preferred_width;
    int preferred_height;
    double minimum;
    double maximum;
    double value;
    // Set on direct children of gallery.root that are compiled-DML pages: the
    // Gallery page id (demo/gallery_page_specs.hpp) whose body mounts the node.
    std::string_view page;
};

inline constexpr unsigned ordinary = enabled | visible;
inline constexpr unsigned interactive = ordinary | focusable;

inline constexpr std::array<NodeSpec, 41> gallery_nodes {{
    {"gallery.root", "", NodeKind::form, LayoutKind::flex_column, "GUI.Forms Gallery", ordinary, 0, 1280, 820, 0.0, 0.0, 0.0},
    {"gallery.command-strip", "gallery.root", NodeKind::command_strip, LayoutKind::flex_row, "", ordinary, 0, 0, 34, 0.0, 0.0, 0.0},
    {"gallery.command.reset", "gallery.command-strip", NodeKind::button, LayoutKind::none, "Reset", interactive, 0, 72, 24, 0.0, 0.0, 0.0},
    {"gallery.command.diagnostics", "gallery.command-strip", NodeKind::button, LayoutKind::none, "Diagnostics", interactive, 1, 96, 24, 0.0, 0.0, 0.0},
    {"gallery.command.status", "gallery.command-strip", NodeKind::label, LayoutKind::none, "Portable core · Host 0.4 · drop-ready", ordinary, 2, 0, 22, 0.0, 0.0, 0.0},
    {"gallery.backplane", "gallery.root", NodeKind::backplane, LayoutKind::stack, "gallery-fresco", ordinary | transparent, 1, 660, 200, 0.0, 0.0, 0.0, "basics"},
    {"gallery.basics-group", "gallery.backplane", NodeKind::group, LayoutKind::form_grid, "Basic controls", ordinary | transparent, 0, 0, 0, 0.0, 0.0, 0.0},
    {"gallery.intro", "gallery.basics-group", NodeKind::label, LayoutKind::none, "Professional retained controls with deterministic native behavior.", ordinary | transparent, 0, 0, 22, 0.0, 0.0, 0.0},
    {"gallery.text-input", "gallery.basics-group", NodeKind::text_input, LayoutKind::none, "Edit this text", interactive, 1, 250, 28, 0.0, 0.0, 0.0},
    {"gallery.default-button", "gallery.basics-group", NodeKind::button, LayoutKind::none, "Apply", interactive | default_action, 2, 92, 28, 0.0, 0.0, 0.0},
    {"gallery.disabled-button", "gallery.basics-group", NodeKind::button, LayoutKind::none, "Unavailable", visible | focusable, 3, 108, 28, 0.0, 0.0, 0.0},
    {"gallery.checkbox", "gallery.basics-group", NodeKind::check_box, LayoutKind::none, "Enable precise updates", interactive | checked, 4, 220, 24, 0.0, 0.0, 1.0},
    {"gallery.radio.classic", "gallery.basics-group", NodeKind::radio_button, LayoutKind::none, "Classic relief", interactive | checked, 5, 180, 24, 0.0, 0.0, 1.0},
    {"gallery.radio.quiet", "gallery.basics-group", NodeKind::radio_button, LayoutKind::none, "Quiet relief", interactive, 6, 180, 24, 0.0, 0.0, 0.0},
    {"gallery.link", "gallery.basics-group", NodeKind::link_label, LayoutKind::none, "Show control contract", interactive, 7, 170, 24, 0.0, 0.0, 0.0},
    {"gallery.checkbox.indeterminate", "gallery.basics-group", NodeKind::check_box, LayoutKind::none, "Three-state option", interactive, 8, 220, 24, 0.0, 0.0, 0.0},
    {"gallery.lifecycle-card", "gallery.basics-group", NodeKind::user_control, LayoutKind::flex_row, "", ordinary, 9, 0, 24, 0.0, 0.0, 0.0},
    {"gallery.lifecycle-title", "gallery.lifecycle-card", NodeKind::label, LayoutKind::none, "COMPOSED", ordinary, 0, 82, 20, 0.0, 0.0, 0.0},
    {"gallery.lifecycle-status", "gallery.lifecycle-card", NodeKind::label, LayoutKind::none, "Awaiting attach", ordinary, 1, 0, 20, 0.0, 0.0, 0.0},
    {"gallery.values-group", "gallery.root", NodeKind::group, LayoutKind::stack, "Values and retained state", ordinary, 2, 660, 116, 0.0, 0.0, 0.0, "values"},
    {"gallery.slider", "gallery.values-group", NodeKind::slider, LayoutKind::none, "", interactive, 0, 250, 28, 0.0, 100.0, 42.0},
    {"gallery.progress", "gallery.values-group", NodeKind::progress, LayoutKind::none, "", ordinary, 1, 250, 20, 0.0, 100.0, 42.0},
    {"gallery.value-label", "gallery.values-group", NodeKind::label, LayoutKind::none, "42%", ordinary, 2, 64, 22, 0.0, 0.0, 42.0},
    {"gallery.collection-group", "gallery.root", NodeKind::group, LayoutKind::stack, "Retained collection", ordinary, 3, 660, 144, 0.0, 0.0, 0.0, "retained-rows"},
    {"gallery.collection", "gallery.collection-group", NodeKind::list, LayoutKind::stack, "", interactive, 0, 300, 112, 0.0, 0.0, 0.0},
    {"gallery.collection.alpha", "gallery.collection", NodeKind::row, LayoutKind::none, "Alpha channel", interactive | selected, 0, 0, 26, 0.0, 0.0, 0.0},
    {"gallery.collection.beta", "gallery.collection", NodeKind::row, LayoutKind::none, "Beta channel", interactive, 1, 0, 26, 0.0, 0.0, 0.0},
    {"gallery.collection.gamma", "gallery.collection", NodeKind::row, LayoutKind::none, "Gamma channel", interactive, 2, 0, 26, 0.0, 0.0, 0.0},
    {"gallery.collection.delta", "gallery.collection", NodeKind::row, LayoutKind::none, "Delta channel", interactive, 3, 0, 26, 0.0, 0.0, 0.0},
    {"gallery.instrument-group", "gallery.root", NodeKind::group, LayoutKind::stack, "Custom instrument", ordinary, 4, 660, 150, 0.0, 0.0, 0.0, "instrument"},
    {"gallery.instrument", "gallery.instrument-group", NodeKind::instrument, LayoutKind::none, "", ordinary, 0, 0, 118, 0.0, 1.0, 0.42},
    {"gallery.diagnostics", "gallery.root", NodeKind::diagnostics, LayoutKind::flex_column, "Diagnostics", enabled, 5, 280, 0, 0.0, 0.0, 0.0},
    {"gallery.diagnostics.about", "gallery.diagnostics", NodeKind::label, LayoutKind::none, "GUI.Forms Gallery", ordinary, 0, 0, 52, 0.0, 0.0, 0.0},
    {"gallery.diagnostics.renderer", "gallery.diagnostics", NodeKind::label, LayoutKind::none, "renderer", ordinary, 1, 0, 20, 0.0, 0.0, 0.0},
    {"gallery.diagnostics.controls", "gallery.diagnostics", NodeKind::label, LayoutKind::none, "controls", ordinary, 2, 0, 20, 0.0, 0.0, 0.0},
    {"gallery.diagnostics.layout", "gallery.diagnostics", NodeKind::label, LayoutKind::none, "measure / arrange", ordinary, 3, 0, 20, 0.0, 0.0, 0.0},
    {"gallery.diagnostics.paint", "gallery.diagnostics", NodeKind::label, LayoutKind::none, "paint / damage", ordinary, 4, 0, 20, 0.0, 0.0, 0.0},
    {"gallery.diagnostics.input", "gallery.diagnostics", NodeKind::label, LayoutKind::none, "input / focus / activation", ordinary, 5, 0, 20, 0.0, 0.0, 0.0},
    {"gallery.diagnostics.flush", "gallery.diagnostics", NodeKind::label, LayoutKind::none, "wake / ticks / active", ordinary, 6, 0, 20, 0.0, 0.0, 0.0},
    {"gallery.diagnostics.present", "gallery.diagnostics", NodeKind::label, LayoutKind::none, "present / worst", ordinary, 7, 0, 20, 0.0, 0.0, 0.0},
    {"gallery.diagnostics.font", "gallery.diagnostics", NodeKind::label, LayoutKind::none, "Controls: Portsmouth Rapids 1.0; Fields: Carlito body specimen.", ordinary, 8, 0, 52, 0.0, 0.0, 0.0},
}};

[[nodiscard]] consteval bool ids_are_unique()
{
    for (std::size_t left = 0; left < gallery_nodes.size(); ++left) {
        if (gallery_nodes[left].id.empty()) {
            return false;
        }
        for (std::size_t right = left + 1; right < gallery_nodes.size(); ++right) {
            if (gallery_nodes[left].id == gallery_nodes[right].id) {
                return false;
            }
        }
    }
    return true;
}

[[nodiscard]] consteval bool parents_resolve()
{
    for (const NodeSpec& node : gallery_nodes) {
        if (node.parent_id.empty()) {
            if (node.id != "gallery.root") {
                return false;
            }
            continue;
        }

        bool found = false;
        for (const NodeSpec& candidate : gallery_nodes) {
            if (candidate.id == node.parent_id) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] constexpr const NodeSpec* find(std::string_view id) noexcept
{
    for (const NodeSpec& node : gallery_nodes) {
        if (node.id == id) {
            return &node;
        }
    }
    return nullptr;
}

[[nodiscard]] consteval bool pages_are_declared_on_mounts_only()
{
    for (const NodeSpec& node : gallery_nodes) {
        if (!node.page.empty() && node.parent_id != "gallery.root") {
            return false;
        }
    }
    return true;
}

static_assert(ids_are_unique());
static_assert(parents_resolve());
static_assert(pages_are_declared_on_mounts_only());

}  // namespace gui_forms::gallery::dml
