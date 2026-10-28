#include "gui_forms/text/rich_text/rich_text.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace gui_forms {

namespace {

// The UTF-8 encoding of rich_line_break (U+2028).
constexpr std::string_view line_break_utf8 = "\xE2\x80\xA8";

// Decodes the scalar at offset of text that validate_utf8 has accepted.
[[nodiscard]] char32_t decode_valid_scalar(std::string_view text, std::size_t offset, std::size_t& length) noexcept {
    const unsigned char lead = static_cast<unsigned char>(text[offset]);
    if (lead < 0x80U) {
        length = 1U;
        return lead;
    }
    std::size_t count = 4U;
    char32_t scalar = lead & 0x07U;
    if (lead < 0xE0U) {
        count = 2U;
        scalar = lead & 0x1FU;
    } else if (lead < 0xF0U) {
        count = 3U;
        scalar = lead & 0x0FU;
    }
    for (std::size_t index = 1U; index < count; ++index) {
        const unsigned char next = static_cast<unsigned char>(text[offset + index]);
        scalar = (scalar << 6U) | (static_cast<char32_t>(next) & 0x3FU);
    }
    length = count;
    return scalar;
}

// The characters a document may not hold (stx_rtf2md's normal form) and the
// model does not admit in its text: C0 controls other than tab and line
// feed, DEL, NEL, U+2029 and U+FEFF. U+2028 is the model's line break.
[[nodiscard]] bool forbidden_in_text(char32_t scalar) noexcept {
    if (scalar == U'\t' || scalar == U'\n') return false;
    if (scalar < 0x20U || scalar == 0x7FU || scalar == 0x85U) return true;
    return scalar == U' ' || scalar == U'﻿';
}

[[nodiscard]] bool is_separator(char32_t scalar) noexcept {
    return scalar == rich_paragraph_separator || scalar == rich_line_break;
}

// Throws std::invalid_argument unless text is valid UTF-8 the model admits.
void require_admitted_text(std::string_view text) {
    const Utf8ValidationResult validation = validate_utf8(text);
    if (!validation.valid()) throw std::invalid_argument("rich text is not valid UTF-8");
    for (std::size_t offset = 0U; offset < text.size();) {
        std::size_t length{};
        const char32_t scalar = decode_valid_scalar(text, offset, length);
        if (forbidden_in_text(scalar)) throw std::invalid_argument("rich text holds a character it may not hold");
        offset += length;
    }
}

// Whether text holds a scalar that is not a separator.
[[nodiscard]] bool has_content(std::string_view text) noexcept {
    for (std::size_t offset = 0U; offset < text.size();) {
        std::size_t length{};
        const char32_t scalar = decode_valid_scalar(text, offset, length);
        if (!is_separator(scalar)) return true;
        offset += length;
    }
    return false;
}

[[nodiscard]] std::size_t count_line_feeds(std::string_view text) noexcept {
    const std::size_t count = static_cast<std::size_t>(std::count(text.begin(), text.end(), '\n'));
    return count;
}

[[nodiscard]] bool has_attribute(const RichRunStyle& style, RichStyleAttribute attribute) noexcept {
    switch (attribute) {
    case RichStyleAttribute::bold: return style.bold;
    case RichStyleAttribute::italic: return style.italic;
    case RichStyleAttribute::underline: return style.underline;
    case RichStyleAttribute::strikethrough: return style.strikethrough;
    case RichStyleAttribute::code: return style.code;
    case RichStyleAttribute::superscript: return style.baseline == RichBaseline::superscript;
    case RichStyleAttribute::subscript: return style.baseline == RichBaseline::subscript;
    }
    // An attribute outside the enumeration is no attribute.
    return false;
}

void apply_attribute(RichRunStyle& style, RichStyleAttribute attribute, bool on) noexcept {
    switch (attribute) {
    case RichStyleAttribute::bold:
        style.bold = on;
        return;
    case RichStyleAttribute::italic:
        style.italic = on;
        return;
    case RichStyleAttribute::underline:
        style.underline = on;
        return;
    case RichStyleAttribute::strikethrough:
        style.strikethrough = on;
        return;
    case RichStyleAttribute::code:
        style.code = on;
        return;
    case RichStyleAttribute::superscript:
        if (on) style.baseline = RichBaseline::superscript;
        else if (style.baseline == RichBaseline::superscript) style.baseline = RichBaseline::normal;
        return;
    case RichStyleAttribute::subscript:
        if (on) style.baseline = RichBaseline::subscript;
        else if (style.baseline == RichBaseline::subscript) style.baseline = RichBaseline::normal;
        return;
    }
    // An attribute outside the enumeration changes nothing.
    return;
}

void require_quote_depth(std::uint8_t quote_depth) {
    if (quote_depth > maximum_rich_quote_depth) throw std::invalid_argument("rich text quote depth is out of range");
}

void require_list_level(std::uint8_t level) {
    if (level > maximum_rich_list_level) throw std::invalid_argument("rich text list level is out of range");
}

// The style changes restyle applies, one functor per setter.
struct SetAttribute final {
    RichStyleAttribute attribute{};
    bool on{};
    void operator()(RichRunStyle& style) const noexcept { apply_attribute(style, attribute, on); }
};

struct SetColour final {
    std::optional<RichRgb> colour{};
    void operator()(RichRunStyle& style) const noexcept { style.colour = colour; }
};

struct SetBackground final {
    std::optional<RichRgb> background{};
    void operator()(RichRunStyle& style) const noexcept { style.background = background; }
};

struct SetSizeStep final {
    std::int8_t size_step{};
    void operator()(RichRunStyle& style) const noexcept { style.size_step = size_step; }
};

struct SetLink final {
    std::optional<std::string> target{};
    void operator()(RichRunStyle& style) const { style.link = target; }
};

} // namespace

