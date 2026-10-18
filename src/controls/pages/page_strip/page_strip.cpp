#include "gui_forms/controls/pages/page_strip/page_strip.hpp"

#include "gui_forms/delegate.hpp"
#include "gui_forms/text.hpp"
#include "gui_forms/theme/part_painting/part_painting.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace gui_forms {
namespace {

// Strip geometry in logical units at text scale 1.
constexpr double strip_height = 26.0;
// The gap before the first tab, and after the last button.
constexpr double edge_inset = 2.0;
// The gap between a tab and the strip's edge away from the content; the
// tabs touch the edge the content is on.
constexpr double away_gap = 2.0;
constexpr double tab_padding = 8.0;
constexpr double image_size = 16.0;
constexpr double image_text_gap = 4.0;
constexpr double button_width = 16.0;
constexpr double glyph_size = 7.0;
constexpr double seam_thickness = 1.0;
constexpr double focus_inset = 2.0;

// Virtual semantic children carry a runtime ID derived from their stable ID,
// in the high half so it never meets a control's own runtime ID.
std::uint64_t node_runtime_id(std::string_view id) noexcept {
    std::uint64_t hash = 1469598103934665603ULL;
    for (const unsigned char byte : id) {
        hash ^= byte;
        hash *= 1099511628211ULL;
    }
    const std::uint64_t result = hash | (1ULL << 63U);
    return result;
}

// A title shown with mnemonics off keeps every `&`; the page list's menu
// parses markers, so each `&` is doubled for it.
std::string literal_menu_text(std::string_view title) {
    std::string text;
    text.reserve(title.size());
    for (const char byte : title) {
        text.push_back(byte);
        if (byte == '&') text.push_back('&');
    }
    return text;
}

Rect to_window(Rect absolute, Rect local) noexcept {
    const Rect result{absolute.x + local.x, absolute.y + local.y, local.width, local.height};
    return result;
}

bool valid_widths(const PageItemWidths& widths) noexcept {
    const bool finite = std::isfinite(widths.minimum) && std::isfinite(widths.maximum) &&
                        std::isfinite(widths.fixed);
    if (!finite) return false;
    const bool ordered = widths.minimum > 0.0 && widths.minimum <= widths.maximum &&
                         widths.maximum <= PageItemWidths::maximum_width;
    const bool fixed = widths.fixed > 0.0 && widths.fixed <= PageItemWidths::maximum_width;
    const bool result = ordered && fixed;
    return result;
}

// The pinned custom state is the first (and only) custom state of the page
// item system.
constexpr CustomStateIndex pinned_state{0U};

} // namespace

PageStrip::PageStrip(StableId stable_id, PageSet& pages) : Control(std::move(stable_id)), pages_(&pages) {
    set_focusable(true);
    set_cursor(CursorKind::arrow);
    set_paint_plane(PaintPlane::control);
    pages_changed_token_ = (*pages_).pages_changed().subscribe(
        *this, Delegate<const PageSetChange&>::bind<PageStrip, &PageStrip::on_pages_changed>(*this));
    selection_token_ = (*pages_).selected_page_changed().subscribe(
        *this, Delegate<const PageSelectionChange&>::bind<PageStrip, &PageStrip::on_selection_changed>(*this));
    list_ = std::make_unique<ContextMenu>(std::string((*this).stable_id().value()) + ".list");
    list_invoked_token_ = (*list_).item_invoked().subscribe(
        *this, Delegate<const MenuItemInvocation&>::bind<PageStrip, &PageStrip::on_list_item_invoked>(*this));
    list_changed_token_ = (*list_).open_changed().subscribe(
        *this, Delegate<bool>::bind<PageStrip, &PageStrip::on_list_open_changed>(*this));
}

PageStrip::~PageStrip() {
    close_page_list();
}

void PageStrip::on_dispose() noexcept {
    close_page_list();
    if (list_) (*list_).dispose();
    Control::on_dispose();
}

