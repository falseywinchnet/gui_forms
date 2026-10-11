#include "demo/gallery.hpp"
#include "demo/gallery_page_specs.hpp"
#include "gui_forms/gui_forms.hpp"
#include "headless_host.hpp"

#include <cstdlib>
#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

using namespace gui_forms;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

template <typename Predicate>
void require_eventually(Predicate predicate, const char* message) {
    const std::chrono::steady_clock::time_point deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (!predicate()) {
        if (std::chrono::steady_clock::now() >= deadline) {
            throw std::runtime_error(message);
        }
        std::this_thread::yield();
    }
}

std::uint64_t poll_active_surface_count(Window& window) {
    static_cast<void>(window.poll_frame_schedule(FrameTime{}));
    return window.metrics_snapshot().active_surface_count;
}

class DispatcherHasOnePending final {
public:
    explicit DispatcherHasOnePending(Window& window) noexcept : window_(window) {}

    bool operator()() const {
        return window_.dispatcher_snapshot().pending == 1U;
    }

private:
    Window& window_;
};

class GalleryDialogHandler final {
public:
    explicit GalleryDialogHandler(bool& cancel_save) noexcept
        : cancel_save_(cancel_save) {}

    HostDialogResult operator()(
        const HostDialogRequest& request,
        host::HeadlessHostServices&) const {
        if (std::holds_alternative<HostMessageDialogRequest>(request.payload)) {
            return {{}, request.request_id,
                    HostMessageDialogResult{HostDialogOutcome::accepted,
                                            HostDialogChoice::yes}};
        }

        if (std::holds_alternative<HostOpenFileDialogRequest>(request.payload)) {
            const HostOpenFileDialogRequest& payload =
                std::get<HostOpenFileDialogRequest>(request.payload);
            std::vector<std::string> paths{"/tmp/alpha.txt"};
            if (payload.allow_multiple) paths.push_back("/tmp/beta.log");
            return {{}, request.request_id,
                    HostPathDialogResult{HostDialogOutcome::accepted,
                                         std::move(paths)}};
        }

        if (std::holds_alternative<HostSaveFileDialogRequest>(request.payload)) {
            return cancel_save_
                ? HostDialogResult{{}, request.request_id,
                      HostPathDialogResult{HostDialogOutcome::cancelled, {}}}
                : HostDialogResult{{}, request.request_id,
                      HostPathDialogResult{HostDialogOutcome::accepted,
                                           {"/tmp/gui-forms-evidence.txt"}}};
        }

        if (std::holds_alternative<HostFolderDialogRequest>(request.payload)) {
            return {{}, request.request_id,
                    HostPathDialogResult{HostDialogOutcome::accepted,
                                         {"/tmp/evidence"}}};
        }

        return {{}, request.request_id,
                HostColorDialogResult{HostDialogOutcome::accepted,
                                      0x315F89FFU}};
    }

private:
    bool& cancel_save_;
};

class CountingPainter final : public Painter {
public:
    void save() override { ++commands; }
    void restore() override { ++commands; }
    void translate(Point) override { ++commands; }
    void clip_rect(Rect) override { ++commands; }
    void fill_rect(Rect, Color) override { ++commands; }
    void stroke_rect(Rect, Color, double) override { ++commands; }
    void draw_line(Point, Point, Color, double) override { ++commands; }
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override { ++commands; }
    void draw_image(ImageId, Rect, double) override {
        ++commands;
        ++images;
    }
    void draw_image_region(ImageId, Rect, Rect, double) override {
        ++commands;
        ++image_regions;
    }
    void fill_image_pattern(ImageId, Size, Rect, Size,
                            ImagePatternWrap, double) override {
        ++commands;
        ++image_patterns;
    }
    std::uint64_t commands{};
    std::uint64_t images{};
    std::uint64_t image_regions{};
    std::uint64_t image_patterns{};
};

class FillTracePainter final : public Painter {
public:
    void save() override {}
    void restore() override {}
    void translate(Point) override {}
    void clip_rect(Rect) override {}
    void fill_rect(Rect rect, Color) override { fills.push_back(rect); }
    void stroke_rect(Rect, Color, double) override {}
    void draw_line(Point, Point, Color, double) override {}
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override {}
    void draw_image(ImageId, Rect, double) override {}

    std::vector<Rect> fills;
};

std::vector<Rect> paint_fill_trace(const Control::Ptr& control) {
    FillTracePainter painter;
    const Rect arranged = (*control).committed_arranged_bounds();
    (*control).on_paint(painter, {0.0, 0.0, arranged.width, arranged.height});
    return painter.fills;
}

Point center(const Control::Ptr& control) {
    const Rect bounds = (*control).absolute_bounds();
    return {bounds.x + bounds.width * 0.5, bounds.y + bounds.height * 0.5};
}

void click(Window& window, const Control::Ptr& control) {
    if (!control) throw std::runtime_error("gallery click target is missing");
    const Point point = center(control);
    const bool down = window.dispatch_pointer(
        {PointerAction::down, PointerButton::primary, point});
    const bool up = window.dispatch_pointer(
        {PointerAction::up, PointerButton::primary, point});
    if (!down || !up) {
        const Rect bounds = (*control).absolute_bounds();
        const Control::Ptr hit = window.hit_test(point);
        const Control::Ptr captured = window.captured_control();
        throw std::runtime_error("gallery click failed for " +
                                 std::string((*control).stable_id().value()) +
                                 " (down=" + (down ? "true" : "false") +
                                 ", up=" + (up ? "true" : "false") +
                                 ", bounds=" + std::to_string(bounds.x) + "," +
                                 std::to_string(bounds.y) + "," +
                                 std::to_string(bounds.width) + "," +
                                 std::to_string(bounds.height) +
                                 ", hit=" + (hit ? std::string((*hit).stable_id().value())
                                                : std::string("none")) +
                                 ", eligible=" +
                                 ((*control).eligible_for_input() ? "true" : "false") +
                                 ", visible=" +
                                 ((*control).effectively_visible() ? "true" : "false") +
                                 ", enabled=" +
                                 ((*control).effectively_enabled() ? "true" : "false") +
                                 ", captured=" +
                                 (captured ? std::string((*captured).stable_id().value())
                                           : std::string("none")) +
                                 ")");
    }
}

// Selects a page the way a user does (click its navigation row), then proves
// exactly that page's retained surface is visible.
void select_page(Window& window, std::string_view page_id) {
    const Control::Ptr navigation = window.find(
        std::string(gallery::navigation_id_prefix) + std::string(page_id));
    require(navigation != nullptr, "gallery navigation stable ID must resolve");
    click(window, navigation);
    for (const gallery::GalleryPageSpec& page : gallery::gallery_pages) {
        const Control::Ptr surface = window.find(
            std::string(gallery::surface_id_prefix) + std::string(page.id));
        require(surface && (*surface).visible() == (page.id == page_id),
                "gallery navigation must expose exactly one retained page");
    }
    require(gallery::selected_gallery_page(window) == page_id,
            "the selected page must be observable by stable id");
}

