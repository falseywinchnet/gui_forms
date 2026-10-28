// RichTextModel's load and save: the conversion between the model and
// stx_rtf2md's Document, and the library's readers and writers around it.

#include "gui_forms/text/rich_text/rich_text.hpp"

#include "stx_rtf2md/document.hpp"
#include "stx_rtf2md/limits.hpp"
#include "stx_rtf2md/markdown.hpp"
#include "stx_rtf2md/rtf.hpp"

#include <algorithm>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

namespace gui_forms {

namespace {

namespace rtf2md = stx_rtf2md;

// The UTF-8 encoding of rich_line_break (U+2028). UTF-8 is self-
// synchronising, so a match at a scalar boundary is always the line break.
constexpr std::string_view line_break_utf8 = "\xE2\x80\xA8";

// An order over styles, so that loading interns each distinct style once
// without a linear search per run.
struct RichStyleOrder final {
    [[nodiscard]] static int compare_rgb(const std::optional<RichRgb>& left,
                                         const std::optional<RichRgb>& right) noexcept {
        if (left.has_value() != right.has_value()) return left.has_value() ? 1 : -1;
        if (!left) return 0;
        const std::uint32_t a = (static_cast<std::uint32_t>((*left).red) << 16U) |
                                (static_cast<std::uint32_t>((*left).green) << 8U) | (*left).blue;
        const std::uint32_t b = (static_cast<std::uint32_t>((*right).red) << 16U) |
                                (static_cast<std::uint32_t>((*right).green) << 8U) | (*right).blue;
        if (a == b) return 0;
        return a < b ? -1 : 1;
    }

