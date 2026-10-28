#include "gui_forms/text.hpp"

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

using namespace gui_forms;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

ScalarRange range(std::size_t start, std::size_t end) {
    const ScalarRange result{ScalarIndex(start), ScalarIndex(end)};
    return result;
}

RichRunStyle bold_style() {
    RichRunStyle style;
    style.bold = true;
    return style;
}

// Each functor makes one call the model must refuse, for require_throws.
struct InsertText final {
    RichTextModel* model{};
    std::string_view text{};
    void operator()() const { (*model).replace(range(0U, 0U), text, RichRunStyle{}); }
};

struct ReplacePastEnd final {
    RichTextModel* model{};
    void operator()() const { (*model).replace(range(0U, 999U), "x", RichRunStyle{}); }
};

struct SetSizeStep final {
    RichTextModel* model{};
    std::int8_t step{};
    void operator()() const { (*model).set_size_step(range(0U, 1U), step); }
};

struct SetLinkTarget final {
    RichTextModel* model{};
    std::string_view target{};
    void operator()() const { (*model).set_link(range(0U, 1U), target); }
};

struct MakeHeading final {
    std::uint8_t level{};
    void operator()() const { static_cast<void>(RichParagraphFormat::heading(level)); }
};

struct MakeNumbered final {
    std::uint32_t ordinal{};
    void operator()() const { static_cast<void>(RichParagraphFormat::numbered(0U, ordinal)); }
};

template <typename Exception, typename Call> void require_throws(const Call& call, const char* message) {
    bool thrown = false;
    try {
        call();
    } catch (const Exception&) {
        thrown = true;
    }
    require(thrown, message);
}

void new_model_and_editing() {
    RichTextModel model;
    require(model.text().empty() && model.paragraph_count() == 1U &&
                model.paragraph_format(0U) == RichParagraphFormat::body() && model.scalar_count() == ScalarIndex(0U),
            "a new model holds one empty body paragraph");

    model.replace(range(0U, 0U), "Hello world", bold_style());
    require(model.text() == "Hello world" && model.style_at(ScalarIndex(0U)) == bold_style() &&
                model.style_at(ScalarIndex(11U)) == RichRunStyle{} &&
                model.insertion_style(ScalarIndex(11U)) == bold_style(),
            "inserted text takes the given style, and typing after it continues it");
    model.replace(range(5U, 5U), " there", RichRunStyle{});
    require(model.text() == "Hello there world" && model.style_at(ScalarIndex(6U)) == RichRunStyle{} &&
                model.style_at(ScalarIndex(12U)) == bold_style(),
            "text inserted inside a run splits it");

    // Paragraphs: a line feed splits off a paragraph in the format of the
    // one it is typed in; removing it joins them into the first.
    model.set_paragraph_format(range(0U, 0U), RichParagraphFormat::heading(2U));
    model.replace(range(5U, 6U), "\n", RichRunStyle{});
    require(model.paragraph_count() == 2U && model.text() == "Hello\nthere world" &&
                model.paragraph_format(1U) == RichParagraphFormat::heading(2U) &&
                model.paragraph_range(1U).start == ScalarIndex(6U) &&
                model.paragraph_range(1U).end == ScalarIndex(17U) && model.paragraph_at(ScalarIndex(6U)) == 1U &&
                model.paragraph_at(ScalarIndex(5U)) == 0U,
            "a line feed splits a paragraph, the new one in the same format");
    model.set_paragraph_format(range(6U, 6U), RichParagraphFormat::bulleted(0U));
    model.replace(range(5U, 6U), " ", RichRunStyle{});
    require(model.paragraph_count() == 1U && model.paragraph_format(0U) == RichParagraphFormat::heading(2U),
            "joining paragraphs keeps the first one's format");
    model.replace(range(0U, 0U), "A\nB\nC\n", RichRunStyle{});
    model.set_paragraph_format(range(0U, 3U), RichParagraphFormat::numbered(0U, 1U));
    require(model.paragraph_count() == 4U && model.paragraph_format(0U) == RichParagraphFormat::numbered(0U, 1U) &&
                model.paragraph_format(1U) == RichParagraphFormat::numbered(0U, 1U) &&
                model.paragraph_format(2U) == RichParagraphFormat::heading(2U),
            "a paragraph format applies to the paragraphs the range reaches, not one it ends at");

    const std::uint64_t revision = model.revision();
    const std::string before(model.text());
    require_throws<std::invalid_argument>(InsertText{&model, "a\rb"}, "a carriage return is refused");
    require_throws<std::invalid_argument>(InsertText{&model, "\xEF\xBB\xBF"}, "a byte-order mark is refused");
    require_throws<std::invalid_argument>(InsertText{&model, "\xE2\x80\xA9"}, "a paragraph separator is refused");
    require_throws<std::invalid_argument>(InsertText{&model, "\xC3"}, "invalid UTF-8 is refused");
    require_throws<std::out_of_range>(ReplacePastEnd{&model}, "a range past the end is refused");
    require(model.text() == before && model.revision() == revision, "a refused edit changes nothing");

    model.clear();
    require(model.text().empty() && model.paragraph_count() == 1U &&
                model.paragraph_format(0U) == RichParagraphFormat::body(),
            "clear leaves one empty body paragraph");
}

