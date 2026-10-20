// The theme engine's entry point and its per-window part table (T2 plan,
// phase E2a): a keyed part has a record in its window's table, a stale
// handle finds nothing, a control's records are released when it leaves the
// window, the index fallback is counted, every window of an application
// shares one engine, and a key changes nothing that is drawn.

#include "gui_forms/control.hpp"
#include "gui_forms/engine.hpp"
#include "gui_forms/resources.hpp"
#include "gui_forms/theme.hpp"
#include "gui_forms/window.hpp"

#include <array>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

// Records each command's name and its first rectangle or point, enough to
// tell whether two paints drew the same thing.
class TracePainter final : public Painter {
public:
    void save() override { trace += "save\n"; }
    void restore() override { trace += "restore\n"; }
    void translate(Point offset) override { line("translate", {offset.x, offset.y, 0.0, 0.0}); }
    void clip_rect(Rect rect) override { line("clip", rect); }
    void fill_rect(Rect rect, Color) override { line("fill", rect); }
    void fill_rounded_rect(Rect rect, double, Color) override { line("fill_rounded", rect); }
    void stroke_rect(Rect rect, Color, double) override { line("stroke", rect); }
    void stroke_rounded_rect(Rect rect, double, Color, double) override { line("stroke_rounded", rect); }
    void draw_line(Point from, Point to, Color, double) override { line("line", {from.x, from.y, to.x, to.y}); }
    void draw_text_utf8(Point origin, std::string_view text, FontSpec, Color) override {
        line("text", {origin.x, origin.y, 0.0, 0.0});
        trace += text;
        trace += '\n';
    }
    void draw_image(ImageId image, Rect destination, double opacity) override {
        line("image", destination);
        trace += std::to_string(image.value) + " " + std::to_string(opacity) + "\n";
        last_image_opacity = opacity;
    }

    std::string trace;
    double last_image_opacity{};

private:
    void line(const char* name, Rect rect) {
        trace += name;
        trace += ' ' + std::to_string(rect.x) + ' ' + std::to_string(rect.y) + ' ' + std::to_string(rect.width) +
                 ' ' + std::to_string(rect.height) + '\n';
    }
};

// A control that paints three keyed parts: a face, an item with a stable
// ID and an item without one (the index fallback), and one part with no
// key. `keyed` turns the keys off, to compare what is drawn.
class KeyedControl final : public Control {
public:
    explicit KeyedControl(StableId id) : Control(std::move(id)) {}

    void on_paint(Painter& painter, Rect) override {
        PartPaint face{};
        face.role = PartRole::face;
        face.family = ControlVisualRole::button;
        face.bounds = {0.0, 0.0, 60.0, 20.0};
        face.states = face_states;
        if (keyed) face.key = PartKey{"face", {}, {}};
        paint_part(painter, *this, face);

        PartPaint named = face;
        named.role = PartRole::label;
        named.bounds = {0.0, 24.0, 60.0, 14.0};
        named.text = "Named";
        named.key.reset();
        if (keyed) named.key = PartKey{"item_label", "item.alpha", item_position};
        paint_part(painter, *this, named);

        PartPaint indexed = named;
        indexed.bounds = {0.0, 40.0, 60.0, 14.0};
        indexed.text = "Indexed";
        indexed.key.reset();
        if (keyed) indexed.key = PartKey{"item_label", {}, item_position};
        paint_part(painter, *this, indexed);

        PartPaint plain = face;
        plain.bounds = {0.0, 56.0, 60.0, 4.0};
        plain.role = PartRole::divider;
        plain.key.reset();
        paint_part(painter, *this, plain);

        // A keyed part with empty bounds paints nothing and is not recorded.
        PartPaint empty = face;
        empty.bounds = {};
        if (keyed) empty.key = PartKey{"empty", {}, {}};
        paint_part(painter, *this, empty);
    }

    bool keyed{true};
    std::size_t item_position{0U};
    AnatomyStateSet face_states{};
};

void paint_window(Window& window) {
    TracePainter painter;
    static_cast<void>(window.paint(painter, Rect{0.0, 0.0, 200.0, 120.0}));
}

// -- the table alone -----------------------------------------------------------