    [[nodiscard]] bool operator()(const RichRunStyle& left, const RichRunStyle& right) const noexcept {
        const std::uint32_t left_flags = (left.bold ? 1U : 0U) | (left.italic ? 2U : 0U) |
                                         (left.underline ? 4U : 0U) | (left.strikethrough ? 8U : 0U) |
                                         (left.code ? 16U : 0U) |
                                         (static_cast<std::uint32_t>(left.baseline) << 5U);
        const std::uint32_t right_flags = (right.bold ? 1U : 0U) | (right.italic ? 2U : 0U) |
                                          (right.underline ? 4U : 0U) | (right.strikethrough ? 8U : 0U) |
                                          (right.code ? 16U : 0U) |
                                          (static_cast<std::uint32_t>(right.baseline) << 5U);
        if (left_flags != right_flags) return left_flags < right_flags;
        if (left.size_step != right.size_step) return left.size_step < right.size_step;
        const int colour = compare_rgb(left.colour, right.colour);
        if (colour != 0) return colour < 0;
        const int background = compare_rgb(left.background, right.background);
        if (background != 0) return background < 0;
        if (left.link.has_value() != right.link.has_value()) return !left.link.has_value();
        if (!left.link) return false;
        return *left.link < *right.link;
    }
};

[[nodiscard]] RichTextError error_of(rtf2md::Status status) {
    switch (status) {
    case rtf2md::Status::malformed: return RichTextError::malformed;
    case rtf2md::Status::limit_exceeded: return RichTextError::limit_exceeded;
    case rtf2md::Status::unsupported_encoding: return RichTextError::unsupported_encoding;
    case rtf2md::Status::effort_exceeded: return RichTextError::effort_exceeded;
    case rtf2md::Status::ok:
    case rtf2md::Status::output_too_small:
    case rtf2md::Status::invalid_document:
        // Not a failure the caller can meet: ok is success, the writers are
        // retried at the size they ask for, and the model writes only
        // documents in normal form.
        throw std::logic_error("stx_rtf2md reported a status the model does not expect");
    }
    // A status outside the enumeration is a defect in the library.
    throw std::logic_error("stx_rtf2md reported an unknown status");
}

[[nodiscard]] std::uint64_t total_losses(const rtf2md::LossReport& loss) noexcept {
    std::uint64_t total = 0U;
    for (std::size_t kind = 0U; kind < rtf2md::loss_kind_count; ++kind) {
        total += loss.entry(static_cast<rtf2md::LossKind>(kind)).count;
    }
    return total;
}

[[nodiscard]] RichAlignment alignment_of(rtf2md::Alignment alignment) noexcept {
    if (alignment == rtf2md::Alignment::center) return RichAlignment::center;
    if (alignment == rtf2md::Alignment::right) return RichAlignment::right;
    // Left is the remaining alignment.
    return RichAlignment::left;
}

[[nodiscard]] rtf2md::Alignment document_alignment(RichAlignment alignment) noexcept {
    if (alignment == RichAlignment::center) return rtf2md::Alignment::center;
    if (alignment == RichAlignment::right) return rtf2md::Alignment::right;
    // Left is the remaining alignment.
    return rtf2md::Alignment::left;
}

[[nodiscard]] RichParagraphFormat format_of(const rtf2md::Block& block) {
    const RichAlignment alignment = alignment_of(block.alignment);
    switch (block.kind) {
    case rtf2md::BlockKind::paragraph: return RichParagraphFormat::body(alignment, block.quote_depth);
    case rtf2md::BlockKind::heading: return RichParagraphFormat::heading(block.level, alignment, block.quote_depth);
    case rtf2md::BlockKind::list_item:
        if (block.marker == rtf2md::ListMarker::ordered) {
            return RichParagraphFormat::numbered(block.level, block.ordinal, alignment, block.quote_depth);
        }
        return RichParagraphFormat::bulleted(block.level, alignment, block.quote_depth);
    case rtf2md::BlockKind::code_block: return RichParagraphFormat::code_block(block.quote_depth);
    }
    // A block kind outside the enumeration is a defect in the reader.
    throw std::logic_error("stx_rtf2md read an unknown block kind");
}

[[nodiscard]] std::optional<RichRgb> colour_of(const std::optional<rtf2md::Rgb>& colour) noexcept {
    if (!colour) return std::nullopt;
    const std::optional<RichRgb> result = RichRgb{(*colour).red, (*colour).green, (*colour).blue};
    return result;
}

[[nodiscard]] std::optional<rtf2md::Rgb> document_colour(const std::optional<RichRgb>& colour) noexcept {
    if (!colour) return std::nullopt;
    const std::optional<rtf2md::Rgb> result = rtf2md::Rgb{(*colour).red, (*colour).green, (*colour).blue};
    return result;
}

// The model's parts for one document, before the model adopts them.
struct ModelParts final {
    std::string text;
    std::vector<TextStyleSpan> spans;
    std::vector<RichParagraphFormat> formats;
    std::vector<RichRunStyle> styles;
};

// Interns a style while loading; none for the default style.
class LoadStyles final {
public:
    explicit LoadStyles(std::vector<RichRunStyle>& styles) : styles_(styles) {}