// Every registry record that names a Gallery page must resolve to a built
// page, every page must describe itself in its header strip, and the
// navigation must list each page exactly once.
void test_catalog_pages_resolve_and_headers_describe_pages() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    for (const gallery::CatalogEntry& entry : gallery::control_catalog) {
        if (entry.gallery_page.empty()) continue;
        require(gallery::find_gallery_page(entry.gallery_page) != nullptr,
                "every catalog gallery_page must name a declared Gallery page");
        require(window.find(std::string(gallery::surface_id_prefix) +
                            std::string(entry.gallery_page)) != nullptr,
                "every catalog gallery_page must resolve to a built page surface");
    }
    std::size_t pages_with_controls = 0;
    for (const gallery::GalleryPageSpec& page : gallery::gallery_pages) {
        const std::string surface_id =
            std::string(gallery::surface_id_prefix) + std::string(page.id);
        require(window.find(std::string(gallery::navigation_id_prefix) +
                            std::string(page.id)) != nullptr,
                "every page must have a navigation row");
        const std::shared_ptr<Label> title =
            std::dynamic_pointer_cast<Label>(window.find(surface_id + ".title"));
        require(title != nullptr && (*title).text() == page.title,
                "every page header must carry the page title");
        const std::size_t listed = gallery::catalog_entries_on_page(page.id);
        if (listed == 0U) {
            const std::shared_ptr<Label> summary =
                std::dynamic_pointer_cast<Label>(window.find(surface_id + ".summary"));
            require(summary != nullptr && !(*summary).text().empty(),
                    "a page no registry record names must state what it demonstrates");
            continue;
        }
        ++pages_with_controls;
        for (std::size_t line = 0; line < listed; ++line) {
            const std::shared_ptr<Label> control_line = std::dynamic_pointer_cast<Label>(
                window.find(surface_id + ".control." + std::to_string(line)));
            require(control_line != nullptr &&
                        (*control_line).text().find(" · WinForms: ") != std::string::npos,
                    "every page header must list each demonstrated control as a "
                    "Type · header · WinForms line");
        }
        require(window.find(surface_id + ".control." + std::to_string(listed)) == nullptr,
                "a page header must list exactly the controls assigned to it");
        require(gallery::find_catalog_entry("button") != nullptr,
                "catalog lookup by registry id must work");
    }
    require(pages_with_controls >= 14U,
            "most Gallery pages must demonstrate at least one registry control");
    require(gallery::select_gallery_page(window, "basics") &&
                gallery::selected_gallery_page(window) == "basics" &&
                (*window.find("gallery.surface.basics")).visible() &&
                !(*window.find("gallery.surface.control-spectrum")).visible(),
            "select_gallery_page must switch pages by stable id");
    require(!gallery::select_gallery_page(window, "no-such-page"),
            "select_gallery_page must reject unknown ids");
    for (const gallery::GalleryPageSpec& page : gallery::gallery_pages) {
        require(!gallery::gallery_page_group(page).empty(),
                "every page must derive a navigation group");
        require(!page.lab || gallery::gallery_page_group(page) == gallery::labs_group,
                "lab pages must be grouped under Labs");
    }
}

void test_page_contract_and_rendering() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    window.perform_layout();
    require(window.metrics_snapshot().control_count >= 200U,
            "the Gallery must retain a broad public control population");
    require(window.find("gallery.text.primary") &&
                window.find("gallery.collections.single.list") &&
                window.find("gallery.animation.easing") &&
                window.find("gallery.containers.split") &&
                window.find("gallery.images.picture.4") &&
                window.find("gallery.date.primary") &&
                window.find("gallery.host.clipboard.editor") &&
                window.find("gallery.layout.flow.0") &&
                window.find("gallery.layout.table") &&
                window.find("gallery.dock.canvas") &&
                window.find("gallery.anchor.canvas"),
            "the Gallery must publish stable IDs for every proving family");

    CountingPainter painter;
    window.paint(painter, {0.0, 0.0, 1280.0, 820.0});
    require(painter.commands > 150U,
            "the Gallery must generate a substantial retained painter command surface");
    for (const gallery::GalleryPageSpec& page : gallery::gallery_pages) {
        select_page(window, page.id);
    }
}

void test_dock_anchor_gallery_interaction() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    select_page(window, "dock-and-anchor");
    const std::shared_ptr<gui_forms::Panel> dock_canvas = std::dynamic_pointer_cast<Panel>(
        window.find("gallery.dock.canvas"));
    const std::shared_ptr<gui_forms::Button> left = std::dynamic_pointer_cast<Button>(
        window.find("gallery.dock.left"));
    const std::shared_ptr<gui_forms::Button> fill = std::dynamic_pointer_cast<Button>(
        window.find("gallery.dock.fill"));
    const std::shared_ptr<gui_forms::Button> top_first = std::dynamic_pointer_cast<Button>(
        window.find("gallery.dock.top.first"));
    const std::shared_ptr<gui_forms::Button> top_second = std::dynamic_pointer_cast<Button>(
        window.find("gallery.dock.top.second"));
    require(dock_canvas && left && fill && top_first && top_second &&
                (*left).dock() == DockStyle::left &&
                (*fill).dock() == DockStyle::fill,
            "dock page must use public Dock properties on stock controls");
    const Rect fill_before = (*fill).arranged_bounds();
    click(window, window.find("gallery.dock.toggle-left"));
    window.perform_layout();
    require(!(*left).visible() &&
                (*fill).arranged_bounds().width > fill_before.width + 70.0,
            "dock visibility command must release the hidden edge into Fill");
    click(window, window.find("gallery.dock.toggle-left"));
    window.perform_layout();
    require((*left).visible() && (*fill).arranged_bounds() == fill_before,
            "dock visibility restore must recover exact retained geometry");

    const double first_y = (*top_first).arranged_bounds().y;
    const double second_y = (*top_second).arranged_bounds().y;
    require(first_y < second_y,
            "Top A must initially be topmost in the dock z order");
    click(window, window.find("gallery.dock.swap-top"));
    window.perform_layout();
    require((*top_second).arranged_bounds().y == first_y &&
                (*top_first).arranged_bounds().y == second_y,
            "public SetChildIndex must swap the two dock edge allocations");

    const std::shared_ptr<gui_forms::Button> fixed = std::dynamic_pointer_cast<Button>(
        window.find("gallery.anchor.fixed"));
    const std::shared_ptr<gui_forms::Button> stretch = std::dynamic_pointer_cast<Button>(
        window.find("gallery.anchor.stretch"));
    const std::shared_ptr<gui_forms::Button> centered = std::dynamic_pointer_cast<Button>(
        window.find("gallery.anchor.centered"));
    const std::shared_ptr<gui_forms::Button> bottom_right = std::dynamic_pointer_cast<Button>(
        window.find("gallery.anchor.bottom-right"));
    require(fixed && stretch && centered && bottom_right &&
                (*stretch).anchor() == (AnchorStyles::left | AnchorStyles::right |
                                      AnchorStyles::top) &&
                (*centered).anchor() == AnchorStyles::none,
            "anchor page must retain public compound Anchor configurations");
    const Rect fixed_before = (*fixed).arranged_bounds();
    const Rect stretch_before = (*stretch).arranged_bounds();
    const Rect centered_before = (*centered).arranged_bounds();
    const Rect right_before = (*bottom_right).arranged_bounds();
    const Rect stretch_authored = (*stretch).requested_bounds();
    click(window, window.find("gallery.anchor.resize"));
    window.perform_layout();
    require((*fixed).arranged_bounds() == fixed_before &&
                (*stretch).arranged_bounds().width > stretch_before.width + 120.0 &&
                (*centered).arranged_bounds().x > centered_before.x + 60.0 &&
                (*bottom_right).arranged_bounds().x > right_before.x + 120.0 &&
                (*bottom_right).arranged_bounds().y > right_before.y + 15.0 &&
                (*stretch).requested_bounds() == stretch_authored,
            "anchor specimen resize must stretch, center, and edge-shift without rewriting authored bounds");
    click(window, window.find("gallery.anchor.resize"));
    window.perform_layout();
    require((*stretch).arranged_bounds() == stretch_before &&
                (*centered).arranged_bounds() == centered_before &&
                (*bottom_right).arranged_bounds() == right_before &&
                window.metrics_snapshot().bounded_pass_limit_hits == 0U,
            "anchor restore must be exact and bounded without competing layout owners");
    const std::string semantics = window.semantic_snapshot().to_json();
    require(semantics.find("Dock layout specimen") != std::string::npos &&
                semantics.find("Compound anchor specimen") != std::string::npos,
            "dock and anchor specimens must publish named semantic groups");
}