void test_table_records_and_handles() {
    EnginePartTable table{};
    require(table.record_count() == 0U, "a new table is empty");
    require(table.record(PartRecordHandle{}) == nullptr, "a default handle names nothing");

    const PartRecordAcquisition face = table.acquire("control.a", PartKey{"face", {}, {}});
    require(face.created && table.record_count() == 1U, "the first acquire makes a record");
    const PartRecordAcquisition again = table.acquire("control.a", PartKey{"face", {}, {}});
    require(!again.created && again.handle == face.handle, "the same key finds the same record");
    const EnginePartRecord* const record = table.record(face.handle);
    require(record != nullptr && (*record).control == "control.a" && (*record).part == "face",
            "the record owns its identity");

    // An item with a stable ID keeps its record wherever it moves; an item
    // without one is identified by its index.
    const PartRecordAcquisition first = table.acquire("control.a", PartKey{"row", "item.1", 0U});
    const PartRecordAcquisition moved = table.acquire("control.a", PartKey{"row", "item.1", 5U});
    require(first.created && !moved.created && moved.handle == first.handle,
            "an item with a stable ID is the same part at any index");
    const PartRecordAcquisition at_zero = table.acquire("control.a", PartKey{"row", {}, 0U});
    const PartRecordAcquisition at_one = table.acquire("control.a", PartKey{"row", {}, 1U});
    require(at_zero.created && at_one.created && !(at_zero.handle == at_one.handle),
            "without a stable ID each index is its own part");
    require((*table.record(at_one.handle)).item_index == std::optional<std::size_t>{1U},
            "the fallback record keeps its index");
    require(!(*table.record(first.handle)).item_index, "an item with a stable ID keeps no index");

    // The control's ID is part of the key, and field boundaries are exact.
    const PartRecordAcquisition other = table.acquire("control.b", PartKey{"face", {}, {}});
    require(other.created, "another control's part is another record");
    const PartRecordAcquisition shifted = table.acquire("control.af", PartKey{"ace", {}, {}});
    require(shifted.created, "keys that join to the same bytes stay distinct");
    require(table.record_count() == 6U, "six records are live");

    require(table.find("control.a", PartKey{"face", {}, {}}) == std::optional<PartRecordHandle>{face.handle},
            "find returns the live record's handle");
    require(!table.find("control.a", PartKey{"missing", {}, {}}), "find makes nothing");
    require(table.record_count() == 6U, "find leaves the table unchanged");
}

void test_release_makes_handles_stale() {
    EnginePartTable table{};
    const PartRecordAcquisition face = table.acquire("control.a", PartKey{"face", {}, {}});
    const PartRecordAcquisition row = table.acquire("control.a", PartKey{"row", {}, 2U});
    const PartRecordAcquisition kept = table.acquire("control.b", PartKey{"face", {}, {}});
    require(table.release_control("control.a") == 2U, "both of the control's records are released");
    require(table.record_count() == 1U, "the other control's record remains");
    require(table.record(face.handle) == nullptr && table.record(row.handle) == nullptr,
            "a released record's handle is stale");
    require(table.record(kept.handle) != nullptr, "another control's handle stays live");
    require(!table.find("control.a", PartKey{"face", {}, {}}), "a released key finds nothing");
    require(table.release_control("control.a") == 0U, "releasing again releases nothing");

    // A released slot is reused, under a new generation: the old handle
    // still finds nothing.
    const PartRecordAcquisition reused = table.acquire("control.c", PartKey{"face", {}, {}});
    require(reused.created && (reused.handle.slot == face.handle.slot || reused.handle.slot == row.handle.slot),
            "a released slot is reused before the table grows");
    require(table.record(face.handle) == nullptr && table.record(row.handle) == nullptr,
            "a handle from before the release stays stale after its slot is reused");
    require(table.record(reused.handle) != nullptr, "the new record is live");
}

// -- the engine in a window ----------------------------------------------------

