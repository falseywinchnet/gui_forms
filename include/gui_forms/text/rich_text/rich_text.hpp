#pragma once

#include "gui_forms/text/text_store/text_store.hpp"
#include "gui_forms/text/types/text_types.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace gui_forms {

// RichTextBox's run model (stage W, owner decision W-Q1): paragraphs of
// styled runs over an inline markdown-like document, read and written
// through stx_rtf2md. The model holds the text in a TextStore: paragraphs
// are separated by a line feed, and a line break inside a paragraph is
// U+2028 LINE SEPARATOR. Positions are scalar indices into that text.

// An sRGB colour without alpha, as rich text documents carry it.
struct RichRgb final {
    std::uint8_t red{};
    std::uint8_t green{};
    std::uint8_t blue{};
};

[[nodiscard]] bool operator==(const RichRgb& left, const RichRgb& right) noexcept;

// A run's position against the baseline. Superscript and subscript exclude
// each other, so one value holds both.
enum class RichBaseline : std::uint8_t { normal, superscript, subscript };

inline constexpr std::int8_t minimum_rich_size_step = -2;
inline constexpr std::int8_t maximum_rich_size_step = 4;
inline constexpr std::uint8_t maximum_rich_heading_level = 6;
inline constexpr std::uint8_t maximum_rich_list_level = 7;
inline constexpr std::uint8_t maximum_rich_quote_depth = 7;
inline constexpr std::uint32_t maximum_rich_ordinal = 999999999U;
// The scalar that separates paragraphs in the model's text.
inline constexpr char32_t rich_paragraph_separator = U'\n';
// The scalar that breaks a line inside a paragraph (RTF's \line, a
// markdown hard break, a line of a code block).
inline constexpr char32_t rich_line_break = U' ';

// How a run of text is set. The default value is body text with no
// attribute. `link` is the link's target; `size_step` counts steps from the
// body size, minimum_rich_size_step to maximum_rich_size_step.
struct RichRunStyle final {
    bool bold{};
    bool italic{};
    bool underline{};
    bool strikethrough{};
    // Inline code: a monospace span that may carry every other attribute.
    bool code{};
    RichBaseline baseline{RichBaseline::normal};
    std::optional<std::string> link{};
    std::optional<RichRgb> colour{};
    std::optional<RichRgb> background{};
    std::int8_t size_step{};
};

[[nodiscard]] bool operator==(const RichRunStyle& left, const RichRunStyle& right) noexcept;

// The on-or-off attributes of a run, for attribute_state and set_attribute.
// superscript and subscript set the baseline: turning one on turns the
// other off.
enum class RichStyleAttribute : std::uint8_t {
    bold,
    italic,
    underline,
    strikethrough,
    code,
    superscript,
    subscript,
};

// Whether every character of a range has an attribute (on), none has it
// (off), or some have it (mixed).
enum class RichAttributeState : std::uint8_t { off, on, mixed };

enum class RichAlignment : std::uint8_t { left, center, right };

enum class RichParagraphKind : std::uint8_t { body, heading, bulleted, numbered, code_block };

// A paragraph's kind and the fields that kind has. Built only through the
// factories, which reject a value out of range with std::invalid_argument:
// a heading's level is 1 to maximum_rich_heading_level, a list item's 0 to
// maximum_rich_list_level, a numbered item's ordinal 0 to
// maximum_rich_ordinal, and any kind's quote depth 0 to
// maximum_rich_quote_depth. A code block has no alignment (left).
class RichParagraphFormat final {
public:
    [[nodiscard]] static RichParagraphFormat body(RichAlignment alignment = RichAlignment::left,
                                                  std::uint8_t quote_depth = 0U);
    [[nodiscard]] static RichParagraphFormat heading(std::uint8_t level,
                                                     RichAlignment alignment = RichAlignment::left,
                                                     std::uint8_t quote_depth = 0U);
    [[nodiscard]] static RichParagraphFormat bulleted(std::uint8_t level,
                                                      RichAlignment alignment = RichAlignment::left,
                                                      std::uint8_t quote_depth = 0U);
    [[nodiscard]] static RichParagraphFormat numbered(std::uint8_t level, std::uint32_t ordinal,
                                                      RichAlignment alignment = RichAlignment::left,
                                                      std::uint8_t quote_depth = 0U);
    [[nodiscard]] static RichParagraphFormat code_block(std::uint8_t quote_depth = 0U);

    [[nodiscard]] RichParagraphKind kind() const noexcept;
    // A heading's level or a list item's nesting; 0 for other kinds.
    [[nodiscard]] std::uint8_t level() const noexcept;
    // A numbered item's ordinal; 0 for other kinds.
    [[nodiscard]] std::uint32_t ordinal() const noexcept;
    [[nodiscard]] RichAlignment alignment() const noexcept;
    [[nodiscard]] std::uint8_t quote_depth() const noexcept { return quote_depth_; }