    [[nodiscard]] std::optional<TextStyleId> intern(const RichRunStyle& style) {
        const RichRunStyle default_style{};
        if (style == default_style) return std::nullopt;
        const std::map<RichRunStyle, std::uint32_t, RichStyleOrder>::const_iterator found = index_.find(style);
        if (found != index_.end()) return TextStyleId{(*found).second};
        const std::uint32_t id = static_cast<std::uint32_t>(styles_.size());
        styles_.push_back(style);
        index_.emplace(style, id);
        return TextStyleId{id};
    }

private:
    std::vector<RichRunStyle>& styles_;
    std::map<RichRunStyle, std::uint32_t, RichStyleOrder> index_;
};

[[nodiscard]] RichRunStyle style_of(const rtf2md::Document& document, const rtf2md::RunStyle& style) {
    RichRunStyle result;
    result.bold = style.bold;
    result.italic = style.italic;
    result.underline = style.underline;
    result.strikethrough = style.strikethrough;
    result.code = style.code;
    if (style.superscript) result.baseline = RichBaseline::superscript;
    else if (style.subscript) result.baseline = RichBaseline::subscript;
    if (style.link) {
        const rtf2md::Link& link = document.links[(*style.link).value];
        result.link = document.text.substr(link.target_start.value, link.target_length.value);
    }
    result.colour = colour_of(style.colour);
    result.background = colour_of(style.background);
    result.size_step = style.size_step;
    return result;
}

// Reads a document in normal form into the model's parts: one paragraph per
// block, a code block's line feeds as line breaks.
void read_document(const rtf2md::Document& document, ModelParts& parts) {
    LoadStyles styles(parts.styles);
    for (std::size_t index = 0U; index < document.blocks.size(); ++index) {
        const rtf2md::Block& block = document.blocks[index];
        if (index > 0U) parts.text.push_back('\n');
        parts.formats.push_back(format_of(block));
        const bool code_block = block.kind == rtf2md::BlockKind::code_block;
        for (std::uint32_t offset = 0U; offset < block.run_count; ++offset) {
            const rtf2md::Run& run = document.runs[block.first_run.value + offset];
            if (run.kind == rtf2md::RunKind::line_break) {
                parts.text.append(line_break_utf8);
                continue;
            }
            const std::size_t start = parts.text.size();
            const std::string_view piece(document.text.data() + run.start.value, run.length.value);
            if (code_block) {
                for (const char byte : piece) {
                    if (byte == '\n') parts.text.append(line_break_utf8);
                    else parts.text.push_back(byte);
                }
            } else {
                parts.text.append(piece);
            }
            const std::optional<TextStyleId> id = styles.intern(style_of(document, run.style));
            if (id) parts.spans.push_back({{Utf8Offset(start), Utf8Offset(parts.text.size())}, *id});
        }
    }
    if (parts.formats.empty()) parts.formats.push_back(RichParagraphFormat::body());
}

// One run of a paragraph being saved: a text run names bytes of the
// paragraph's scratch text; a line break names none.
struct PendingRun final {
    rtf2md::RunKind kind{rtf2md::RunKind::text};
    std::size_t start{};
    std::size_t length{};
    // The run's style, or null for the default style; borrowed from the
    // model's style table for the length of the save.
    const RichRunStyle* style{};
};

// Whether two styles write the same run: a heading's runs ignore size steps.
[[nodiscard]] bool same_written_style(const RichRunStyle* left, const RichRunStyle* right, bool heading) noexcept {
    const RichRunStyle default_style{};
    const RichRunStyle& a = left != nullptr ? *left : default_style;
    const RichRunStyle& b = right != nullptr ? *right : default_style;
    if (!heading) return a == b;
    RichRunStyle sized_a = a;
    RichRunStyle sized_b = b;
    sized_a.size_step = 0;
    sized_b.size_step = 0;
    return sized_a == sized_b;
}

// What one save works on: the model's text, spans, styles, paragraph starts
// and formats, borrowed for the call.
struct SaveSource final {
    std::string_view text;
    std::span<const TextStyleSpan> spans;
    std::span<const RichRunStyle> styles;
    std::span<const std::size_t> paragraph_starts;
    std::span<const RichParagraphFormat> formats;
};

// Builds the document for a save, in normal form, and reports what it had
// to change (RichTextSaveReport).
class DocumentBuilder final {
public:
    DocumentBuilder(const SaveSource& source, rtf2md::Document& document, RichTextSaveReport& report)
        : source_(source), document_(document), report_(report) {}

    void build() {
        document_.clear();
        std::size_t span = 0U;
        for (std::size_t paragraph = 0U; paragraph < source_.formats.size(); ++paragraph) {
            const std::size_t start = source_.paragraph_starts[paragraph];
            const std::size_t end = paragraph + 1U < source_.paragraph_starts.size()
                ? source_.paragraph_starts[paragraph + 1U] - 1U
                : source_.text.size();
            collect(start, end, source_.formats[paragraph], span);
            emit(source_.formats[paragraph]);
        }
        for (const std::string_view target : link_targets_) {
            rtf2md::Link link;
            link.target_start.value = static_cast<std::uint32_t>(document_.text.size());
            link.target_length.value = static_cast<std::uint32_t>(target.size());
            document_.text.append(target);
            document_.links.push_back(link);
        }
    }

private:
    [[nodiscard]] const RichRunStyle* style_at(std::size_t offset, std::size_t& span) const noexcept {
        while (span < source_.spans.size() && source_.spans[span].range.end.value() <= offset) ++span;
        if (span < source_.spans.size() && source_.spans[span].range.start.value() <= offset) {
            return &source_.styles[source_.spans[span].style.value];
        }
        return nullptr;
    }

