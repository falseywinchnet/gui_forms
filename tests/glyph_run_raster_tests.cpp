#include "../src/render/text/glyph_run/glyph_run_raster.hpp"

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// ADR-004 glyph-run rasterization: light auto-hinting, grayscale, integer
// 26.6 placement with a snapped baseline and a 1/64 pixel horizontal phase.
namespace {
using namespace gui_forms;
using namespace gui_forms::render::text;

void require(const bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

std::vector<std::byte> read_file(const char* path) {
    std::ifstream stream(path, std::ios::binary);
    require(static_cast<bool>(stream), "font fixture opens");
    const std::vector<char> characters((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    std::vector<std::byte> bytes(characters.size());
    for (std::size_t index = 0; index < characters.size(); ++index) bytes[index] = static_cast<std::byte>(characters[index]);
    return bytes;
}

double ink(const GlyphRunCoverage& coverage) {
    double total = 0.0;
    for (const std::uint8_t value : coverage.pixels) total += static_cast<double>(value) / 255.0;
    return total;
}

const std::string corpus = "Hamburgefonstiv quick brown fox 0123456789 Help";

void origin_keeps_phase_and_snaps_baseline() {
    const GlyphRunOrigin positive = glyph_run_origin(10.25, 20.6);
    require(positive.pixel_x == 10 && positive.phase_x == 16 && positive.pixel_y == 21, "positive origin phase and baseline");
    const GlyphRunOrigin negative = glyph_run_origin(-0.5, -2.4);
    require(negative.pixel_x == -1 && negative.phase_x == 32 && negative.pixel_y == -2, "negative origin floors its pixel");
    const GlyphRunOrigin whole = glyph_run_origin(7.0, 3.0);
    require(whole.pixel_x == 7 && whole.phase_x == 0 && whole.pixel_y == 3, "whole-pixel origin has no phase");
}

void rasterization_is_deterministic_and_scales(const char* font_path) {
    HarfBuzzFontEngine engine;
    require(engine.register_typeface(FontRole::control, 400, false, read_file(font_path)).has_value(), "font registers");
    const FontSpec font{FontRole::control, 13.0, 400, false};
    const ShapedText shaped = engine.shape(corpus, font);
    GlyphRunRasterizer rasterizer(engine);
    const GlyphRunCoverage first = rasterizer.rasterize(shaped, font, 1.0, 0);
    const GlyphRunCoverage again = rasterizer.rasterize(shaped, font, 1.0, 0);
    require(first.width > 0 && first.height > 0 && first.missing_glyphs == 0U, "run rasterizes completely");
    require(first.pixels == again.pixels && first.left == again.left && first.top == again.top,
            "identical input gives identical coverage");
    // The mask spans the shaped advance (within a pixel of side bearings).
    require(std::abs(static_cast<double>(first.width) - shaped.width) < 3.0, "mask width follows the shaped width");
    // Ink is conserved across subpixel phases.
    const GlyphRunCoverage shifted = rasterizer.rasterize(shaped, font, 1.0, 32);
    require(shifted.pixels != first.pixels, "phase moves coverage");
    require(std::abs(ink(shifted) - ink(first)) / ink(first) < 0.03, "phase preserves ink");
    // Ink grows with the square of the device scale.
    const GlyphRunCoverage doubled = rasterizer.rasterize(shaped, font, 2.0, 0);
    const double ratio = ink(doubled) / ink(first);
    require(ratio > 3.6 && ratio < 4.4, "ink scales with device area");
    require(std::abs(static_cast<double>(doubled.width) - shaped.width * 2.0) < 4.0, "2x mask spans twice the width");
}

// FNV-1a over a mask and its placement. Printed so runs on different
// platforms can be compared for byte identity (ADR-004).
std::uint64_t coverage_hash(const GlyphRunCoverage& coverage) {
    std::uint64_t value = 1469598103934665603ULL;
    const std::int64_t header[4]{coverage.left, coverage.top, coverage.width, coverage.height};
    for (const std::int64_t field : header) {
        value ^= static_cast<std::uint64_t>(field);
        value *= 1099511628211ULL;
    }
    for (const std::uint8_t byte : coverage.pixels) {
        value ^= byte;
        value *= 1099511628211ULL;
    }
    return value;
}

// Expected hashes in report order (rapids then carlito; size; scale; phase),
// identical on macOS arm64 and Alpine aarch64 musl, 2026-10-11. Every CI
// platform must reproduce them.
constexpr std::uint64_t expected_identity[36]{
    0xc5214daa69de2890ULL,  // rapids 9 1 0
    0xc90c2fd02163deefULL,  // rapids 9 1 23
    0x64405a9166c0a3a9ULL,  // rapids 9 1.5 0
    0xc8375f0cd99fbb67ULL,  // rapids 9 1.5 23
    0x5e168d691cb0de4dULL,  // rapids 9 2 0
    0x24e647927da5a5bdULL,  // rapids 9 2 23
    0x7d7dba25a3eac0fULL,  // rapids 13 1 0
    0x62f300fa8b54567aULL,  // rapids 13 1 23
    0x1413749b73e00469ULL,  // rapids 13 1.5 0
    0x79e7eb9b291a9442ULL,  // rapids 13 1.5 23
    0x57a38740bff65ca1ULL,  // rapids 13 2 0
    0xeb1ece057f8e50fdULL,  // rapids 13 2 23
    0xf8185b254e18bd6bULL,  // rapids 20 1 0
    0xc0ffb19c5f732c24ULL,  // rapids 20 1 23
    0x808b1880a380e506ULL,  // rapids 20 1.5 0
    0xb5ac0ec84e0bc0f7ULL,  // rapids 20 1.5 23
    0x20453454113ca2d2ULL,  // rapids 20 2 0
    0xc683469cb990571cULL,  // rapids 20 2 23
    0x1fe2c22874baa87ULL,  // carlito 9 1 0
    0xae3f9580e1f2be27ULL,  // carlito 9 1 23
    0x4e53156bb6f6a168ULL,  // carlito 9 1.5 0
    0xb1703db90555fb4ULL,  // carlito 9 1.5 23
    0x8b378aa0547e6fc9ULL,  // carlito 9 2 0
    0x3b71d356a2251cc6ULL,  // carlito 9 2 23
    0x402f75218c874ea7ULL,  // carlito 13 1 0
    0x5acd323bbe73f564ULL,  // carlito 13 1 23
    0xd3ae1596e22aa3d8ULL,  // carlito 13 1.5 0
    0xae12c2447d728a4bULL,  // carlito 13 1.5 23
    0x167ec98c14fbe984ULL,  // carlito 13 2 0
    0xc9832d66402bfbecULL,  // carlito 13 2 23
    0x3710d46d4095643fULL,  // carlito 20 1 0
    0xccfa8f070265b808ULL,  // carlito 20 1 23
    0xea2b90c0c5764bd8ULL,  // carlito 20 1.5 0
    0xda97a2a42763cefeULL,  // carlito 20 1.5 23
    0x1d1f6592b02fdc77ULL,  // carlito 20 2 0
    0xee09aa358d62088aULL,  // carlito 20 2 23
};

std::size_t identity_index = 0;

void report_identity_hashes(const char* name, const char* font_path) {
    HarfBuzzFontEngine engine;
    require(engine.register_typeface(FontRole::control, 400, false, read_file(font_path)).has_value(), "font registers");
    GlyphRunRasterizer rasterizer(engine);
    const double sizes[3]{9.0, 13.0, 20.0};
    const double scales[3]{1.0, 1.5, 2.0};
    const std::int32_t phases[2]{0, 23};
    for (const double size : sizes) {
        const FontSpec font{FontRole::control, size, 400, false};
        const ShapedText shaped = engine.shape(corpus, font);
        for (const double scale : scales) {
            for (const std::int32_t phase : phases) {
                const GlyphRunCoverage coverage = rasterizer.rasterize(shaped, font, scale, phase);
                const std::uint64_t observed = coverage_hash(coverage);
                std::cout << "identity " << name << ' ' << size << ' ' << scale << ' ' << phase << ' ' << std::hex
                          << observed << std::dec << '\n';
                require(identity_index < 36U && observed == expected_identity[identity_index],
                        "glyph-run coverage differs from the cross-platform expectation");
                ++identity_index;
            }
        }
    }
}

void unknown_faces_are_reported() {
    HarfBuzzFontEngine engine;
    GlyphRunRasterizer rasterizer(engine);
    ShapedText shaped{};
    ShapedFontRun run{};
    run.face = FontFaceId{999U};
    run.glyphs.push_back(ShapedGlyph{});
    shaped.runs.push_back(run);
    const GlyphRunCoverage coverage = rasterizer.rasterize(shaped, {FontRole::control, 13.0, 400, false}, 1.0, 0);
    require(coverage.missing_glyphs == 1U && coverage.pixels.empty(), "unknown face draws nothing and is counted");
}

} // namespace

int main() {
    origin_keeps_phase_and_snaps_baseline();
    rasterization_is_deterministic_and_scales(GUI_FORMS_TEST_RAPIDS_REGULAR);
    rasterization_is_deterministic_and_scales(GUI_FORMS_TEST_CARLITO_REGULAR);
    unknown_faces_are_reported();
    report_identity_hashes("rapids", GUI_FORMS_TEST_RAPIDS_REGULAR);
    report_identity_hashes("carlito", GUI_FORMS_TEST_CARLITO_REGULAR);
    return 0;
}