bool operator==(const RichRgb& left, const RichRgb& right) noexcept {
    return left.red == right.red && left.green == right.green && left.blue == right.blue;
}

bool operator==(const RichRunStyle& left, const RichRunStyle& right) noexcept {
    return left.bold == right.bold && left.italic == right.italic && left.underline == right.underline &&
           left.strikethrough == right.strikethrough && left.code == right.code &&
           left.baseline == right.baseline && left.link == right.link && left.colour == right.colour &&
           left.background == right.background && left.size_step == right.size_step;
}

// -- paragraph formats ----------------------------------------------------------

RichParagraphFormat::RichParagraphFormat(Shape shape, std::uint8_t quote_depth) noexcept
    : shape_(std::move(shape)), quote_depth_(quote_depth) {}

RichParagraphFormat RichParagraphFormat::body(RichAlignment alignment, std::uint8_t quote_depth) {
    require_quote_depth(quote_depth);
    const RichParagraphFormat format(Body{alignment}, quote_depth);
    return format;
}

RichParagraphFormat RichParagraphFormat::heading(std::uint8_t level, RichAlignment alignment,
                                                 std::uint8_t quote_depth) {
    require_quote_depth(quote_depth);
    if (level < 1U || level > maximum_rich_heading_level) {
        throw std::invalid_argument("rich text heading level is out of range");
    }
    const RichParagraphFormat format(Heading{level, alignment}, quote_depth);
    return format;
}

RichParagraphFormat RichParagraphFormat::bulleted(std::uint8_t level, RichAlignment alignment,
                                                  std::uint8_t quote_depth) {
    require_quote_depth(quote_depth);
    require_list_level(level);
    const RichParagraphFormat format(Bulleted{level, alignment}, quote_depth);
    return format;
}