void test_keyed_parts_have_records() {
    const std::shared_ptr<Control> root = make_control<Control>(StableId("root"));
    const std::shared_ptr<KeyedControl> keyed = make_control<KeyedControl>(StableId("keyed"));
    (*keyed).set_requested_bounds({0.0, 0.0, 80.0, 70.0});
    (*root).add_child(keyed);
    Window window(root, Size{200.0, 120.0});
    window.perform_layout();
    paint_window(window);

    const EnginePartTable& table = window.engine_parts();
    MetricsSnapshot metrics = window.metrics_snapshot();
    require(table.record_count() == 3U, "the three keyed parts have records; the unkeyed and empty ones do not");
    require(metrics.engine_part_records == 3U, "the metrics count the live records");
    require(metrics.engine_part_index_fallbacks == 1U, "the item without a stable ID counts one fallback");

    // A repaint finds the same records and counts no new fallback.
    paint_window(window);
    metrics = window.metrics_snapshot();
    require(window.engine_parts().record_count() == 3U && metrics.engine_part_index_fallbacks == 1U,
            "a repaint reuses the records");

    // The record keeps the states the part was last painted in.
    AnatomyStateSet hot{};
    hot.insert(AnatomyState::hot);
    (*keyed).face_states = hot;
    (*keyed).invalidate(Dirty::paint);
    paint_window(window);
    const EnginePartTable& probe = window.engine_parts();
    const std::optional<PartRecordHandle> face = probe.find("keyed", PartKey{"face", {}, {}});
    require(face.has_value(), "the face's record is found under the control's stable ID");
    require((*probe.record(*face)).states.contains(AnatomyState::hot), "the record holds the last painted states");

    // Moving the fallback item to another index makes a new record, and
    // counts another fallback; the named item keeps its record.
    (*keyed).item_position = 3U;
    (*keyed).invalidate(Dirty::paint);
    paint_window(window);
    metrics = window.metrics_snapshot();
    require(window.engine_parts().record_count() == 4U, "only the fallback item gains a record when it moves");
    require(metrics.engine_part_index_fallbacks == 2U, "the moved fallback item counts again");

    // Leaving the window releases every record of the control.
    const Control::Ptr removed = (*root).remove_child((*keyed).runtime_id());
    require(removed != nullptr, "the keyed control leaves the window");
    metrics = window.metrics_snapshot();
    require(window.engine_parts().record_count() == 0U, "the control's records are released on detach");
    require(metrics.engine_part_records == 0U && metrics.engine_part_records_released == 4U,
            "the metrics count the released records");
    require(probe.record(*face) == nullptr, "a handle kept past the detach is stale");

    // A detached control's keyed parts are drawn statically, with no record.
    TracePainter detached;
    (*keyed).on_paint(detached, Rect{0.0, 0.0, 80.0, 70.0});
    require(window.engine_parts().record_count() == 0U && !detached.trace.empty(),
            "a detached control paints without records");
}

void test_key_changes_nothing_drawn() {
    const std::shared_ptr<Control> root = make_control<Control>(StableId("root"));
    const std::shared_ptr<KeyedControl> keyed = make_control<KeyedControl>(StableId("keyed"));
    (*keyed).set_requested_bounds({0.0, 0.0, 80.0, 70.0});
    (*root).add_child(keyed);
    Window window(root, Size{200.0, 120.0});
    window.perform_layout();

    TracePainter with_keys;
    (*keyed).on_paint(with_keys, Rect{0.0, 0.0, 80.0, 70.0});
    (*keyed).keyed = false;
    TracePainter without_keys;
    (*keyed).on_paint(without_keys, Rect{0.0, 0.0, 80.0, 70.0});
    require(!with_keys.trace.empty() && with_keys.trace == without_keys.trace,
            "a key is identity: the same part draws the same commands with or without it");
}

void test_windows_share_the_application_engine() {
    const std::shared_ptr<ApplicationResources> resources = std::make_shared<ApplicationResources>();
    const std::shared_ptr<Control> first_root = make_control<Control>(StableId("first"));
    const std::shared_ptr<Control> second_root = make_control<Control>(StableId("second"));
    Window first(first_root, Size{100.0, 100.0}, resources);
    Window second(second_root, Size{100.0, 100.0}, resources);
    require(&first.theme().engine() == &second.theme().engine(), "windows sharing resources share one engine");
    require(&first.engine_parts() != &second.engine_parts(), "each window has its own part table");
    require(first.theme().engine().registry().find("dithered_bulge") != nullptr,
            "the engine's registry is the built-in one");
}

// -- the image role ------------------------------------------------------------

void test_image_role_draws_as_given() {
    const std::shared_ptr<Control> control = make_control<Control>(StableId("image.part"));
    require(part_role_name(PartRole::image) == "image", "the image role is registered");
    std::optional<PartRole> parsed = part_role_from_name("image");
    require(parsed == std::optional<PartRole>{PartRole::image}, "the image role parses by name");

    PartPaint picture{};
    picture.role = PartRole::image;
    picture.bounds = {2.0, 3.0, 40.0, 30.0};
    picture.image = ImageId{7U};
    TracePainter painter;
    paint_part(painter, *control, picture);
    require(painter.trace == "image 2.000000 3.000000 40.000000 30.000000\n7 1.000000\n",
            "an image part draws its picture once, into its bounds, at full opacity");

    picture.states.insert(AnatomyState::disabled);
    TracePainter disabled;
    paint_part(disabled, *control, picture);
    require(disabled.last_image_opacity == 0.45, "a disabled image part is drawn at 45 % opacity");

    PartPaint none = picture;
    none.image = ImageId{};
    TracePainter empty;
    paint_part(empty, *control, none);
    require(empty.trace.empty(), "an image part without a picture draws nothing");
}

} // namespace

int main() {
    test_table_records_and_handles();
    test_release_makes_handles_stale();
    test_keyed_parts_have_records();
    test_key_changes_nothing_drawn();
    test_windows_share_the_application_engine();
    test_image_role_draws_as_given();
    std::cout << "engine part table tests passed\n";
    return 0;
}