    friend bool operator==(const RichParagraphFormat& left, const RichParagraphFormat& right) noexcept;

private:
    struct Body final {
        RichAlignment alignment{RichAlignment::left};
    };
    struct Heading final {
        std::uint8_t level{1U};
        RichAlignment alignment{RichAlignment::left};
    };
    struct Bulleted final {
        std::uint8_t level{};
        RichAlignment alignment{RichAlignment::left};
    };
    struct Numbered final {
        std::uint8_t level{};
        std::uint32_t ordinal{};
        RichAlignment alignment{RichAlignment::left};
    };
    struct CodeBlock final {};
    using Shape = std::variant<Body, Heading, Bulleted, Numbered, CodeBlock>;

    RichParagraphFormat(Shape shape, std::uint8_t quote_depth) noexcept;

    Shape shape_{Body{}};
    std::uint8_t quote_depth_{};
};

// Why a document could not be read or written. malformed carries the byte
// offset where reading stopped.
enum class RichTextError : std::uint8_t {
    malformed,
    limit_exceeded,
    unsupported_encoding,
    effort_exceeded,
};

// The result of RichTextModel::load_markdown and load_rtf: loaded, with the
// number of constructs the reader dropped or kept as plain text (0 when the
// document was read whole), or failed, with the error and the input offset
// where reading stopped. A failed load leaves the model unchanged.
class RichTextLoadResult final {
public:
    [[nodiscard]] static RichTextLoadResult loaded(std::uint64_t losses) noexcept;
    [[nodiscard]] static RichTextLoadResult failed(RichTextError error, std::uint64_t offset) noexcept;

    [[nodiscard]] bool succeeded() const noexcept;
    // The losses of a load that succeeded; 0 for a failure.
    [[nodiscard]] std::uint64_t losses() const noexcept;
    // The error of a load that failed; none when it succeeded.
    [[nodiscard]] std::optional<RichTextError> error() const noexcept;
    // The input offset of a load that failed; 0 when it succeeded.
    [[nodiscard]] std::uint64_t offset() const noexcept;

private:
    struct Loaded final {
        std::uint64_t losses{};
    };
    struct Failed final {
        RichTextError error{RichTextError::malformed};
        std::uint64_t offset{};
    };
    explicit RichTextLoadResult(std::variant<Loaded, Failed> state) noexcept;

    std::variant<Loaded, Failed> state_;
};

// What saving changed so that the document fits the format: the model can
// hold more than a document can say. Empty paragraphs are dropped; a line
// break that ends a paragraph is dropped; a heading's line break becomes a
// space; a heading's size steps and a code block's run styles are dropped
// (styles_simplified counts the runs); a list item nested deeper than the
// item before it allows is raised to the deepest level it may have.
struct RichTextSaveReport final {
    std::size_t empty_paragraphs_dropped{};
    std::size_t line_breaks_dropped{};
    std::size_t line_breaks_in_headings{};
    std::size_t styles_simplified{};
    std::size_t list_levels_lowered{};
};

// The result of RichTextModel::save_markdown and save_rtf: saved, with its
// report, or failed with an error (a document past the writer's limits).
// A failed save leaves the output empty.
class RichTextSaveResult final {
public:
    [[nodiscard]] static RichTextSaveResult saved(const RichTextSaveReport& report) noexcept;
    [[nodiscard]] static RichTextSaveResult failed(RichTextError error) noexcept;

    [[nodiscard]] bool succeeded() const noexcept;
    // The report of a save that succeeded; none for a failure.
    [[nodiscard]] std::optional<RichTextSaveReport> report() const noexcept;
    // The error of a save that failed; none when it succeeded.
    [[nodiscard]] std::optional<RichTextError> error() const noexcept;

private:
    struct Failed final {
        RichTextError error{RichTextError::limit_exceeded};
    };
    explicit RichTextSaveResult(std::variant<RichTextSaveReport, Failed> state) noexcept;

    std::variant<RichTextSaveReport, Failed> state_;
};

// Paragraphs of styled runs. A new model holds one empty body paragraph.
//
// Text: the model's text is its paragraphs joined by
// rich_paragraph_separator, with rich_line_break inside a paragraph. Text
// given to replace may contain both; it may not contain another C0 control
// than tab and line feed, DEL, U+0085, U+2029 or U+FEFF (carriage returns are
// the caller's to turn into line feeds), and must be valid UTF-8, or
// std::invalid_argument is thrown and the model is unchanged.
//
// Paragraphs: replacing text across paragraph separators joins those
// paragraphs into the first, which keeps its format; each separator the new
// text brings splits off a paragraph with the format of the paragraph it is
// typed in.
//
// Styles: a style change over a range applies to every character in it,
// separators included; the queries over a range read only the characters
// that are not separators. A range is checked against scalar_count and an
// out-of-range position throws std::out_of_range.
class RichTextModel final {
public:
    explicit RichTextModel(TextStoreLimits limits = {});