RichParagraphFormat RichParagraphFormat::numbered(std::uint8_t level, std::uint32_t ordinal,
                                                  RichAlignment alignment, std::uint8_t quote_depth) {
    require_quote_depth(quote_depth);
    require_list_level(level);
    if (ordinal > maximum_rich_ordinal) throw std::invalid_argument("rich text ordinal is out of range");
    const RichParagraphFormat format(Numbered{level, ordinal, alignment}, quote_depth);
    return format;
}

RichParagraphFormat RichParagraphFormat::code_block(std::uint8_t quote_depth) {
    require_quote_depth(quote_depth);
    const RichParagraphFormat format(CodeBlock{}, quote_depth);
    return format;
}

RichParagraphKind RichParagraphFormat::kind() const noexcept {
    if (std::holds_alternative<Heading>(shape_)) return RichParagraphKind::heading;
    if (std::holds_alternative<Bulleted>(shape_)) return RichParagraphKind::bulleted;
    if (std::holds_alternative<Numbered>(shape_)) return RichParagraphKind::numbered;
    if (std::holds_alternative<CodeBlock>(shape_)) return RichParagraphKind::code_block;
    // Body is the remaining alternative.
    return RichParagraphKind::body;
}

std::uint8_t RichParagraphFormat::level() const noexcept {
    if (const Heading* const heading = std::get_if<Heading>(&shape_)) return (*heading).level;
    if (const Bulleted* const bulleted = std::get_if<Bulleted>(&shape_)) return (*bulleted).level;
    if (const Numbered* const numbered = std::get_if<Numbered>(&shape_)) return (*numbered).level;
    // Body and code block have no level.
    return 0U;
}

std::uint32_t RichParagraphFormat::ordinal() const noexcept {
    if (const Numbered* const numbered = std::get_if<Numbered>(&shape_)) return (*numbered).ordinal;
    // Only a numbered item has an ordinal.
    return 0U;
}

RichAlignment RichParagraphFormat::alignment() const noexcept {
    if (const Body* const body = std::get_if<Body>(&shape_)) return (*body).alignment;
    if (const Heading* const heading = std::get_if<Heading>(&shape_)) return (*heading).alignment;
    if (const Bulleted* const bulleted = std::get_if<Bulleted>(&shape_)) return (*bulleted).alignment;
    if (const Numbered* const numbered = std::get_if<Numbered>(&shape_)) return (*numbered).alignment;
    // A code block is set flush left.
    return RichAlignment::left;
}

bool operator==(const RichParagraphFormat& left, const RichParagraphFormat& right) noexcept {
    return left.kind() == right.kind() && left.level() == right.level() && left.ordinal() == right.ordinal() &&
           left.alignment() == right.alignment() && left.quote_depth() == right.quote_depth();
}

// -- results --------------------------------------------------------------------

RichTextLoadResult::RichTextLoadResult(std::variant<Loaded, Failed> state) noexcept : state_(std::move(state)) {}

RichTextLoadResult RichTextLoadResult::loaded(std::uint64_t losses) noexcept {
    const RichTextLoadResult result(Loaded{losses});
    return result;
}

RichTextLoadResult RichTextLoadResult::failed(RichTextError error, std::uint64_t offset) noexcept {
    const RichTextLoadResult result(Failed{error, offset});
    return result;
}

bool RichTextLoadResult::succeeded() const noexcept { return std::holds_alternative<Loaded>(state_); }

std::uint64_t RichTextLoadResult::losses() const noexcept {
    if (const Loaded* const loaded = std::get_if<Loaded>(&state_)) return (*loaded).losses;
    // A failed load lost nothing it reports.
    return 0U;
}

std::optional<RichTextError> RichTextLoadResult::error() const noexcept {
    if (const Failed* const failed = std::get_if<Failed>(&state_)) return (*failed).error;
    // A load that succeeded has no error.
    return std::nullopt;
}

std::uint64_t RichTextLoadResult::offset() const noexcept {
    if (const Failed* const failed = std::get_if<Failed>(&state_)) return (*failed).offset;
    // A load that succeeded stopped nowhere.
    return 0U;
}

