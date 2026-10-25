#import <AppKit/AppKit.h>

#include "gui_forms/platform/macos_host.hpp"
#include "gui_forms/gui_forms.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

// Stage DP gate on macOS: two distant damaged rectangles paint their sum, not
// their bounding box; AppKit hands the host exactly that rectangle set to
// copy; and no pixel outside them changes in the presented frame. The record
// is results/2026-10-25-dp-gate-macos.

// Existing private native diagnostics and occlusion hooks, not a public API addition.
@interface NSView (DamageRectanglesProbe)
- (void)collectDamage;
- (void)notifyOcclusion:(BOOL)occluded;
@end

namespace {
using namespace gui_forms;

constexpr const char* window_title = "GUI.Forms damage rectangles check";

void require(const bool condition, const char* const message) {
    if (!condition) throw std::runtime_error(message);
}

class ColorBlock final : public Control {
public:
    ColorBlock(StableId id, const Color color) : Control(std::move(id)), color_(color) {}
    void set_color(const Color color) {
        color_ = color;
        invalidate(Dirty::paint);
    }
protected:
    void on_paint(Painter& painter, const gui_forms::Rect) override {
        painter.fill_rect(client_rectangle(), color_);
    }
private:
    Color color_{};
};

// The presented frame, read back through AppKit's cached display: `width` x
// `height` device pixels, four sRGB bytes each, top row first.
struct Capture final {
    NSInteger width{0};
    NSInteger height{0};
    std::vector<std::uint8_t> rgba{};
};

enum class Stage { first_frame, settle, changed };

struct State final {
    std::shared_ptr<ColorBlock> top_left{};
    std::shared_ptr<ColorBlock> bottom_right{};
    std::array<gui_forms::Rect, 2> blocks{};
    gui_forms::Window* model{}; // Borrowed only while the host run is alive.
    std::function<void()> close{};
    Stage stage{Stage::first_frame};
    std::chrono::steady_clock::time_point deadline{};
    std::uint32_t settle_steps{0};
    std::uint64_t previous_presented{0};
    MetricsSnapshot before_metrics{};
    Capture before{};
    double scale{1.0};
    bool finished{false};
    bool host_closed{false};
    std::string failure{};
};
struct DeferredStep final {
    std::shared_ptr<State> state{};
};

NSWindow* find_window() {
    NSWindow* result = nil;
    NSArray<NSWindow*>* const windows = NSApp.windows;
    const NSUInteger count = windows.count;
    for (NSUInteger index = 0; index < count; ++index) {
        NSWindow* const candidate = [windows objectAtIndex:index];
        if ([candidate.title isEqualToString:[NSString stringWithUTF8String:window_title]]) {
            result = candidate;
            break;
        }
    }
    return result;
}
void close_fixture(State& state) {
    // closed can clear the stored target synchronously. This local copy owns
    // the callable until this invocation has returned.
    const std::function<void()> request_close = state.close;
    if (request_close) request_close();
}
void set_stage(State& state, const Stage stage) {
    state.stage = stage;
    state.deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
}
void run_step(void* const context);
void enqueue(const std::shared_ptr<State>& state) {
    std::unique_ptr<DeferredStep> owner = std::make_unique<DeferredStep>();
    (*owner).state = state;
    const std::int64_t delay_nanoseconds = 50'000'000;
    const dispatch_time_t when = dispatch_time(DISPATCH_TIME_NOW, delay_nanoseconds);
    void* const context = owner.release();
    dispatch_after_f(when, dispatch_get_main_queue(), context, run_step);
}

// Reads the presented raster while the model is occluded, so the cached
// display copies the last frame and paints nothing (counters unchanged).
Capture capture_occluded(State& state, NSView* const view) {
    [view notifyOcclusion:YES];
    require((*state.model).occluded(), "occlusion hook did not reach the model");
    const MetricsSnapshot before = (*state.model).metrics_snapshot();
    Capture result{};
    @autoreleasepool {
        NSBitmapImageRep* const bitmap = [view bitmapImageRepForCachingDisplayInRect:view.bounds];
        require(bitmap != nil, "AppKit bitmap allocation");
        [view cacheDisplayInRect:view.bounds toBitmapImageRep:bitmap];
        result.width = bitmap.pixelsWide;
        result.height = bitmap.pixelsHigh;
        require(result.width > 0 && result.height > 0 && result.width <= 2048 && result.height <= 2048,
            "bounded snapshot extent");
        const std::size_t count = static_cast<std::size_t>(result.width) * static_cast<std::size_t>(result.height);
        result.rgba.resize(count * 4U);
        NSColorSpace* const color_space = NSColorSpace.sRGBColorSpace;
        for (NSInteger row = 0; row < result.height; ++row) {
            const std::size_t offset = static_cast<std::size_t>(row) * static_cast<std::size_t>(result.width) * 4U;
            for (NSInteger column = 0; column < result.width; ++column) {
                NSColor* const color = [[bitmap colorAtX:column y:row] colorUsingColorSpace:color_space];
                require(color != nil, "snapshot color conversion");
                const std::array<double, 4> components{static_cast<double>(color.redComponent),
                    static_cast<double>(color.greenComponent), static_cast<double>(color.blueComponent),
                    static_cast<double>(color.alphaComponent)};
                const std::size_t pixel = offset + static_cast<std::size_t>(column) * 4U;
                for (std::size_t channel = 0; channel < components.size(); ++channel) {
                    require(std::isfinite(components[channel]), "finite snapshot color");
                    const double bounded = std::clamp(components[channel], 0.0, 1.0);
                    const long value = std::lround(bounded * 255.0);
                    result.rgba[pixel + channel] = static_cast<std::uint8_t>(value);
                }
            }
        }
    }
    const MetricsSnapshot after = (*state.model).metrics_snapshot();
    require(after.paint_passes == before.paint_passes, "the occluded readback painted");
    [view notifyOcclusion:NO];
    return result;
}

// Pixels that differ between two captures, counted by whether the pixel's
// centre (in logical units, `scale` device pixels per unit) lies in a block.
struct ChangedPixels final {
    std::size_t inside{0};
    std::size_t outside{0};
};
bool centre_inside(const NSInteger column, const NSInteger row, const double scale,
                   const std::span<const gui_forms::Rect> blocks) {
    const gui_forms::Point centre{(static_cast<double>(column) + 0.5) / scale, (static_cast<double>(row) + 0.5) / scale};
    for (std::size_t index = 0; index < blocks.size(); ++index) {
        if (blocks[index].contains(centre)) return true;
    }
    return false;
}
ChangedPixels changed_pixels(const Capture& before, const Capture& after, const double scale,
                             const std::span<const gui_forms::Rect> blocks) {
    require(before.width == after.width && before.height == after.height, "captures differ in extent");
    ChangedPixels changed{};
    for (NSInteger row = 0; row < before.height; ++row) {
        for (NSInteger column = 0; column < before.width; ++column) {
            const std::size_t pixel =
                (static_cast<std::size_t>(row) * static_cast<std::size_t>(before.width) +
                 static_cast<std::size_t>(column)) * 4U;
            bool same = true;
            for (std::size_t channel = 0; channel < 4U; ++channel) {
                if (before.rgba[pixel + channel] != after.rgba[pixel + channel]) same = false;
            }
            if (same) continue;
            if (centre_inside(column, row, scale, blocks)) {
                ++changed.inside;
            } else {
                ++changed.outside;
            }
        }
    }
    return changed;
}

void measure(State& state, NSView* const view) {
    const MetricsSnapshot after = (*state.model).metrics_snapshot();
    const Capture captured = capture_occluded(state, view);
    const MetricsSnapshot& before = state.before_metrics;
    const double sum = state.blocks[0].area() + state.blocks[1].area();
    const double painted = after.painted_damage_area - before.painted_damage_area;
    const double bounding = after.paint_bounding_area - before.paint_bounding_area;
    const std::uint64_t presents = after.frames_presented - before.frames_presented;
    const std::uint64_t rectangles = after.retained_rectangles_presented - before.retained_rectangles_presented;
    const std::uint64_t pixels = after.retained_pixels_presented - before.retained_pixels_presented;
    const double device_pixels_per_unit = state.scale * state.scale;
    const std::uint64_t expected_pixels = static_cast<std::uint64_t>(std::llround(sum * device_pixels_per_unit));
    const ChangedPixels changed = changed_pixels(state.before, captured, state.scale, state.blocks);
    std::cout << "dp-gate|host=macos|scale=" << state.scale << "|sum_area=" << sum << "|painted_area=" << painted
              << "|bounding_area=" << bounding << "|presents=" << presents << "|rectangles_presented=" << rectangles
              << "|pixels_presented=" << pixels << "|expected_pixels=" << expected_pixels
              << "|changed_inside=" << changed.inside << "|changed_outside=" << changed.outside << '\n';
    require(presents == 1U, "two distant changes did not present one frame");
    const double difference = painted - sum;
    require(difference < 1.0e-6 && difference > -1.0e-6, "painted area differs from the rectangles' sum");
    require(bounding > painted * 10.0, "the rectangles are not distant: the test cannot tell a sum from a box");
    require(rectangles == 2U, "the host did not receive the two-rectangle set");
    require(pixels == expected_pixels, "the host copied more than the two rectangles");
    require(changed.outside == 0U, "a pixel outside the damaged rectangles changed");
    require(changed.inside == static_cast<std::size_t>(expected_pixels), "the damaged rectangles did not change");
}

void advance(State& state) {
    NSWindow* const window = find_window();
    if (state.stage == Stage::first_frame) {
        if (window == nil || !window.isVisible || window.contentView == nil) return;
        const PaintLeaseSnapshot lease = (*state.model).paint_lease_snapshot();
        if (lease.presented_revision == 0) return;
        state.scale = static_cast<double>(window.backingScaleFactor);
        require(std::isfinite(state.scale) && state.scale >= 0.5 && state.scale <= 4.0, "backing scale supported");
        state.before = capture_occluded(state, window.contentView);
        state.settle_steps = 0;
        set_stage(state, Stage::settle);
        return;
    }
    require(window != nil && window.contentView != nil, "native fixture window disappeared");
    NSView* const view = window.contentView;
    require(static_cast<double>(window.backingScaleFactor) == state.scale, "backing scale changed during the fixture");
    if (state.stage == Stage::settle) {
        // Let any frame that unocclusion asks for land before the baseline.
        ++state.settle_steps;
        if (state.settle_steps < 4U) return;
        const PaintLeaseSnapshot lease = (*state.model).paint_lease_snapshot();
        state.previous_presented = lease.presented_revision;
        state.before_metrics = (*state.model).metrics_snapshot();
        (*state.top_left).set_color(Color::rgba(200, 40, 40));
        (*state.bottom_right).set_color(Color::rgba(40, 40, 200));
        [view collectDamage];
        set_stage(state, Stage::changed);
        return;
    }
    const PaintLeaseSnapshot lease = (*state.model).paint_lease_snapshot();
    if (lease.presented_revision <= state.previous_presented) return;
    measure(state, view);
    state.finished = true;
    close_fixture(state);
}
void run_step(void* const context) {
    const std::unique_ptr<DeferredStep> owner(static_cast<DeferredStep*>(context));
    const std::shared_ptr<State> state = (*owner).state;
    if ((*state).host_closed || (*state).finished) return;
    @try {
        try {
            require(std::chrono::steady_clock::now() < (*state).deadline, "bounded native stage timed out");
            advance(*state);
            if (!(*state).finished && !(*state).host_closed) enqueue(state);
        } catch (const std::exception& error) {
            (*state).failure = error.what();
            (*state).finished = true;
            close_fixture(*state);
        }
    } @catch (NSException* exception) {
        NSString* const reason = exception.reason == nil ? @"unknown AppKit exception" : exception.reason;
        const char* const message = reason.UTF8String;
        (*state).failure = message == nullptr ? "AppKit exception" : message;
        (*state).finished = true;
        close_fixture(*state);
    }
}
struct Closed final {
    std::shared_ptr<State> state{};
    void operator()() const {
        (*state).host_closed = true;
        (*state).model = nullptr;
        (*state).close = {};
    }
};
struct Ready final {
    std::shared_ptr<State> state{};
    void operator()(std::function<void()>, std::function<void()> close,
        std::function<HostDialogResult(const HostDialogRequest&)>,
        std::function<HostServiceStatus(const HostTooltipRequest&)>, std::function<void()>,
        std::function<HostClipboardTextResult()>, std::function<HostServiceStatus(std::string_view)>) const {
        (*state).close = std::move(close);
        set_stage(*state, Stage::first_frame);
        enqueue(state);
    }
};
}