void styles_over_ranges() {
    RichTextModel model;
    model.replace(range(0U, 0U), "one two\nthree", RichRunStyle{});
    model.set_attribute(range(0U, 3U), RichStyleAttribute::bold, true);
    require(model.attribute_state(range(0U, 3U), RichStyleAttribute::bold) == RichAttributeState::on &&
                model.attribute_state(range(0U, 7U), RichStyleAttribute::bold) == RichAttributeState::mixed &&
                model.attribute_state(range(4U, 7U), RichStyleAttribute::bold) == RichAttributeState::off &&
                !model.uniform_style(range(0U, 7U)) && model.uniform_style(range(0U, 3U)) == bold_style(),
            "attribute states and the uniform style read a range's characters");
    require(model.uniform_style(range(7U, 8U)) == RichRunStyle{} &&
                model.attribute_state(range(3U, 3U), RichStyleAttribute::bold) == RichAttributeState::on,
            "a range of separators only, or an empty one, has the insertion style");

    model.set_attribute(range(2U, 5U), RichStyleAttribute::superscript, true);
    model.set_attribute(range(4U, 5U), RichStyleAttribute::subscript, true);
    require(model.style_at(ScalarIndex(2U)).baseline == RichBaseline::superscript &&
                model.style_at(ScalarIndex(4U)).baseline == RichBaseline::subscript &&
                model.attribute_state(range(2U, 5U), RichStyleAttribute::superscript) == RichAttributeState::mixed,
            "superscript and subscript replace each other");
    model.set_attribute(range(0U, 13U), RichStyleAttribute::superscript, false);
    model.set_attribute(range(0U, 13U), RichStyleAttribute::subscript, false);
    model.set_attribute(range(0U, 13U), RichStyleAttribute::bold, false);
    require(model.uniform_style(range(0U, 13U)) == RichRunStyle{}, "clearing every attribute leaves body text");

    model.set_colour(range(8U, 13U), RichRgb{200U, 10U, 20U});
    model.set_background(range(8U, 10U), RichRgb{1U, 2U, 3U});
    model.set_size_step(range(8U, 13U), 2);
    model.set_link(range(0U, 3U), std::string_view("https://example.test/a"));
    const RichRunStyle three = model.style_at(ScalarIndex(9U));
    require(three.colour == RichRgb{200U, 10U, 20U} && three.background == RichRgb{1U, 2U, 3U} &&
                three.size_step == 2 && model.style_at(ScalarIndex(0U)).link == std::string("https://example.test/a"),
            "colours, size steps and links apply to their range");
    model.set_link(range(0U, 3U), std::nullopt);
    require(!model.style_at(ScalarIndex(0U)).link, "a link is removed");
    require_throws<std::invalid_argument>(SetSizeStep{&model, 5}, "a size step past the largest is refused");
    require_throws<std::invalid_argument>(SetSizeStep{&model, -3}, "a size step past the smallest is refused");
    require_throws<std::invalid_argument>(SetLinkTarget{&model, ""}, "an empty link target is refused");
    require_throws<std::invalid_argument>(SetLinkTarget{&model, "a\nb"}, "a link target with a line feed is refused");
    require_throws<std::invalid_argument>(MakeHeading{0U}, "heading level 0 is refused");
    require_throws<std::invalid_argument>(MakeHeading{7U}, "heading level 7 is refused");
    require_throws<std::invalid_argument>(MakeNumbered{1000000000U}, "an ordinal past nine digits is refused");
    require(RichParagraphFormat::code_block(1U).alignment() == RichAlignment::left &&
                RichParagraphFormat::numbered(2U, 7U, RichAlignment::right).ordinal() == 7U &&
                RichParagraphFormat::bulleted(3U).level() == 3U && RichParagraphFormat::body().ordinal() == 0U,
            "paragraph formats report their kind's fields");
}