RichTextSaveResult::RichTextSaveResult(std::variant<RichTextSaveReport, Failed> state) noexcept
    : state_(std::move(state)) {}

RichTextSaveResult RichTextSaveResult::saved(const RichTextSaveReport& report) noexcept {
    const RichTextSaveResult result(report);
    return result;
}

RichTextSaveResult RichTextSaveResult::failed(RichTextError error) noexcept {
    const RichTextSaveResult result(Failed{error});
    return result;
}

bool RichTextSaveResult::succeeded() const noexcept { return std::holds_alternative<RichTextSaveReport>(state_); }

std::optional<RichTextSaveReport> RichTextSaveResult::report() const noexcept {
    if (const RichTextSaveReport* const report = std::get_if<RichTextSaveReport>(&state_)) return *report;
    // A failed save has no report.
    return std::nullopt;
}

std::optional<RichTextError> RichTextSaveResult::error() const noexcept {
    if (const Failed* const failed = std::get_if<Failed>(&state_)) return (*failed).error;
    // A save that succeeded has no error.
    return std::nullopt;
}

// -- the model ------------------------------------------------------------------

RichTextModel::RichTextModel(TextStoreLimits limits)
    : store_({}, limits), formats_{RichParagraphFormat::body()}, paragraph_starts_{0U} {}

Utf8Offset RichTextModel::utf8_position(ScalarIndex position) const {
    if (position > store_.scalar_count()) throw std::out_of_range("rich text position is past the end");
    const Utf8Offset offset = store_.utf8_offset(position);
    return offset;
}

Utf8Range RichTextModel::utf8_range(ScalarRange range) const {
    if (range.end < range.start) throw std::out_of_range("rich text range ends before it starts");
    const Utf8Range bytes{utf8_position(range.start), utf8_position(range.end)};
    return bytes;
}

std::size_t RichTextModel::paragraph_at_byte(Utf8Offset position) const {
    const std::vector<std::size_t>::const_iterator after =
        std::upper_bound(paragraph_starts_.begin(), paragraph_starts_.end(), position.value());
    const std::size_t paragraph = static_cast<std::size_t>(after - paragraph_starts_.begin()) - 1U;
    return paragraph;
}

std::size_t RichTextModel::paragraph_at(ScalarIndex position) const {
    const std::size_t paragraph = paragraph_at_byte(utf8_position(position));
    return paragraph;
}

ScalarRange RichTextModel::paragraph_range(std::size_t paragraph) const {
    if (paragraph >= formats_.size()) throw std::out_of_range("rich text paragraph is past the end");
    const std::size_t start = paragraph_starts_[paragraph];
    const std::size_t end = paragraph + 1U < paragraph_starts_.size() ? paragraph_starts_[paragraph + 1U] - 1U
                                                                      : store_.utf8().size();
    const ScalarRange range{store_.scalar_index(Utf8Offset(start)), store_.scalar_index(Utf8Offset(end))};
    return range;
}

const RichParagraphFormat& RichTextModel::paragraph_format(std::size_t paragraph) const {
    if (paragraph >= formats_.size()) throw std::out_of_range("rich text paragraph is past the end");
    return formats_[paragraph];
}

void RichTextModel::set_paragraph_format(ScalarRange range, const RichParagraphFormat& format) {
    const Utf8Range bytes = utf8_range(range);
    const std::size_t first = paragraph_at_byte(bytes.start);
    std::size_t last = paragraph_at_byte(bytes.end);
    // A range that ends where a paragraph starts does not reach into it.
    if (last > first && paragraph_starts_[last] == bytes.end.value()) --last;
    bool changed = false;
    for (std::size_t paragraph = first; paragraph <= last; ++paragraph) {
        if (formats_[paragraph] == format) continue;
        formats_[paragraph] = format;
        changed = true;
    }
    if (changed) ++revision_;
}

