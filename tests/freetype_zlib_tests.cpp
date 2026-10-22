// Decision 3A: FreeType is built without its bundled zlib. Its gzip entry
// point must refuse a valid zlib stream as unimplemented (so a WOFF 1 font
// with compressed tables or a gzip-wrapped font is refused too), while an
// ordinary bundled TrueType font still loads.
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GZIP_H
#include FT_SYSTEM_H

#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <type_traits>
#include <iterator>
#include <vector>

namespace {
// A plain heap allocator for the FT_Memory that FT_Gzip_Uncompress takes.
void* allocate(FT_Memory, long size) { return std::malloc(static_cast<std::size_t>(size)); }
void release(FT_Memory, void* block) { std::free(block); }
void* reallocate(FT_Memory, long, long size, void* block) {
    return std::realloc(block, static_cast<std::size_t>(size));
}
// The callbacks take FreeType's declared types (house style §9).
static_assert(std::is_same_v<decltype(&allocate), FT_Alloc_Func>);
static_assert(std::is_same_v<decltype(&release), FT_Free_Func>);
static_assert(std::is_same_v<decltype(&reallocate), FT_Realloc_Func>);

void require(bool condition, const char* message) {
    if (condition) return;
    std::fprintf(stderr, "freetype_zlib_tests: %s\n", message);
    std::exit(1);
}
} // namespace

int main() {
    FT_Library library = nullptr;
    require(FT_Init_FreeType(&library) == 0, "FreeType did not initialize");
    // zlib stream of the single byte 'a'.
    const std::array<FT_Byte, 9> stream{0x78, 0x9c, 0x4b, 0x04, 0x00, 0x00, 0x62, 0x00, 0x62};
    std::array<FT_Byte, 16> output{};
    FT_ULong size = output.size();
    FT_MemoryRec_ memory{nullptr, &allocate, &release, &reallocate};
    const FT_Error inflated = FT_Gzip_Uncompress(&memory, output.data(), &size, stream.data(), stream.size());
    require(inflated == FT_Err_Unimplemented_Feature, "FreeType still inflates compressed data");

    std::ifstream font(GUI_FORMS_TEST_FONT, std::ios::binary);
    require(static_cast<bool>(font), "bundled font missing");
    const std::vector<char> text((std::istreambuf_iterator<char>(font)), std::istreambuf_iterator<char>());
    std::vector<FT_Byte> bytes(text.size());
    for (std::size_t index = 0; index < text.size(); ++index) bytes[index] = static_cast<FT_Byte>(text[index]);
    FT_Face face = nullptr;
    require(FT_New_Memory_Face(library, bytes.data(), static_cast<FT_Long>(bytes.size()), 0, &face) == 0,
            "an uncompressed TrueType font no longer loads");
    require(FT_Done_Face(face) == 0, "face release failed");
    require(FT_Done_FreeType(library) == 0, "FreeType release failed");
    return 0;
}