    void add_text(std::string_view bytes, const RichRunStyle* style, bool heading) {
        if (!runs_.empty()) {
            PendingRun& last = runs_.back();
            if (last.kind == rtf2md::RunKind::text && same_written_style(last.style, style, heading)) {
                scratch_.append(bytes);
                last.length += bytes.size();
                return;
            }
        }
        runs_.push_back({rtf2md::RunKind::text, scratch_.size(), bytes.size(), style});
        scratch_.append(bytes);
    }

    // Splits the paragraph [start, end) into runs at style changes and line
    // breaks; a heading's line break becomes a space in the style of the
    // break, a code block's a line feed.
    void collect(std::size_t start, std::size_t end, const RichParagraphFormat& format, std::size_t& span) {
        runs_.clear();
        scratch_.clear();
        const bool heading = format.kind() == RichParagraphKind::heading;
        const bool code_block = format.kind() == RichParagraphKind::code_block;
        std::size_t offset = start;
        while (offset < end) {
            const RichRunStyle* style = style_at(offset, span);
            const bool line_break = source_.text.compare(offset, line_break_utf8.size(), line_break_utf8) == 0;
            if (line_break) {
                if (code_block) {
                    add_text("\n", style, false);
                } else if (heading) {
                    add_text(" ", style, true);
                    ++report_.line_breaks_in_headings;
                } else {
                    runs_.push_back({rtf2md::RunKind::line_break, scratch_.size(), 0U, nullptr});
                }
                offset += line_break_utf8.size();
                continue;
            }
            // The text up to the next line break or style boundary.
            std::size_t next = source_.text.find(line_break_utf8, offset);
            if (next == std::string_view::npos || next > end) next = end;
            if (span < source_.spans.size()) {
                const std::size_t span_start = source_.spans[span].range.start.value();
                const std::size_t span_end = source_.spans[span].range.end.value();
                const std::size_t boundary = span_start > offset ? span_start : span_end;
                next = std::min(next, boundary);
            }
            add_text(source_.text.substr(offset, next - offset), style, heading);
            offset = next;
        }
        if (code_block) {
            // One unstyled run; trailing line feeds are dropped.
            std::size_t styled = 0U;
            for (const PendingRun& run : runs_) {
                if (run.style != nullptr) ++styled;
            }
            report_.styles_simplified += styled;
            while (!scratch_.empty() && scratch_.back() == '\n') {
                scratch_.pop_back();
                ++report_.line_breaks_dropped;
            }
            runs_.clear();
            if (!scratch_.empty()) runs_.push_back({rtf2md::RunKind::text, 0U, scratch_.size(), nullptr});
            return;
        }
        while (!runs_.empty() && runs_.back().kind == rtf2md::RunKind::line_break) {
            runs_.pop_back();
            ++report_.line_breaks_dropped;
        }
    }

    [[nodiscard]] rtf2md::RunStyle written_style(const RichRunStyle* style, bool heading) {
        rtf2md::RunStyle result;
        if (style == nullptr) {
            open_link_.reset();
            return result;
        }
        const RichRunStyle& source = *style;
        result.bold = source.bold;
        result.italic = source.italic;
        result.underline = source.underline;
        result.strikethrough = source.strikethrough;
        result.code = source.code;
        result.superscript = source.baseline == RichBaseline::superscript;
        result.subscript = source.baseline == RichBaseline::subscript;
        result.colour = document_colour(source.colour);
        result.background = document_colour(source.background);
        result.size_step = heading ? std::int8_t{0} : source.size_step;
        if (heading && source.size_step != 0) ++report_.styles_simplified;
        if (!source.link) {
            open_link_.reset();
            return result;
        }
        // A link continues while its target does, across line breaks.
        const bool continues = open_link_ && link_targets_[*open_link_] == *source.link;
        if (!continues) {
            open_link_ = static_cast<std::uint32_t>(link_targets_.size());
            link_targets_.push_back(*source.link);
        }
        result.link = rtf2md::LinkIndex{*open_link_};
        return result;
    }

