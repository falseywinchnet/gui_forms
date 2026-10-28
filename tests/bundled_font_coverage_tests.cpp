// The compiled coverage tables of the bundled fallback faces (item 4, step 5). For each fallback
// face, the table must be exactly the set of Unicode scalars FreeType maps to a glyph after
// selecting the face's Unicode charmap, as the text engine opens it, so loading a fallback face
// only when its table covers a cluster selects the same face the engine would have selected
// with every face loaded. Checked two ways: the table's ranges equal FreeType's enumeration,
// and every scalar from 0 to 0x10FFFF agrees with FT_Get_Char_Index. The table must also name
// the manifest digest of the face it describes.
//
// `gui_forms_bundled_font_coverage_tests --write <bundled_font_coverage.inc>` regenerates the
// tables from the faces in assets/fonts.

#include "../src/core/resources/fonts/bundled_font_set.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {

using gui_forms::detail::BundledFaceRecord;
using gui_forms::detail::BundledFaceUse;
using gui_forms::detail::CodepointRange;

constexpr char32_t last_scalar = 0x10FFFFU;

// FreeType's library, released on every exit.
class Library final {
public:
    Library() {
        if (FT_Init_FreeType(&library_) != 0) library_ = nullptr;
    }
    ~Library() {
        if (library_ != nullptr) FT_Done_FreeType(library_);
    }
    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;
    [[nodiscard]] FT_Library get() const noexcept { return library_; }

private:
    FT_Library library_{};
};

// One face opened from memory with its Unicode charmap selected, as the engine opens it.
class Face final {
public:
    Face(FT_Library library, const std::vector<unsigned char>& bytes) {
        if (FT_New_Memory_Face(library, bytes.data(), static_cast<FT_Long>(bytes.size()), 0, &face_) != 0)
            face_ = nullptr;
        if (face_ != nullptr && FT_Select_Charmap(face_, FT_ENCODING_UNICODE) != 0) {
            FT_Done_Face(face_);
            face_ = nullptr;
        }
    }
    ~Face() {
        if (face_ != nullptr) FT_Done_Face(face_);
    }
    Face(const Face&) = delete;
    Face& operator=(const Face&) = delete;
    [[nodiscard]] FT_Face get() const noexcept { return face_; }

private:
    FT_Face face_{};
};

std::vector<unsigned char> read_face(std::string_view file_name) {
    const std::filesystem::path path =
        std::filesystem::path(GUI_FORMS_TEST_FONT_DIRECTORY) / std::filesystem::path(file_name);
    std::ifstream stream(path, std::ios::binary);
    std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(stream)),
                                     std::istreambuf_iterator<char>());
    return bytes;
}

// The scalars FreeType maps to a glyph, as sorted disjoint ranges. FT_Get_Next_Char skips
// codes whose glyph index is zero, so this is the set FT_Get_Char_Index accepts.
std::vector<CodepointRange> enumerate(FT_Face face) {
    std::vector<CodepointRange> ranges;
    FT_UInt glyph = 0;
    FT_ULong code = FT_Get_First_Char(face, &glyph);
    while (glyph != 0U) {
        if (code <= last_scalar) {
            const char32_t scalar = static_cast<char32_t>(code);
            if (!ranges.empty() && ranges.back().last + 1U == scalar) {
                ranges.back().last = scalar;
            } else {
                ranges.push_back(CodepointRange{scalar, scalar});
            }
        }
        code = FT_Get_Next_Char(face, code, &glyph);
    }
    return ranges;
}

std::optional<std::string_view> manifest_digest(std::string_view file_name) {
    for (const BundledFaceRecord& record : gui_forms::detail::bundled_face_manifest()) {
        if (record.file_name == file_name) return record.sha256_hex;
    }
    return std::nullopt;
}

struct FaceCoverage final {
    std::string_view file_name;
    std::string_view digest;
    std::vector<CodepointRange> ranges;
};

// Every fallback face's FreeType coverage, in registration order; empty on a failure.
std::vector<FaceCoverage> measure(FT_Library library) {
    std::vector<FaceCoverage> result;
    for (const BundledFaceUse& use : gui_forms::detail::bundled_face_uses()) {
        if (use.role) continue;
        const std::optional<std::string_view> digest = manifest_digest(use.file_name);
        const std::vector<unsigned char> bytes = read_face(use.file_name);
        const Face face(library, bytes);
        if (!digest || bytes.empty() || face.get() == nullptr) {
            std::printf("bundled_font_coverage: cannot open %.*s\n",
                        static_cast<int>(use.file_name.size()), use.file_name.data());
            return {};
        }
        result.push_back(FaceCoverage{use.file_name, *digest, enumerate(face.get())});
    }
    return result;
}

