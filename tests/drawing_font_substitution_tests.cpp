#include "gui_forms/drawing/font_substitution/font_substitution.hpp"

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <set>
#include <span>
#include <string>
#include <string_view>

namespace {

using gui_drawing::BundledFontFace;
using gui_drawing::FontFaceStyle;
using gui_drawing::FontFamilyResolution;
using gui_drawing::FontMappingClass;
using gui_drawing::FontResolutionSource;
using gui_drawing::FontSubstitution;

[[noreturn]] void fail(const char* expression, int line) {
    std::cerr << "drawing_font_substitution_tests:" << line << ": " << expression << '\n';
    std::exit(1);
}

#define CHECK(expression) do { if (!(expression)) fail(#expression, __LINE__); } while (false)

[[nodiscard]] std::string folded(std::string_view value) {
    std::string result(value);
    for (char& character : result) {
        if (character >= 'A' && character <= 'Z') character = static_cast<char>(character - 'A' + 'a');
    }
    return result;
}

// Decisions 4, 11, 14 to 16: measured substitutions, their aliases and the
// generic names, matched ignoring ASCII case.
void table_maps_requests_to_bundled_families() {
    const FontFamilyResolution calibri = gui_drawing::resolve_font_family("Calibri", FontFaceStyle::regular);
    CHECK(calibri.source == FontResolutionSource::substitution);
    CHECK(calibri.mapping == FontMappingClass::metric_compatible);
    CHECK(calibri.family == "Carlito" && calibri.face_file == "Carlito-Regular.ttf");
    CHECK(calibri.measured_as == "Calibri" && calibri.requested == "Calibri");

    const FontFamilyResolution shouted = gui_drawing::resolve_font_family("CALIBRI", FontFaceStyle::bold_italic);
    CHECK(shouted.source == FontResolutionSource::substitution);
    CHECK(shouted.face_file == "Carlito-BoldItalic.ttf" && shouted.face_style == FontFaceStyle::bold_italic);

    const FontFamilyResolution helvetica = gui_drawing::resolve_font_family("helvetica", FontFaceStyle::italic);
    CHECK(helvetica.source == FontResolutionSource::alias);
    CHECK(helvetica.mapping == FontMappingClass::metric_compatible);
    CHECK(helvetica.measured_as == "Arial" && helvetica.face_file == "Arimo-Italic.ttf");

    const FontFamilyResolution dialog = gui_drawing::resolve_font_family("MS Shell Dlg 2", FontFaceStyle::regular);
    CHECK(dialog.source == FontResolutionSource::alias && dialog.measured_as == "Tahoma");
    CHECK(dialog.family == "Selawik" && dialog.mapping == FontMappingClass::nearest);

    const FontFamilyResolution semibold = gui_drawing::resolve_font_family("Segoe UI Semibold", FontFaceStyle::bold);
    CHECK(semibold.family == "Selawik Semibold" && semibold.face_file == "Selawik-Bold.ttf");

    const FontFamilyResolution generic = gui_drawing::resolve_font_family("sans-serif", FontFaceStyle::regular);
    CHECK(generic.source == FontResolutionSource::generic && generic.mapping == FontMappingClass::none);
    CHECK(generic.family == "Carlito" && generic.measured_as.empty());

    const FontFamilyResolution own = gui_drawing::resolve_font_family("portsmouth rapids", FontFaceStyle::italic);
    CHECK(own.source == FontResolutionSource::bundled && own.family == "Portsmouth Rapids");
    CHECK(own.face_file == "PortsmouthRapids-Italic.ttf" && own.mapping == FontMappingClass::none);
}

// Decision 7: a missing style resolves to the family's nearest face, and
// the answer says so; nothing is synthesized.
void missing_styles_resolve_to_the_nearest_face() {
    const FontFamilyResolution italic = gui_drawing::resolve_font_family("Segoe UI", FontFaceStyle::italic);
    CHECK(italic.requested_style == FontFaceStyle::italic && italic.face_style == FontFaceStyle::regular);
    CHECK(italic.face_file == "Selawik-Regular.ttf");
    const FontFamilyResolution bold_italic = gui_drawing::resolve_font_family("Tahoma", FontFaceStyle::bold_italic);
    CHECK(bold_italic.face_style == FontFaceStyle::bold && bold_italic.face_file == "Selawik-Bold.ttf");
    const FontFamilyResolution mono = gui_drawing::resolve_font_family("Consolas", FontFaceStyle::bold);
    CHECK(mono.family == "Portsmouth Mono" && mono.face_style == FontFaceStyle::regular);
    CHECK(mono.face_file == "PortsmouthMono.ttf");
}

// Decision 10: the table leaves an unknown family unmapped for the host
// step, and its content fallback is the content family.
void unmapped_families_fall_back_to_the_content_family() {
    const FontFamilyResolution unknown = gui_drawing::resolve_font_family("Lucida Grande", FontFaceStyle::bold);
    CHECK(unknown.source == FontResolutionSource::unmapped);
    CHECK(unknown.family.empty() && unknown.face_file.empty());
    CHECK(unknown.requested_style == FontFaceStyle::bold);
    CHECK(gui_drawing::resolve_font_family("", FontFaceStyle::regular).source == FontResolutionSource::unmapped);
    CHECK(gui_drawing::resolve_font_family("Arial ", FontFaceStyle::regular).source == FontResolutionSource::unmapped);
    CHECK(gui_drawing::resolve_font_family("\xc3\x84rial", FontFaceStyle::regular).source ==
          FontResolutionSource::unmapped);

    const FontFamilyResolution content = gui_drawing::content_font_resolution("Lucida Grande", FontFaceStyle::italic);
    CHECK(content.source == FontResolutionSource::content);
    CHECK(content.family == gui_drawing::content_font_family() && content.family == "Carlito");
    CHECK(content.face_file == "Carlito-Italic.ttf" && content.requested == "Lucida Grande");

    CHECK(gui_drawing::font_face_style(0x0fU) == FontFaceStyle::bold_italic);
    CHECK(gui_drawing::font_face_style(0x04U) == FontFaceStyle::regular);
    CHECK(std::strcmp(gui_drawing::font_resolution_source_name(FontResolutionSource::content), "content") == 0);
    CHECK(std::strcmp(gui_drawing::font_mapping_class_name(FontMappingClass::nearest), "nearest") == 0);
}

// The generated records against the bundled directory: every face file
// exists, names are unique ignoring case, every mapped family has a regular
// face, and every measured class names its evidence.
void table_records_are_consistent(const std::filesystem::path& font_directory) {
    const std::span<const BundledFontFace> faces = gui_drawing::bundled_font_faces();
    const std::span<const FontSubstitution> substitutions = gui_drawing::font_substitutions();
    CHECK(!faces.empty() && !substitutions.empty());
    std::set<std::string> family_names;
    std::set<std::string> face_keys;
    for (const BundledFontFace& face : faces) {
        CHECK(std::filesystem::is_regular_file(font_directory / std::string(face.file)));
        family_names.insert(folded(face.family));
        CHECK(face_keys.insert(folded(face.family) + '\x1f' + static_cast<char>('0' + static_cast<int>(face.style)))
                  .second);
    }
    std::set<std::string> requested_names = family_names;
    for (const FontSubstitution& substitution : substitutions) {
        CHECK(requested_names.insert(folded(substitution.requested)).second);
        CHECK(family_names.count(folded(substitution.family)) == 1U);
        const FontFamilyResolution resolution =
            gui_drawing::resolve_font_family(substitution.requested, FontFaceStyle::regular);
        CHECK(resolution.source == substitution.source && !resolution.face_file.empty());
        const bool generic = substitution.source == FontResolutionSource::generic;
        CHECK(generic == (substitution.mapping == FontMappingClass::none));
        CHECK(generic == substitution.evidence.empty());
        CHECK(generic == substitution.measured_as.empty());
    }
    for (const std::string& family : family_names) {
        CHECK(gui_drawing::resolve_font_family(family, FontFaceStyle::regular).face_style == FontFaceStyle::regular);
    }
}

} // namespace

int main(int argument_count, char** arguments) {
    CHECK(argument_count == 2);
    table_maps_requests_to_bundled_families();
    missing_styles_resolve_to_the_nearest_face();
    unmapped_families_fall_back_to_the_content_family();
    table_records_are_consistent(std::filesystem::path(arguments[1]));
    return 0;
}
