// The theme engine's composite (T2 plan, phase E2b; GIVEN A2-2, C1-1): a
// part raster is composited by premultiplied source-over through one integer
// arithmetic, weighted by the part's opacity and the clip; the retained
// Painter command keeps the raster, replays it unchanged and is inspectable.
// The cross-host byte identity of the composite is checked by
// part_composite_identity_tests (Skia) and windows_part_composite_tests.

#include "../src/core/text/composite/text_composite.hpp"

#include "gui_forms/control.hpp"
#include "gui_forms/engine.hpp"
#include "gui_forms/visual_inspection.hpp"
#include "gui_forms/window.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

namespace {
using namespace gui_forms;
using detail::PartRasterSource;
using detail::TextCompositeClip;
using detail::TextCompositeStatus;
using detail::TextCompositeTarget;
using detail::TextPixelOrder;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

// A 32-bit pixel from four channels in the target's memory order.
std::uint32_t pack(TextPixelOrder order, unsigned red, unsigned green, unsigned blue, unsigned alpha) {
    if (order == TextPixelOrder::rgba) return red | (green << 8U) | (blue << 16U) | (alpha << 24U);
    return blue | (green << 8U) | (red << 16U) | (alpha << 24U);
}

// A target of `width` x `height` pixels, every one `fill`.
struct TestTarget final {
    std::vector<std::uint32_t> pixels;
    TextCompositeTarget target{};
};

TestTarget make_target(std::uint32_t width, std::uint32_t height, std::uint32_t fill, TextPixelOrder order,
                       bool opaque) {
    TestTarget made{};
    made.pixels.assign(static_cast<std::size_t>(width) * height, fill);
    made.target.pixels = std::span<std::uint32_t>(made.pixels.data(), made.pixels.size());
    made.target.width = width;
    made.target.height = height;
    made.target.stride = width;
    made.target.order = order;
    made.target.opaque = opaque;
    return made;
}

TextCompositeClip whole(std::uint32_t width, std::uint32_t height) {
    TextCompositeClip clip{};
    clip.rect = {0.0, 0.0, static_cast<double>(width), static_cast<double>(height)};
    return clip;
}

// One premultiplied RGBA source pixel repeated over `width` x `height`.
std::vector<std::uint8_t> solid_source(std::uint32_t width, std::uint32_t height, std::array<std::uint8_t, 4> pixel) {
    std::vector<std::uint8_t> bytes{};
    bytes.reserve(static_cast<std::size_t>(width) * height * 4U);
    for (std::size_t index = 0U; index < static_cast<std::size_t>(width) * height; ++index) {
        bytes.push_back(pixel[0]);
        bytes.push_back(pixel[1]);
        bytes.push_back(pixel[2]);
        bytes.push_back(pixel[3]);
    }
    return bytes;
}

PartRasterSource source_of(const std::vector<std::uint8_t>& bytes, std::uint32_t width, std::uint32_t height,
                           std::int64_t left, std::int64_t top) {
    const PartRasterSource source{std::span<const std::uint8_t>(bytes.data(), bytes.size()), width, height,
                                  static_cast<std::size_t>(width) * 4U, left, top};
    return source;
}

void test_opacity_quantization() {
    require(detail::part_composite_opacity(0.0) == 0U, "opacity 0 is byte 0");
    require(detail::part_composite_opacity(1.0) == 255U, "opacity 1 is byte 255");
    require(detail::part_composite_opacity(0.5) == 128U, "opacity 0.5 rounds half up to 128");
    require(detail::part_composite_opacity(std::nan("")) == 0U, "NaN opacity is 0");
    require(detail::part_composite_opacity(-1.0) == 0U && detail::part_composite_opacity(2.0) == 255U,
            "opacity outside [0, 1] is clamped");
}

void test_blend_arithmetic() {
    const TextPixelOrder orders[] = {TextPixelOrder::rgba, TextPixelOrder::bgra};
    for (const TextPixelOrder order : orders) {
        // An opaque source at full opacity and coverage is copied exactly.
        TestTarget copied = make_target(2U, 1U, pack(order, 0U, 0U, 255U, 255U), order, false);
        const std::vector<std::uint8_t> opaque = solid_source(2U, 1U, {200, 100, 50, 255});
        require(detail::composite_part_raster(source_of(opaque, 2U, 1U, 0, 0), copied.target, whole(2U, 1U), 255U) ==
                    TextCompositeStatus::success,
                "the composite succeeds");
        require(copied.pixels[0] == pack(order, 200U, 100U, 50U, 255U) && copied.pixels[1] == copied.pixels[0],
                "an opaque raster is copied exactly");

        // A half-transparent premultiplied source over opaque blue:
        // out = s + d * (255 - 128) / 255, rounded once.
        TestTarget half = make_target(1U, 1U, pack(order, 0U, 0U, 255U, 255U), order, false);
        const std::vector<std::uint8_t> translucent = solid_source(1U, 1U, {128, 64, 0, 128});
        require(detail::composite_part_raster(source_of(translucent, 1U, 1U, 0, 0), half.target, whole(1U, 1U), 255U) ==
                    TextCompositeStatus::success,
                "the translucent composite succeeds");
        require(half.pixels[0] == pack(order, 128U, 64U, 127U, 255U), "premultiplied source-over, rounded once");

        // Opacity 128 weights every channel by 128 * 256 / 65280, rounded.
        TestTarget faded = make_target(1U, 1U, pack(order, 0U, 0U, 0U, 255U), order, false);
        require(detail::composite_part_raster(source_of(opaque, 1U, 1U, 0, 0), faded.target, whole(1U, 1U), 128U) ==
                    TextCompositeStatus::success,
                "the faded composite succeeds");
        require(faded.pixels[0] == pack(order, 100U, 50U, 25U, 255U), "opacity weights each premultiplied channel");

        // An opaque target keeps alpha 255 whatever the blend gives.
        TestTarget dib = make_target(1U, 1U, pack(order, 0U, 0U, 0U, 0U), order, true);
        require(detail::composite_part_raster(source_of(translucent, 1U, 1U, 0, 0), dib.target, whole(1U, 1U), 255U) ==
                    TextCompositeStatus::success,
                "the opaque-target composite succeeds");
        require((dib.pixels[0] >> 24U) == 255U, "an opaque target is written with alpha 255");
    }
}

void test_transparency_and_zero_opacity_write_nothing() {
    TestTarget target = make_target(3U, 3U, 0x11223344U, TextPixelOrder::rgba, false);
    const std::vector<std::uint32_t> before = target.pixels;
    const std::vector<std::uint8_t> clear = solid_source(3U, 3U, {0, 0, 0, 0});
    require(detail::composite_part_raster(source_of(clear, 3U, 3U, 0, 0), target.target, whole(3U, 3U), 255U) ==
                TextCompositeStatus::success,
            "a transparent raster composites");
    require(target.pixels == before, "a transparent raster changes nothing (passthrough transparency)");
    const std::vector<std::uint8_t> opaque = solid_source(3U, 3U, {9, 9, 9, 255});
    require(detail::composite_part_raster(source_of(opaque, 3U, 3U, 0, 0), target.target, whole(3U, 3U), 0U) ==
                TextCompositeStatus::success,
            "opacity 0 composites");
    require(target.pixels == before, "opacity 0 changes nothing");
}

void test_placement_and_clip() {
    const std::uint32_t background = pack(TextPixelOrder::rgba, 10U, 20U, 30U, 255U);
    const std::uint32_t white = pack(TextPixelOrder::rgba, 255U, 255U, 255U, 255U);
    const std::vector<std::uint8_t> opaque = solid_source(3U, 3U, {255, 255, 255, 255});

    // Placed partly outside the target: only the overlap is written.
    TestTarget shifted = make_target(4U, 4U, background, TextPixelOrder::rgba, false);
    require(detail::composite_part_raster(source_of(opaque, 3U, 3U, -1, 2), shifted.target, whole(4U, 4U), 255U) ==
                TextCompositeStatus::success,
            "a raster partly outside the target composites");
    for (std::uint32_t y = 0U; y < 4U; ++y) {
        for (std::uint32_t x = 0U; x < 4U; ++x) {
            const bool inside = x < 2U && y >= 2U;
            const std::uint32_t pixel = shifted.pixels[static_cast<std::size_t>(y) * 4U + x];
            require(pixel == (inside ? white : background), "only the raster's overlap with the target changes");
        }
    }

    // A clip edge through the middle of a pixel blends that pixel partly;
    // pixels outside the clip are untouched.
    TestTarget clipped = make_target(3U, 1U, background, TextPixelOrder::rgba, false);
    TextCompositeClip clip{};
    clip.rect = {0.0, 0.0, 1.5, 1.0};
    require(detail::composite_part_raster(source_of(opaque, 3U, 1U, 0, 0), clipped.target, clip, 255U) ==
                TextCompositeStatus::success,
            "a clipped composite succeeds");
    const unsigned middle_red = clipped.pixels[1] & 255U;
    require(clipped.pixels[0] == white, "a pixel inside the clip is covered");
    require(middle_red > 10U && middle_red < 255U, "a pixel half inside the clip is blended in part");
    require(clipped.pixels[2] == background, "a pixel outside the clip is untouched");
}

void test_refusals_write_nothing() {
    TestTarget target = make_target(2U, 2U, 0x01020304U, TextPixelOrder::rgba, false);
    const std::vector<std::uint32_t> before = target.pixels;
    const std::vector<std::uint8_t> bytes = solid_source(2U, 2U, {50, 50, 50, 255});
    PartRasterSource short_buffer = source_of(bytes, 2U, 2U, 0, 0);
    short_buffer.pixels = short_buffer.pixels.first(short_buffer.pixels.size() - 1U);
    require(detail::composite_part_raster(short_buffer, target.target, whole(2U, 2U), 255U) ==
                TextCompositeStatus::invalid_geometry,
            "a short source buffer is refused");
    PartRasterSource short_rows = source_of(bytes, 2U, 2U, 0, 0);
    short_rows.row_bytes = 7U;
    require(detail::composite_part_raster(short_rows, target.target, whole(2U, 2U), 255U) ==
                TextCompositeStatus::invalid_geometry,
            "rows shorter than their pixels are refused");
    TextCompositeClip bad{};
    bad.rect = {0.0, 0.0, std::nan(""), 1.0};
    require(detail::composite_part_raster(source_of(bytes, 2U, 2U, 0, 0), target.target, bad, 255U) ==
                TextCompositeStatus::invalid_geometry,
            "a non-finite clip is refused");
    require(target.pixels == before, "every refusal writes nothing");
}

// Every premultiplied source alpha and channel at every opacity stays
// within 0..255 and never darkens a white destination below the source.
void test_premultiplied_sources_never_overflow() {
    const std::array<unsigned, 6> alphas{1U, 37U, 128U, 200U, 254U, 255U};
    const std::array<std::uint8_t, 5> opacities{1U, 64U, 128U, 200U, 255U};
    for (const unsigned alpha : alphas) {
        for (const std::uint8_t opacity : opacities) {
            TestTarget target = make_target(1U, 1U, 0xFFFFFFFFU, TextPixelOrder::rgba, false);
            const std::uint8_t channel = static_cast<std::uint8_t>(alpha);
            const std::vector<std::uint8_t> bytes = solid_source(1U, 1U, {channel, channel, channel, channel});
            require(detail::composite_part_raster(source_of(bytes, 1U, 1U, 0, 0), target.target, whole(1U, 1U),
                                                  opacity) == TextCompositeStatus::success,
                    "a premultiplied source composites");
            require(target.pixels[0] == 0xFFFFFFFFU, "white over white stays white at every weight");
        }
    }
}

// -- PartRaster -----------------------------------------------------------------

void test_part_raster() {
    const std::shared_ptr<PartRaster> raster = PartRaster::create(DeviceExtent{3U, 2U}, DeviceScale{1.5});
    require(raster != nullptr, "a raster is made");
    require((*raster).row_bytes() == 12U && (*raster).pixels().size() == 24U, "rows are tight");
    bool transparent = true;
    for (const std::uint8_t byte : (*raster).pixels()) {
        if (byte != 0U) transparent = false;
    }
    require(transparent, "a new raster is transparent");
    const PartRasterView view = (*raster).view();
    require(validate_raster_view(view) == RasterViewDefect::none, "its view validates");
    require(PartRaster::create(DeviceExtent{maximum_part_raster_side + 1U, 1U}, DeviceScale{1.0}) == nullptr,
            "an oversized raster is refused");
    require(PartRaster::create(DeviceExtent{1U, 1U}, DeviceScale{0.25}) == nullptr, "a scale below 0.5 is refused");
    require(PartRaster::create(DeviceExtent{1U, 1U}, DeviceScale{std::nan("")}) == nullptr, "a NaN scale is refused");
}

// -- the retained command --------------------------------------------------------

struct PartRasterCall final {
    std::shared_ptr<const PartRaster> raster;
    Point origin{};
    double opacity{};
};

class CapturePainter final : public Painter {
public:
    void save() override {}
    void restore() override {}
    void translate(Point) override {}
    void clip_rect(Rect) override {}
    void fill_rect(Rect, Color) override {}
    void stroke_rect(Rect, Color, double) override {}
    void draw_line(Point, Point, Color, double) override {}
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override {}
    void draw_image(ImageId, Rect, double) override {}
    void draw_part_raster(const std::shared_ptr<const PartRaster>& raster, Point origin, double opacity) override {
        calls.push_back(PartRasterCall{raster, origin, opacity});
    }