void markdown_load_and_save() {
    const std::string_view source =
        "# Title\n"
        "\n"
        "Some **bold** and [a link](https://example.test) and [under]{u} text\\\n"
        "on two lines\n"
        "\n"
        "- item\n"
        "\n"
        "  - nested\n"
        "\n"
        "3. third\n"
        "\n"
        "> quoted\n"
        "\n"
        "```\n"
        "code line\n"
        "second\n"
        "```\n";
    RichTextModel model;
    const RichTextLoadResult loaded = model.load_markdown(source);
    require(loaded.succeeded() && loaded.losses() == 0U && !loaded.error(), "the dialect loads whole");
    require(model.paragraph_count() == 7U, "one paragraph per block");
    require(model.paragraph_format(0U) == RichParagraphFormat::heading(1U) &&
                model.paragraph_format(1U) == RichParagraphFormat::body() &&
                model.paragraph_format(2U) == RichParagraphFormat::bulleted(0U) &&
                model.paragraph_format(3U) == RichParagraphFormat::bulleted(1U) &&
                model.paragraph_format(4U) == RichParagraphFormat::numbered(0U, 3U) &&
                model.paragraph_format(5U) == RichParagraphFormat::body(RichAlignment::left, 1U) &&
                model.paragraph_format(6U) == RichParagraphFormat::code_block(),
            "block kinds become paragraph formats");
    const std::string_view text = model.text();
    require(text.find("text\xE2\x80\xA8on two lines") != std::string_view::npos &&
                text.find("code line\xE2\x80\xA8second") != std::string_view::npos,
            "a hard break and a code block's line feed become line breaks");
    const std::size_t bold_at = text.find("bold");
    const std::size_t link_at = text.find("a link");
    const std::size_t under_at = text.find("under");
    require(model.style_at(ScalarIndex(bold_at)).bold &&
                model.style_at(ScalarIndex(link_at)).link == std::string("https://example.test") &&
                model.style_at(ScalarIndex(under_at)).underline,
            "run styles are read (the text is ASCII, so bytes are scalars)");

    std::string saved;
    const RichTextSaveResult result = model.save_markdown(saved);
    require(result.succeeded() && saved == source, "the canonical dialect saves byte for byte");
    const std::optional<RichTextSaveReport> report = result.report();
    require(report && (*report).empty_paragraphs_dropped == 0U && (*report).styles_simplified == 0U,
            "a loaded document saves without changes");

    const std::uint64_t revision = model.revision();
    const RichTextLoadResult failed = model.load_markdown(std::string_view("\xC3\x28", 2U));
    require(!failed.succeeded() && failed.error() == RichTextError::malformed && model.revision() == revision &&
                model.paragraph_count() == 7U,
            "a failed load leaves the model as it was");
}

void rtf_round_trip() {
    RichTextModel model;
    static_cast<void>(model.load_markdown("## Head\n\n**Bold** [blue]{color=#0000ff} [up]{sup}\n\n- a\n"));
    std::vector<std::byte> rtf;
    const RichTextSaveResult saved = model.save_rtf(rtf);
    require(saved.succeeded() && rtf.size() > 5U && static_cast<char>(rtf[0]) == '{', "the model saves as RTF");
    RichTextModel copy;
    const RichTextLoadResult loaded = copy.load_rtf(rtf);
    require(loaded.succeeded() && copy.text() == model.text() && copy.paragraph_count() == model.paragraph_count(),
            "RTF reads back the same text and paragraphs");
    for (std::size_t paragraph = 0U; paragraph < model.paragraph_count(); ++paragraph) {
        require(copy.paragraph_format(paragraph) == model.paragraph_format(paragraph),
                "RTF reads back each paragraph's format");
    }
    for (std::size_t index = 0U; index < model.scalar_count().value(); ++index) {
        require(copy.style_at(ScalarIndex(index)) == model.style_at(ScalarIndex(index)),
                "RTF reads back each character's style");
    }
    const std::string_view not_rtf = "plain words";
    const std::vector<std::byte> bytes(reinterpret_cast<const std::byte*>(not_rtf.data()),
                                       reinterpret_cast<const std::byte*>(not_rtf.data()) + not_rtf.size());
    const RichTextLoadResult refused = copy.load_rtf(bytes);
    require(!refused.succeeded() && refused.error() == RichTextError::malformed && copy.text() == model.text(),
            "input without an RTF header is malformed and changes nothing");
}

// What the model holds beyond a document is fitted on save, and reported.
void save_fits_the_document() {
    RichTextModel model;
    RichRunStyle big;
    big.size_step = 2;
    model.replace(range(0U, 0U), "Head\xE2\x80\xA8line\n\nbody\xE2\x80\xA8\nfn x()\nnested", big);
    model.set_paragraph_format(range(0U, 0U), RichParagraphFormat::heading(1U));
    const std::size_t code_start = model.paragraph_range(3U).start.value();
    model.set_paragraph_format(range(code_start, code_start), RichParagraphFormat::code_block());
    const std::size_t nested_start = model.paragraph_range(4U).start.value();
    model.set_paragraph_format(range(nested_start, nested_start), RichParagraphFormat::bulleted(3U));
    std::string saved;
    const RichTextSaveResult result = model.save_markdown(saved);
    const std::optional<RichTextSaveReport> report = result.report();
    require(result.succeeded() && report, "a model outside the document's rules still saves");
    require((*report).empty_paragraphs_dropped == 1U && (*report).line_breaks_dropped == 1U &&
                (*report).line_breaks_in_headings == 1U && (*report).styles_simplified == 2U &&
                (*report).list_levels_lowered == 1U,
            "the report counts each change saving made");
    require(saved == "# Head line\n\n[body]{size=+2}\n\n```\nfn x()\n```\n\n- [nested]{size=+2}\n",
            "the saved text has the fitted document");
}

} // namespace

int main() {
    try {
        new_model_and_editing();
        styles_over_ranges();
        markdown_load_and_save();
        rtf_round_trip();
        save_fits_the_document();
        std::cout << "gui_forms_rich_text_model_tests: all tests passed\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "gui_forms_rich_text_model_tests: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