const RichRunStyle& RichTextModel::style_of(std::optional<std::size_t> style) const noexcept {
    static const RichRunStyle default_style{};
    if (!style) return default_style;
    return styles_[*style];
}

std::optional<std::size_t> RichTextModel::style_index_at(Utf8Offset position) const {
    const std::optional<TextStyleId> id = store_.style_at(position);
    if (!id) return std::nullopt;
    const std::optional<std::size_t> index = static_cast<std::size_t>((*id).value);
    return index;
}

RichRunStyle RichTextModel::style_at(ScalarIndex position) const {
    const Utf8Offset offset = utf8_position(position);
    if (offset == store_.utf8_size()) {
        const RichRunStyle end_style{};
        return end_style;
    }
    const RichRunStyle style = style_of(style_index_at(offset));
    return style;
}

RichRunStyle RichTextModel::insertion_style(ScalarIndex position) const {
    const Utf8Offset offset = utf8_position(position);
    const std::size_t paragraph = paragraph_at_byte(offset);
    const std::size_t start = paragraph_starts_[paragraph];
    const std::size_t end = paragraph + 1U < paragraph_starts_.size() ? paragraph_starts_[paragraph + 1U] - 1U
                                                                      : store_.utf8().size();
    if (offset.value() > start) {
        const RichRunStyle before = style_of(style_index_at(store_.previous_scalar_boundary(offset)));
        return before;
    }
    if (offset.value() < end) {
        const RichRunStyle after = style_of(style_index_at(offset));
        return after;
    }
    const RichRunStyle empty_paragraph{};
    return empty_paragraph;
}

void RichTextModel::segments(Utf8Range range, std::vector<StyleSegment>& out) const {
    out.clear();
    std::size_t position = range.start.value();
    const std::size_t end = range.end.value();
    for (const TextStyleSpan& span : store_.style_spans()) {
        if (position >= end) break;
        if (span.range.end.value() <= position) continue;
        if (span.range.start.value() >= end) break;
        if (span.range.start.value() > position) {
            out.push_back({{Utf8Offset(position), span.range.start}, std::nullopt});
            position = span.range.start.value();
        }
        const std::size_t segment_end = std::min(span.range.end.value(), end);
        out.push_back({{Utf8Offset(position), Utf8Offset(segment_end)}, static_cast<std::size_t>(span.style.value)});
        position = segment_end;
    }
    if (position < end) out.push_back({{Utf8Offset(position), Utf8Offset(end)}, std::nullopt});
}

std::optional<RichRunStyle> RichTextModel::uniform_style(ScalarRange range) const {
    const Utf8Range bytes = utf8_range(range);
    std::vector<StyleSegment> parts;
    segments(bytes, parts);
    const std::string_view text = store_.utf8();
    bool found = false;
    std::optional<std::size_t> first_style;
    for (const StyleSegment& segment : parts) {
        const std::string_view piece =
            text.substr(segment.range.start.value(), segment.range.end.value() - segment.range.start.value());
        if (!has_content(piece)) continue;
        if (!found) {
            found = true;
            first_style = segment.style;
            continue;
        }
        if (!(style_of(first_style) == style_of(segment.style))) return std::nullopt;
    }
    if (!found) {
        const std::optional<RichRunStyle> insertion = insertion_style(range.start);
        return insertion;
    }
    const std::optional<RichRunStyle> uniform = style_of(first_style);
    return uniform;
}

