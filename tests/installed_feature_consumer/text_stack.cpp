// An installed-package consumer of GUIForms::FreeType and GUIForms::HarfBuzz,
// the pinned text stack `tools/build_sdk.py --features all` exports. It
// includes the installed headers the way Pocket Library's page compositor
// does (ft2build.h, hb.h, hb-ft.h), opens one bundled font with FreeType,
// shapes a Latin string with HarfBuzz through the FreeType face, and requires
// one real glyph per character.
//
// Usage: installed_text_stack_consumer <font file>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb-ft.h>
#include <hb.h>

#include <cstdio>

namespace {

// The text shaped, one glyph per byte (ASCII, no ligatures in Arimo's
// default features for these letters).
constexpr const char* sample_text = "GUI.Forms";
constexpr unsigned int sample_glyph_count = 9U;

// Every handle the consumer opens; release_text_stack() releases the ones
// set, in reverse order of creation. The HarfBuzz font holds its own
// reference to the FreeType face (hb_ft_font_create_referenced), so the face
// is released after the font.
struct TextStack {
    FT_Library library = nullptr;
    FT_Face face = nullptr;
    hb_font_t* font = nullptr;
    hb_buffer_t* buffer = nullptr;
};

void release_text_stack(TextStack& stack) {
    if (stack.buffer != nullptr) hb_buffer_destroy(stack.buffer);
    if (stack.font != nullptr) hb_font_destroy(stack.font);
    if (stack.face != nullptr) FT_Done_Face(stack.face);
    if (stack.library != nullptr) FT_Done_FreeType(stack.library);
    stack = TextStack{};
}

// Shapes sample_text with the font at `font_path`. Returns 0 on success and
// 1 after printing the failed step; `stack` is released either way by the
// caller.
int shape_sample(TextStack& stack, const char* const font_path) {
    const FT_Error initialised = FT_Init_FreeType(&stack.library);
    if (initialised != 0) {
        std::fprintf(stderr, "FT_Init_FreeType failed: %d\n", initialised);
        return 1;
    }
    FT_Int major = 0;
    FT_Int minor = 0;
    FT_Int patch = 0;
    FT_Library_Version(stack.library, &major, &minor, &patch);
    std::printf("FreeType %d.%d.%d, HarfBuzz %s\n", major, minor, patch, hb_version_string());

    const FT_Error opened = FT_New_Face(stack.library, font_path, 0, &stack.face);
    if (opened != 0) {
        std::fprintf(stderr, "FT_New_Face(%s) failed: %d\n", font_path, opened);
        return 1;
    }
    // 16 px at 72 dpi; the size only has to be valid for hb-ft's metrics.
    const FT_Error sized = FT_Set_Char_Size(stack.face, 0, 16 * 64, 72, 72);
    if (sized != 0) {
        std::fprintf(stderr, "FT_Set_Char_Size failed: %d\n", sized);
        return 1;
    }

    stack.font = hb_ft_font_create_referenced(stack.face);
    stack.buffer = hb_buffer_create();
    if (!hb_buffer_allocation_successful(stack.buffer)) {
        std::fprintf(stderr, "hb_buffer_create failed\n");
        return 1;
    }
    hb_buffer_add_utf8(stack.buffer, sample_text, -1, 0, -1);
    hb_buffer_guess_segment_properties(stack.buffer);
    hb_shape(stack.font, stack.buffer, nullptr, 0U);

    unsigned int glyph_count = 0U;
    const hb_glyph_info_t* const glyphs = hb_buffer_get_glyph_infos(stack.buffer, &glyph_count);
    if (glyph_count != sample_glyph_count) {
        std::fprintf(stderr, "shaped %u glyphs, expected %u\n", glyph_count, sample_glyph_count);
        return 1;
    }
    for (unsigned int index = 0U; index < glyph_count; ++index) {
        // Glyph 0 is .notdef: the font lacked the character or shaping failed.
        if (glyphs[index].codepoint == 0U) {
            std::fprintf(stderr, "glyph %u is .notdef\n", index);
            return 1;
        }
    }
    std::printf("shaped %u glyphs\n", glyph_count);
    return 0;
}

}  // namespace

int main(const int argc, char** const argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: installed_text_stack_consumer <font file>\n");
        return 2;
    }
    TextStack stack{};
    const int status = shape_sample(stack, argv[1]);
    release_text_stack(stack);
    return status;
}
