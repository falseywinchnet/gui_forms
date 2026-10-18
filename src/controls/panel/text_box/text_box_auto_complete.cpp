// TextBox auto-complete: the suggestion list and appended completions
// (Windows Forms' AutoCompleteMode, AutoCompleteSource and
// AutoCompleteCustomSource). The main sequence is auto_complete_after_edit,
// which runs after each user edit, and auto_complete_key, which runs before
// the box's own key handling.

#include "gui_forms/controls/panel/text_box/text_box.hpp"

#include "text_box_auto_complete_session.hpp"
#include "gui_forms/host.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace gui_forms {

TextBox::~TextBox() {
    // The window may close the list after this box is gone; its revocation
    // and collection callbacks must not reach a destroyed box.
    if (auto_complete_session_) {
        detail::TextBoxAutoCompleteSession& session = *auto_complete_session_;
        session.revocation.disconnect();
        session.collection_change.disconnect();
    }
}

namespace {

// The rows the suggestion list shows before it scrolls.
constexpr std::size_t auto_complete_visible_rows = 8U;

bool suggests(AutoCompleteMode mode) noexcept {
    const bool result = mode == AutoCompleteMode::suggest || mode == AutoCompleteMode::suggest_append;
    return result;
}

bool appends(AutoCompleteMode mode) noexcept {
    const bool result = mode == AutoCompleteMode::append || mode == AutoCompleteMode::suggest_append;
    return result;
}

// Adds `candidate` to `matches` when it starts with `typed` and is not
// already there, up to the suggestion limit.
void add_match(std::string_view candidate, std::string_view typed, bool case_sensitive,
               std::vector<std::string>& matches) {
    if (matches.size() >= maximum_auto_complete_suggestions) return;
    const std::optional<std::size_t> found =
        find_text_match(candidate, typed, TextMatchMode::prefix, case_sensitive);
    if (!found) return;
    const std::vector<std::string>::const_iterator existing =
        std::find(matches.cbegin(), matches.cend(), candidate);
    if (existing != matches.cend()) return;
    matches.emplace_back(candidate);
}

} // namespace

void TextBox::set_auto_complete_mode(AutoCompleteMode mode) {
    require_mutable();
    if (auto_complete_mode_ == mode) return;
    end_auto_complete(false);
    auto_complete_mode_ = mode;
    invalidate(Dirty::semantics);
}

void TextBox::set_auto_complete_source(AutoCompleteSource source) {
    require_mutable();
    if (source == AutoCompleteSource::list_items) {
        throw std::invalid_argument(
            "TextBox auto-complete has no list items; use custom_source");
    }
    if (auto_complete_source_ == source) return;
    end_auto_complete(false);
    auto_complete_source_ = source;
    invalidate(Dirty::semantics);
}

void TextBox::set_auto_complete_custom_source(std::shared_ptr<AutoCompleteStringCollection> source) {
    require_mutable();
    if (!source) {
        throw std::invalid_argument("TextBox auto-complete custom source may not be null");
    }
    if (auto_complete_custom_source_ == source) return;
    end_auto_complete(false);
    auto_complete_custom_source_ = std::move(source);
}

void TextBox::set_auto_complete_case_sensitive(bool case_sensitive) {
    require_mutable();
    if (auto_complete_case_sensitive_ == case_sensitive) return;
    end_auto_complete(false);
    auto_complete_case_sensitive_ = case_sensitive;
}

HostServiceStatus TextBox::auto_complete_source_status() {
    HostServiceStatus status;
    const std::optional<HostAutoCompleteSource> host_source =
        host_auto_complete_source(auto_complete_source_);
    if (!host_source) return status;
    if (window() == nullptr || (*window()).host_services() == nullptr) {
        status.error = HostServiceError::unsupported;
        return status;
    }
    const HostAutoCompleteRequest request{.source = *host_source, .prefix_utf8 = {},
                                          .maximum_candidates = 0U};
    const HostAutoCompleteResult result = (*(*window()).host_services()).query_auto_complete(request);
    status = result.status;
    return status;
}