    [[nodiscard]] rtf2md::Block written_block(const RichParagraphFormat& format) {
        rtf2md::Block block;
        block.quote_depth = format.quote_depth();
        block.alignment = document_alignment(format.alignment());
        const RichParagraphKind kind = format.kind();
        if (kind == RichParagraphKind::heading) {
            block.kind = rtf2md::BlockKind::heading;
            block.level = format.level();
        } else if (kind == RichParagraphKind::bulleted || kind == RichParagraphKind::numbered) {
            block.kind = rtf2md::BlockKind::list_item;
            block.level = list_level(format);
            if (kind == RichParagraphKind::numbered) {
                block.marker = rtf2md::ListMarker::ordered;
                block.ordinal = format.ordinal();
            }
        } else if (kind == RichParagraphKind::code_block) {
            block.kind = rtf2md::BlockKind::code_block;
            block.alignment = rtf2md::Alignment::left;
        } else {
            // A body paragraph is the remaining kind.
            block.kind = rtf2md::BlockKind::paragraph;
        }
        return block;
    }

    // A nested item needs an item before it at its parent's level or deeper
    // in the same quote; deeper nesting is lowered to what that allows.
    [[nodiscard]] std::uint8_t list_level(const RichParagraphFormat& format) {
        std::uint8_t allowed = 0U;
        if (!document_.blocks.empty()) {
            const rtf2md::Block& previous = document_.blocks.back();
            if (previous.kind == rtf2md::BlockKind::list_item && previous.quote_depth == format.quote_depth()) {
                allowed = static_cast<std::uint8_t>(previous.level + 1U);
            }
        }
        if (format.level() <= allowed) return format.level();
        ++report_.list_levels_lowered;
        return allowed;
    }

    void emit(const RichParagraphFormat& format) {
        bool has_text = false;
        for (const PendingRun& run : runs_) {
            if (run.kind == rtf2md::RunKind::text) has_text = true;
        }
        if (!has_text) {
            report_.line_breaks_dropped += runs_.size();
            ++report_.empty_paragraphs_dropped;
            return;
        }
        rtf2md::Block block = written_block(format);
        block.first_run.value = static_cast<std::uint32_t>(document_.runs.size());
        block.run_count = static_cast<std::uint32_t>(runs_.size());
        const bool heading = block.kind == rtf2md::BlockKind::heading;
        open_link_.reset();
        for (const PendingRun& pending : runs_) {
            rtf2md::Run run;
            run.kind = pending.kind;
            run.start.value = static_cast<std::uint32_t>(document_.text.size());
            if (pending.kind == rtf2md::RunKind::text) {
                run.length.value = static_cast<std::uint32_t>(pending.length);
                run.style = written_style(pending.style, heading);
                document_.text.append(scratch_, pending.start, pending.length);
            }
            document_.runs.push_back(run);
        }
        open_link_.reset();
        document_.blocks.push_back(block);
    }