void test_timing_and_tooltip_runtime() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    select_page(window, "timing-and-tooltips");
    std::shared_ptr<gui_forms::Label> status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.timing.tick.status"));
    std::shared_ptr<gui_forms::ProgressBar> progress = std::dynamic_pointer_cast<ProgressBar>(
        window.find("gallery.timing.progress"));
    require(status && progress && window.next_wake().has_value(),
            "timing page must activate its public UI Timer without a hidden control");
    const std::string before = (*status).text();
    const FrameTime timer_deadline = *window.next_wake();
    const FramePollResult timer_tick = window.poll_frame_schedule(timer_deadline);
    require(timer_tick.ui_timer_ticks == 1U && (*status).text() != before &&
                (*progress).value() > 0.0,
            "gallery Timer tick must update retained public controls on the UI queue");
    click(window, window.find("gallery.timing.stop"));
    require(!window.next_wake().has_value() &&
                (*status).text().find("zero idle wakeups") != std::string::npos,
            "gallery Timer stop must make the window scheduler quiescent");

    const Control::Ptr hover_target = window.find("gallery.timing.tooltip.hover");
    const Rect hover_bounds = (*hover_target).absolute_bounds();
    const Point hover_point{hover_bounds.x + 12.0, hover_bounds.y + 12.0};
    static_cast<void>(window.dispatch_pointer(
        {PointerAction::move, PointerButton::none, hover_point}));
    require(window.next_wake().has_value(),
            "gallery hover target must arm the public ToolTip delay");
    static_cast<void>(window.poll_frame_schedule(*window.next_wake()));
    window.perform_layout();
    require(window.semantic_snapshot().to_json().find("\"role\":\"tool_tip\"") !=
                std::string::npos,
            "gallery ToolTip must materialize as a retained semantic overlay");
    static_cast<void>(window.dispatch_pointer(
        {PointerAction::move, PointerButton::none, {1000.0, 700.0}}));
    require(window.semantic_snapshot().to_json().find("\"role\":\"tool_tip\"") ==
                std::string::npos,
            "gallery ToolTip must cancel immediately when hover leaves its owner");
    click(window, window.find("gallery.timing.tooltip.show-disabled"));
    require(window.semantic_snapshot().to_json().find(
                "This action is disabled because no compatible device is selected") !=
                std::string::npos,
            "gallery must explain a disabled target through passive popup ownership");
}