RichAttributeState RichTextModel::attribute_state(ScalarRange range, RichStyleAttribute attribute) const {
    const Utf8Range bytes = utf8_range(range);
    std::vector<StyleSegment> parts;
    segments(bytes, parts);
    const std::string_view text = store_.utf8();
    bool any_on = false;
    bool any_off = false;
    for (const StyleSegment& segment : parts) {
        const std::string_view piece =
            text.substr(segment.range.start.value(), segment.range.end.value() - segment.range.start.value());
        if (!has_content(piece)) continue;
        if (has_attribute(style_of(segment.style), attribute)) any_on = true;
        else any_off = true;
    }
    if (any_on && any_off) return RichAttributeState::mixed;
    if (any_on) return RichAttributeState::on;
    if (any_off) return RichAttributeState::off;
    const bool inserted_on = has_attribute(insertion_style(range.start), attribute);
    return inserted_on ? RichAttributeState::on : RichAttributeState::off;
}

// Styles are interned: equal styles share one entry. A full table is first
// compacted to the styles the spans use; an edit that would still overflow
// it throws std::length_error before anything changes.
std::optional<TextStyleId> RichTextModel::intern(const RichRunStyle& style) {
    const RichRunStyle default_style{};
    if (style == default_style) return std::nullopt;
    for (std::size_t index = 0U; index < styles_.size(); ++index) {
        if (styles_[index] == style) return TextStyleId{static_cast<std::uint32_t>(index)};
    }
    if (styles_.size() >= store_.limits().maximum_style_spans) {
        throw std::length_error("rich text has more styles than its limit");
    }
    styles_.push_back(style);
    const std::optional<TextStyleId> id = TextStyleId{static_cast<std::uint32_t>(styles_.size() - 1U)};
    return id;
}

void RichTextModel::compact_styles() {
    const std::span<const TextStyleSpan> spans = store_.style_spans();
    std::vector<std::optional<std::uint32_t>> remap(styles_.size());
    std::vector<RichRunStyle> kept;
    std::vector<TextStyleSpan> renamed;
    renamed.reserve(spans.size());
    for (const TextStyleSpan& span : spans) {
        std::optional<std::uint32_t>& target = remap[span.style.value];
        if (!target) {
            target = static_cast<std::uint32_t>(kept.size());
            kept.push_back(styles_[span.style.value]);
        }
        renamed.push_back({span.range, TextStyleId{*target}});
    }
    store_.set_style_spans(renamed);
    styles_ = std::move(kept);
}

template <typename Change> void RichTextModel::restyle(ScalarRange range, const Change& change) {
    const Utf8Range bytes = utf8_range(range);
    if (bytes.start == bytes.end) return;
    std::vector<StyleSegment> parts;
    segments(bytes, parts);
    if (styles_.size() + parts.size() > store_.limits().maximum_style_spans) {
        compact_styles();
        segments(bytes, parts);
    }
    // The changed style of each segment, interned before the spans change.
    std::vector<TextStyleSpan> next;
    next.reserve(store_.style_spans().size() + parts.size() + 2U);
    for (const TextStyleSpan& span : store_.style_spans()) {
        if (span.range.end <= bytes.start || span.range.start >= bytes.end) {
            next.push_back(span);
            continue;
        }
        if (span.range.start < bytes.start) next.push_back({{span.range.start, bytes.start}, span.style});
        if (span.range.end > bytes.end) next.push_back({{bytes.end, span.range.end}, span.style});
    }
    for (const StyleSegment& segment : parts) {
        RichRunStyle style = style_of(segment.style);
        change(style);
        const std::optional<TextStyleId> id = intern(style);
        if (id) next.push_back({segment.range, *id});
    }
    store_.set_style_spans(next);
    ++revision_;
}

void RichTextModel::set_attribute(ScalarRange range, RichStyleAttribute attribute, bool on) {
    restyle(range, SetAttribute{attribute, on});
}

void RichTextModel::set_colour(ScalarRange range, std::optional<RichRgb> colour) {
    restyle(range, SetColour{colour});
}

void RichTextModel::set_background(ScalarRange range, std::optional<RichRgb> background) {
    restyle(range, SetBackground{background});
}