bool write_tables(const std::vector<FaceCoverage>& coverages, const char* path) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out << "// Generated by `gui_forms_bundled_font_coverage_tests --write <this file>`; do not edit.\n";
    out << "// Each table: the Unicode scalars FreeType maps to a glyph in the face's Unicode charmap.\n";
    for (std::size_t index = 0; index < coverages.size(); ++index) {
        const FaceCoverage& coverage = coverages[index];
        out << "// " << coverage.file_name << ", " << coverage.ranges.size() << " ranges\n";
        out << "constexpr CodepointRange coverage_" << index << "[] = {\n";
        char line[64];
        for (std::size_t range = 0; range < coverage.ranges.size(); ++range) {
            std::snprintf(line, sizeof(line), "%s{0x%lXU, 0x%lXU},", range % 4U == 0U ? "    " : " ",
                          static_cast<unsigned long>(coverage.ranges[range].first),
                          static_cast<unsigned long>(coverage.ranges[range].last));
            out << line;
            if (range % 4U == 3U || range + 1U == coverage.ranges.size()) out << "\n";
        }
        out << "};\n";
    }
    out << "constexpr std::array<BundledFaceCoverage, " << coverages.size() << "> generated_coverages{{\n";
    for (std::size_t index = 0; index < coverages.size(); ++index) {
        out << "    {\"" << coverages[index].file_name << "\", \"" << coverages[index].digest
            << "\", coverage_" << index << "},\n";
    }
    out << "}};\n";
    out.close();
    const bool written = !out.fail();
    return written;
}

bool same_ranges(std::span<const CodepointRange> left, const std::vector<CodepointRange>& right) {
    if (left.size() != right.size()) return false;
    for (std::size_t index = 0; index < left.size(); ++index) {
        if (left[index].first != right[index].first || left[index].last != right[index].last) return false;
    }
    return true;
}

bool check_tables(FT_Library library, const std::vector<FaceCoverage>& coverages) {
    if (gui_forms::detail::bundled_face_coverages().size() != coverages.size()) {
        std::printf("bundled_font_coverage: %zu tables for %zu fallback faces; regenerate\n",
                    gui_forms::detail::bundled_face_coverages().size(), coverages.size());
        return false;
    }
    for (const FaceCoverage& measured : coverages) {
        const std::optional<std::span<const CodepointRange>> table =
            gui_forms::detail::bundled_face_coverage(measured.file_name);
        if (!table || !same_ranges(*table, measured.ranges)) {
            std::printf("bundled_font_coverage: %.*s's table is missing, stale or wrong; regenerate\n",
                        static_cast<int>(measured.file_name.size()), measured.file_name.data());
            return false;
        }
        const std::vector<unsigned char> bytes = read_face(measured.file_name);
        const Face face(library, bytes);
        for (char32_t scalar = 0; scalar <= last_scalar; ++scalar) {
            const bool mapped = FT_Get_Char_Index(face.get(), static_cast<FT_ULong>(scalar)) != 0U;
            if (mapped != gui_forms::detail::coverage_contains(*table, scalar)) {
                std::printf("bundled_font_coverage: %.*s disagrees with FreeType at U+%04lX\n",
                            static_cast<int>(measured.file_name.size()), measured.file_name.data(),
                            static_cast<unsigned long>(scalar));
                return false;
            }
        }
    }
    // A face without a table, and a primary, have no coverage.
    const bool primaries_absent = !gui_forms::detail::bundled_face_coverage("Carlito-Regular.ttf") &&
                                  !gui_forms::detail::bundled_face_coverage("no-such-face.ttf");
    if (!primaries_absent) std::printf("bundled_font_coverage: a face without a table reported coverage\n");
    return primaries_absent;
}

} // namespace

int main(int argument_count, char** arguments) {
    const Library library;
    if (library.get() == nullptr) return 2;
    const std::vector<FaceCoverage> coverages = measure(library.get());
    if (coverages.empty()) return 1;
    if (argument_count == 3 && std::string_view(arguments[1]) == "--write") {
        const bool written = write_tables(coverages, arguments[2]);
        return written ? 0 : 1;
    }
    if (!check_tables(library.get(), coverages)) return 1;
    std::printf("bundled_font_coverage: %zu fallback tables match FreeType\n", coverages.size());
    return 0;
}