void TextBox::set_auto_complete_anchor(const Control::Ptr& anchor) {
    require_mutable();
    auto_complete_anchor_ = anchor;
}

bool TextBox::auto_complete_open() const noexcept {
    const bool result = auto_complete_session_ && (*auto_complete_session_).open;
    return result;
}

std::span<const std::string> TextBox::auto_complete_suggestions() const noexcept {
    if (!auto_complete_session_ || !(*auto_complete_session_).open) return {};
    const std::span<const std::string> result((*auto_complete_session_).suggestions);
    return result;
}

std::optional<std::size_t> TextBox::auto_complete_highlighted() const noexcept {
    if (!auto_complete_session_ || !(*auto_complete_session_).open) return std::nullopt;
    return (*auto_complete_session_).highlighted;
}

void TextBox::close_auto_complete() {
    require_mutable();
    end_auto_complete(false);
}

bool TextBox::auto_complete_active() const noexcept {
    const bool result = auto_complete_mode_ != AutoCompleteMode::none &&
        auto_complete_source_ != AutoCompleteSource::none && !multiline_ &&
        !password_protected() && !read_only_ && !edit_filter_;
    return result;
}

detail::TextBoxAutoCompleteSession& TextBox::auto_complete_session() {
    if (!auto_complete_session_) auto_complete_session_ = std::make_unique<detail::TextBoxAutoCompleteSession>();
    return *auto_complete_session_;
}

void TextBox::collect_auto_complete_candidates(std::string_view typed,
                                               std::vector<std::string>& matches) {
    matches.clear();
    if (typed.empty()) return;
    if (auto_complete_source_ == AutoCompleteSource::custom_source) {
        const std::span<const std::string> items = (*auto_complete_custom_source_).items();
        for (const std::string& candidate : items) {
            add_match(candidate, typed, auto_complete_case_sensitive_, matches);
        }
        return;
    }
    const std::optional<HostAutoCompleteSource> host_source =
        host_auto_complete_source(auto_complete_source_);
    if (!host_source || window() == nullptr || (*window()).host_services() == nullptr) return;
    const HostAutoCompleteRequest request{.source = *host_source, .prefix_utf8 = typed,
                                          .maximum_candidates = maximum_auto_complete_suggestions};
    const HostAutoCompleteResult result = (*(*window()).host_services()).query_auto_complete(request);
    if (!result.status.accepted()) return;
    for (const std::string& candidate : result.candidates_utf8) {
        add_match(candidate, typed, auto_complete_case_sensitive_, matches);
    }
}

void TextBox::auto_complete_after_edit(TextEditKind kind, bool composing) {
    if (!auto_complete_active() || composing) {
        // Composition text is the input method's until it is committed:
        // nothing is appended to it and no list stays open over it.
        end_auto_complete(false);
        return;
    }
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    session.typed = std::string(text());
    session.typed_selection = selection_;
    session.highlighted.reset();
    session.shown = false;
    session.appended = false;
    collect_auto_complete_candidates(session.typed, session.suggestions);

    const bool caret_at_end = selection_.empty() && selection_.caret == store_.utf8_size();
    if (appends(auto_complete_mode_) && kind == TextEditKind::typing && caret_at_end) {
        const std::size_t typed_size = session.typed.size();
        for (const std::string& candidate : session.suggestions) {
            if (candidate.size() <= typed_size) continue;
            // The typed text keeps its case; the rest comes from the
            // candidate, and must begin on a grapheme boundary of the result.
            std::string completed = session.typed;
            completed.append(candidate, typed_size, std::string::npos);
            const TextStore probe(completed);
            const Utf8Offset typed_end(typed_size);
            if (!probe.is_grapheme_boundary(typed_end)) continue;
            if (maximum_length_ != 0U && probe.scalar_count().value() > maximum_length_) break;
            const Utf8Offset completed_end(completed.size());
            show_completion_text(completed, TextSelection{.anchor = typed_end, .caret = completed_end});
            session.appended = true;
            session.completion_selection = selection_;
            break;
        }
    }
    if (suggests(auto_complete_mode_) && !session.suggestions.empty()) {
        open_auto_complete_list();
    } else {
        close_auto_complete_list();
    }
}