void test_binding_source_gallery_runtime() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    select_page(window, "values-and-spinners");
    const std::shared_ptr<gui_forms::TextBox> editor = std::dynamic_pointer_cast<TextBox>(
        window.find("gallery.values.binding.name"));
    const std::shared_ptr<gui_forms::CheckBox> enabled = std::dynamic_pointer_cast<CheckBox>(
        window.find("gallery.values.binding.enabled"));
    const std::shared_ptr<gui_forms::TrackBar> gain = std::dynamic_pointer_cast<TrackBar>(
        window.find("gallery.values.binding.gain"));
    const std::shared_ptr<gui_forms::Label> status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.values.binding.status"));
    const std::shared_ptr<gui_forms::PropertyGrid> property_grid = std::dynamic_pointer_cast<PropertyGrid>(
        window.find("gallery.values.property-grid"));
    const std::shared_ptr<gui_forms::NumericUpDown> inspected_numeric = std::dynamic_pointer_cast<NumericUpDown>(
        window.find("gallery.values.numeric.1"));
    const std::shared_ptr<NumericUpDown> value_editor = property_grid
        ? std::dynamic_pointer_cast<NumericUpDown>((*property_grid).editor("Value"))
        : std::shared_ptr<NumericUpDown>{};
    const std::shared_ptr<NumericUpDown> bounds_x_editor = property_grid
        ? std::dynamic_pointer_cast<NumericUpDown>(
              (*property_grid).editor("Bounds.X"))
        : std::shared_ptr<NumericUpDown>{};
    require(property_grid && inspected_numeric && value_editor &&
                bounds_x_editor && (*bounds_x_editor).visible() &&
                (*property_grid).selected_object() == inspected_numeric &&
                (*property_grid).selected_origin("Value") ==
                    PropertyValueOrigin::local,
            "gallery must dogfood the public metadata-driven PropertyGrid against a stock compound control");
    require(window.request_focus((*value_editor).editor()),
            "gallery PropertyGrid numeric editor must expose its ordinary retained text focus target");
    (*value_editor).set_value(4.5);
    require((*inspected_numeric).value() == 4.5,
            "gallery PropertyGrid numeric factory must commit through the inspected NumericUpDown's registered Value property");
    const std::shared_ptr<gui_forms::ComboBox> items_target = std::dynamic_pointer_cast<ComboBox>(
        window.find("gallery.values.items-target"));
    click(window, window.find("gallery.values.inspect-items"));
    require(items_target && (*property_grid).selected_object() == items_target &&
                (*property_grid).property_expanded("Items") == true &&
                (*std::dynamic_pointer_cast<TextBox>(
                    (*property_grid).editor("Items[1]"))).text() == "FM",
            "gallery must expose the stock ComboBox.Items collection through the same recursive PropertyGrid");
    click(window, window.find("gallery.values.add-item"));
    require((*items_target).items().size() == 4U &&
                (*items_target).items().back() == "CW" &&
                (*std::dynamic_pointer_cast<TextBox>(
                    (*property_grid).editor("Items[3]"))).text() == "CW",
            "gallery collection command must mutate the real stock Items property rather than a local display model");
    click(window, window.find("gallery.values.inspect-numeric"));
    require((*property_grid).selected_object() == inspected_numeric,
            "gallery inspector target switching must restore the stock numeric specimen without stale collection rows");
    const std::shared_ptr<gui_forms::Label> inspection_status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.values.status"));
    click(window, window.find("gallery.values.inspect-style"));
    const std::shared_ptr<gui_forms::ColorValueEditor> color_editor = std::dynamic_pointer_cast<ColorValueEditor>(
        (*property_grid).editor("ForeColor"));
    const std::shared_ptr<gui_forms::FlagsValueEditor> flags_editor = std::dynamic_pointer_cast<FlagsValueEditor>(
        (*property_grid).editor("Anchor"));
    require(inspection_status &&
                (*property_grid).selected_object() == inspection_status &&
                color_editor && (*color_editor).editor() && flags_editor,
            "gallery Style inspection must dogfood the reusable color and flags editor services");
    (*(*color_editor).editor()).set_text("#245A92FF");
    require(window.request_focus((*color_editor).editor()) &&
                window.dispatch_key({KeyAction::down, PhysicalKey::enter}) &&
                (*inspection_status).foreground() == Color::rgba(36, 90, 146),
            "gallery color dogfood must commit through the stock Label ForeColor descriptor");
    require(editor && enabled && gain && status &&
                (*editor).text() == "Local index" && (*enabled).checked() &&
                (*gain).value() == 28.0 &&
                (*status).text().find("profile.local") != std::string::npos,
            "gallery binding specimen must project the initial retained record");
    require(window.request_focus(editor),
            "gallery bound editor must focus before its retained edit");
    (*editor).set_text("Local index edited");
    click(window, window.find("gallery.values.binding.next"));
    require((*editor).text() == "Archive review" && !(*enabled).checked() &&
                (*gain).value() == 61.0 &&
                (*status).text().find("profile.archive") != std::string::npos,
            "gallery binding currency command must update all three stock control families");
    click(window, window.find("gallery.values.binding.previous"));
    require((*editor).text() == "Local index edited",
            "gallery OnValidation binding must preserve the committed edit across currency movement");
    require(window.request_focus(editor),
            "gallery bound editor must accept focus for invalid-input dogfood");
    (*editor).set_text({});
    require(!window.request_focus(window.find("gallery.values.binding.next")) &&
                window.focused_control() == editor &&
                window.semantic_snapshot().to_json().find(
                    "Profile name may not be empty") != std::string::npos,
            "gallery validation must retain focus and project a binding-aware ErrorProvider error");
    (*editor).set_text("Local index repaired");
    require(window.request_focus(window.find("gallery.values.binding.next")) &&
                window.semantic_snapshot().to_json().find(
                    "Profile name may not be empty") == std::string::npos,
            "corrected gallery input must commit and revoke its retained error adornment");
}

void test_gallery_semantic_surface() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    const SemanticSnapshot snapshot = window.semantic_snapshot();
    require(snapshot.node_count >= 30U && !snapshot.roots.empty(),
            "visible gallery page must publish a substantial semantic surface");
    require(snapshot.to_json().find("gallery.controls.button.0") != std::string::npos &&
                snapshot.to_json().find("\"role\":\"check_box\"") != std::string::npos &&
                snapshot.to_json().find("\"role\":\"radio_button\"") != std::string::npos,
            "gallery semantics must include distinct stock control roles and stable IDs");
}