void PageStrip::release_page_set() noexcept {
    close_page_list();
    pages_changed_token_.disconnect();
    selection_token_.disconnect();
    pages_ = nullptr;
    keyboard_page_.reset();
    hot_ = {};
    pressed_ = {};
    first_visible_ = 0U;
    ++settings_revision_;
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

// -- settings ------------------------------------------------------------------

void PageStrip::bump_settings() {
    ++settings_revision_;
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void PageStrip::set_placement(PageStripPlacement placement) {
    require_mutable();
    if (placement != PageStripPlacement::top && placement != PageStripPlacement::bottom) {
        throw std::invalid_argument("PageStrip placement is invalid");
    }
    if (placement_ == placement) return;
    placement_ = placement;
    bump_settings();
}

void PageStrip::set_item_sizing(PageItemSizing sizing) {
    require_mutable();
    if (sizing != PageItemSizing::content && sizing != PageItemSizing::fixed && sizing != PageItemSizing::fill) {
        throw std::invalid_argument("PageStrip item sizing is invalid");
    }
    if (item_sizing_ == sizing) return;
    item_sizing_ = sizing;
    bump_settings();
}

void PageStrip::set_item_widths(PageItemWidths widths) {
    require_mutable();
    if (!valid_widths(widths)) {
        throw std::invalid_argument(
            "PageStrip widths need 0 < minimum <= maximum <= 1024 and 0 < fixed <= 1024");
    }
    item_widths_ = widths;
    bump_settings();
}

void PageStrip::set_activation(PageStripActivation activation) {
    require_mutable();
    if (activation != PageStripActivation::automatic && activation != PageStripActivation::manual) {
        throw std::invalid_argument("PageStrip activation is invalid");
    }
    if (activation_ == activation) return;
    activation_ = activation;
    // Under automatic activation the keyboard is always on the selection.
    if (activation_ == PageStripActivation::automatic && focused_ && pages_ != nullptr) {
        keyboard_page_ = (*pages_).selected_page();
    }
    invalidate(Dirty::paint | Dirty::semantics);
}

void PageStrip::set_mnemonics_enabled(bool enabled) {
    require_mutable();
    if (mnemonics_enabled_ == enabled) return;
    mnemonics_enabled_ = enabled;
    bump_settings();
}

void PageStrip::set_font(FontSpec font) {
    require_mutable();
    if (!valid_font_spec(font)) throw std::invalid_argument("PageStrip font specification is invalid");
    if (font_ == font) return;
    font_ = font;
    ++settings_revision_;
    invalidate(Dirty::measure | Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void PageStrip::set_image_list(std::shared_ptr<ImageList> image_list) {
    require_mutable();
    if (image_list && !(*image_list).is_alive()) {
        throw std::invalid_argument("PageStrip requires a live ImageList");
    }
    if (image_list && window() != nullptr && !(*image_list).belongs_to(*window())) {
        throw std::invalid_argument("PageStrip and ImageList must belong to one Window");
    }
    if (image_list_ == image_list) return;
    image_list_token_.disconnect();
    image_list_ = std::move(image_list);
    if (image_list_) {
        image_list_token_ = (*image_list_).changed().subscribe(
            *this, Delegate<const ImageListChange&>::bind<PageStrip, &PageStrip::on_image_list_changed>(*this));
    }
    bump_settings();
}

void PageStrip::on_image_list_changed(const ImageListChange&) {
    if (is_alive()) bump_settings();
}

void PageStrip::on_attached_to_window() {
    Control::on_attached_to_window();
    if (image_list_ && !(*image_list_).belongs_to(*window())) {
        throw std::logic_error("PageStrip cannot attach to a different ImageList Window");
    }
}

// -- set notifications ---------------------------------------------------------

void PageStrip::on_pages_changed(const PageSetChange& change) {
    if (pages_ == nullptr) return;
    if (keyboard_page_ && !(*pages_).find(*keyboard_page_)) keyboard_page_ = (*pages_).selected_page();
    const bool hot_stale = hot_.zone == Zone::tab && hot_.position >= (*pages_).count();
    if (hot_stale || change.membership || change.order) hot_ = {};
    if (change.membership || change.order) {
        pressed_ = {};
        close_page_list();
    }
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void PageStrip::on_selection_changed(const PageSelectionChange& change) {
    if (pages_ == nullptr) return;
    if (focused_ && activation_ == PageStripActivation::automatic) keyboard_page_ = change.new_page;
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

// -- layout --------------------------------------------------------------------

Size PageStrip::measure(Size available) {
    const double scale = effective_text_scale();
    const ResolvedTextLayout metrics = resolve_text_layout_utf8("Mg", effective_font(font_));
    const double preferred = std::max(strip_height * scale, metrics.logical_size.height + 10.0 * scale);
    const Size result{.width = available.width, .height = std::min(available.height, preferred)};
    return result;
}

void PageStrip::arrange(Rect final_bounds) {
    arrange_self(final_bounds);
}

double PageStrip::item_extent_scale() const noexcept {
    const double scale = effective_text_scale();
    return scale;
}

PageStrip::LayoutKey PageStrip::current_key() const {
    const Rect surface = client_rectangle();
    LayoutKey key;
    key.surface = Size{surface.width, surface.height};
    key.scale = item_extent_scale();
    key.settings_revision = settings_revision_;
    key.scroll_revision = scroll_revision_;
    key.bound = pages_ != nullptr;
    if (pages_ != nullptr) {
        key.set_revision = (*pages_).revision();
        key.page_count = (*pages_).count();
    }
    return key;
}

const PageStrip::Layout& PageStrip::layout() const {
    const LayoutKey key = current_key();
    if (!layout_.valid || !(layout_.key == key)) compute_layout(key);
    return layout_;
}

// Fills each slot's label, icon flag and width (before fill sharing).
void PageStrip::measure_tabs() const {
    const double scale = item_extent_scale();
    const FontSpec font = effective_font(font_);
    const bool images = image_list_ && (*image_list_).is_alive();
    const double minimum = item_widths_.minimum * scale;
    const double maximum = item_widths_.maximum * scale;
    const double fixed = item_widths_.fixed * scale;
    const std::size_t count = layout_.tabs.size();
    for (std::size_t position = 0; position < count; ++position) {
        const PageRecord& record = (*pages_).record_at(PagePosition{position});
        TabSlot& slot = layout_.tabs[position];
        if (mnemonics_enabled_) {
            slot.label = parse_mnemonic_text(record.title).display_text;
        } else {
            slot.label = record.title;
        }
        slot.has_icon = images && !record.image_key.empty() && (*image_list_).contains_key(record.image_key);
        slot.bounds = {};
        if (item_sizing_ == PageItemSizing::fixed) {
            slot.width = fixed;
            continue;
        }
        double content = tab_padding * scale * 2.0;
        if (slot.has_icon) content += image_size * scale;
        if (slot.has_icon && !slot.label.empty()) content += image_text_gap * scale;
        if (!slot.label.empty()) content += resolve_text_layout_utf8(slot.label, font).logical_size.width;
        slot.width = std::clamp(content, minimum, maximum);
    }
}

// The smallest first tab from which the row still shows the tab at
// `target` within `area`; `target` itself when that tab alone is wider.
std::size_t PageStrip::first_showing(std::size_t target, double area) const {
    double used = layout_.tabs[target].width;
    std::size_t first = target;
    while (first > 0U && used + layout_.tabs[first - 1U].width <= area) {
        used += layout_.tabs[first - 1U].width;
        --first;
    }
    return first;
}

// Lays out whole tabs from `first_visible` while they fit in `area` after
// `start`; the first tab always shows, cut to the area when it is wider.
void PageStrip::place_tabs(double start, double area) const {
    const double scale = item_extent_scale();
    const double height = layout_.key.surface.height;
    const double gap = away_gap * scale;
    const double tab_top = placement_ == PageStripPlacement::top ? gap : 0.0;
    const double tab_height = std::max(0.0, height - gap);
    const double end = start + area;
    const std::size_t count = layout_.tabs.size();
    double x = start;
    layout_.visible_count = 0U;
    for (std::size_t position = layout_.first_visible; position < count; ++position) {
        const double width = layout_.tabs[position].width;
        if (position > layout_.first_visible && x + width > end) break;
        const double shown = std::min(width, end - x);
        if (shown <= 0.0) break;
        layout_.tabs[position].bounds = {x, tab_top, shown, tab_height};
        ++layout_.visible_count;
        x += width;
    }
}

void PageStrip::compute_layout(const LayoutKey& key) const {
    // Anything but a scroll scrolls the selected (or keyboard) page back
    // into view.
    const LayoutKey previous = layout_.key;
    const bool scroll_only = layout_.valid && previous.surface == key.surface && previous.scale == key.scale &&
                             previous.set_revision == key.set_revision &&
                             previous.settings_revision == key.settings_revision &&
                             previous.page_count == key.page_count && previous.bound == key.bound;
    layout_.key = key;
    layout_.valid = true;
    layout_.rail = {0.0, 0.0, key.surface.width, key.surface.height};
    layout_.scroll_back = {};
    layout_.scroll_forward = {};
    layout_.list_button = {};
    layout_.first_visible = 0U;
    layout_.visible_count = 0U;
    layout_.last_first = 0U;
    layout_.overflow = false;
    layout_.selected = selected_position();
    const std::size_t count = key.bound ? key.page_count : 0U;
    layout_.tabs.resize(count);
    if (count == 0U) {
        first_visible_ = 0U;
        return;
    }
    measure_tabs();
    const double scale = key.scale;
    const double inset = edge_inset * scale;
    const double available = std::max(0.0, key.surface.width - inset * 2.0);
    if (item_sizing_ == PageItemSizing::fill) {
        const double share = available / static_cast<double>(count);
        const double width = std::clamp(share, item_widths_.minimum * scale, item_widths_.maximum * scale);
        for (TabSlot& slot : layout_.tabs) slot.width = width;
    }
    double total = 0.0;
    for (const TabSlot& slot : layout_.tabs) total += slot.width;
    if (total <= available) {
        first_visible_ = 0U;
        place_tabs(inset, available);
        return;
    }
    // Overflow (the strip policy): scroll back, scroll forward and the list
    // button at the end, the tabs in the area before them.
    layout_.overflow = true;
    const double buttons = button_width * scale;
    const double area = std::max(0.0, available - buttons * 3.0);
    const double gap = away_gap * scale;
    const double band_top = placement_ == PageStripPlacement::top ? gap : 0.0;
    const double band_height = std::max(0.0, key.surface.height - gap);
    const double end = key.surface.width - inset;
    layout_.scroll_back = {end - buttons * 3.0, band_top, buttons, band_height};
    layout_.scroll_forward = {end - buttons * 2.0, band_top, buttons, band_height};
    layout_.list_button = {end - buttons, band_top, buttons, band_height};
    layout_.last_first = first_showing(count - 1U, area);
    std::size_t first = std::min(first_visible_, layout_.last_first);
    std::optional<std::size_t> target = layout_.selected;
    if (focused_ && activation_ == PageStripActivation::manual) target = keyboard_position();
    if (!scroll_only && target) {
        if (*target < first) {
            first = *target;
        } else {
            double used = 0.0;
            for (std::size_t position = first; position <= *target; ++position) {
                used += layout_.tabs[position].width;
            }
            if (*target != first && used > area) first = first_showing(*target, area);
        }
    }
    layout_.first_visible = first;
    first_visible_ = first;
    place_tabs(inset, area);
}

// -- part geometry -------------------------------------------------------------

// One tab's parts, from its laid-out bounds; all absent when the tab is
// scrolled out of view. Interior, from the start: padding, icon, gap, label,
// padding. Phase P4 reserves its close, pin and modified slot after the label
// by narrowing `interior_end` here.
PageStrip::ItemParts PageStrip::tab_parts(std::size_t position) const {
    const Layout& placed = layout();
    ItemParts parts;
    if (position >= placed.tabs.size()) return parts;
    const TabSlot& slot = placed.tabs[position];
    const Rect bounds = slot.bounds;
    if (bounds.empty()) return parts;
    const double scale = placed.key.scale;
    parts.tab = PartGeometry::from_bounds(bounds);
    const double padding = tab_padding * scale;
    const double interior_end = bounds.x + bounds.width - padding;
    double x = bounds.x + padding;
    if (slot.has_icon) {
        const double side = image_size * scale;
        const Rect icon{x, std::floor(bounds.y + (bounds.height - side) * 0.5), side, side};
        parts.tab_icon = PartGeometry::from_bounds(Rect::intersection(icon, bounds));
        x += side + image_text_gap * scale;
    }
    if (!slot.label.empty() && interior_end > x) {
        const Rect label{x, bounds.y, interior_end - x, bounds.height};
        parts.tab_label = PartGeometry::from_bounds(label);
    }
    if (placed.selected && *placed.selected == position) {
        const double thickness = seam_thickness * scale;
        Rect seam{bounds.x, bounds.y + bounds.height - thickness, bounds.width, thickness};
        if (placement_ == PageStripPlacement::bottom) seam = {bounds.x, bounds.y, bounds.width, thickness};
        parts.tab_seam = PartGeometry::from_bounds(seam);
    }
    const double inset = focus_inset * scale;
    const Rect focus{bounds.x + inset, bounds.y + inset, std::max(0.0, bounds.width - inset * 2.0),
                     std::max(0.0, bounds.height - inset * 2.0)};
    parts.tab_focus = PartGeometry::from_bounds(focus);
    return parts;
}

// A button's glyph: a square centred in it, on whole units.
Rect PageStrip::scroll_glyph_bounds(Rect button) const {
    const double side = glyph_size * item_extent_scale();
    if (button.width < side || button.height < side) return {};
    const Rect glyph{std::floor(button.x + (button.width - side) * 0.5),
                     std::floor(button.y + (button.height - side) * 0.5), side, side};
    return glyph;
}

PageStrip::Parts PageStrip::strip_parts() const {
    const Layout& placed = layout();
    const Rect surface = client_rectangle();
    Parts record;
    record.region = PartGeometry::from_bounds(surface);
    record.rail = PartGeometry::from_bounds(placed.rail);
    record.scroll_back = PartGeometry::from_bounds(placed.scroll_back);
    record.scroll_back_glyph = PartGeometry::from_bounds(scroll_glyph_bounds(placed.scroll_back));
    record.scroll_forward = PartGeometry::from_bounds(placed.scroll_forward);
    record.scroll_forward_glyph = PartGeometry::from_bounds(scroll_glyph_bounds(placed.scroll_forward));
    record.list_button = PartGeometry::from_bounds(placed.list_button);
    record.list_glyph = PartGeometry::from_bounds(scroll_glyph_bounds(placed.list_button));
    record.focus = PartGeometry::from_bounds(surface);
    if (placed.overflow) {
        const AnatomyStateSet back = button_states(Zone::scroll_back);
        const AnatomyStateSet forward = button_states(Zone::scroll_forward);
        const AnatomyStateSet list = button_states(Zone::list_button);
        record.scroll_back.states = back;
        record.scroll_back_glyph.states = back;
        record.scroll_forward.states = forward;
        record.scroll_forward_glyph.states = forward;
        record.list_button.states = list;
        record.list_glyph.states = list;
    }
    return record;
}

// -- states --------------------------------------------------------------------

bool PageStrip::focus_cue() const noexcept {
    const Window* const host = attached_window();
    const bool result = focused_ && host != nullptr && (*host).focus_cue_visible();
    return result;
}

std::optional<std::size_t> PageStrip::selected_position() const {
    if (pages_ == nullptr || !(*pages_).selected_page()) return std::nullopt;
    const std::optional<PagePosition> found = (*pages_).find(*(*pages_).selected_page());
    if (!found) return std::nullopt;
    const std::size_t position = (*found).value;
    return position;
}

std::optional<std::size_t> PageStrip::keyboard_position() const {
    if (pages_ == nullptr || !focused_) return std::nullopt;
    if (!keyboard_page_) return selected_position();
    const std::optional<PagePosition> found = (*pages_).find(*keyboard_page_);
    if (!found) return selected_position();
    const std::size_t position = (*found).value;
    return position;
}

AnatomyStateSet PageStrip::tab_states(std::size_t position) const {
    AnatomyStateSet states;
    if (hot_.zone == Zone::tab && hot_.position == position) states.insert(AnatomyState::hot);
    if (pressed_.zone == Zone::tab && pressed_.position == position) states.insert(AnatomyState::pressed);
    const std::optional<std::size_t> selected = layout().selected;
    if (selected && *selected == position) states.insert(AnatomyState::selected);
    if (!effectively_enabled()) states.insert(AnatomyState::disabled);
    if (focus_cue()) {
        const std::optional<std::size_t> keyboard = keyboard_position();
        if (keyboard && *keyboard == position) states.insert(AnatomyState::focus_visible);
    }
    if (pages_ != nullptr && position < (*pages_).count() && (*pages_).record_at(PagePosition{position}).pinned) {
        states.insert(pinned_state);
    }
    return states;
}

bool PageStrip::can_scroll_backward() const {
    const Layout& placed = layout();
    const bool result = placed.overflow && placed.first_visible > 0U;
    return result;
}

bool PageStrip::can_scroll_forward() const {
    const Layout& placed = layout();
    const bool result = placed.overflow && placed.first_visible < placed.last_first;
    return result;
}

AnatomyStateSet PageStrip::button_states(Zone zone) const {
    AnatomyStateSet states;
    if (hot_.zone == zone) states.insert(AnatomyState::hot);
    if (pressed_.zone == zone) states.insert(AnatomyState::pressed);
    bool available = effectively_enabled();
    if (zone == Zone::scroll_back) available = available && can_scroll_backward();
    if (zone == Zone::scroll_forward) available = available && can_scroll_forward();
    if (zone == Zone::list_button && list_open_) states.insert(AnatomyState::open);
    if (!available && zone != Zone::list_button) states.insert(AnatomyState::disabled);
    return states;
}

// -- paint ---------------------------------------------------------------------

// Paints from the cached layout and the strip's own state: no allocation per
// tab (a tab's label is the layout's string, borrowed for the call).
void PageStrip::on_paint(Painter& painter, Rect) {
    const Layout& placed = layout();
    PartPaint part;
    part.font = effective_font(font_);
    part.family = ControlVisualRole::panel;
    part.role = PartRole::face;
    part.bounds = placed.rail;
    paint_part(painter, *this, part);
    // Tabs: flat at rest, a plate when hot or selected (the menu-item
    // family) until T4 draws Plan Paint's tab stack (owner decision DK-40).
    part.family = ControlVisualRole::menu_item;
    part.alignment = PartTextAlignment::center;
    const double scale = window() != nullptr ? (*window()).scale() : 1.0;
    const std::size_t end = placed.first_visible + placed.visible_count;
    for (std::size_t position = placed.first_visible; position < end; ++position) {
        const ItemParts parts = tab_parts(position);
        part.states = tab_states(position);
        part.role = PartRole::face;
        part.bounds = parts.tab.bounds;
        paint_part(painter, *this, part);
        if (parts.tab_icon.present) {
            const PageRecord& record = (*pages_).record_at(PagePosition{position});
            const ImageVisualState state = part.states.contains(AnatomyState::selected) ? ImageVisualState::selected
                                                                                          : ImageVisualState::normal;
            const ImageListResolution resolved = (*image_list_).resolve(record.image_key, state, scale);
            part.role = PartRole::icon;
            part.bounds = parts.tab_icon.bounds;
            part.image = resolved.image;
            paint_part(painter, *this, part);
            part.image = {};
        }
        if (parts.tab_label.present) {
            part.role = PartRole::label;
            part.bounds = parts.tab_label.bounds;
            part.text = placed.tabs[position].label;
            paint_part(painter, *this, part);
            part.text = {};
        }
        if (parts.tab_seam.present) {
            part.role = PartRole::seam;
            part.bounds = parts.tab_seam.bounds;
            paint_part(painter, *this, part);
        }
        part.role = PartRole::focus;
        part.bounds = parts.tab_focus.bounds;
        paint_part(painter, *this, part);
    }
    if (!placed.overflow) return;
    const Parts own = strip_parts();
    const std::array<const PartGeometry*, 3> buttons{&own.scroll_back, &own.scroll_forward, &own.list_button};
    const std::array<const PartGeometry*, 3> glyphs{&own.scroll_back_glyph, &own.scroll_forward_glyph,
                                                    &own.list_glyph};
    // The list opens a list of pages: an open down chevron, as a combo
    // box's, rather than the tool strip's overflow mark.
    const std::array<PartMark, 3> marks{PartMark::arrow_left, PartMark::arrow_right, PartMark::chevron_down};
    for (std::size_t index = 0; index < buttons.size(); ++index) {
        part.states = (*buttons[index]).states;
        part.role = PartRole::actuator;
        part.bounds = (*buttons[index]).bounds;
        paint_part(painter, *this, part);
        part.role = PartRole::glyph;
        part.bounds = (*glyphs[index]).bounds;
        part.mark = marks[index];
        paint_part(painter, *this, part);
        part.mark = PartMark::none;
    }
}

// -- input ---------------------------------------------------------------------

PageStrip::Hit PageStrip::hit_at(Point window_point) const {
    Hit hit;
    const Rect absolute = absolute_bounds();
    if (!absolute.contains(window_point)) return hit;
    const Point local{window_point.x - absolute.x, window_point.y - absolute.y};
    const Layout& placed = layout();
    if (placed.overflow) {
        if (placed.scroll_back.contains(local)) hit.zone = Zone::scroll_back;
        if (placed.scroll_forward.contains(local)) hit.zone = Zone::scroll_forward;
        if (placed.list_button.contains(local)) hit.zone = Zone::list_button;
        if (hit.zone != Zone::none) return hit;
    }
    const std::size_t end = placed.first_visible + placed.visible_count;
    for (std::size_t position = placed.first_visible; position < end; ++position) {
        if (!placed.tabs[position].bounds.contains(local)) continue;
        hit.zone = Zone::tab;
        hit.position = position;
        return hit;
    }
    return hit;
}

std::optional<PageId> PageStrip::page_at(Point window_point) const {
    const Hit hit = hit_at(window_point);
    if (hit.zone != Zone::tab || pages_ == nullptr) return std::nullopt;
    const PageId page = (*pages_).record_at(PagePosition{hit.position}).id;
    return page;
}

std::optional<PageId> PageStrip::focused_page() const {
    const std::optional<std::size_t> position = keyboard_position();
    if (!position) return std::nullopt;
    const PageId page = (*pages_).record_at(PagePosition{*position}).id;
    return page;
}

void PageStrip::set_hot(Hit hit) {
    if (hot_ == hit) return;
    hot_ = hit;
    invalidate(Dirty::paint);
}

bool PageStrip::choose(std::size_t position, PageSelectionCause cause) {
    if (pages_ == nullptr || position >= (*pages_).count()) return false;
    const PageId page = (*pages_).record_at(PagePosition{position}).id;
    const PageEditStatus status = (*pages_).select(page, cause);
    if (!page_edit_succeeded(status)) return false;
    // A handler may have released the set or disposed the strip.
    if (pages_ == nullptr || !is_alive()) return false;
    if (focused_) keyboard_page_ = page;
    const PageInvocation invoked{page, cause};
    page_invoked_.emit(invoked);
    return true;
}

void PageStrip::reveal(std::size_t position) {
    const Layout& placed = layout();
    if (!placed.overflow || position >= placed.tabs.size()) return;
    const std::size_t end = placed.first_visible + placed.visible_count;
    if (position >= placed.first_visible && position < end) return;
    const double scale = placed.key.scale;
    const double available = std::max(0.0, placed.key.surface.width - edge_inset * scale * 2.0);
    const double area = std::max(0.0, available - button_width * scale * 3.0);
    std::size_t first = position;
    if (position >= end) first = first_showing(position, area);
    first_visible_ = std::min(first, placed.last_first);
    ++scroll_revision_;
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

bool PageStrip::scroll_by(int direction) {
    const bool possible = direction < 0 ? can_scroll_backward() : can_scroll_forward();
    if (!possible) return false;
    const Layout& placed = layout();
    if (direction < 0) {
        first_visible_ = placed.first_visible - 1U;
    } else {
        first_visible_ = placed.first_visible + 1U;
    }
    ++scroll_revision_;
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
    return true;
}

bool PageStrip::scroll_backward() {
    require_mutable();
    const bool scrolled = scroll_by(-1);
    return scrolled;
}

bool PageStrip::scroll_forward() {
    require_mutable();
    const bool scrolled = scroll_by(1);
    return scrolled;
}

bool PageStrip::ensure_visible(const PageId& page) {
    require_mutable();
    if (pages_ == nullptr) return false;
    const std::optional<PagePosition> found = (*pages_).find(page);
    if (!found) return false;
    reveal((*found).value);
    return true;
}

bool PageStrip::overflowing() const {
    const bool result = layout().overflow;
    return result;
}

std::vector<PageId> PageStrip::visible_pages() const {
    std::vector<PageId> visible;
    const Layout& placed = layout();
    visible.reserve(placed.visible_count);
    const std::size_t end = placed.first_visible + placed.visible_count;
    for (std::size_t position = placed.first_visible; position < end; ++position) {
        visible.push_back((*pages_).record_at(PagePosition{position}).id);
    }
    return visible;
}

bool PageStrip::select_relative(int direction, PageSelectionCause cause) {
    require_mutable();
    if (pages_ == nullptr || (*pages_).count() == 0U || (direction != 1 && direction != -1)) return false;
    const std::size_t count = (*pages_).count();
    const std::size_t current = selected_position().value_or(0U);
    const std::size_t next = direction > 0 ? (current + 1U) % count : (current + count - 1U) % count;
    const bool chosen = choose(next, cause);
    return chosen;
}

// The keyboard moves to `position`: under automatic activation that selects
// it; under manual activation only the keyboard's page moves.
void PageStrip::move_keyboard(std::size_t position) {
    if (activation_ == PageStripActivation::automatic) {
        static_cast<void>(choose(position, PageSelectionCause::keyboard));
        return;
    }
    keyboard_page_ = (*pages_).record_at(PagePosition{position}).id;
    reveal(position);
    invalidate(Dirty::paint | Dirty::semantics);
}

void PageStrip::on_pointer(PointerEvent& event) {
    if (event.action == PointerAction::leave) {
        set_hot({});
        return;
    }
    const Hit hit = hit_at(event.position);
    if (event.action == PointerAction::move || event.action == PointerAction::enter) {
        set_hot(hit);
        return;
    }
    if (event.action == PointerAction::wheel) {
        // The wheel scrolls an overflowing strip (owner decision DK-37):
        // turned up or left towards the start.
        if (!layout().overflow) return;
        int direction = event.wheel_delta.y > 0.0 ? -1 : 1;
        if (event.wheel_delta.y == 0.0) direction = event.wheel_delta.x > 0.0 ? 1 : -1;
        if (event.wheel_delta.y == 0.0 && event.wheel_delta.x == 0.0) return;
        static_cast<void>(scroll_by(direction));
        event.handled = true;
        return;
    }
    if (event.button != PointerButton::primary) return;
    if (event.action == PointerAction::up) {
        const bool engaged = pressed_.zone != Zone::none;
        pressed_ = {};
        if (engaged) invalidate(Dirty::paint);
        event.handled = engaged;
        return;
    }
    if (event.action != PointerAction::down || hit.zone == Zone::none) return;
    const Control::Ptr keep = shared_from_this();
    pressed_ = hit;
    invalidate(Dirty::paint);
    event.handled = true;
    if (hit.zone == Zone::scroll_back || hit.zone == Zone::scroll_forward) {
        static_cast<void>(scroll_by(hit.zone == Zone::scroll_back ? -1 : 1));
        return;
    }
    if (hit.zone == Zone::list_button) {
        pressed_ = {};
        if (list_open_) {
            close_page_list();
        } else {
            static_cast<void>(show_page_list(MenuOpenMode::pointer));
        }
        return;
    }
    // A tab: the selection first, so a view behind the strip can remember
    // the focus its old page held, then the focus.
    static_cast<void>(choose(hit.position, PageSelectionCause::pointer));
    if (!is_alive()) return;
    if (window() != nullptr) static_cast<void>((*window()).request_focus(shared_from_this()));
}

void PageStrip::on_key(KeyEvent& event) {
    if (!focused_ || event.action != KeyAction::down || pages_ == nullptr || (*pages_).count() == 0U) return;
    const std::size_t count = (*pages_).count();
    const bool plain = event.modifiers == Modifier::none;
    const bool control_only = event.modifiers == Modifier::control;
    const std::size_t current = keyboard_position().value_or(0U);
    if (control_only && (event.physical_key == PhysicalKey::page_down || event.physical_key == PhysicalKey::page_up)) {
        static_cast<void>(select_relative(event.physical_key == PhysicalKey::page_down ? 1 : -1,
                                          PageSelectionCause::keyboard));
    } else if (plain && event.physical_key == PhysicalKey::right) {
        move_keyboard((current + 1U) % count);
    } else if (plain && event.physical_key == PhysicalKey::left) {
        move_keyboard((current + count - 1U) % count);
    } else if (plain && event.physical_key == PhysicalKey::home) {
        move_keyboard(0U);
    } else if (plain && event.physical_key == PhysicalKey::end) {
        move_keyboard(count - 1U);
    } else if (plain && (event.physical_key == PhysicalKey::space || event.physical_key == PhysicalKey::enter)) {
        static_cast<void>(choose(current, PageSelectionCause::keyboard));
    } else {
        return;
    }
    event.handled = true;
}

void PageStrip::on_focus_changed(bool focused) {
    focused_ = focused;
    keyboard_page_.reset();
    if (focused_ && pages_ != nullptr) keyboard_page_ = (*pages_).selected_page();
    invalidate(Dirty::paint | Dirty::semantics);
}

// -- mnemonics -----------------------------------------------------------------

bool PageStrip::mnemonic_matches(char32_t character) const noexcept {
    if (!mnemonics_enabled_ || pages_ == nullptr) return false;
    const std::size_t count = (*pages_).count();
    for (std::size_t position = 0; position < count; ++position) {
        if (is_mnemonic(character, (*pages_).record_at(PagePosition{position}).title)) return true;
    }
    return false;
}

// A mnemonic selects its page and focuses the strip on its tab.
bool PageStrip::process_mnemonic_self(char32_t character) {
    if (!mnemonics_enabled_ || pages_ == nullptr) return false;
    const std::size_t count = (*pages_).count();
    for (std::size_t position = 0; position < count; ++position) {
        if (!is_mnemonic(character, (*pages_).record_at(PagePosition{position}).title)) continue;
        const Control::Ptr keep = shared_from_this();
        static_cast<void>(choose(position, PageSelectionCause::keyboard));
        if (is_alive() && window() != nullptr) static_cast<void>((*window()).request_focus(keep));
        return true;
    }
    return false;
}

// -- page list -----------------------------------------------------------------

void PageStrip::prepare_list_commands() {
    const std::size_t count = (*pages_).count();
    const std::string prefix = std::string(stable_id().value()) + ".list.";
    while (list_commands_.size() < count) {
        const std::string identity = prefix + std::to_string(list_commands_.size());
        list_commands_.push_back(std::make_shared<Command>(identity, std::string{}));
    }
    const std::optional<std::size_t> selected = selected_position();
    for (std::size_t position = 0; position < count; ++position) {
        const PageRecord& record = (*pages_).record_at(PagePosition{position});
        Command& command = *list_commands_[position];
        command.set_text(mnemonics_enabled_ ? record.title : literal_menu_text(record.title));
        command.set_checked(selected && *selected == position);
        command.set_enabled(true);
    }
}

bool PageStrip::show_page_list(MenuOpenMode mode) {
    require_mutable();
    if (mode != MenuOpenMode::keyboard && mode != MenuOpenMode::pointer) {
        throw std::invalid_argument("PageStrip opening mode is invalid");
    }
    if (pages_ == nullptr || attached_window() == nullptr || !effectively_enabled()) return false;
    const Layout& placed = layout();
    if (!placed.overflow) return false;
    if (list_open_ && (*list_).is_open()) return true;
    prepare_list_commands();
    const std::size_t count = (*pages_).count();
    std::vector<MenuItemSpec> rows;
    rows.reserve(count);
    for (std::size_t position = 0; position < count; ++position) {
        const PageRecord& record = (*pages_).record_at(PagePosition{position});
        const std::string text = (*list_commands_[position]).state().text;
        rows.emplace_back(record.id.value, MenuItemKind::radio, list_commands_[position], text);
    }
    (*list_).set_items(std::move(rows));
    const Rect absolute = absolute_bounds();
    const Rect button = placed.list_button;
    const double right = absolute.x + button.x + button.width;
    // Right-aligned below the list button; the popup layer keeps it on screen.
    const Point anchor{std::max(absolute.x, right - (*list_).preferred_width()),
                       absolute.y + button.y + button.height};
    (*list_).show(shared_from_this(), anchor, mode);
    list_open_ = true;
    invalidate(Dirty::paint | Dirty::semantics);
    return true;
}

void PageStrip::close_page_list() noexcept {
    if (list_) (*list_).close();
}

bool PageStrip::page_list_open() const noexcept {
    const bool result = list_ && (*list_).is_open();
    return result;
}

void PageStrip::on_list_open_changed(bool open) {
    list_open_ = open;
    invalidate(Dirty::paint | Dirty::semantics);
}

// A page chosen from the list is selected and scrolled into view (the strip
// policy, interview P13; the document preset's "takes the last visible
// position" is phase P4's).
void PageStrip::on_list_item_invoked(const MenuItemInvocation& invocation) {
    if (pages_ == nullptr) return;
    const PageId page{invocation.item_id};
    const std::optional<PagePosition> found = (*pages_).find(page);
    if (!found) return;
    static_cast<void>(choose((*found).value, PageSelectionCause::pointer));
    if (!is_alive() || pages_ == nullptr) return;
    const std::optional<PagePosition> still = (*pages_).find(page);
    if (still) reveal((*still).value);
}

// -- semantics -----------------------------------------------------------------

std::string PageStrip::tab_identity(const PageId& page) const {
    std::string identity = std::string(stable_id().value()) + ".tab." + page.value;
    return identity;
}

std::string PageStrip::page_panel_identity(const PageId&) const {
    return {};
}

std::optional<std::size_t> PageStrip::position_of_tab_identity(std::string_view identity) const {
    if (pages_ == nullptr) return std::nullopt;
    const std::string prefix = std::string(stable_id().value()) + ".tab.";
    if (identity.size() <= prefix.size() || identity.substr(0U, prefix.size()) != prefix) return std::nullopt;
    const PageId page{std::string(identity.substr(prefix.size()))};
    const std::optional<PagePosition> found = (*pages_).find(page);
    if (!found) return std::nullopt;
    const std::size_t position = (*found).value;
    return position;
}

SemanticDescriptor PageStrip::semantic_descriptor() const {
    SemanticDescriptor descriptor{};
    descriptor.role = SemanticRole::tab_group;
    descriptor.name = accessible_name().empty() ? "Pages" : accessible_name();
    descriptor.description = accessible_description();
    const std::optional<std::size_t> selected = selected_position();
    if (selected) descriptor.value = layout().tabs[*selected].label;
    descriptor.orientation = SemanticOrientation::horizontal;
    descriptor.actions = {SemanticAction::focus};
    descriptor.exposed = true;
    return descriptor;
}

// Every page is a tab, laid out or scrolled out of view (then without the
// visible state and bounds), so assistive technology reaches each one; the
// scroll and list buttons follow while the strip overflows.
std::vector<SemanticNode> PageStrip::semantic_virtual_children() const {
    std::vector<SemanticNode> nodes{};
    if (pages_ == nullptr) return nodes;
    const Layout& placed = layout();
    const Rect absolute = absolute_bounds();
    const std::size_t count = placed.tabs.size();
    const bool enabled = effectively_enabled();
    const std::optional<std::size_t> selected = selected_position();
    const std::optional<std::size_t> keyboard = keyboard_position();
    nodes.reserve(count + 3U);
    for (std::size_t position = 0; position < count; ++position) {
        const PageRecord& record = (*pages_).record_at(PagePosition{position});
        const Rect bounds = placed.tabs[position].bounds;
        SemanticNode node{};
        node.stable_id = tab_identity(record.id);
        node.runtime_id = node_runtime_id(node.stable_id);
        node.role = SemanticRole::tab;
        node.name = placed.tabs[position].label;
        node.position_in_set = position + 1U;
        node.set_size = count;
        node.controlled_panel = page_panel_identity(record.id);
        node.states = SemanticState::focusable;
        if (enabled) node.states |= SemanticState::enabled;
        if (!bounds.empty()) {
            node.bounds = to_window(absolute, bounds);
            node.states |= SemanticState::visible;
        }
        if (selected && *selected == position) node.states |= SemanticState::selected;
        if (keyboard && *keyboard == position) node.states |= SemanticState::focused;
        // Tabs publish select and, where closable, close; never press
        // (owner decision DK-35).
        node.actions = {SemanticAction::focus, SemanticAction::select};
        if (record.closable) node.actions.push_back(SemanticAction::close);
        nodes.push_back(std::move(node));
    }
    if (!placed.overflow) return nodes;
    const std::string strip_id(stable_id().value());
    const std::array<Rect, 2> scroll_bounds{placed.scroll_back, placed.scroll_forward};
    const std::array<const char*, 2> scroll_ids{".scroll_back", ".scroll_forward"};
    const std::array<const char*, 2> scroll_names{"Scroll tabs back", "Scroll tabs forward"};
    const std::array<bool, 2> scroll_possible{can_scroll_backward(), can_scroll_forward()};
    for (std::size_t index = 0; index < scroll_bounds.size(); ++index) {
        SemanticNode button{};
        button.stable_id = strip_id + scroll_ids[index];
        button.runtime_id = node_runtime_id(button.stable_id);
        button.role = SemanticRole::button;
        button.name = scroll_names[index];
        button.bounds = to_window(absolute, scroll_bounds[index]);
        button.states = SemanticState::visible;
        if (enabled && scroll_possible[index]) button.states |= SemanticState::enabled;
        button.actions = {SemanticAction::press};
        nodes.push_back(std::move(button));
    }
    SemanticNode list{};
    list.stable_id = strip_id + ".list_button";
    list.runtime_id = node_runtime_id(list.stable_id);
    list.role = SemanticRole::menu_button;
    list.name = "All pages";
    list.has_popup = true;
    list.bounds = to_window(absolute, placed.list_button);
    list.states = SemanticState::visible;
    if (enabled) list.states |= SemanticState::enabled;
    if (list_open_) list.states |= SemanticState::expanded;
    list.actions = {SemanticAction::press, SemanticAction::show_menu, SemanticAction::collapse};
    nodes.push_back(std::move(list));
    return nodes;
}

bool PageStrip::on_semantic_child_action(std::string_view id, SemanticAction action, std::string_view) {
    if (pages_ == nullptr) return false;
    const std::string strip_id(stable_id().value());
    if (id == strip_id + ".scroll_back" || id == strip_id + ".scroll_forward") {
        if (action != SemanticAction::press) return false;
        const bool scrolled = scroll_by(id == strip_id + ".scroll_back" ? -1 : 1);
        return scrolled;
    }
    if (id == strip_id + ".list_button") {
        if (action == SemanticAction::collapse) {
            if (!list_open_) return false;
            close_page_list();
            return true;
        }
        if (action != SemanticAction::press && action != SemanticAction::show_menu) return false;
        const bool opened = show_page_list(MenuOpenMode::keyboard);
        return opened;
    }
    const std::optional<std::size_t> position = position_of_tab_identity(id);
    if (!position) return false;
    const Control::Ptr keep = shared_from_this();
    if (action == SemanticAction::focus) {
        if (window() != nullptr) static_cast<void>((*window()).request_focus(keep));
        if (!is_alive() || pages_ == nullptr || !focused_) return false;
        if (activation_ == PageStripActivation::automatic) {
            static_cast<void>(choose(*position, PageSelectionCause::semantic));
        } else {
            move_keyboard(*position);
        }
        return true;
    }
    // A press sent anyway still selects, as TabControl's tabs do.
    if (action == SemanticAction::select || action == SemanticAction::press) {
        const bool chosen = choose(*position, PageSelectionCause::semantic);
        if (chosen && is_alive() && pages_ != nullptr) reveal(*position);
        return chosen;
    }
    if (action == SemanticAction::close) {
        const PageRecord& record = (*pages_).record_at(PagePosition{*position});
        if (!record.closable) return false;
        PageCloseRequest request{record.id, PageCloseCause::semantic, false};
        page_close_requested_.emit(request);
        return request.handled;
    }
    return false;
}

// -- anatomy -------------------------------------------------------------------

namespace {

constexpr std::array<AnatomyState, 3> scroll_button_states{
    {AnatomyState::hot, AnatomyState::pressed, AnatomyState::disabled}};
constexpr std::array<AnatomyState, 3> list_button_states{
    {AnatomyState::hot, AnatomyState::pressed, AnatomyState::open}};

constexpr std::array<PartField<PageStrip::Parts>, 9> page_strip_part_fields{{
    {.declaration = {.name = "region", .role = PartRole::region, .composed_from = {}},
     .member = &PageStrip::Parts::region},
    {.declaration = {.name = "rail", .role = PartRole::face, .composed_from = {}},
     .member = &PageStrip::Parts::rail},
    {.declaration = {.name = "scroll_back", .role = PartRole::actuator, .composed_from = "button",
                     .states = scroll_button_states},
     .member = &PageStrip::Parts::scroll_back},
    {.declaration = {.name = "scroll_back_glyph", .role = PartRole::glyph, .composed_from = {},
                     .states = scroll_button_states},
     .member = &PageStrip::Parts::scroll_back_glyph},
    {.declaration = {.name = "scroll_forward", .role = PartRole::actuator, .composed_from = "button",
                     .states = scroll_button_states},
     .member = &PageStrip::Parts::scroll_forward},
    {.declaration = {.name = "scroll_forward_glyph", .role = PartRole::glyph, .composed_from = {},
                     .states = scroll_button_states},
     .member = &PageStrip::Parts::scroll_forward_glyph},
    {.declaration = {.name = "list_button", .role = PartRole::actuator, .composed_from = "button",
                     .states = list_button_states},
     .member = &PageStrip::Parts::list_button},
    {.declaration = {.name = "list_glyph", .role = PartRole::glyph, .composed_from = {},
                     .states = list_button_states},
     .member = &PageStrip::Parts::list_glyph},
    {.declaration = {.name = "focus", .role = PartRole::focus, .composed_from = {}},
     .member = &PageStrip::Parts::focus},
}};

constexpr std::array<PartField<PageStrip::ItemParts>, 5> page_strip_item_part_fields{{
    {.declaration = {.name = "tab", .role = PartRole::face, .composed_from = {}},
     .member = &PageStrip::ItemParts::tab},
    {.declaration = {.name = "tab_icon", .role = PartRole::icon, .composed_from = {}},
     .member = &PageStrip::ItemParts::tab_icon},
    {.declaration = {.name = "tab_label", .role = PartRole::label, .composed_from = {}},
     .member = &PageStrip::ItemParts::tab_label},
    {.declaration = {.name = "tab_seam", .role = PartRole::seam, .composed_from = {}},
     .member = &PageStrip::ItemParts::tab_seam},
    {.declaration = {.name = "tab_focus", .role = PartRole::focus, .composed_from = {}},
     .member = &PageStrip::ItemParts::tab_focus},
}};

constexpr std::array<StateField<PageStrip::ItemStates>, 5> page_strip_item_state_fields{{
    {.state = AnatomyState::hot, .member = &PageStrip::ItemStates::hot},
    {.state = AnatomyState::pressed, .member = &PageStrip::ItemStates::pressed},
    {.state = AnatomyState::selected, .member = &PageStrip::ItemStates::selected},
    {.state = AnatomyState::disabled, .member = &PageStrip::ItemStates::disabled},
    {.state = AnatomyState::focus_visible, .member = &PageStrip::ItemStates::focus_visible},
}};

constexpr std::array<PartDeclaration, 9> page_strip_parts = part_declarations(page_strip_part_fields);
constexpr std::array<PartDeclaration, 5> page_strip_item_parts = part_declarations(page_strip_item_part_fields);
constexpr std::array<AnatomyState, 5> page_strip_item_states = state_declarations(page_strip_item_state_fields);
// The admitted custom state `pinned` (DK-40); `modified` joins it with the
// document preset (P4), which has the flag to show.
constexpr std::array<CustomStateDeclaration, 1> page_strip_item_custom_states{{pinned_custom_state}};
constexpr std::array<AnatomyState, 5> page_strip_states{
    AnatomyState::normal, AnatomyState::enabled, AnatomyState::disabled,
    AnatomyState::focused, AnatomyState::focus_visible,
};

constexpr std::array<ItemSystemDeclaration, 1> page_strip_item_systems{{
    {.name = "page", .parts = page_strip_item_parts, .states = page_strip_item_states,
     .custom_states = page_strip_item_custom_states},
}};

constexpr ControlAnatomy page_strip_anatomy{
    .control_id = "page_strip",
    .parts = page_strip_parts,
    .states = page_strip_states,
    .custom_states = {},
    .item_systems = page_strip_item_systems,
};

} // namespace

const ControlAnatomy& PageStrip::anatomy() const noexcept {
    return page_strip_anatomy;
}

void PageStrip::publish_part_geometry(std::span<PartGeometry> geometry) const {
    const Parts record = strip_parts();
    write_parts(anatomy(), page_strip_part_fields, record, geometry);
}

PageStrip::Parts PageStrip::parts() const {
    const PartGeometryBuffer published = published_part_geometry();
    Parts record;
    read_parts(anatomy(), page_strip_part_fields, published.parts(), record);
    return record;
}

std::size_t PageStrip::realized_item_count() const {
    const std::size_t count = layout().visible_count;
    return count;
}

std::string PageStrip::realized_item_stable_id(std::size_t item) const {
    const std::size_t position = layout().first_visible + item;
    const std::string identity = tab_identity((*pages_).record_at(PagePosition{position}).id);
    return identity;
}

void PageStrip::publish_realized_item_geometry(std::size_t item, std::span<PartGeometry> geometry) const {
    const std::size_t position = layout().first_visible + item;
    const ItemParts record = tab_parts(position);
    write_item_parts(anatomy().item_systems.front(), page_strip_item_part_fields, record, geometry);
}

AnatomyStateSet PageStrip::realized_item_anatomy_states(std::size_t item) const {
    const std::size_t position = layout().first_visible + item;
    const AnatomyStateSet states = tab_states(position);
    return states;
}

std::vector<PageStrip::Item> PageStrip::realized_items() const {
    const std::size_t count = realized_item_count();
    std::vector<Item> records;
    records.reserve(count);
    for (std::size_t item = 0; item < count; ++item) {
        const PartGeometryBuffer published = published_realized_item_geometry(item);
        const std::size_t position = layout().first_visible + item;
        Item record;
        record.position = PagePosition{position};
        record.page = (*pages_).record_at(record.position).id;
        record.stable_id = realized_item_stable_id(item);
        read_item_parts(anatomy().item_systems.front(), page_strip_item_part_fields, published.parts(),
                        record.parts);
        const AnatomyStateSet states = realized_item_anatomy_states(item);
        read_states(page_strip_item_state_fields, states, record.states);
        record.states.pinned = states.contains(pinned_state);
        records.push_back(record);
    }
    return records;
}

namespace {

// The events PageStrip adds to those of Control, in declaration order.
constexpr std::array<InteractionDeclaration, 2> page_strip_interactions{{
    {.name = "page_invoked", .payload = "PageInvocation", .mutable_payload = false},
    {.name = "page_close_requested", .payload = "PageCloseRequest", .mutable_payload = true},
}};

} // namespace

void PageStrip::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    Control::append_interactions(interactions);
    interactions.insert(interactions.end(), page_strip_interactions.begin(), page_strip_interactions.end());
}

} // namespace gui_forms