bool TextBox::auto_complete_completion_pending() const noexcept {
    if (!auto_complete_session_) return false;
    const detail::TextBoxAutoCompleteSession& session = *auto_complete_session_;
    const bool result = session.appended && session.shown &&
        session.shown_revision == store_.revision() && selection_ == session.completion_selection;
    return result;
}

bool TextBox::auto_complete_key(const KeyEvent& event) {
    if (!auto_complete_session_) return false;
    detail::TextBoxAutoCompleteSession& session = *auto_complete_session_;
    const bool open = session.open;
    const bool pending = auto_complete_completion_pending();
    if (!open && !pending) return false;
    const bool plain = event.modifiers == Modifier::none;
    if (open && plain && (event.physical_key == PhysicalKey::down ||
                          event.physical_key == PhysicalKey::up)) {
        // Down from the typed text goes to the first suggestion and from the
        // last back to the typed text; Up goes the other way.
        const std::size_t count = session.suggestions.size();
        std::optional<std::size_t> next{};
        if (event.physical_key == PhysicalKey::down) {
            if (!session.highlighted) {
                next = std::size_t{0U};
            } else if (*session.highlighted + 1U < count) {
                next = *session.highlighted + 1U;
            }
        } else {
            if (!session.highlighted) {
                next = count - 1U;
            } else if (*session.highlighted > 0U) {
                next = *session.highlighted - 1U;
            }
        }
        highlight_auto_complete(next);
        return true;
    }
    if (event.physical_key == PhysicalKey::enter || event.physical_key == PhysicalKey::tab) {
        if (open && session.highlighted) {
            std::string accepted = session.suggestions[*session.highlighted];
            accept_auto_complete(std::move(accepted));
            return true;
        }
        if (pending) {
            std::string accepted(text());
            accept_auto_complete(std::move(accepted));
            return true;
        }
        // Nothing to accept: the list closes and the key does its usual work.
        end_auto_complete(false);
        return false;
    }
    if (event.physical_key == PhysicalKey::escape) {
        end_auto_complete(true);
        return true;
    }
    return false;
}

void TextBox::show_completion_text(std::string_view shown_text, TextSelection selection) {
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    if (shown_text != store_.utf8()) {
        store_.set_text(shown_text);
        layout_positions_.clear();
        layout_offsets_.clear();
        selection_ = selection;
        reset_caret_blink();
        invalidate(Dirty::measure | Dirty::paint | Dirty::semantics);
        const std::string value(store_.utf8());
        publish_change(text_changed_, value);
        publish_change(selection_changed_, selection_);
    } else {
        set_selection(selection);
    }
    session.shown = true;
    session.shown_revision = store_.revision();
}

void TextBox::highlight_auto_complete(std::optional<std::size_t> index) {
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    session.highlighted = index;
    session.appended = false;
    if (session.list) {
        session.synchronizing = true;
        if (index) {
            (*session.list).select_index(*index);
        } else {
            (*session.list).clear_selection();
        }
        (*session.list).synchronize_scroll();
        session.synchronizing = false;
    }
    if (index) {
        const std::string& suggestion = session.suggestions[*index];
        const Utf8Offset end(suggestion.size());
        show_completion_text(suggestion, TextSelection{.anchor = end, .caret = end});
    } else {
        restore_auto_complete_typed_text();
    }
}

void TextBox::restore_auto_complete_typed_text() {
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    if (!session.shown || session.shown_revision != store_.revision()) return;
    // The text is still the completion the box showed: put back what the
    // user typed, with the caret where it was.
    const std::string typed = session.typed;
    const TextSelection typed_selection = session.typed_selection;
    show_completion_text(typed, typed_selection);
    session.shown = false;
}