void test_ranges_containers_and_animation() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    select_page(window, "ranges-and-progress");
    require(window.semantic_snapshot().to_json().find("\"role\":\"scroll_bar\"") !=
                std::string::npos,
            "gallery range page must publish native-neutral scrollbar semantics");
    std::shared_ptr<gui_forms::TrackBar> slider = std::dynamic_pointer_cast<TrackBar>(
        window.find("gallery.ranges.slider.1"));
    std::shared_ptr<gui_forms::ProgressBar> progress = std::dynamic_pointer_cast<ProgressBar>(
        window.find("gallery.ranges.progress.1"));
    std::shared_ptr<gui_forms::HScrollBar> horizontal_scroll = std::dynamic_pointer_cast<HScrollBar>(
        window.find("gallery.ranges.scroll.horizontal"));
    std::shared_ptr<gui_forms::VScrollBar> vertical_scroll = std::dynamic_pointer_cast<VScrollBar>(
        window.find("gallery.ranges.scroll.vertical"));
    require(slider && progress && horizontal_scroll && vertical_scroll,
            "range proving controls must retain public slider, progress, and scrollbar types");
    const Rect track = (*slider).absolute_bounds();
    const Point from{track.x + track.width * 0.2, track.y + track.height * 0.5};
    const Point to{track.x + track.width * 0.82, track.y + track.height * 0.5};
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary, from}) &&
                window.dispatch_pointer({PointerAction::move, PointerButton::none, to}) &&
                window.dispatch_pointer({PointerAction::up, PointerButton::primary, to}) &&
                (*slider).value() > 75.0 && (*progress).value() == (*slider).value(),
            "gallery TrackBar drag must update linked ProgressBar continuously");
    const Rect scroll_bounds = (*horizontal_scroll).absolute_bounds();
    const Rect scroll_thumb = (*horizontal_scroll).thumb_bounds();
    const Point scroll_from{scroll_bounds.x + scroll_thumb.x + scroll_thumb.width * 0.5,
                            scroll_bounds.y + scroll_thumb.height * 0.5};
    const Point scroll_to{scroll_bounds.x + (*horizontal_scroll).track_bounds().x +
                              (*horizontal_scroll).track_bounds().width - 2.0,
                          scroll_from.y};
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary,
                                     scroll_from}) &&
                window.dispatch_pointer({PointerAction::move, PointerButton::none,
                                         scroll_to}) &&
                window.dispatch_pointer({PointerAction::up, PointerButton::primary,
                                         scroll_to}) &&
                (*horizontal_scroll).value() > 95.0 &&
                (*vertical_scroll).value() == (*horizontal_scroll).value(),
            "gallery ScrollBar drag must track continuously and synchronize orientations");

    // Reproduce the reported interaction family without depending on wall
    // clock luck: mutate a captured slider continuously, release it, replace
    // the visible retained page immediately, service the new animation lease,
    // and paint. Repeating both directions exercises stale deadlines and page
    // visibility invalidation rather than merely proving that Animation opens.
    const std::uint64_t frame_faults_before =
        window.metrics_snapshot().frame_callback_faults;
    for (std::size_t cycle = 0U; cycle < 48U; ++cycle) {
        select_page(window, "ranges-and-progress");
        const Rect live_track = (*slider).absolute_bounds();
        const double start_ratio = cycle % 2U == 0U ? 0.18 : 0.82;
        const double finish_ratio = cycle % 2U == 0U ? 0.82 : 0.18;
        const Point drag_start{
            live_track.x + live_track.width * start_ratio,
            live_track.y + live_track.height * 0.5};
        require(window.dispatch_pointer(
                    {PointerAction::down, PointerButton::primary, drag_start}),
                "slider-to-animation stress must acquire the TrackBar");
        for (std::size_t step = 1U; step <= 9U; ++step) {
            const double ratio = start_ratio +
                (finish_ratio - start_ratio) *
                    static_cast<double>(step) / 9.0;
            require(window.dispatch_pointer(
                        {PointerAction::move, PointerButton::none,
                         {live_track.x + live_track.width * ratio,
                          drag_start.y}}),
                    "slider-to-animation stress must deliver every captured move");
        }
        const Point drag_finish{
            live_track.x + live_track.width * finish_ratio, drag_start.y};
        require(window.dispatch_pointer(
                    {PointerAction::up, PointerButton::primary, drag_finish}) &&
                    !window.captured_control(),
                "slider-to-animation stress must release capture before navigation");

        select_page(window, "animation-laboratory");
        require(window.next_wake().has_value(),
                "opening Animation after a slider drag must retain a live deadline");
        static_cast<void>(window.poll_frame_schedule(*window.next_wake()));
        CountingPainter transition_painter;
        const DamageRegion transition_damage = window.take_damage();
        static_cast<void>(window.paint(
            transition_painter,
            transition_damage.empty()
                ? Rect{0.0, 0.0, 1280.0, 820.0}
                : transition_damage.bounds()));
        require(transition_painter.commands > 0U,
                "slider-to-animation transition must complete a retained paint");
    }
    require(window.metrics_snapshot().frame_callback_faults ==
                frame_faults_before,
            "slider-to-animation stress must not fault an active surface callback");

    select_page(window, "containers-and-focus");
    std::shared_ptr<gui_forms::SplitContainer> split = std::dynamic_pointer_cast<SplitContainer>(
        window.find("gallery.containers.split"));
    std::shared_ptr<gui_forms::ContainerControl> base_container = std::dynamic_pointer_cast<ContainerControl>(
        window.find("gallery.containers.base"));
    std::shared_ptr<gui_forms::UserControl> user_control = std::dynamic_pointer_cast<UserControl>(
        window.find("gallery.containers.user"));
    std::shared_ptr<gui_forms::Label> user_status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.containers.user.status"));
    require(base_container && user_control && user_status &&
                (*user_control).is_loaded() && (*user_control).is_attached() &&
                (*user_control).attachment_count() == 1U &&
                (*user_status).text().find("Loaded fired once") != std::string::npos,
            "gallery must directly dogfood public ContainerControl and UserControl lifecycle");
    click(window, window.find("gallery.containers.base.focus"));
    require((*base_container).active_control() ==
                window.find("gallery.containers.base.focus") &&
                window.semantic_snapshot().to_json().find(
                    "UserControl lifecycle specimen") != std::string::npos,
            "public container focus and named group semantics must work in the gallery");
    click(window, window.find("gallery.containers.collapse"));
    require(split && (*split).first_collapsed(),
            "gallery collapse command must mutate the public SplitContainer");
    click(window, window.find("gallery.containers.collapse"));
    require(!(*split).first_collapsed(),
            "gallery SplitContainer must restore its retained allocation");

    select_page(window, "animation-laboratory");
    const Control::Ptr easing = window.find("gallery.animation.easing");
    const std::shared_ptr<gui_forms::Button> pause_button = std::dynamic_pointer_cast<Button>(
        window.find("gallery.animation.pause"));
    const std::shared_ptr<gui_forms::ProgressBar> animated_progress = std::dynamic_pointer_cast<ProgressBar>(
        window.find("gallery.ranges.progress.2"));
    require(easing != nullptr && pause_button != nullptr && animated_progress != nullptr,
            "animation page must retain its public-frame proving surface");
    (*animated_progress).on_frame(
        FrameClock::now() + std::chrono::milliseconds(410));
    const double retained_progress_phase = (*animated_progress).animation_phase();
    const std::uint64_t full_motion_surfaces =
        poll_active_surface_count(window);
    require(retained_progress_phase > 0.0 && full_motion_surfaces >= 4U,
            "gallery motion must begin with independently retained easing and progress surfaces");
    require(window.next_wake().has_value(),
            "visible animation page must publish a retained frame deadline");
    click(window, window.find("gallery.animation.reduced"));
    require(window.next_wake().has_value() &&
                poll_active_surface_count(window) == full_motion_surfaces &&
                (*animated_progress).animation_phase() == retained_progress_phase &&
                has_semantic_state((*animated_progress).semantic_descriptor().states,
                                   SemanticState::busy),
            "reduced motion must retain every animated surface at a calm cadence");
    const std::vector<Rect> reduced_geometry_before = paint_fill_trace(easing);
    const double reduced_phase_before = (*animated_progress).animation_phase();
    std::this_thread::sleep_for(std::chrono::milliseconds(110));
    const FramePollResult reduced_frame =
        window.poll_frame_schedule(FrameClock::now());
    const std::vector<Rect> reduced_geometry = paint_fill_trace(easing);
    require(reduced_frame.active_surface_ticks > 0U,
            "reduced motion must emit retained animation ticks");
    require(reduced_geometry != reduced_geometry_before,
            "reduced easing geometry must advance rather than present a frozen substitute");
    require((*animated_progress).animation_phase() == reduced_phase_before,
            "effectively hidden reduced progress must remain scheduler-suspended");
    const double reduced_progress_phase = reduced_phase_before;
    click(window, window.find("gallery.animation.pause"));
    require(!window.next_wake().has_value() &&
                (*pause_button).text() == "Resume motion",
            "pausing under reduced motion must quiesce its frame lease");
    const std::vector<Rect> paused_reduced_geometry = paint_fill_trace(easing);
    static_cast<void>(window.poll_frame_schedule(
        FrameClock::now() + std::chrono::seconds(1)));
    require(paint_fill_trace(easing) == paused_reduced_geometry,
            "paused reduced motion must retain stable geometry without wakeups");
    click(window, window.find("gallery.animation.pause"));
    require(window.next_wake().has_value() &&
                poll_active_surface_count(window) == full_motion_surfaces &&
                paint_fill_trace(easing) == paused_reduced_geometry &&
                (*animated_progress).animation_phase() == reduced_progress_phase &&
                (*pause_button).text() == "Pause motion",
            "one resume action under reduced motion must restore calm animation");
    click(window, window.find("gallery.animation.reduced"));
    require(window.next_wake().has_value() &&
                poll_active_surface_count(window) == full_motion_surfaces &&
                (*animated_progress).animation_phase() == reduced_progress_phase,
            "leaving reduced motion must preserve phase while restoring full cadence");
    click(window, window.find("gallery.status.live"));
    require(!window.next_wake().has_value() && !(*pause_button).enabled() &&
                (*pause_button).text() == "Motion disabled",
            "global live-off must quiesce motion and disable the page command");
    click(window, window.find("gallery.animation.reduced"));
    require(!window.next_wake().has_value(),
            "changing substitution while globally stopped must not restart motion");
    const std::vector<Rect> disabled_reduced_geometry = paint_fill_trace(easing);
    click(window, window.find("gallery.status.live"));
    require(window.next_wake().has_value() && (*pause_button).enabled() &&
                (*pause_button).text() == "Pause motion" &&
                paint_fill_trace(easing) == disabled_reduced_geometry,
            "one global live-on action must restore the retained reduced animation policy");

    // A connected deadline is not proof of animation. Repeatedly pause,
    // replace the active-surface lease, poll its first deadline, and require
    // the rendered marker geometry to advance each time.
    click(window, window.find("gallery.animation.reduced"));
    require(window.next_wake().has_value(),
            "leaving the reduced policy must arm exactly one continuous lease");
    for (std::size_t cycle = 0U; cycle < 8U; ++cycle) {
        click(window, window.find("gallery.animation.pause"));
        require(!window.next_wake().has_value() &&
                    (*pause_button).text() == "Resume motion",
                "every pause cycle must quiesce and publish one coherent command");
        const std::vector<Rect> before = paint_fill_trace(easing);
        click(window, window.find("gallery.animation.pause"));
        require(window.next_wake().has_value() &&
                    (*pause_button).text() == "Pause motion" &&
                    poll_active_surface_count(window) == full_motion_surfaces,
                "every resume cycle must replace the lease and publish one command");
        const FrameTime due = *window.next_wake();
        const FramePollResult frame = window.poll_frame_schedule(due);
        const std::vector<Rect> after = paint_fill_trace(easing);
        require(frame.active_surface_ticks == 1U && before != after,
                "a resumed deadline must advance visible easing geometry");
    }
    click(window, window.find("gallery.animation.pause"));
    require(!window.next_wake().has_value(),
            "the repeated-toggle fixture must reach a quiescent paused state");
    click(window, window.find("gallery.status.live"));
    require(!(*pause_button).enabled() && (*pause_button).text() == "Motion disabled",
            "the master gate must replace a paused command with one disabled state");
    click(window, window.find("gallery.status.live"));
    require(!window.next_wake().has_value() && (*pause_button).enabled() &&
                (*pause_button).text() == "Resume motion",
            "the master gate must preserve the orthogonal page-local pause latch");
    click(window, window.find("gallery.animation.pause"));
    require(window.next_wake().has_value() &&
                (*pause_button).text() == "Pause motion",
            "one explicit resume after a master-gate cycle must restore motion");
    click(window, window.find("gallery.animation.pause"));
    require(!window.next_wake().has_value(),
            "the repeated-toggle fixture must end in a quiescent paused state");
}