    const SaveSource& source_;
    rtf2md::Document& document_;
    RichTextSaveReport& report_;
    // The paragraph being saved: its runs and their text.
    std::vector<PendingRun> runs_;
    std::string scratch_;
    // Link targets in order of first use, borrowed from the style table.
    std::vector<std::string_view> link_targets_;
    std::optional<std::uint32_t> open_link_;
};

// Builds and checks a save's document. A document that fails the normal
// form is a defect in the builder.
void build_document(const SaveSource& source, rtf2md::Document& document, RichTextSaveReport& report) {
    DocumentBuilder builder(source, document, report);
    builder.build();
    const rtf2md::NormalFormCheck check = rtf2md::check_normal_form(document);
    if (check.violation != rtf2md::NormalFormViolation::none) {
        throw std::logic_error("RichTextModel built a document outside stx_rtf2md's normal form");
    }
}

} // namespace

RichTextLoadResult RichTextModel::load_markdown(std::string_view markdown) {
    const rtf2md::Limits limits{};
    rtf2md::MarkdownReader reader(limits);
    rtf2md::Document document;
    rtf2md::LossReport loss;
    const rtf2md::Result read = reader.read(std::span<const char>(markdown.data(), markdown.size()), document, loss);
    if (read.status != rtf2md::Status::ok) return RichTextLoadResult::failed(error_of(read.status), read.stop.value);
    ModelParts parts;
    read_document(document, parts);
    try {
        adopt(parts.text, parts.spans, std::move(parts.formats), std::move(parts.styles));
    } catch (const std::length_error&) {
        return RichTextLoadResult::failed(RichTextError::limit_exceeded, 0U);
    }
    return RichTextLoadResult::loaded(total_losses(loss));
}

RichTextLoadResult RichTextModel::load_rtf(std::span<const std::byte> rtf) {
    const rtf2md::Limits limits{};
    rtf2md::RtfReader reader(limits);
    rtf2md::Document document;
    rtf2md::LossReport loss;
    const rtf2md::Result read = reader.read(rtf, document, loss);
    if (read.status != rtf2md::Status::ok) return RichTextLoadResult::failed(error_of(read.status), read.stop.value);
    ModelParts parts;
    read_document(document, parts);
    try {
        adopt(parts.text, parts.spans, std::move(parts.formats), std::move(parts.styles));
    } catch (const std::length_error&) {
        return RichTextLoadResult::failed(RichTextError::limit_exceeded, 0U);
    }
    return RichTextLoadResult::loaded(total_losses(loss));
}

RichTextSaveResult RichTextModel::save_markdown(std::string& output) const {
    output.clear();
    const SaveSource source{store_.utf8(), store_.style_spans(), styles_, paragraph_starts_, formats_};
    rtf2md::Document document;
    RichTextSaveReport report;
    build_document(source, document, report);
    const rtf2md::Limits limits{};
    rtf2md::MarkdownWriter writer(limits);
    // The writer states the exact size it needs when the first guess is short.
    output.resize(document.text.size() * 2U + 64U);
    rtf2md::Result written = writer.write(document, std::span<char>(output.data(), output.size()));
    if (written.status == rtf2md::Status::output_too_small) {
        output.resize(written.required);
        written = writer.write(document, std::span<char>(output.data(), output.size()));
    }
    if (written.status != rtf2md::Status::ok) {
        output.clear();
        return RichTextSaveResult::failed(error_of(written.status));
    }
    output.resize(written.produced);
    return RichTextSaveResult::saved(report);
}

RichTextSaveResult RichTextModel::save_rtf(std::vector<std::byte>& output) const {
    output.clear();
    const SaveSource source{store_.utf8(), store_.style_spans(), styles_, paragraph_starts_, formats_};
    rtf2md::Document document;
    RichTextSaveReport report;
    build_document(source, document, report);
    const rtf2md::Limits limits{};
    rtf2md::RtfWriter writer(limits);
    output.resize(document.text.size() * 2U + 1024U);
    rtf2md::Result written = writer.write(document, std::span<std::byte>(output.data(), output.size()));
    if (written.status == rtf2md::Status::output_too_small) {
        output.resize(written.required);
        written = writer.write(document, std::span<std::byte>(output.data(), output.size()));
    }
    if (written.status != rtf2md::Status::ok) {
        output.clear();
        return RichTextSaveResult::failed(error_of(written.status));
    }
    output.resize(written.produced);
    return RichTextSaveResult::saved(report);
}

} // namespace gui_forms