void TextBox::accept_auto_complete(std::string accepted) {
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    const Utf8Offset end(accepted.size());
    show_completion_text(accepted, TextSelection{.anchor = end, .caret = end});
    session.shown = false;
    session.appended = false;
    session.highlighted.reset();
    close_auto_complete_list();
    if (!is_alive()) return;
    publish_change(auto_complete_accepted_, accepted);
}

void TextBox::end_auto_complete(bool restore_typed) {
    if (!auto_complete_session_) return;
    detail::TextBoxAutoCompleteSession& session = *auto_complete_session_;
    if (restore_typed) restore_auto_complete_typed_text();
    session.shown = false;
    session.appended = false;
    session.highlighted.reset();
    close_auto_complete_list();
}

void TextBox::open_auto_complete_list() {
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    if (session.open) {
        session.synchronizing = true;
        (*session.list).set_items(session.suggestions);
        (*session.list).clear_selection();
        (*session.list).set_top_index(0U);
        session.synchronizing = false;
        place_auto_complete_list();
        return;
    }
    if (!attached() || window() == nullptr || !effectively_visible() || !effectively_enabled()) return;
    // The anchor is this box or a control that contains it.
    Control::Ptr anchor = auto_complete_anchor_.lock();
    bool contains_this = false;
    for (const Control* current = this; current != nullptr && anchor; current = (*current).parent().get()) {
        if (current == anchor.get()) {
            contains_this = true;
            break;
        }
    }
    if (!contains_this) anchor = shared_from_this();
    const std::string prefix(stable_id().value());
    std::shared_ptr<AnchoredPopupLayer> layer =
        make_control<AnchoredPopupLayer>(StableId(prefix + ".auto_complete"), anchor);
    std::shared_ptr<DropDownList> list = make_control<DropDownList>(StableId(prefix + ".auto_complete.list"));
    // The list never takes focus: typing stays in this box.
    (*list).set_focusable(false);
    (*list).set_paint_plane(PaintPlane::overlay);
    (*list).set_font(font_);
    (*list).set_items(session.suggestions);
    (*layer).set_content(list);
    PopupToken token = (*window()).open_popup(shared_from_this(), layer);
    session.layer = layer;
    session.list = list;
    session.token = std::move(token);
    session.list_selection = (*list).selection_changed().subscribe(*this,
        Delegate<const ListSelectionChange&>::bind<TextBox, &TextBox::auto_complete_list_selection_changed>(*this));
    session.list_activation = (*list).item_activated().subscribe(*this,
        Delegate<std::size_t>::bind<TextBox, &TextBox::auto_complete_list_activated>(*this));
    session.dismissal = (*layer).dismiss_requested().subscribe(*this,
        Delegate<PopupDismissReason>::bind<TextBox, &TextBox::auto_complete_list_dismissed>(*this));
    if (Event<>* closed = session.token.closed_event()) {
        session.revocation = (*closed).subscribe(*this,
            Delegate<>::bind<TextBox, &TextBox::auto_complete_popup_revoked>(*this));
    }
    if (auto_complete_source_ == AutoCompleteSource::custom_source) {
        session.collection_change = (*auto_complete_custom_source_).changed().subscribe(*this,
            Delegate<>::bind<TextBox, &TextBox::auto_complete_collection_changed>(*this));
    }
    session.open = true;
    place_auto_complete_list();
    invalidate(Dirty::semantics);
    publish_change(auto_complete_open_changed_, true);
}

void TextBox::place_auto_complete_list() {
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    if (!session.layer || !session.list) return;
    const Control::Ptr anchor = (*session.layer).anchor();
    const double width = anchor ? (*anchor).absolute_bounds().width : absolute_bounds().width;
    const std::size_t rows = std::min(auto_complete_visible_rows, session.suggestions.size());
    const double row_height = (*session.list).item_height() * effective_text_scale();
    const double height = static_cast<double>(rows) * row_height + 4.0;
    AnchoredPopupPlacement placement{};
    placement.preferred_size = Size{std::max(width, 80.0), height};
    (*session.layer).set_placement(placement);
}

