// T2's gate evidence (T2 plan, phase E11). Two halves of the gate are
// checked here for every registered primitive, without listing any by hand.
//
// - Reference images: every primitive in builtin_primitive_registry() is
//   declared by a theme rule on one part, drawn through the window at four
//   scales and three states, and hashed: the extent and bytes of every part
//   raster composited. The manifest also counts the window's other drawing
//   calls (the built-in recipe draws a part its primitive refused) and the
//   engine's refusals. With GUI_FORMS_ENGINE_GATE_MANIFEST=<file>
//   the hashes are written as a manifest. Every CI host writes one, and
//   tools/engine_gate.py compares them across hosts, failing on any
//   difference. A primitive registered later joins the manifest by
//   registration alone.
// - Idle frames: once inputs stop and every settling primitive has settled,
//   the window counters show no frame over a two-second window. This is
//   checked for each primitive's window and for the ribbon's rise. The
//   ribbon under reduced motion is also checked: hovering it runs no
//   motion frame.
//
// The other two halves are proved by the primitives' own tests and listed
// in results/2026-10-22-t2-engine-gate/README.md: cost fallback (a part
// whose cost cannot be bounded is drawn by the built-in recipe) and
// reduced motion.

#include "gui_forms/gui_forms.hpp"

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace {
using namespace gui_forms;
using std::chrono::milliseconds;

void require(bool condition, const std::string& message) {
    if (condition) return;
    std::cerr << "gui_forms_engine_gate_tests: " << message << '\n';
    std::exit(1);
}

// FNV-1a over bytes and words, in order.
struct GateHash final {
    std::uint64_t value{14695981039346656037ULL};

    void add_byte(std::uint8_t byte) {
        value = value ^ static_cast<std::uint64_t>(byte);
        value = value * 1099511628211ULL;
    }
    void add_bytes(std::span<const std::uint8_t> bytes) {
        for (const std::uint8_t byte : bytes) add_byte(byte);
    }
    void add_word(std::uint64_t word) {
        for (int shift = 0; shift < 64; shift += 8) add_byte(static_cast<std::uint8_t>((word >> shift) & 0xFFU));
    }
};

std::string hex(std::uint64_t value) {
    std::ostringstream text;
    text << std::hex << std::setw(16) << std::setfill('0') << value;
    return text.str();
}

class ManualTime final : public MotionTimeSource {
public:
    [[nodiscard]] FrameTime now() const noexcept override { return time; }
    FrameTime time{std::chrono::seconds(1000)};
};

// A painter that hashes the part rasters composited and counts the
// other drawing calls (fills, strokes, lines, box shadows); it draws nothing.
class GatePainter final : public Painter {
public:
    void save() override {}
    void restore() override {}
    void translate(Point) override {}
    void clip_rect(Rect) override {}
    void fill_rect(Rect, Color) override { painter_calls = painter_calls + 1U; }
    void stroke_rect(Rect, Color, double) override { painter_calls = painter_calls + 1U; }
    void draw_line(Point, Point, Color, double) override { painter_calls = painter_calls + 1U; }
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override {}
    void draw_image(ImageId, Rect, double) override {}
    void draw_box_shadow(Rect, double, Point, double, double, Color) override {
        painter_calls = painter_calls + 1U;
    }
    void draw_part_raster(const std::shared_ptr<const PartRaster>& raster, Point, double) override {
        add_raster(*raster);
    }
    void draw_part_raster_scaled(const std::shared_ptr<const PartRaster>& raster, Rect, double) override {
        add_raster(*raster);
    }

    void reset() {
        hash = GateHash{};
        rasters = 0U;
        painter_calls = 0U;
    }

    GateHash hash{};
    std::size_t rasters{};
    std::size_t painter_calls{};

private:
    void add_raster(const PartRaster& raster) {
        const DeviceExtent extent = raster.extent();
        hash.add_word(extent.width);
        hash.add_word(extent.height);
        hash.add_bytes(raster.pixels());
        rasters = rasters + 1U;
    }
};

// One part of the primitive's first role, 96 by 28 at (20, 20) with radius
// 4, in the states the test sets.
class Subject final : public Control {
public:
    Subject(StableId stable_id, PartRole role) : Control(std::move(stable_id)), role_(role) {}
    void on_paint(Painter& painter, Rect) override {
        PartPaint part{};
        part.role = role_;
        part.family = ControlVisualRole::button;
        part.bounds = Rect{20.0, 20.0, 96.0, 28.0};
        part.corner_radius = 4.0;
        part.key = PartKey{"subject", {}, {}};
        part.states = states;
        paint_part(painter, *this, part);
    }
    AnatomyStateSet states{};

private:
    PartRole role_;
};

struct SubjectFixture final {
    ManualTime time{};
    std::shared_ptr<Panel> root;
    std::shared_ptr<Subject> subject;
    std::unique_ptr<Window> window;
    GatePainter painter{};
    std::size_t diagnostics{};
};

std::unique_ptr<SubjectFixture> make_subject(const EnginePrimitive& primitive, double scale,
                                             AnatomyStateSet states) {
    const PrimitiveDescriptor& descriptor = primitive.descriptor();
    require(!descriptor.roles.empty(), std::string(descriptor.name) + " paints at least one role");
    const PartRole role = descriptor.roles[0];
    ThemeDefinition definition = windows_professional_theme_definition();
    definition.id = "engine-gate";
    ThemeRule rule{};
    rule.label = "gate";
    rule.level = CascadeLevel::theme_sheet;
    rule.selector.stable_id = "subject";
    rule.selector.role = role;
    rule.declarations.push_back(RuleDeclaration::primitive(std::string(descriptor.name)));
    definition.rules.rules.push_back(rule);
    std::unique_ptr<SubjectFixture> fixture = std::make_unique<SubjectFixture>();
    SubjectFixture& made = *fixture;
    made.root = make_control<Panel>(StableId("root"));
    made.subject = make_control<Subject>(StableId("subject"), role);
    (*made.subject).states = states;
    (*made.subject).set_requested_bounds(Rect{0.0, 0.0, 136.0, 68.0});
    (*made.root).add_child(made.subject);
    made.window = std::make_unique<Window>(made.root, Size{176.0, 108.0});
    Window& window = *made.window;
    window.motion_clock().set_time_source(&made.time);
    window.set_scale(scale);
    window.set_theme(Theme::create(definition));
    made.diagnostics = window.theme().engine().rules().diagnostics().size();
    window.perform_layout();
    made.painter.reset();
    window.paint(made.painter);
    static_cast<void>(window.take_damage());
    return fixture;
}

// One 16 ms frame of the test's time: poll, then paint what it damaged.
// Returns whether the poll ran anything or left damage.
bool run_frame(Window& window, ManualTime& time, Painter& painter) {
    time.time += milliseconds(16);
    const FramePollResult result = window.poll_frame_schedule(time.time);
    window.perform_layout();
    const DamageRegion damage = window.take_damage();
    if (!damage.empty()) window.paint(painter, damage.bounds());
    const std::uint64_t work = result.deadlines_fired + result.active_surface_ticks + result.ui_timer_ticks +
                               result.motion_clock_ticks;
    const bool ran = work != 0U || !damage.empty() || result.damage_pending;
    return ran;
}

// What the window counters showed over an idle stretch.
struct IdleWindow final {
    std::size_t frames_run{};
    std::uint64_t frame_requests{};
    std::uint64_t paint_passes{};
    std::uint64_t motion_clock_ticks{};
    std::uint64_t ambient_frames{};
};

constexpr std::size_t settle_limit = 240U;
constexpr std::size_t idle_polls = 125U; // two seconds of 16 ms frames

// Runs frames until one runs nothing and no idle keyframe bake waits, then
// counts what the window does over idle_polls more.
IdleWindow settle_then_watch(Window& window, ManualTime& time, Painter& painter, const std::string& subject) {
    std::size_t step = 0U;
    while (step < settle_limit) {
        const bool ran = run_frame(window, time, painter);
        step = step + 1U;
        if (!ran && !window.engine_parts().idle_keyframes_waiting()) break;
    }
    require(step < settle_limit, subject + " settles within " + std::to_string(settle_limit) + " frames");
    const MetricsSnapshot before = window.metrics_snapshot();
    IdleWindow idle{};
    for (std::size_t poll = 0U; poll < idle_polls; ++poll) {
        if (run_frame(window, time, painter)) idle.frames_run = idle.frames_run + 1U;
    }
    const MetricsSnapshot after = window.metrics_snapshot();
    idle.frame_requests = after.scheduled_frame_requests - before.scheduled_frame_requests;
    idle.paint_passes = after.paint_passes - before.paint_passes;
    idle.motion_clock_ticks = after.motion_clock_ticks - before.motion_clock_ticks;
    idle.ambient_frames = after.engine_ambient_frames - before.engine_ambient_frames;
    return idle;
}

bool silent(const IdleWindow& idle) {
    const bool quiet = idle.frames_run == 0U && idle.frame_requests == 0U && idle.paint_passes == 0U &&
                       idle.motion_clock_ticks == 0U && idle.ambient_frames == 0U;
    return quiet;
}

// The manifest's lines, written at the end when GUI_FORMS_ENGINE_GATE_MANIFEST
// names a file.
std::vector<std::string> manifest_lines{};

void add_counter(const std::string& name, std::uint64_t value) {
    manifest_lines.push_back("counter\t" + name + '\t' + std::to_string(value));
}

void add_idle(const std::string& subject, const IdleWindow& idle) {
    add_counter(subject + ".idle_frames_run", idle.frames_run);
    add_counter(subject + ".idle_frame_requests", idle.frame_requests);
    add_counter(subject + ".idle_paint_passes", idle.paint_passes);
    add_counter(subject + ".idle_motion_clock_ticks", idle.motion_clock_ticks);
    add_counter(subject + ".idle_ambient_frames", idle.ambient_frames);
}

struct StateCase final {
    const char* name;
    AnatomyState state;
    bool set;
};

constexpr std::array<StateCase, 3> state_cases{{
    {"rest", AnatomyState::hot, false},
    {"hot", AnatomyState::hot, true},
    {"pressed", AnatomyState::pressed, true},
}};

constexpr std::array<double, 4> gate_scales{1.0, 1.25, 1.5, 2.0};

void every_registered_primitive() {
    const std::span<const EnginePrimitive* const> primitives = builtin_primitive_registry().primitives();
    require(!primitives.empty(), "the built-in registry holds primitives");
    manifest_lines.push_back("registry\t" + std::to_string(primitives.size()));
    for (const EnginePrimitive* const primitive : primitives) {
        const std::string name((*primitive).descriptor().name);
        for (const StateCase& state_case : state_cases) {
            AnatomyStateSet states{};
            if (state_case.set) states.insert(state_case.state);
            for (const double scale : gate_scales) {
                std::unique_ptr<SubjectFixture> fixture = make_subject(*primitive, scale, states);
                SubjectFixture& made = *fixture;
                const std::string subject = name + ' ' + state_case.name + " at " + std::to_string(scale);
                require(made.diagnostics == 0U, subject + ": a rule declaring the primitive compiles cleanly");
                require(made.painter.rasters > 0U || made.painter.painter_calls > 0U,
                        subject + ": the part is drawn by the primitive or by the built-in recipe");
                const std::uint64_t first = made.painter.hash.value;
                const std::size_t rasters = made.painter.rasters;
                const std::size_t painter_calls = made.painter.painter_calls;
                const std::uint64_t refusals = (*made.window).metrics_snapshot().engine_primitive_refusals;
                // A second window draws the same bytes: nothing depends on
                // anything but the primitive, the part and the scale.
                std::unique_ptr<SubjectFixture> again = make_subject(*primitive, scale, states);
                require((*again).painter.hash.value == first && (*again).painter.rasters == rasters,
                        subject + ": drawing it again gives the same bytes");
                const IdleWindow idle = settle_then_watch(*made.window, made.time, made.painter, subject);
                require(silent(idle), subject + ": once settled, the window runs no frame");
                std::ostringstream scale_text;
                scale_text << std::fixed << std::setprecision(2) << scale;
                manifest_lines.push_back("primitive\t" + name + '\t' + state_case.name + '\t' + scale_text.str() +
                                         '\t' + std::to_string(rasters) + '\t' + std::to_string(painter_calls) +
                                         '\t' + std::to_string(refusals) + '\t' + hex(first));
                if (state_case.set || scale != 1.0) continue;
                add_idle("primitive." + name, idle);
            }
        }
    }
}

// -- the ribbon's rise: inputs, then rest --------------------------------------

struct RibbonFixture final {
    ManualTime time{};
    std::shared_ptr<Command> paste = std::make_shared<Command>("paste", "Paste");
    std::shared_ptr<Ribbon> ribbon;
    std::unique_ptr<Window> window;
    GatePainter painter{};
};

std::unique_ptr<RibbonFixture> make_ribbon(bool reduced) {
    std::unique_ptr<RibbonFixture> fixture = std::make_unique<RibbonFixture>();
    RibbonFixture& made = *fixture;
    std::vector<RibbonTabSpec> tabs{};
    RibbonTabSpec home{.stable_id = "home", .text = "&Home"};
    RibbonGroupSpec clipboard{.stable_id = "clipboard", .caption = "Clipboard"};
    clipboard.commands.push_back({.stable_id = "paste", .command = made.paste});
    home.groups.push_back(std::move(clipboard));
    tabs.push_back(std::move(home));
    made.ribbon = make_control<Ribbon>(StableId("ribbon"));
    (*made.ribbon).set_tabs(std::move(tabs));
    made.window = std::make_unique<Window>(made.ribbon, Size{600.0, 112.0});
    Window& window = *made.window;
    window.motion_clock().set_time_source(&made.time);
    if (reduced) {
        PresentationSettings settings = window.presentation_settings();
        settings.reduced_motion = true;
        window.set_presentation_settings(settings);
    }
    window.perform_layout();
    window.paint(made.painter);
    static_cast<void>(window.take_damage());
    return fixture;
}

Point paste_centre(const Ribbon& ribbon) {
    const std::vector<Ribbon::CommandItem> items = ribbon.realized_commands();
    for (const Ribbon::CommandItem& item : items) {
        if (item.stable_id != "paste") continue;
        const Rect bounds = item.parts.command_face.bounds;
        const Point centre{bounds.x + bounds.width * 0.5, bounds.y + bounds.height * 0.5};
        return centre;
    }
    require(false, "the paste command is realized");
    return {};
}

void move_to(Window& window, Point point) {
    static_cast<void>(window.dispatch_pointer({PointerAction::move, PointerButton::none, point}));
    window.perform_layout();
}

void ribbon_rests_after_motion() {
    std::unique_ptr<RibbonFixture> fixture = make_ribbon(false);
    RibbonFixture& made = *fixture;
    Window& window = *made.window;
    const MetricsSnapshot start = window.metrics_snapshot();
    // Inputs: onto the paste face (it rises), away (it falls), on again and
    // away again before either settles.
    move_to(window, paste_centre(*made.ribbon));
    for (int frame = 0; frame < 10; ++frame) static_cast<void>(run_frame(window, made.time, made.painter));
    move_to(window, Point{590.0, 60.0});
    for (int frame = 0; frame < 5; ++frame) static_cast<void>(run_frame(window, made.time, made.painter));
    move_to(window, paste_centre(*made.ribbon));
    for (int frame = 0; frame < 5; ++frame) static_cast<void>(run_frame(window, made.time, made.painter));
    move_to(window, Point{590.0, 60.0});
    const MetricsSnapshot moved = window.metrics_snapshot();
    require(moved.motion_clock_ticks > start.motion_clock_ticks, "the ribbon's rise ran motion frames");
    const IdleWindow idle = settle_then_watch(window, made.time, made.painter, "the ribbon");
    require(window.metrics_snapshot().motion_values_moving == 0U && !window.motion_clock().armed(),
            "every motion value of the ribbon has settled");
    require(silent(idle), "once the ribbon has settled, the window runs no frame for two seconds");
    add_counter("ribbon.motion_clock_ticks_while_moving", moved.motion_clock_ticks - start.motion_clock_ticks);
    add_idle("ribbon", idle);
}

void ribbon_without_motion() {
    std::unique_ptr<RibbonFixture> fixture = make_ribbon(true);
    RibbonFixture& made = *fixture;
    Window& window = *made.window;
    const MetricsSnapshot start = window.metrics_snapshot();
    move_to(window, paste_centre(*made.ribbon));
    for (int frame = 0; frame < 10; ++frame) static_cast<void>(run_frame(window, made.time, made.painter));
    const MetricsSnapshot after = window.metrics_snapshot();
    require(after.motion_clock_ticks == start.motion_clock_ticks,
            "under reduced motion, hovering the ribbon runs no motion frame");
    require(after.motion_values_finished > start.motion_values_finished,
            "the rise jumps to its end instead");
    add_counter("reduced_motion.ribbon.motion_clock_ticks", after.motion_clock_ticks - start.motion_clock_ticks);
    add_counter("reduced_motion.ribbon.values_jumped", after.motion_values_finished - start.motion_values_finished);
}

void write_manifest() {
    const char* const path = std::getenv("GUI_FORMS_ENGINE_GATE_MANIFEST");
    if (path == nullptr || *path == '\0') return;
    std::FILE* const file = std::fopen(path, "wb");
    require(file != nullptr, std::string("the manifest can be written: ") + path);
    std::string text = "# gui_forms engine gate manifest v1\n";
    text += "# primitive\tname\tstate\tscale\trasters\tpainter_calls\trefusals\thash\n";
    for (const std::string& line : manifest_lines) text += line + '\n';
    const std::size_t written = std::fwrite(text.data(), 1U, text.size(), file);
    const int closed = std::fclose(file);
    require(written == text.size() && closed == 0, std::string("the manifest is written whole: ") + path);
}

} // namespace

int main() {
    every_registered_primitive();
    ribbon_rests_after_motion();
    ribbon_without_motion();
    write_manifest();
    return 0;
}
