#include "font_metrics_differential.hpp"

#include "harfbuzz_font_engine.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ADVANCES_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H
#include FT_TRUETYPE_TABLES_H

#include <algorithm>
#include <optional>

// One face's measurements: facts and per-code-point advances from FreeType in
// font units, then line metrics, corpus string widths and greedy line breaks
// from the bundled HarfBuzz engine, which shapes exactly as GUI.Forms does
// (ADR-004: unhinted advances at the logical size).

namespace gui_forms::tools::font_metrics {

namespace {

// Owns one FreeType library for the measurement of one face.
class FreeTypeLibrary final {
public:
    FreeTypeLibrary() {
        if (FT_Init_FreeType(&handle_) != 0) throw std::runtime_error("FreeType initialization failed");
    }
    ~FreeTypeLibrary() { FT_Done_FreeType(handle_); }
    FreeTypeLibrary(const FreeTypeLibrary&) = delete;
    FreeTypeLibrary& operator=(const FreeTypeLibrary&) = delete;
    [[nodiscard]] FT_Library handle() const noexcept { return handle_; }

private:
    FT_Library handle_{};
};

// Owns one memory face; the encoded bytes must outlive it.
class FreeTypeFace final {
public:
    FreeTypeFace(const FreeTypeLibrary& library, std::span<const std::byte> encoded) {
        const FT_Byte* const base = reinterpret_cast<const FT_Byte*>(encoded.data());
        const FT_Long length = static_cast<FT_Long>(encoded.size());
        if (FT_New_Memory_Face(library.handle(), base, length, 0, &handle_) != 0) {
            throw InputRejected("FreeType refused the face");
        }
    }
    ~FreeTypeFace() { FT_Done_Face(handle_); }
    FreeTypeFace(const FreeTypeFace&) = delete;
    FreeTypeFace& operator=(const FreeTypeFace&) = delete;
    [[nodiscard]] FT_Face handle() const noexcept { return handle_; }

private:
    FT_Face handle_{};
};

void append_utf8(std::string& output, char32_t scalar) {
    if (scalar < 0x80U) {
        output.push_back(static_cast<char>(scalar));
    } else if (scalar < 0x800U) {
        output.push_back(static_cast<char>(0xC0U | (scalar >> 6U)));
        output.push_back(static_cast<char>(0x80U | (scalar & 0x3FU)));
    } else if (scalar < 0x10000U) {
        output.push_back(static_cast<char>(0xE0U | (scalar >> 12U)));
        output.push_back(static_cast<char>(0x80U | ((scalar >> 6U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | (scalar & 0x3FU)));
    } else {
        output.push_back(static_cast<char>(0xF0U | (scalar >> 18U)));
        output.push_back(static_cast<char>(0x80U | ((scalar >> 12U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | ((scalar >> 6U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | (scalar & 0x3FU)));
    }
}

// Decodes a Windows-platform name (UTF-16BE). An unpaired surrogate becomes
// U+FFFD.
std::string utf8_from_utf16be(const FT_Byte* units, FT_UInt byte_count) {
    std::string output{};
    output.reserve(byte_count);
    const FT_UInt unit_count = byte_count / 2U;
    FT_UInt index = 0U;
    while (index < unit_count) {
        const char32_t unit = (static_cast<char32_t>(units[index * 2U]) << 8U) | units[index * 2U + 1U];
        ++index;
        const bool high = unit >= 0xD800U && unit <= 0xDBFFU;
        if (high && index < unit_count) {
            const char32_t next = (static_cast<char32_t>(units[index * 2U]) << 8U) | units[index * 2U + 1U];
            if (next >= 0xDC00U && next <= 0xDFFFU) {
                ++index;
                const char32_t scalar = 0x10000U + ((unit - 0xD800U) << 10U) + (next - 0xDC00U);
                append_utf8(output, scalar);
                continue;
            }
        }
        const bool surrogate = unit >= 0xD800U && unit <= 0xDFFFU;
        append_utf8(output, surrogate ? U'�' : unit);
    }
    return output;
}

// The name with name_id, preferring Windows Unicode US English, then any
// Windows Unicode name, then Macintosh Roman read as ASCII.
std::string face_name(FT_Face face, FT_UShort name_id) {
    const FT_UInt count = FT_Get_Sfnt_Name_Count(face);
    std::optional<std::string> windows_any{};
    std::optional<std::string> macintosh{};
    for (FT_UInt index = 0U; index < count; ++index) {
        FT_SfntName name{};
        if (FT_Get_Sfnt_Name(face, index, &name) != 0 || name.name_id != name_id) continue;
        if (name.platform_id == TT_PLATFORM_MICROSOFT && name.encoding_id == TT_MS_ID_UNICODE_CS) {
            std::string decoded = utf8_from_utf16be(name.string, name.string_len);
            if (name.language_id == TT_MS_LANGID_ENGLISH_UNITED_STATES) return decoded;
            if (!windows_any) windows_any = std::move(decoded);
        } else if (name.platform_id == TT_PLATFORM_MACINTOSH && name.encoding_id == TT_MAC_ID_ROMAN && !macintosh) {
            std::string ascii{};
            ascii.reserve(name.string_len);
            for (FT_UInt octet = 0U; octet < name.string_len; ++octet) {
                const FT_Byte value = name.string[octet];
                ascii.push_back(value < 0x80U ? static_cast<char>(value) : '?');
            }
            macintosh = std::move(ascii);
        }
    }
    if (windows_any) return *windows_any;
    if (macintosh) return *macintosh;
    return std::string{};
}

FaceFacts read_facts(FT_Face face, std::string_view file_name, std::span<const std::byte> encoded) {
    const TT_HoriHeader* const horizontal = static_cast<const TT_HoriHeader*>(FT_Get_Sfnt_Table(face, FT_SFNT_HHEA));
    const TT_OS2* const os2 = static_cast<const TT_OS2*>(FT_Get_Sfnt_Table(face, FT_SFNT_OS2));
    if (horizontal == nullptr || os2 == nullptr) throw InputRejected("face lacks hhea or OS/2");
    FaceFacts facts{};
    facts.file_name = std::string(file_name);
    facts.byte_count = encoded.size();
    facts.sha256_hex = sha256_hex(encoded);
    facts.family = face_name(face, TT_NAME_ID_FONT_FAMILY);
    facts.version = face_name(face, TT_NAME_ID_VERSION_STRING);
    facts.units_per_em = (*face).units_per_EM;
    facts.weight_class = (*os2).usWeightClass;
    facts.italic = ((*os2).fsSelection & 1U) != 0U;
    facts.hhea_ascender = (*horizontal).Ascender;
    facts.hhea_descender = (*horizontal).Descender;
    facts.hhea_line_gap = (*horizontal).Line_Gap;
    facts.typo_ascender = (*os2).sTypoAscender;
    facts.typo_descender = (*os2).sTypoDescender;
    facts.typo_line_gap = (*os2).sTypoLineGap;
    facts.win_ascent = (*os2).usWinAscent;
    facts.win_descent = (*os2).usWinDescent;
    facts.glyph_count = static_cast<std::uint32_t>((*face).num_glyphs);
    if (facts.units_per_em == 0U) throw InputRejected("face has zero units per em");
    return facts;
}

std::vector<CodepointAdvance> read_advances(FT_Face face) {
    std::size_t count = 0U;
    for (const CodepointRange& range : advance_coverage) count += range.last - range.first + 1U;
    std::vector<CodepointAdvance> advances{};
    advances.reserve(count);
    for (const CodepointRange& range : advance_coverage) {
        for (char32_t codepoint = range.first; codepoint <= range.last; ++codepoint) {
            CodepointAdvance advance{};
            advance.codepoint = codepoint;
            const FT_UInt glyph = FT_Get_Char_Index(face, codepoint);
            if (glyph != 0U) {
                FT_Fixed units{};
                if (FT_Get_Advance(face, glyph, FT_LOAD_NO_SCALE, &units) != 0) {
                    throw std::runtime_error("FreeType advance query failed");
                }
                advance.mapped = true;
                advance.advance_units = static_cast<std::int32_t>(units);
            }
            advances.push_back(advance);
        }
    }
    return advances;
}

FontSpec measured_font(double size_px) {
    FontSpec font{};
    font.role = FontRole::content;
    font.size = size_px;
    font.weight = 400U;
    font.italic = false;
    font.letter_spacing = 0.0;
    return font;
}

// The words of a paragraph: views into it, split at single spaces (the corpus
// parser rejects leading, trailing and doubled spaces).
std::vector<std::string_view> split_words(std::string_view paragraph) {
    std::vector<std::string_view> words{};
    words.reserve(static_cast<std::size_t>(std::count(paragraph.begin(), paragraph.end(), ' ')) + 1U);
    std::size_t start = 0U;
    while (start <= paragraph.size()) {
        const std::size_t space = paragraph.find(' ', start);
        const std::size_t end = space == std::string_view::npos ? paragraph.size() : space;
        words.push_back(paragraph.substr(start, end - start));
        start = end + 1U;
    }
    return words;
}

// Greedy breaking: a line takes words while its shaped width fits; a word
// wider than the line stays alone on its own line. line is the caller's
// reusable buffer, reserved to the paragraph's length.
std::vector<std::uint32_t> wrap_paragraph(render::text::HarfBuzzFontEngine& engine,
                                          const std::vector<std::string_view>& words,
                                          FontSpec font, double width_px, std::string& line) {
    std::vector<std::uint32_t> starts{};
    line.assign(words.front());
    for (std::size_t index = 1U; index < words.size(); ++index) {
        line.push_back(' ');
        line.append(words[index]);
        const render::text::ShapedText shaped = engine.shape(line, font);
        if (shaped.width <= width_px) continue;
        starts.push_back(static_cast<std::uint32_t>(index));
        line.assign(words[index]);
    }
    return starts;
}

std::vector<EngineSizeMeasurement> measure_sizes(std::span<const std::byte> encoded, const Corpus& corpus) {
    render::text::HarfBuzzFontEngine engine{};
    const std::optional<FontFaceId> registered = engine.register_typeface(FontRole::content, 400U, false, encoded);
    if (!registered) throw InputRejected("the bundled engine refused the face");
    std::vector<std::vector<std::string_view>> paragraph_words{};
    paragraph_words.reserve(corpus.paragraphs.size());
    std::size_t longest = 0U;
    for (const std::string& paragraph : corpus.paragraphs) {
        paragraph_words.push_back(split_words(paragraph));
        longest = std::max(longest, paragraph.size());
    }
    std::string line{};
    line.reserve(longest);

    std::vector<EngineSizeMeasurement> sizes{};
    sizes.reserve(measured_sizes_px.size());
    for (const double size_px : measured_sizes_px) {
        const FontSpec font = measured_font(size_px);
        const std::optional<render::text::PrimaryLineMetrics> metrics = engine.primary_line_metrics(font);
        if (!metrics) throw std::runtime_error("the registered face has no primary line metrics");
        EngineSizeMeasurement size{};
        size.size_px = size_px;
        size.ascent_px = (*metrics).ascent_device;
        size.descent_px = (*metrics).descent_device;
        size.line_gap_px = (*metrics).line_gap_device;
        size.string_widths_px.reserve(corpus.strings.size());
        for (const std::string& text : corpus.strings) {
            const render::text::ShapedText shaped = engine.shape(text, font);
            size.string_widths_px.push_back(shaped.width);
            size.missing_clusters += shaped.missing_clusters;
        }
        size.line_starts.reserve(paragraph_words.size() * wrap_widths_em.size());
        for (const std::vector<std::string_view>& words : paragraph_words) {
            for (const double width_em : wrap_widths_em) {
                size.line_starts.push_back(wrap_paragraph(engine, words, font, width_em * size_px, line));
            }
        }
        sizes.push_back(std::move(size));
    }
    return sizes;
}

} // namespace

FaceMeasurement measure_face(std::string_view file_name, std::span<const std::byte> encoded,
                             const Corpus& corpus, bool with_gdi) {
    validate_face_bytes(encoded);
    FaceMeasurement measurement{};
    {
        const FreeTypeLibrary library{};
        const FreeTypeFace face(library, encoded);
        measurement.facts = read_facts(face.handle(), file_name, encoded);
        measurement.advances = read_advances(face.handle());
    }
    measurement.sizes = measure_sizes(encoded, corpus);
    if (with_gdi) {
        measurement.gdi = measure_with_gdi(encoded, measurement.facts, corpus);
    } else {
        measurement.gdi.unavailable_reason = "not requested";
    }
    return measurement;
}

} // namespace gui_forms::tools::font_metrics