    std::vector<PartRasterCall> calls;
};

class RasterControl final : public Control {
public:
    explicit RasterControl(StableId id) : Control(std::move(id)) {}

    void on_paint(Painter& painter, Rect) override { painter.draw_part_raster(raster, Point{3.0, 4.0}, 0.75); }

    std::shared_ptr<const PartRaster> raster;
};

void test_command_is_recorded_replayed_and_inspected() {
    const std::shared_ptr<Control> root = make_control<Control>(StableId("root"));
    const std::shared_ptr<RasterControl> control = make_control<RasterControl>(StableId("raster"));
    (*control).set_requested_bounds({10.0, 10.0, 40.0, 30.0});
    const std::shared_ptr<PartRaster> made = PartRaster::create(DeviceExtent{20U, 10U}, DeviceScale{1.0});
    require(made != nullptr, "a raster is made");
    (*control).raster = made;
    (*root).add_child(control);
    Window window(root, Size{100.0, 80.0});
    window.perform_layout();

    CapturePainter first;
    static_cast<void>(window.paint(first, Rect{0.0, 0.0, 100.0, 80.0}));
    require(first.calls.size() == 1U, "the command replays once");
    require(first.calls[0].raster == made, "the recording keeps the very raster");
    require(first.calls[0].origin.x == 3.0 && first.calls[0].origin.y == 4.0 && first.calls[0].opacity == 0.75,
            "origin and opacity replay unchanged");

    // The recording owns a reference: the raster outlives its maker's.
    const std::weak_ptr<const PartRaster> observed = made;
    (*control).raster.reset();
    require(!observed.expired(), "the recorded command keeps the raster alive");
    CapturePainter second;
    static_cast<void>(window.paint(second, Rect{0.0, 0.0, 100.0, 80.0}));
    require(second.calls.size() == 1U && second.calls[0].raster == first.calls[0].raster,
            "a reused display chunk replays the kept raster");

    const VisualInspectionSnapshot snapshot = window.visual_inspection_snapshot();
    const VisualControlInspection* const inspected = snapshot.find("raster");
    require(inspected != nullptr && (*inspected).display_operations.size() == 1U, "the operation is inspectable");
    const VisualPaintOperationSnapshot& operation = (*inspected).display_operations[0];
    require(operation.operation == VisualPaintOperation::draw_part_raster, "it is named draw_part_raster");
    require(std::string(visual_paint_operation_name(operation.operation)) == "draw_part_raster",
            "its registered name is draw_part_raster");
    require(operation.rect.x == 3.0 && operation.rect.y == 4.0 && operation.rect.width == 20.0 &&
                operation.rect.height == 10.0 && operation.scalar == 0.75,
            "inspection reports the raster's logical bounds and opacity");

    // The base painter draws nothing, and a null raster is not recorded.
    (*control).raster = nullptr;
    (*control).invalidate(Dirty::paint);
    CapturePainter third;
    static_cast<void>(window.paint(third, Rect{0.0, 0.0, 100.0, 80.0}));
    require(third.calls.empty(), "a null raster records nothing");
}

} // namespace

int main() {
    test_opacity_quantization();
    test_blend_arithmetic();
    test_transparency_and_zero_opacity_write_nothing();
    test_placement_and_clip();
    test_refusals_write_nothing();
    test_premultiplied_sources_never_overflow();
    test_part_raster();
    test_command_is_recorded_replayed_and_inspected();
    std::cout << "part composite tests passed\n";
    return 0;
}