void TextBox::close_auto_complete_list() {
    if (!auto_complete_session_) return;
    detail::TextBoxAutoCompleteSession& session = *auto_complete_session_;
    if (!session.open && !session.layer) return;
    session.closing = true;
    session.list_selection.disconnect();
    session.list_activation.disconnect();
    session.dismissal.disconnect();
    session.revocation.disconnect();
    session.collection_change.disconnect();
    session.token.disconnect();
    session.list.reset();
    session.layer.reset();
    const bool changed = session.open;
    session.open = false;
    session.closing = false;
    if (!changed) return;
    invalidate(Dirty::semantics);
    publish_change(auto_complete_open_changed_, false);
}

void TextBox::discard_auto_complete_list() noexcept {
    if (!auto_complete_session_) return;
    detail::TextBoxAutoCompleteSession& session = *auto_complete_session_;
    session.closing = true;
    session.list_selection.disconnect();
    session.list_activation.disconnect();
    session.dismissal.disconnect();
    session.revocation.disconnect();
    session.collection_change.disconnect();
    session.token.disconnect();
    session.list.reset();
    session.layer.reset();
    session.open = false;
    session.shown = false;
    session.appended = false;
    session.highlighted.reset();
    session.closing = false;
}

void TextBox::auto_complete_list_selection_changed(const ListSelectionChange& change) {
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    if (session.synchronizing || !change.active_index) return;
    // A press on a row highlights it, as Up and Down do; the release accepts it.
    if (*change.active_index >= session.suggestions.size()) return;
    highlight_auto_complete(change.active_index);
}

void TextBox::auto_complete_list_activated(std::size_t index) {
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    if (index >= session.suggestions.size()) return;
    std::string accepted = session.suggestions[index];
    accept_auto_complete(std::move(accepted));
}

void TextBox::auto_complete_list_dismissed(PopupDismissReason) {
    // A press outside the list closes it and keeps the text.
    end_auto_complete(false);
}

void TextBox::auto_complete_popup_revoked() {
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    if (session.closing) return;
    // The window closed the list (this box was hidden, disabled or removed).
    session.list_selection.disconnect();
    session.list_activation.disconnect();
    session.dismissal.disconnect();
    session.collection_change.disconnect();
    session.list.reset();
    session.layer.reset();
    const bool changed = session.open;
    session.open = false;
    session.highlighted.reset();
    if (!changed) return;
    invalidate(Dirty::semantics);
    publish_change(auto_complete_open_changed_, false);
}

void TextBox::auto_complete_collection_changed() {
    detail::TextBoxAutoCompleteSession& session = auto_complete_session();
    if (!session.open) return;
    // The candidates changed under the open list: match the typed text
    // again, keeping the highlight on the same text when it is still there.
    std::optional<std::string> highlighted_text{};
    if (session.highlighted) highlighted_text = session.suggestions[*session.highlighted];
    collect_auto_complete_candidates(session.typed, session.suggestions);
    if (session.suggestions.empty()) {
        close_auto_complete_list();
        return;
    }
    std::optional<std::size_t> kept{};
    if (highlighted_text) {
        const std::vector<std::string>::const_iterator found =
            std::find(session.suggestions.cbegin(), session.suggestions.cend(), *highlighted_text);
        if (found != session.suggestions.cend()) {
            kept = static_cast<std::size_t>(std::distance(session.suggestions.cbegin(), found));
        }
    }
    session.synchronizing = true;
    (*session.list).set_items(session.suggestions);
    if (kept) {
        (*session.list).select_index(*kept);
    } else {
        (*session.list).clear_selection();
    }
    session.synchronizing = false;
    session.highlighted = kept;
    place_auto_complete_list();
}

} // namespace gui_forms