void RichTextModel::set_size_step(ScalarRange range, std::int8_t size_step) {
    if (size_step < minimum_rich_size_step || size_step > maximum_rich_size_step) {
        throw std::invalid_argument("rich text size step is out of range");
    }
    restyle(range, SetSizeStep{size_step});
}

void RichTextModel::set_link(ScalarRange range, std::optional<std::string_view> target) {
    SetLink change;
    if (target) {
        if ((*target).empty()) throw std::invalid_argument("a rich text link needs a target");
        require_admitted_text(*target);
        if ((*target).find('\n') != std::string_view::npos ||
            (*target).find(line_break_utf8) != std::string_view::npos) {
            throw std::invalid_argument("a rich text link target may not break a line");
        }
        change.target = std::string(*target);
    }
    restyle(range, change);
}

void RichTextModel::replace(ScalarRange range, std::string_view text, const RichRunStyle& style) {
    const Utf8Range bytes = utf8_range(range);
    require_admitted_text(text);
    if (style.size_step < minimum_rich_size_step || style.size_step > maximum_rich_size_step) {
        throw std::invalid_argument("rich text size step is out of range");
    }
    // Everything the edit allocates is prepared before the store changes, so
    // a failure leaves the model as it was.
    const std::string_view current = store_.utf8();
    const std::size_t removed_lines =
        count_line_feeds(current.substr(bytes.start.value(), bytes.end.value() - bytes.start.value()));
    const std::size_t paragraph = paragraph_at_byte(bytes.start);
    std::vector<RichParagraphFormat> next_formats;
    next_formats.reserve(formats_.size() - removed_lines + count_line_feeds(text));
    std::vector<std::size_t> next_starts;
    next_starts.reserve(next_formats.capacity());
    for (std::size_t index = 0U; index <= paragraph; ++index) {
        next_formats.push_back(formats_[index]);
        next_starts.push_back(paragraph_starts_[index]);
    }
    for (std::size_t offset = 0U; offset < text.size(); ++offset) {
        if (text[offset] != '\n') continue;
        next_formats.push_back(formats_[paragraph]);
        next_starts.push_back(bytes.start.value() + offset + 1U);
    }
    const std::size_t removed_bytes = bytes.end.value() - bytes.start.value();
    for (std::size_t index = paragraph + 1U + removed_lines; index < formats_.size(); ++index) {
        next_formats.push_back(formats_[index]);
        next_starts.push_back(paragraph_starts_[index] - removed_bytes + text.size());
    }
    if (styles_.size() >= store_.limits().maximum_style_spans) compact_styles();
    const std::optional<TextStyleId> id = intern(style);
    const TextEditResult edit = store_.replace(bytes, text, id);
    static_cast<void>(edit);
    formats_.swap(next_formats);
    paragraph_starts_.swap(next_starts);
    ++revision_;
}

void RichTextModel::clear() {
    std::vector<RichParagraphFormat> next_formats{RichParagraphFormat::body()};
    std::vector<std::size_t> next_starts{0U};
    store_.set_text({});
    formats_.swap(next_formats);
    paragraph_starts_.swap(next_starts);
    styles_.clear();
    ++revision_;
}

void RichTextModel::adopt(std::string_view text, std::span<const TextStyleSpan> spans,
                          std::vector<RichParagraphFormat>&& formats, std::vector<RichRunStyle>&& styles) {
    std::vector<std::size_t> starts{0U};
    for (std::size_t offset = 0U; offset < text.size(); ++offset) {
        if (text[offset] == '\n') starts.push_back(offset + 1U);
    }
    if (formats.size() != starts.size()) {
        throw std::logic_error("a loaded rich text document has one format per paragraph");
    }
    TextStore next(text, store_.limits());
    next.set_style_spans(spans);
    store_ = std::move(next);
    formats_ = std::move(formats);
    styles_ = std::move(styles);
    paragraph_starts_.swap(starts);
    ++revision_;
}

} // namespace gui_forms
