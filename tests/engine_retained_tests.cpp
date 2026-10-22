// The engine's retained forms (T2 plan, phase E7r; owner, 2026-10-22):
// nothing in the engine bakes per frame. A part's face that changes between
// discrete states bakes each endpoint once and cross-fades them through a
// paint group (form a): the old face at 1 - t, the new one added at t.

#include "gui_forms/gui_forms.hpp"

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace {
using namespace gui_forms;
using std::chrono::milliseconds;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << "gui_forms_engine_retained_tests: " << message << '\n';
    std::exit(1);
}

class ManualTime final : public MotionTimeSource {
public:
    [[nodiscard]] FrameTime now() const noexcept override { return time; }
    FrameTime time{std::chrono::seconds(1000)};
};

// What one paint composited: each part raster with its opacity, and each
// group with its opacity and composite, in order.
enum class CallKind : std::uint8_t { raster, begin_group, end_group };

struct PaintCall final {
    CallKind kind{CallKind::raster};
    std::shared_ptr<const PartRaster> raster{};
    double opacity{};
    PaintGroupComposite composite{PaintGroupComposite::source_over};
    bool isolated{};
};

class CallWatch final : public Painter {
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
    void draw_part_raster(const std::shared_ptr<const PartRaster>& raster, Point, double opacity) override {
        PaintCall call{};
        call.raster = raster;
        call.opacity = opacity;
        calls.push_back(call);
    }
    void begin_group(const PaintGroup& group) override {
        PaintCall call{};
        call.kind = CallKind::begin_group;
        call.opacity = group.opacity;
        call.composite = group.composite;
        call.isolated = group.isolated;
        calls.push_back(call);
    }
    void end_group() override {
        PaintCall call{};
        call.kind = CallKind::end_group;
        calls.push_back(call);
    }
    std::vector<PaintCall> calls;
};

// A control showing one keyed face, hot while the pointer is over it.
class Pad final : public Control {
public:
    explicit Pad(StableId stable_id) : Control(std::move(stable_id)) {}
    void on_pointer(PointerEvent& event) override {
        if (event.action == PointerAction::enter) {
            hot = true;
            invalidate(Dirty::paint);
        }
        if (event.action == PointerAction::leave) {
            hot = false;
            invalidate(Dirty::paint);
        }
    }
    void on_paint(Painter& painter, Rect) override {
        PartPaint part{};
        part.role = PartRole::face;
        part.family = ControlVisualRole::button;
        part.bounds = Rect{20.0, 20.0, 40.0, 24.0};
        part.key = PartKey{"face", {}, {}};
        if (hot) part.states.insert(AnatomyState::hot);
        paint_part(painter, *this, part);
    }
    bool hot{};
};

// The pad's face is the material plate, cross-faded over 100 ms.
ThemeRule fading_rule(std::string id) {
    ThemeRule rule{};
    rule.label = id;
    rule.level = CascadeLevel::theme_sheet;
    rule.selector.stable_id = std::move(id);
    rule.selector.part = "face";
    rule.declarations.push_back(RuleDeclaration::primitive("pill_plate"));
    PropertyTransition fade{};
    fade.property = std::string(face_property);
    fade.curve = MotionCurve::timed(milliseconds(100), MotionEasing::linear);
    rule.transitions.push_back(fade);
    return rule;
}

struct Fixture final {
    ManualTime time{};
    std::shared_ptr<Panel> root;
    std::shared_ptr<Pad> pad;
    std::unique_ptr<Window> window;
    CallWatch painter{};
};

std::unique_ptr<Fixture> make_fixture() {
    std::unique_ptr<Fixture> fixture = std::make_unique<Fixture>();
    Fixture& made = *fixture;
    made.root = make_control<Panel>(StableId("root"));
    made.pad = make_control<Pad>(StableId("pad"));
    (*made.pad).set_requested_bounds(Rect{100.0, 100.0, 100.0, 80.0});
    (*made.root).add_child(made.pad);
    made.window = std::make_unique<Window>(made.root, Size{400.0, 300.0});
    Window& window = *made.window;
    window.motion_clock().set_time_source(&made.time);
    ThemeDefinition definition = windows_professional_theme_definition();
    definition.id = "engine-retained-test";
    definition.rules.rules.push_back(fading_rule("pad"));
    window.set_theme(Theme::create(definition));
    require(window.theme().engine().rules().diagnostics().empty(), "the fading theme compiles cleanly");
    window.perform_layout();
    window.paint(made.painter);
    static_cast<void>(window.take_damage());
    return fixture;
}

void repaint(Fixture& fixture) {
    Window& window = *fixture.window;
    fixture.painter.calls.clear();
    window.perform_layout();
    const DamageRegion damage = window.take_damage();
    if (!damage.empty()) window.paint(fixture.painter, damage.bounds());
}

std::uint64_t bakes(const Window& window) {
    const std::uint64_t count = window.engine_parts().cache_statistics().bakes;
    return count;
}