void test_text_collections_and_popup_lifecycle() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    host::HeadlessHost host(window);
    select_page(window, "text-and-input");
    std::shared_ptr<gui_forms::TextBox> text = std::dynamic_pointer_cast<TextBox>(window.find("gallery.text.primary"));
    std::shared_ptr<gui_forms::TextBox> password = std::dynamic_pointer_cast<TextBox>(
        window.find("gallery.text.password"));
    require(text && window.request_focus(text) && window.dispatch_text({"dogfood Ω"}) &&
                (*text).text() == "dogfood Ω" && (*text).can_undo(),
            "gallery public TextBox must accept normalized Unicode input");
    click(window, window.find("gallery.text.command.0"));
    click(window, window.find("gallery.text.command.1"));
    require((*text).text() == "GUI.Forms",
            "gallery TextBox commands must operate on the public selection surface");
    click(window, window.find("gallery.text.command.2"));
    require((*text).text() == "dogfood Ω",
            "gallery TextBox Undo command must restore its prior snapshot");
    (*text).select_all();
    click(window, window.find("gallery.text.command.4"));
    (*text).set_text("replace me");
    (*text).select_all();
    click(window, window.find("gallery.text.command.6"));
    require((*text).text() == "dogfood Ω",
            "gallery clipboard buttons must round-trip through HostServices");
    (*text).select_all();
    click(window, window.find("gallery.text.command.5"));
    require((*text).text().empty(),
            "gallery Cut must mutate through the public TextBox command");
    click(window, window.find("gallery.text.command.6"));
    require((*text).text() == "dogfood Ω",
            "gallery Paste must restore the cut Unicode text");
    require(password && (*password).password_protected() &&
                (*password).semantic_descriptor().value.empty() &&
                has_semantic_state((*password).semantic_descriptor().states,
                                   SemanticState::protected_content) &&
                window.semantic_snapshot().to_json().find("Portsmouth-Ω-2026") ==
                    std::string::npos,
            "gallery protected field must mask paint/semantic export by contract");
    (*password).select_all();
    require(!(*password).copy(),
            "gallery protected field must reject clipboard export");
    const HostServicesSnapshot clipboard_metrics = host.services().snapshot();
    require(clipboard_metrics.clipboard_writes == 2U &&
                clipboard_metrics.clipboard_reads == 2U,
            "gallery clipboard commands must leave exact transport accounting");

    select_page(window, "collections-and-popups");
    std::shared_ptr<gui_forms::ListBox> multi = std::dynamic_pointer_cast<ListBox>(
        window.find("gallery.collections.multi.list"));
    require(multi && (*multi).selected_indices().size() == 4U,
            "gallery extended ListBox must retain its initial selection range");
    std::shared_ptr<gui_forms::ComboBox> combo = std::dynamic_pointer_cast<ComboBox>(
        window.find("gallery.collections.combo.density"));
    click(window, combo);
    require((*combo).dropped_down() && window.focus_scope_depth() == 1U &&
                window.find("gallery.collections.combo.density.popup.list"),
            "gallery ComboBox must open through the public popup controller");
    const Control::Ptr popup =
        window.find("gallery.collections.combo.density.popup.list");
    window.perform_layout();
    const Rect bounds = (*popup).absolute_bounds();
    const Point choice{bounds.x + 20.0, bounds.y + 2.0 + 2.0 * 26.0 + 13.0};
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary, choice}) &&
                window.dispatch_pointer({PointerAction::up, PointerButton::primary, choice}) &&
                (*combo).selected_index() == 2U && !(*combo).dropped_down() &&
                window.focus_scope_depth() == 0U,
            "gallery ComboBox popup must commit and restore focus without residue");

    select_page(window, "values-and-spinners");
    std::shared_ptr<gui_forms::NumericUpDown> numeric = std::dynamic_pointer_cast<NumericUpDown>(
        window.find("gallery.values.numeric.1"));
    require(numeric && window.request_focus((*numeric).editor()),
            "gallery NumericUpDown must expose a focusable public editor");
    KeyEvent up{KeyAction::down, PhysicalKey::up};
    require(window.dispatch_key(up) && (*numeric).value() == 4.0 &&
                (*(*numeric).editor()).text() == "4.00",
            "gallery NumericUpDown key step must synchronize value and editor");

    select_page(window, "images-and-drawing");
    std::shared_ptr<gui_forms::PictureBox> picture = std::dynamic_pointer_cast<PictureBox>(
        window.find("gallery.images.picture.4"));
    require(picture && (*picture).has_valid_image() &&
                (*picture).size_mode() == PictureBoxSizeMode::zoom &&
                (*picture).image_size() == Size{128.0, 80.0},
            "gallery image page must use the public PictureBox and image registry");
    const std::string image_semantics = window.semantic_snapshot().to_json();
    require(image_semantics.find("\"role\":\"image\"") != std::string::npos &&
                image_semantics.find("128 x 80") != std::string::npos,
            "gallery image page must publish image semantics and intrinsic size");
    CountingPainter image_painter;
    window.paint(image_painter, {0.0, 0.0, 1280.0, 820.0});
    require(image_painter.images == 8U,
            "gallery image page must render all eight public PictureBox consumers");
    require(image_painter.image_regions >= 9U,
            "gallery image page must retain source-cropped nine-patch material");
    require(image_painter.image_patterns == 1U,
            "gallery image page must retain one bounded exact-period tile command");

    select_page(window, "tabs-and-pages");
    std::shared_ptr<gui_forms::TabControl> tabs = std::dynamic_pointer_cast<TabControl>(
        window.find("gallery.tabs.primary"));
    require(tabs && (*tabs).page_count() == 4U && (*tabs).selected_index() == 1U,
            "gallery tab page must retain a real public TabControl page model");
    const Rect security = (*tabs).tab_bounds(2U);
    const Rect tabs_absolute = (*tabs).absolute_bounds();
    const Point security_click{tabs_absolute.x + security.x + security.width * 0.5,
                               tabs_absolute.y + security.y + security.height * 0.5};
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary,
                                     security_click}) &&
                window.dispatch_pointer({PointerAction::up, PointerButton::primary,
                                         security_click}) &&
                (*tabs).selected_index() == 2U &&
                (*window.find("gallery.tabs.primary.security.radio")).effectively_visible(),
            "gallery tabs must select pages through public pointer behavior");
    require(window.perform_semantic_action(
                "gallery.tabs.primary.diagnostics.tab", SemanticAction::select) &&
                (*tabs).selected_index() == 3U,
            "gallery tabs must share semantic and pointer selection behavior");

    select_page(window, "checked-collections");
    std::shared_ptr<gui_forms::CheckedListBox> checked = std::dynamic_pointer_cast<CheckedListBox>(
        window.find("gallery.checked.immediate.list"));
    require(checked && (*checked).check_on_click() &&
                (*checked).item_check_state(3U) == CheckState::indeterminate,
            "gallery checked page must retain a public check-state collection");
    const Rect checked_bounds = (*checked).absolute_bounds();
    const Point last_row{checked_bounds.x + 14.0,
                         checked_bounds.y + 2.0 + 5.5 * (*checked).item_height()};
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary,
                                     last_row}) &&
                window.dispatch_pointer({PointerAction::up, PointerButton::primary,
                                         last_row}) &&
                (*checked).item_checked(5U),
            "gallery CheckOnClick row must select and toggle immediately");
    const std::string checked_semantics = window.semantic_snapshot().to_json();
    require(checked_semantics.find("\"role\":\"check_list_item\"") !=
                std::string::npos,
            "gallery checked rows must publish checkable virtual semantics");
}

