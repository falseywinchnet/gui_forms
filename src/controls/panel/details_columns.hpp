#pragma once

#include "gui_forms/text.hpp"
#include "gui_forms/types.hpp"
#include "gui_forms/types/painter/painter.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>

// Pieces shared by the details views of ObjectView and ListView: the layout
// of one column header and the elision of cell text. Both controls publish
// their column headers as a "column" item system with the same part names;
// this is the one geometry function for those parts.
namespace gui_forms::details_detail {

// One column header's parts in the control's local coordinates. `cell` is
// the column's full header cell, which may extend past the visible header
// band when the view is scrolled sideways.
struct ColumnHeaderGeometry final {
    Rect header{};
    Rect label{};
    Rect sort_glyph{};
    Rect divider{};
    Rect grip{};
};

// Lays out one column header. Distances are logical units multiplied by
// `scale` (ObjectView passes 1, ListView its text scale): the label starts
// six units in and stops 22 units short of the right edge when the column
// is sorted (12 otherwise), the sort glyph is a six-unit square centred ten
// units in from the right edge, the divider is one unit wide inside the
// right edge and three units short of the top and bottom, and the resize
// grip spans four units either side of the right edge. Every part is
// clipped to the visible header (`clip`); the label, glyph and divider also
// to the visible part of their own cell.
inline ColumnHeaderGeometry column_header_geometry(Rect cell, Rect clip, bool sorted,
                                                   double scale) noexcept {
    const double reserved = (sorted ? 22.0 : 12.0) * scale;
    const Rect label{cell.x + 6.0 * scale, cell.y, std::max(0.0, cell.width - reserved), cell.height};
    const double edge = cell.x + cell.width;
    const Rect divider{edge - 1.5, cell.y + 3.0 * scale, 1.0,
                       std::max(0.0, cell.height - 6.0 * scale)};
    const Rect grip{edge - 4.0 * scale, cell.y, 8.0 * scale, cell.height};
    Rect glyph{};
    if (sorted) {
        const double centre = edge - 10.0 * scale;
        glyph = {centre - 3.0 * scale, cell.y + cell.height * 0.5 - 3.0 * scale, 6.0 * scale,
                 6.0 * scale};
    }
    const Rect visible_cell = Rect::intersection(cell, clip);
    ColumnHeaderGeometry result;
    result.header = visible_cell;
    result.label = Rect::intersection(label, visible_cell);
    result.sort_glyph = Rect::intersection(glyph, visible_cell);
    result.divider = Rect::intersection(divider, visible_cell);
    result.grip = Rect::intersection(grip, clip);
    return result;
}

// Writes `text` into `out`, cut at a grapheme boundary and ended with an
// ellipsis when it is wider than `width`; `out` is empty when not even the
// ellipsis fits or `width` is not positive. `out` is reused, so a caller
// that elides many cells keeps one buffer. The search is a bounded interval
// search over grapheme counts: each accepted length was measured to fit, but
// shaping that is not monotonic in length can make the result shorter than
// the longest fitting prefix.
inline void elide_to_width(Painter& painter, std::string_view text, FontSpec font, double width,
                           std::string& out) {
    out.assign(text);
    if (width <= 0.0) {
        out.clear();
        return;
    }
    if (painter.measure_text_utf8(text, font).width <= width) return;
    constexpr std::string_view ellipsis = "…";
    if (painter.measure_text_utf8(ellipsis, font).width > width) {
        out.clear();
        return;
    }
    const TextStore store(text);
    std::size_t lower = 0U;
    std::size_t upper = store.grapheme_count().value();
    while (lower < upper) {
        const std::size_t middle = lower + (upper - lower + 1U) / 2U;
        const std::size_t end = store.utf8_offset(GraphemeIndex(middle)).value();
        out.assign(text.substr(0U, end));
        out.append(ellipsis);
        if (painter.measure_text_utf8(out, font).width <= width) {
            lower = middle;
        } else {
            upper = middle - 1U;
        }
    }
    const std::size_t end = store.utf8_offset(GraphemeIndex(lower)).value();
    out.assign(text.substr(0U, end));
    out.append(ellipsis);
}

} // namespace gui_forms::details_detail
