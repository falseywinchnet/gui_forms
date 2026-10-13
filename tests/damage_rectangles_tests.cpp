// Stage DP: a paint pass over a set of damage rectangles paints each one
// under its own clip. Two distant changes cost the sum of their areas, not
// their bounding box, and nothing is drawn between them.
#include "gui_forms/gui_forms.hpp"

#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {

using namespace gui_forms;

void expect(const bool condition, const std::string_view message) {
    if (condition) return;
    std::cerr << "damage rectangles failure: " << message << '\n';
    std::exit(EXIT_FAILURE);
}

bool near(double left, double right) {
    const double difference = left - right;
    return difference < 1.0e-6 && difference > -1.0e-6;
}

// Tracks the effective clip in window coordinates and records it for every
// draw, so a test can check where pixels could have been touched.
class ClipTrackingPainter final : public Painter {
public:
    struct State final {
        Point offset{};
        Rect clip{};
    };

    explicit ClipTrackingPainter(Rect window) {
        State initial;
        initial.clip = window;
        states_.push_back(initial);
    }

    void save() override { states_.push_back(states_.back()); }
    void restore() override {
        if (states_.size() > 1U) states_.pop_back();
    }
    void translate(Point delta) override {
        states_.back().offset.x += delta.x;
        states_.back().offset.y += delta.y;
    }
    void clip_rect(Rect local) override {
        const Rect window{local.x + states_.back().offset.x, local.y + states_.back().offset.y,
                          local.width, local.height};
        states_.back().clip = Rect::intersection(states_.back().clip, window);
    }
    void clip_rounded_rect(Rect local, double) override { clip_rect(local); }
    void fill_rect(Rect, Color) override { record(); }
    void fill_rounded_rect(Rect, double, Color) override { record(); }
    void stroke_rect(Rect, Color, double) override { record(); }
    void stroke_rounded_rect(Rect, double, Color, double) override { record(); }
    void fill_linear_gradient(Rect, Point, Point, std::span<const GradientStop>) override { record(); }
    void fill_radial_gradient(Rect, Point, Size, std::span<const GradientStop>) override { record(); }
    void draw_box_shadow(Rect, double, Point, double, double, Color) override { record(); }
    void draw_line(Point, Point, Color, double) override { record(); }
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override { record(); }
    void draw_image(ImageId, Rect, double) override { record(); }

    // The clip of every draw since the last reset; an empty clip draws nothing.
    std::vector<Rect> drawn_clips;

private:
    void record() {
        if (!states_.back().clip.empty()) drawn_clips.push_back(states_.back().clip);
    }

    std::vector<State> states_;
};

bool inside_any(Rect clip, std::span<const Rect> allowed) {
    for (std::size_t index = 0; index < allowed.size(); ++index) {
        if (allowed[index].contains(clip)) return true;
    }
    return false;
}

struct Scene final {
    std::shared_ptr<Panel> root;
    std::shared_ptr<Panel> top_left;
    std::shared_ptr<Panel> bottom_right;
};

Scene make_scene() {
    Scene scene;
    scene.root = make_control<Panel>(StableId("dp.root"));
    scene.top_left = make_control<Panel>(StableId("dp.top-left"));
    scene.bottom_right = make_control<Panel>(StableId("dp.bottom-right"));
    (*scene.top_left).set_bounds({10.0, 10.0, 40.0, 30.0});
    (*scene.bottom_right).set_bounds({340.0, 250.0, 40.0, 30.0});
    (*scene.root).add_child(scene.top_left);
    (*scene.root).add_child(scene.bottom_right);
    return scene;
}

void distant_rectangles_paint_their_sum_and_nothing_between() {
    Scene scene = make_scene();
    Window window(scene.root, Size{400.0, 300.0});
    window.perform_layout();
    ClipTrackingPainter first(Rect{0.0, 0.0, 400.0, 300.0});
    static_cast<void>(window.paint(first));
    static_cast<void>(window.take_damage());

    (*scene.top_left).set_background(Color::rgba(200, 40, 40));
    (*scene.bottom_right).set_background(Color::rgba(40, 40, 200));
    window.flush();
    // A host takes the damage and hands the rectangle set back, as the
    // Windows host does with its pending damage and update region.
    const DamageRegion damage = window.take_damage();
    expect(damage.rectangle_count() == 2U, "two distant mutations keep two rectangles");
    const std::span<const Rect> rectangles = damage.rectangles();
    double expected_area = 0.0;
    for (std::size_t index = 0; index < rectangles.size(); ++index) {
        expected_area += rectangles[index].area();
    }

    const MetricsSnapshot before = window.metrics_snapshot();
    ClipTrackingPainter painter(Rect{0.0, 0.0, 400.0, 300.0});
    const std::optional<PaintReceipt> receipt = window.paint(painter, rectangles);
    expect(receipt.has_value(), "one lease produces one receipt");
    const MetricsSnapshot after = window.metrics_snapshot();

    expect(after.paint_passes == before.paint_passes + 1U, "the set paints in one pass");
    expect(after.damage_rectangles_painted == before.damage_rectangles_painted + 2U,
           "both rectangles are painted separately");
    const double painted = after.painted_damage_area - before.painted_damage_area;
    const double bounding = after.paint_bounding_area - before.paint_bounding_area;
    expect(near(painted, expected_area), "painted area equals the rectangles' sum");
    expect(bounding > painted * 10.0, "the bounding box would have cost far more");
    expect(!painter.drawn_clips.empty(), "the pass draws");
    for (std::size_t index = 0; index < painter.drawn_clips.size(); ++index) {
        expect(inside_any(painter.drawn_clips[index], rectangles),
               "no draw reaches outside the damaged rectangles");
    }
}