    [[nodiscard]] std::string_view text() const noexcept { return store_.utf8(); }
    [[nodiscard]] ScalarIndex scalar_count() const noexcept { return store_.scalar_count(); }
    // Counts the model's changes: every edit, style or format change, clear
    // and load adds one.
    [[nodiscard]] std::uint64_t revision() const noexcept { return revision_; }

    [[nodiscard]] std::size_t paragraph_count() const noexcept { return formats_.size(); }
    // The paragraph holding position; the last paragraph for the end.
    [[nodiscard]] std::size_t paragraph_at(ScalarIndex position) const;
    // A paragraph's text, without its separator.
    [[nodiscard]] ScalarRange paragraph_range(std::size_t paragraph) const;
    [[nodiscard]] const RichParagraphFormat& paragraph_format(std::size_t paragraph) const;
    // Sets the format of every paragraph the range touches (an empty range:
    // the paragraph holding it).
    void set_paragraph_format(ScalarRange range, const RichParagraphFormat& format);

    // The style of the scalar at position; the default style at the end.
    [[nodiscard]] RichRunStyle style_at(ScalarIndex position) const;
    // The style text typed at position takes: that of the scalar before it
    // in the same paragraph, else of the scalar after it in the same
    // paragraph, else the default style.
    [[nodiscard]] RichRunStyle insertion_style(ScalarIndex position) const;
    // The style every non-separator character of the range has, or none
    // when they differ. An empty range, or one of separators only, has the
    // insertion style at its start.
    [[nodiscard]] std::optional<RichRunStyle> uniform_style(ScalarRange range) const;
    [[nodiscard]] RichAttributeState attribute_state(ScalarRange range, RichStyleAttribute attribute) const;

    void set_attribute(ScalarRange range, RichStyleAttribute attribute, bool on);
    void set_colour(ScalarRange range, std::optional<RichRgb> colour);
    void set_background(ScalarRange range, std::optional<RichRgb> background);
    // Throws std::invalid_argument for a step outside minimum_rich_size_step
    // to maximum_rich_size_step.
    void set_size_step(ScalarRange range, std::int8_t size_step);
    // A target must be non-empty valid UTF-8 without the characters text
    // may not contain, nor a separator; none removes the link.
    void set_link(ScalarRange range, std::optional<std::string_view> target);

    // Replaces the range with text in one style (see the class comment).
    void replace(ScalarRange range, std::string_view text, const RichRunStyle& style);
    // One empty body paragraph.
    void clear();

    // Replace the model's contents with a document read from the dialect
    // (stx_rtf2md's markdown) or from RTF. A failure leaves the model as it
    // was.
    [[nodiscard]] RichTextLoadResult load_markdown(std::string_view markdown);
    [[nodiscard]] RichTextLoadResult load_rtf(std::span<const std::byte> rtf);
    // Write the model as the dialect's canonical text or as RTF into output,
    // which is replaced (see RichTextSaveReport for what saving changes).
    [[nodiscard]] RichTextSaveResult save_markdown(std::string& output) const;
    [[nodiscard]] RichTextSaveResult save_rtf(std::vector<std::byte>& output) const;

private:
    struct StyleSegment final {
        Utf8Range range{};
        // An index into styles_, or none for the default style.
        std::optional<std::size_t> style{};
    };

    [[nodiscard]] Utf8Range utf8_range(ScalarRange range) const;
    [[nodiscard]] Utf8Offset utf8_position(ScalarIndex position) const;
    [[nodiscard]] std::size_t paragraph_at_byte(Utf8Offset position) const;
    [[nodiscard]] const RichRunStyle& style_of(std::optional<std::size_t> style) const noexcept;
    [[nodiscard]] std::optional<std::size_t> style_index_at(Utf8Offset position) const;
    // The range's bytes in order, each segment one style.
    void segments(Utf8Range range, std::vector<StyleSegment>& out) const;
    [[nodiscard]] std::optional<TextStyleId> intern(const RichRunStyle& style);
    // Calls change on the style of every segment of the range and stores
    // the results.
    template <typename Change> void restyle(ScalarRange range, const Change& change);
    void compact_styles();
    // Replaces the contents with a loaded document's parts, or throws
    // (std::length_error past the store's limits) leaving them unchanged.
    void adopt(std::string_view text, std::span<const TextStyleSpan> spans,
               std::vector<RichParagraphFormat>&& formats, std::vector<RichRunStyle>&& styles);

    TextStore store_;
    // One format per paragraph; paragraph_starts_ holds each paragraph's
    // first byte, in order. Their sizes are equal.
    std::vector<RichParagraphFormat> formats_;
    std::vector<std::size_t> paragraph_starts_;
    // The styles the store's spans name, by TextStyleId value. Styles no
    // span uses stay until compact_styles; the default style has no entry.
    std::vector<RichRunStyle> styles_;
    std::uint64_t revision_{};
};

} // namespace gui_forms