int main() {
    try {
        const std::shared_ptr<State> state = std::make_shared<State>();
        const Color rest = Color::rgba(90, 90, 90);
        const std::shared_ptr<ColorBlock> root =
            make_control<ColorBlock>(StableId("dp.root"), Color::rgba(230, 230, 230));
        (*state).top_left = make_control<ColorBlock>(StableId("dp.top-left"), rest);
        (*state).bottom_right = make_control<ColorBlock>(StableId("dp.bottom-right"), rest);
        (*state).blocks = {gui_forms::Rect{10.0, 10.0, 40.0, 30.0}, gui_forms::Rect{340.0, 250.0, 40.0, 30.0}};
        (*(*state).top_left).set_requested_bounds((*state).blocks[0]);
        (*(*state).bottom_right).set_requested_bounds((*state).blocks[1]);
        (*root).add_child((*state).top_left);
        (*root).add_child((*state).bottom_right);
        gui_forms::host::MacHostOptions options{};
        options.title = window_title;
        options.initial_size = {400, 300};
        options.minimum_size = {400, 300};
        options.print_metrics_on_close = false;
        options.host_ready = Ready{state};
        options.closed = Closed{state};
        std::unique_ptr<gui_forms::Window> model = std::make_unique<gui_forms::Window>(root, options.initial_size);
        (*state).model = model.get();
        const int result = gui_forms::host::run_macos(std::move(model), std::move(options));
        (*state).host_closed = true;
        (*state).model = nullptr;
        (*state).close = {};
        require(result == 0 && (*state).finished && (*state).failure.empty(),
            (*state).failure.empty() ? "native fixture did not complete" : (*state).failure.c_str());
        std::cout << "gui_forms_macos_damage_rectangles_tests: all tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "macos damage rectangles failure: " << error.what() << '\n';
        return 1;
    }
}