// The paint of one cross-fade frame: an isolated group holding the old face
// at 1 - t and a plus group at t holding the new face. Returns t.
double fade_frame(const std::vector<PaintCall>& calls, const std::shared_ptr<const PartRaster>& from,
                  const std::shared_ptr<const PartRaster>& to) {
    require(calls.size() == 6U, "a fading face is two groups and two rasters");
    require(calls[0].kind == CallKind::begin_group && calls[0].isolated && calls[0].opacity == 1.0 &&
                calls[0].composite == PaintGroupComposite::source_over,
            "an isolated group at the part's opacity holds the fade");
    require(calls[1].kind == CallKind::raster && calls[1].raster == from, "the old face is drawn first");
    require(calls[2].kind == CallKind::begin_group && calls[2].composite == PaintGroupComposite::plus,
            "the new face is added through a plus group");
    require(calls[3].kind == CallKind::raster && calls[3].raster == to && calls[3].opacity == 1.0,
            "the new face is drawn whole inside it");
    require(calls[4].kind == CallKind::end_group && calls[5].kind == CallKind::end_group, "both groups end");
    const double fade = calls[2].opacity;
    require(std::abs(calls[1].opacity - (1.0 - fade)) < 1e-12, "the old face shows at one less the new one's share");
    return fade;
}

void face_cross_fades_between_baked_endpoints() {
    std::unique_ptr<Fixture> fixture = make_fixture();
    Fixture& made = *fixture;
    Window& window = *made.window;
    require(made.painter.calls.size() == 1U && made.painter.calls[0].kind == CallKind::raster,
            "at rest the face is one raster");
    const std::shared_ptr<const PartRaster> rest = made.painter.calls[0].raster;
    const std::uint64_t rested = bakes(window);

    static_cast<void>(window.dispatch_pointer({PointerAction::move, PointerButton::none, Point{150.0, 140.0}}));
    repaint(made);
    require(bakes(window) == rested + 1U, "the hot face is baked once, at the change");
    require(made.painter.calls.size() == 6U, "the change starts a cross-fade");
    const std::shared_ptr<const PartRaster> hot = made.painter.calls[3].raster;
    require(hot != rest, "the hot face is a face of its own");
    require(fade_frame(made.painter.calls, rest, hot) == 0.0, "the fade starts at the old face");

    double previous = 0.0;
    std::size_t frames = 0U;
    for (int step = 0; step < 40 && window.motion_clock().armed(); ++step) {
        made.time.time += milliseconds(16);
        static_cast<void>(window.poll_frame_schedule(made.time.time));
        repaint(made);
        require(bakes(window) == rested + 1U, "nothing is baked while the faces cross-fade");
        if (made.painter.calls.size() != 6U) break;
        const double fade = fade_frame(made.painter.calls, rest, hot);
        require(fade > previous, "the new face's share grows");
        previous = fade;
        frames = frames + 1U;
    }
    require(frames >= 3U, "the fade spans frames");
    require(!window.motion_clock().armed(), "the fade settles");
    require(made.painter.calls.size() == 1U && made.painter.calls[0].raster == hot &&
                made.painter.calls[0].opacity == 1.0,
            "settled, the new face is drawn alone");

    // Back: the cold face is baked again once, and fades in.
    static_cast<void>(window.dispatch_pointer({PointerAction::move, PointerButton::none, Point{390.0, 290.0}}));
    repaint(made);
    const std::uint64_t leaving = bakes(window);
    require(leaving == rested + 2U, "the cold face is baked once, at the change");
    const std::shared_ptr<const PartRaster> cold = made.painter.calls[3].raster;
    require(fade_frame(made.painter.calls, hot, cold) == 0.0, "the fade back starts at the hot face");
    for (int step = 0; step < 40 && window.motion_clock().armed(); ++step) {
        made.time.time += milliseconds(16);
        static_cast<void>(window.poll_frame_schedule(made.time.time));
        repaint(made);
        require(bakes(window) == leaving, "nothing is baked while it fades back");
    }
    require(made.painter.calls.size() == 1U && made.painter.calls[0].raster == cold, "the cold face rests alone");
}

void face_transition_is_checked_at_load() {
    ThemeRuleSheet sheet{};
    ThemeRule stated = fading_rule("stated");
    PresentationState state{};
    state.target = 1.0;
    stated.transitions[0].states.push_back(state);
    sheet.rules.push_back(stated);
    sheet.rules.push_back(fading_rule("plain"));
    const CompiledThemeRules rules = CompiledThemeRules::compile(sheet, builtin_primitive_registry());
    std::size_t invalid = 0U;
    for (const RuleDiagnostic& diagnostic : rules.diagnostics()) {
        if (diagnostic.kind == RuleDiagnosticKind::transition_invalid && diagnostic.property == face_property) {
            invalid = invalid + 1U;
        }
    }
    require(invalid == 1U, "a face cross-fade naming states is refused at load");
    CompiledThemeRules::PartFacts facts{};
    facts.stable_id = "plain";
    facts.part = "face";
    facts.role = PartRole::face;
    const CompiledThemeRules::Resolution resolution = rules.resolve(facts);
    require(resolution.transition_count == 1U &&
                (*resolution.transitions[0]).property == PresentationProperty::face,
            "a face transition resolves as the engine property");
    for (const ThemeRule& rule : built_in_theme_rules().rules) {
        for (const PropertyTransition& transition : rule.transitions) {
            require(transition.property != face_property, "the built-in rules never cross-fade faces");
        }
    }
}

} // namespace

int main() {
    face_transition_is_checked_at_load();
    face_cross_fades_between_baked_endpoints();
    std::cout << "engine retained form tests passed\n";
    return 0;
}