void nearly_touching_rectangles_merge() {
    Scene scene = make_scene();
    Window window(scene.root, Size{400.0, 300.0});
    window.perform_layout();
    ClipTrackingPainter first(Rect{0.0, 0.0, 400.0, 300.0});
    static_cast<void>(window.paint(first));

    const std::array<Rect, 2> adjacent{Rect{100.0, 100.0, 50.0, 20.0},
                                       Rect{151.0, 100.0, 50.0, 20.0}};
    const MetricsSnapshot before = window.metrics_snapshot();
    ClipTrackingPainter painter(Rect{0.0, 0.0, 400.0, 300.0});
    static_cast<void>(window.paint(painter, adjacent));
    const MetricsSnapshot after = window.metrics_snapshot();
    expect(after.damage_rectangles_painted == before.damage_rectangles_painted + 1U,
           "rectangles a unit apart paint as one");
}

void single_rectangle_paint_is_unchanged() {
    Scene scene = make_scene();
    Window window(scene.root, Size{400.0, 300.0});
    window.perform_layout();
    ClipTrackingPainter first(Rect{0.0, 0.0, 400.0, 300.0});
    static_cast<void>(window.paint(first));

    (*scene.top_left).set_background(Color::rgba(200, 40, 40));
    (*scene.bottom_right).set_background(Color::rgba(40, 40, 200));
    window.flush();
    const MetricsSnapshot before = window.metrics_snapshot();
    ClipTrackingPainter painter(Rect{0.0, 0.0, 400.0, 300.0});
    static_cast<void>(window.paint(painter));
    const MetricsSnapshot after = window.metrics_snapshot();
    const double painted = after.painted_damage_area - before.painted_damage_area;
    const double bounding = after.paint_bounding_area - before.paint_bounding_area;
    expect(after.damage_rectangles_painted == before.damage_rectangles_painted + 1U &&
               near(painted, bounding),
           "the single-rectangle overload still paints the bounding box");
}

void accumulated_damage_paints_as_rectangles() {
    Scene scene = make_scene();
    Window window(scene.root, Size{400.0, 300.0});
    window.perform_layout();
    ClipTrackingPainter first(Rect{0.0, 0.0, 400.0, 300.0});
    static_cast<void>(window.paint(first));

    (*scene.top_left).set_background(Color::rgba(200, 40, 40));
    (*scene.bottom_right).set_background(Color::rgba(40, 40, 200));
    window.flush();
    const MetricsSnapshot before = window.metrics_snapshot();
    ClipTrackingPainter painter(Rect{0.0, 0.0, 400.0, 300.0});
    const std::span<const Rect> none{};
    static_cast<void>(window.paint(painter, none));
    const MetricsSnapshot after = window.metrics_snapshot();
    expect(after.damage_rectangles_painted == before.damage_rectangles_painted + 2U,
           "an empty set paints the accumulated rectangles, not their bounds");
    expect(window.take_damage().empty(), "the painted rectangles consume their damage");
}

// Hosts report what each present copied to the screen, retained and live
// apart, so the copy cost is a counter on every host.
void presentation_counters_record_copied_area() {
    Scene scene = make_scene();
    Window window(scene.root, Size{400.0, 300.0});
    window.perform_layout();
    ClipTrackingPainter painter(Rect{0.0, 0.0, 400.0, 300.0});
    const std::optional<PaintReceipt> receipt = window.paint(painter);
    expect(receipt.has_value(), "the first frame paints");
    PresentedArea retained;
    retained.device_pixels = 2400U;
    retained.rectangles = 2U;
    expect(window.notify_presented(*receipt, 1000U, retained), "the receipt is accepted");
    PresentedArea live;
    live.device_pixels = 10000U;
    live.rectangles = 1U;
    window.notify_live_presented(500U, live);
    window.notify_live_presented(500U, live);
    const MetricsSnapshot metrics = window.metrics_snapshot();
    expect(metrics.frames_presented == 1U && metrics.retained_pixels_presented == 2400U &&
               metrics.retained_rectangles_presented == 2U,
           "a retained present records its copied pixels and rectangles");
    expect(metrics.live_frames_presented == 2U && metrics.live_pixels_presented == 20000U &&
               metrics.live_rectangles_presented == 2U &&
               metrics.live_present_duration_nanoseconds == 1000U,
           "live frames are counted apart from retained presents");
    const std::string json = metrics.to_json();
    expect(json.find("\"live_pixels_presented\":20000") != std::string::npos &&
               json.find("\"retained_pixels_presented\":2400") != std::string::npos,
           "the counters appear in the metrics JSON");
}

} // namespace

int main() {
    distant_rectangles_paint_their_sum_and_nothing_between();
    nearly_touching_rectangles_merge();
    single_rectangle_paint_is_unchanged();
    accumulated_damage_paints_as_rectangles();
    presentation_counters_record_copied_area();
    std::cout << "gui_forms_damage_rectangles_tests: all tests passed\n";
    return EXIT_SUCCESS;
}