void test_date_time_picker_gallery() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    select_page(window, "dates-and-calendar");

    const std::shared_ptr<gui_forms::DateTimePicker> long_date = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.format.0"));
    const std::shared_ptr<gui_forms::DateTimePicker> short_date = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.format.1"));
    const std::shared_ptr<gui_forms::DateTimePicker> time = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.format.2"));
    const std::shared_ptr<gui_forms::DateTimePicker> custom = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.format.3"));
    const std::shared_ptr<gui_forms::DateTimePicker> optional = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.optional"));
    const std::shared_ptr<gui_forms::DateTimePicker> spinner = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.spinner"));
    const std::shared_ptr<gui_forms::DateTimePicker> provider = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.provider"));
    const std::shared_ptr<gui_forms::DateTimePicker> disabled = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.disabled"));
    const std::shared_ptr<gui_forms::DateTimePicker> primary = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.primary"));
    require(long_date && short_date && time && custom && optional && spinner &&
                provider && disabled && primary,
            "date board must use public DateTimePicker instances for every variant");
    require((*long_date).formatted_value() == "Wednesday, August 5, 2026" &&
                (*short_date).formatted_value() == "8/5/2026" &&
                (*time).formatted_value() == "2:07 PM" &&
                (*custom).formatted_value() == "2026-08-05 · 14:07" &&
                (*provider).formatted_value() == "mercredi 5 août 2026",
            "date board must visibly distinguish all format and provider paths");
    require((*optional).show_check_box() && !(*optional).checked() &&
                (*spinner).show_up_down() && !(*disabled).enabled(),
            "date board must retain nullable, spinner, and disabled states");

    const Rect optional_bounds = (*optional).absolute_bounds();
    const Point optional_check{optional_bounds.x + 12.0,
                               optional_bounds.y + optional_bounds.height * 0.5};
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary,
                                     optional_check}) &&
                window.dispatch_pointer({PointerAction::up, PointerButton::primary,
                                         optional_check}) && (*optional).checked(),
            "optional date checkbox must toggle through its public hit region");
    require(window.perform_semantic_action(
                "gallery.date.optional", SemanticAction::press) &&
                !(*optional).checked() &&
                window.perform_semantic_action(
                    "gallery.date.optional", SemanticAction::set_value,
                    "2026-08-09") && (*optional).checked() &&
                (*optional).value().day == 9U,
            "optional date semantics must toggle and activate a supplied value");
    require(window.request_focus(spinner) &&
                window.dispatch_key({KeyAction::down, PhysicalKey::up}) &&
                (*spinner).value().day == 6U,
            "date spinner must step its civil value through keyboard input");

    click(window, window.find("gallery.date.open"));
    require((*primary).dropped_down() && window.focus_scope_depth() == 1U &&
                window.find("gallery.date.primary.popup.calendar") &&
                window.semantic_snapshot().to_json().find(
                    "gallery.date.primary.popup.calendar.day.2026-08-12") !=
                    std::string::npos,
            "date board must open one retained semantic calendar focus scope");
    require(window.perform_semantic_action(
                "gallery.date.primary.popup.calendar.day.2026-08-12",
                SemanticAction::press) && (*primary).value().day == 12U &&
                !(*primary).dropped_down() && window.focus_scope_depth() == 0U,
            "semantic date-cell commit must share pointer commit and restore focus");

    click(window, window.find("gallery.date.open"));
    require(window.dispatch_key({KeyAction::down, PhysicalKey::left}) &&
                window.dispatch_key({KeyAction::down, PhysicalKey::escape}) &&
                (*primary).value().day == 12U && !(*primary).dropped_down(),
            "calendar Escape must discard popup-local navigation without mutation");

    click(window, window.find("gallery.date.open"));
    (*window.find("gallery.surface.dates-and-calendar")).set_visible(false);
    require(!(*primary).dropped_down() && window.focus_scope_depth() == 0U &&
                !window.find("gallery.date.primary.popup.calendar"),
            "hiding a page must revoke its open calendar and contained focus");
}

void test_dialogs_and_host_services_gallery() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    host::HeadlessHost host(window);
    host::HeadlessHostServices& services =
        static_cast<host::HeadlessHostServices&>(host.services());
    bool cancel_save = false;
    services.set_dialog_handler(GalleryDialogHandler(cancel_save));

    select_page(window, "dialogs-and-host-services");
    const std::shared_ptr<gui_forms::Label> status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.host.status"));
    const std::shared_ptr<gui_forms::TextBox> editor = std::dynamic_pointer_cast<TextBox>(
        window.find("gallery.host.clipboard.editor"));
    require(status && editor,
            "host-services board must expose its public result and clipboard controls");

    click(window, window.find("gallery.host.message.2"));
    require((*status).text().find("accepted · yes") != std::string::npos,
            "message dialog must publish its typed accepted choice");
    click(window, window.find("gallery.host.dialog.1"));
    require((*status).text().find("2 paths · /tmp/alpha.txt") != std::string::npos,
            "multi-open must retain all accepted paths through the portable result");
    cancel_save = true;
    click(window, window.find("gallery.host.dialog.2"));
    require((*status).text().find("Cancelled · preserved: 2 paths · /tmp/alpha.txt") !=
                std::string::npos,
            "dialog cancellation must not overwrite the last accepted value");
    click(window, window.find("gallery.host.dialog.4"));
    require((*status).text().find("#315F89FF") != std::string::npos,
            "color dialog must publish an exact typed RGBA result");

    const std::string clipboard_probe((*editor).text());
    click(window, window.find("gallery.host.service.0"));
    (*editor).set_text("locally replaced");
    click(window, window.find("gallery.host.service.1"));
    require((*editor).text() == clipboard_probe &&
                (*status).text().find("text restored") != std::string::npos,
            "clipboard write/read must round-trip UTF-8 through HostServices");
    click(window, window.find("gallery.host.service.2"));
    require((*status).text().find("headless.primary") != std::string::npos,
            "monitor inspection must identify the primary host monitor and scale");
    click(window, window.find("gallery.host.service.3"));
    require(services.sound_trace().find("sound=operation_complete") !=
                std::string::npos,
            "gallery must exercise the complete operation sound cue");

    const HostServicesSnapshot snapshot = services.snapshot();
    require(snapshot.dialog_requests == 4U &&
                snapshot.dialog_completions == 4U &&
                snapshot.dialog_cancellations == 1U &&
                snapshot.maximum_modal_depth == 1U && snapshot.modal_depth == 0U &&
                snapshot.clipboard_writes == 1U && snapshot.clipboard_reads == 1U &&
                snapshot.monitor_queries == 1U && snapshot.sound_playbacks == 1U,
            "host-services board must leave exact modal and service accounting");
}

void test_dispatcher_gallery() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    host::HeadlessHost host(window);
    select_page(window, "states-and-diagnostics");
    const std::shared_ptr<gui_forms::Label> status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.dispatcher.status"));
    require(status != nullptr,
            "states page must expose the public dispatcher proving status");

    click(window, window.find("gallery.dispatcher.post"));
    require((*status).text().find("is pending") != std::string::npos &&
                window.dispatcher_snapshot().pending == 3U,
            "dispatcher gallery click must return before its posted batch runs");
    const DispatchDrainResult first = window.drain_posted_work();
    require(first.invoked == 3U && first.remaining == 1U &&
                (*status).text().find("order ABC") != std::string::npos &&
                (*status).text().find("D remains posted") != std::string::npos,
            "dispatcher gallery turn one must retain FIFO snapshot semantics");
    const DispatchDrainResult second = window.drain_posted_work();
    require(second.invoked == 1U && second.remaining == 0U &&
                (*status).text().find("order ABCD") != std::string::npos,
            "dispatcher gallery nested work must commit on turn two");

    click(window, window.find("gallery.dispatcher.cancel"));
    require((*status).text().find("before dispatch") != std::string::npos,
            "dispatcher gallery must expose explicit pre-dispatch cancellation");
    const DispatchDrainResult cancelled = window.drain_posted_work();
    require(cancelled.invoked == 0U && cancelled.cancelled == 1U &&
                (*status).text().find("ERROR") == std::string::npos,
            "cancelled gallery work must never execute its callback");

    const std::shared_ptr<gui_forms::Button> invoke = std::dynamic_pointer_cast<Button>(
        window.find("gallery.dispatcher.invoke"));
    click(window, invoke);
    require(invoke && !(*invoke).enabled() &&
                (*status).text().find("Worker blocked") != std::string::npos,
            "dispatcher gallery must visibly expose a blocking worker Invoke");
    require_eventually(
        DispatcherHasOnePending(window),
        "gallery worker Invoke did not reach the dispatcher queue");
    const DispatchDrainResult invoked = host.pump_dispatcher();
    require(invoked.invoked == 1U && (*invoke).enabled() &&
                (*status).text().find("Worker released after UI callback") !=
                    std::string::npos,
            "gallery worker must resume only after its callback ran on the UI thread");
}

} // namespace

int main() {
    try {
        test_page_contract_and_rendering();
        test_catalog_pages_resolve_and_headers_describe_pages();
        test_gallery_semantic_surface();
        test_ranges_containers_and_animation();
        test_text_collections_and_popup_lifecycle();
        test_timing_and_tooltip_runtime();
        test_binding_source_gallery_runtime();
        test_date_time_picker_gallery();
        test_dialogs_and_host_services_gallery();
        test_dispatcher_gallery();
        test_dock_anchor_gallery_interaction();
        std::cout << "gui_forms_gallery_pages_tests: all tests passed\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "gui_forms_gallery_pages_tests: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
