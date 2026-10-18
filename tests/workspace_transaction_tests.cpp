// Stage DK phase W0: the cross-window workspace transaction (decision 9) and
// the framework's capture drag of a native window (decision 12), on the
// headless reference host.
#include "gui_forms/application.hpp"
#include "gui_forms/basic_controls.hpp"
#include "gui_forms/control.hpp"
#include "gui_forms/host.hpp"
#include "application/headless_application.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

std::size_t position(const std::string& text, const char* fragment) {
    const std::size_t found = text.find(fragment);
    require(found != std::string::npos, fragment);
    return found;
}

class NullPainter final : public Painter {
public:
    void save() override {}
    void restore() override {}
    void translate(Point) override {}
    void clip_rect(Rect) override {}
    void clip_rounded_rect(Rect, double) override {}
    void fill_rect(Rect, Color) override {}
    void fill_rounded_rect(Rect, double, Color) override {}
    void stroke_rect(Rect, Color, double) override {}
    void stroke_rounded_rect(Rect, double, Color, double) override {}
    void fill_linear_gradient(Rect, Point, Point, std::span<const GradientStop>) override {}
    void draw_line(Point, Point, Color, double) override {}
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override {}
    void draw_image(ImageId, Rect, double) override {}
};

struct KeepWindow final {
    ApplicationWindowHandle* handle{};
    Window** model{};
    void operator()(Window& window, ApplicationWindowHandle value) const {
        *handle = value;
        *model = &window;
    }
};

// Appends one line to a journal when the transaction publishes.
struct Notify final {
    std::string* journal{};
    const host::HeadlessWindowing* windowing{};
    const char* name{};
    void operator()() const {
        // The notification sees the finished state: the trace already holds
        // the show and the destroy.
        *journal += name;
        *journal += (*windowing).trace().find("destroy id=2") != std::string::npos ? " after-destroy\n"
                                                                                   : " before-destroy\n";
    }
};

ApplicationWindow make_entry(const char* id, const char* owner, HostWindowFloating floating,
                             const std::shared_ptr<Panel>& root, Size size,
                             const std::shared_ptr<ApplicationResources>& resources) {
    ApplicationWindow entry{};
    entry.stable_id = id;
    entry.owner_id = owner;
    entry.floating = floating;
    entry.model = resources ? std::make_unique<Window>(root, size, resources)
                            : std::make_unique<Window>(root, size);
    entry.options.title = id;
    entry.options.initial_size = size;
    entry.options.minimum_size = {100.0, 80.0};
    return entry;
}

std::shared_ptr<Panel> make_panel(const char* id, Rect bounds) {
    const std::shared_ptr<Panel> panel = make_control<Panel>(StableId(id));
    (*panel).set_requested_bounds(bounds);
    return panel;
}

// A main window and one floating tool window holding a panel; the
// transaction docks that panel into the main window, tears another panel out
// of the main window into a new floating window, and closes the emptied
// floating window.
void test_two_window_transaction() {
    ApplicationWindowHandle main{};
    Window* main_model = nullptr;
    ApplicationWindowHandle old_float{};
    Window* old_float_model = nullptr;
    const std::shared_ptr<Panel> main_root = make_panel("main.root", {0.0, 0.0, 800.0, 600.0});
    const std::shared_ptr<Panel> document = make_panel("main.document", {0.0, 0.0, 600.0, 600.0});
    const std::shared_ptr<Panel> outline = make_panel("main.outline", {600.0, 0.0, 200.0, 300.0});
    (*main_root).add_child(document);
    (*main_root).add_child(outline);
    const std::shared_ptr<Panel> float_root = make_panel("float.root", {0.0, 0.0, 200.0, 300.0});
    const std::shared_ptr<Panel> properties = make_panel("float.properties", {0.0, 0.0, 200.0, 300.0});
    (*float_root).add_child(properties);

    std::vector<ApplicationWindow> windows;
    ApplicationWindow primary = make_entry("main", "", HostWindowFloating::none, main_root, {800.0, 600.0}, {});
    primary.options.ready = KeepWindow{&main, &main_model};
    windows.push_back(std::move(primary));
    host::HeadlessApplication application(std::move(windows));
    const std::shared_ptr<ApplicationResources> resources = (*main_model).resources();
    ApplicationWindow torn = make_entry("float.1", "main", HostWindowFloating::tool, float_root,
                                        {200.0, 300.0}, resources);
    torn.options.ready = KeepWindow{&old_float, &old_float_model};
    require(main.windows().open_window(std::move(torn)).status.accepted(), "first floating window refused");
    (*main_model).perform_layout();
    (*old_float_model).perform_layout();
    const MetricsSnapshot main_before = (*main_model).metrics_snapshot();
    host::HeadlessWindowing& windowing = application.windowing();
    windowing.clear_trace();

    std::string journal;
    ApplicationWindowHandle new_float{};
    Window* new_float_model = nullptr;
    {
        WorkspaceTransaction transaction(main.windows());
        transaction.include(*main_model);
        transaction.include(*old_float_model);
        transaction.include(*main_model);
        // Dock the properties panel into the main window.
        const Control::Ptr docked = (*float_root).remove_child((*properties).runtime_id());
        (*main_root).add_child(docked);
        (*properties).set_requested_bounds({600.0, 300.0, 200.0, 300.0});
        // Tear the outline out into a new floating window.
        const std::shared_ptr<Panel> new_root = make_panel("float.2.root", {0.0, 0.0, 200.0, 300.0});
        ApplicationWindow entry = make_entry("float.2", "main", HostWindowFloating::tool, new_root,
                                             {200.0, 300.0}, resources);
        entry.options.ready = KeepWindow{&new_float, &new_float_model};
        const ApplicationWindowOpenResult opened = transaction.open_window(std::move(entry));
        require(opened.status.accepted(), "floating window refused inside the transaction");
        const Control::Ptr outline_moved = (*main_root).remove_child((*outline).runtime_id());
        (*new_root).add_child(outline_moved);
        (*outline).set_requested_bounds({0.0, 0.0, 200.0, 300.0});
        (*document).set_requested_bounds({0.0, 0.0, 600.0, 600.0});
        require(transaction.destroy_window(old_float).accepted(), "destroy refused");
        transaction.defer_notification(Notify{&journal, &windowing, "placement_changed"});
        transaction.defer_notification(Notify{&journal, &windowing, "layout_changed"});
        // Nothing appears or goes before the commit.
        require(!windowing.native_visible(opened.window.id()), "new window shown before the commit");
        require(old_float.active(), "emptied window destroyed before the commit");
        require(journal.empty(), "notification published before the commit");
        require((*main_model).metrics_snapshot().arrange_passes == main_before.arrange_passes,
                "main window laid out inside the transaction");
        const WorkspaceTransactionResult result = transaction.commit();
        require(result.status.accepted() && result.windows_laid_out == 3U && result.windows_shown == 1U &&
                    result.windows_destroyed == 1U && result.notifications == 2U,
                "transaction result wrong");
        require(!transaction.open(), "transaction still open after commit");
        bool refused = false;
        try {
            transaction.include(*main_model);
        } catch (const std::logic_error&) {
            refused = true;
        }
        require(refused, "a committed transaction accepted another window");
    }
    // One layout per window, at the commit.
    const MetricsSnapshot main_after = (*main_model).metrics_snapshot();
    require(main_after.arrange_passes == main_before.arrange_passes + 1U,
            "main window not laid out exactly once by the transaction");
    require(new_float_model != nullptr && (*new_float_model).metrics_snapshot().arrange_passes == 1U,
            "new floating window not laid out exactly once");
    // One paint per window: each has its whole change waiting.
    NullPainter painter;
    static_cast<void>((*main_model).paint(painter));
    static_cast<void>((*new_float_model).paint(painter));
    require((*main_model).metrics_snapshot().paint_passes == main_before.paint_passes + 1U,
            "main window painted more than once for the transaction");
    require((*new_float_model).metrics_snapshot().paint_passes == 1U, "new window painted more than once");
    // Created hidden, shown before the emptied window was destroyed, and the
    // notifications after both.
    const std::string& trace = windowing.trace();
    const std::size_t create = position(trace, "create id=3");
    const std::size_t show = position(trace, "show id=3");
    const std::size_t destroy = position(trace, "destroy id=2");
    require(create < show && show < destroy, "new window not created hidden and shown before the destroy");
    require(journal == "placement_changed after-destroy\nlayout_changed after-destroy\n",
            "notifications not published once, in order, after the windows changed");
    require((*properties).attached_window() == main_model && (*outline).attached_window() == new_float_model,
            "panels not where the transaction put them");
    require(!old_float.active(), "emptied window not destroyed");
    std::cout << "transaction trace:\n" << trace;
}

void test_transaction_destructor_commits() {
    ApplicationWindowHandle main{};
    Window* main_model = nullptr;
    const std::shared_ptr<Panel> main_root = make_panel("main.root", {0.0, 0.0, 400.0, 300.0});
    std::vector<ApplicationWindow> windows;
    ApplicationWindow primary = make_entry("main", "", HostWindowFloating::none, main_root, {400.0, 300.0}, {});
    primary.options.ready = KeepWindow{&main, &main_model};
    windows.push_back(std::move(primary));
    host::HeadlessApplication application(std::move(windows));
    std::string journal;
    {
        WorkspaceTransaction transaction(main.windows());
        transaction.include(*main_model);
        transaction.defer_notification(Notify{&journal, &application.windowing(), "late"});
    }
    require(journal == "late before-destroy\n", "an abandoned transaction did not commit");
    bool empty_refused = false;
    WorkspaceTransaction other(main.windows());
    try {
        other.defer_notification(std::function<void()>{});
    } catch (const std::invalid_argument&) {
        empty_refused = true;
    }
    require(empty_refused, "an empty notification was accepted");
}

// Finding V1 (owner decision V1-1): a host that refuses a window after it
// has taken the entry (a native creation failure) may already have destroyed
// the entry's model, so the transaction must abandon, not close, the scope it
// opened on that model. The headless host injects the failure. With
// refuse_and_keep the model survives in refused_models(), and the scope's
// update level is still entered: nothing left it, and no layout ran. With
// refuse_and_destroy the model is gone; closing its scope would be a
// use-after-free, which the sanitizer job reports.
void test_refusal_after_the_host_took_the_entry() {
    ApplicationWindowHandle main{};
    Window* main_model = nullptr;
    const std::shared_ptr<Panel> main_root = make_panel("main.root", {0.0, 0.0, 400.0, 300.0});
    std::vector<ApplicationWindow> windows;
    ApplicationWindow primary = make_entry("main", "", HostWindowFloating::none, main_root, {400.0, 300.0}, {});
    primary.options.ready = KeepWindow{&main, &main_model};
    windows.push_back(std::move(primary));
    host::HeadlessApplication application(std::move(windows));
    const std::shared_ptr<ApplicationResources> resources = (*main_model).resources();
    {
        WorkspaceTransaction transaction(main.windows());
        application.inject_open_fault(host::HeadlessOpenFault::refuse_and_keep);
        ApplicationWindow entry = make_entry("float.kept", "main", HostWindowFloating::tool,
                                             make_panel("float.kept.root", {0.0, 0.0, 200.0, 300.0}),
                                             {200.0, 300.0}, resources);
        const ApplicationWindowOpenResult refused = transaction.open_window(std::move(entry));
        require(refused.status.error == HostServiceError::backend_failure && !refused.window.active(),
                "the injected failure did not refuse the window");
        require(!entry.model, "the host did not take the entry before refusing it");
        require(application.refused_models().size() == 1U, "the refused model was not kept");
        const MetricsSnapshot kept = (*application.refused_models()[0]).metrics_snapshot();
        require(kept.update_scope_depth == 1U && kept.arrange_passes == 0U,
                "the transaction closed the scope of a model the host had taken and refused");
        const WorkspaceTransactionResult result = transaction.commit();
        require(result.status.accepted() && result.windows_laid_out == 0U && result.windows_shown == 0U,
                "the refused window stayed in the transaction");
    }
    const MetricsSnapshot after = (*application.refused_models()[0]).metrics_snapshot();
    require(after.update_scope_depth == 1U && after.arrange_passes == 0U,
            "the commit touched the refused model");
    {
        WorkspaceTransaction transaction(main.windows());
        application.inject_open_fault(host::HeadlessOpenFault::refuse_and_destroy);
        ApplicationWindow entry = make_entry("float.lost", "main", HostWindowFloating::tool,
                                             make_panel("float.lost.root", {0.0, 0.0, 200.0, 300.0}),
                                             {200.0, 300.0}, resources);
        const ApplicationWindowOpenResult refused = transaction.open_window(std::move(entry));
        require(refused.status.error == HostServiceError::backend_failure, "the destroying fault did not refuse");
        const WorkspaceTransactionResult result = transaction.commit();
        require(result.windows_laid_out == 0U, "a destroyed model stayed in the transaction");
    }
    require(main.windows().window_count() == 1U, "a refused window stayed in the run");
}

std::uint64_t milliseconds(std::uint64_t value) {
    const std::uint64_t nanoseconds = value * 1000000U;
    return nanoseconds;
}

void test_capture_drag() {
    ApplicationWindowHandle main{};
    Window* main_model = nullptr;
    const std::shared_ptr<Panel> main_root = make_panel("main.root", {0.0, 0.0, 800.0, 600.0});
    std::vector<ApplicationWindow> windows;
    ApplicationWindow primary = make_entry("main", "", HostWindowFloating::none, main_root, {800.0, 600.0}, {});
    primary.options.ready = KeepWindow{&main, &main_model};
    windows.push_back(std::move(primary));
    host::HeadlessApplication application(std::move(windows));
    ApplicationWindowHandle tool{};
    Window* tool_model = nullptr;
    ApplicationWindow entry = make_entry("tool", "main", HostWindowFloating::tool,
                                         make_panel("tool.root", {0.0, 0.0, 200.0, 300.0}), {200.0, 300.0},
                                         (*main_model).resources());
    entry.options.ready = KeepWindow{&tool, &tool_model};
    require(main.windows().open_window(std::move(entry)).status.accepted(), "tool window refused");
    host::HeadlessWindowing& windowing = application.windowing();
    require(WindowFrameDrag::mode_for(main.windows().capabilities()) == WindowFrameDragMode::framework_capture,
            "headless host not driven by the framework's capture drag");
    const Rect start = tool.frame().frame.content;

    // A drag at 60 pointer moves per second over the tool window's caption.
    windowing.clear_trace();
    WindowFrameDrag drag;
    windowing.set_pointer({start.x + 40.0, start.y - 10.0});
    require(drag.begin(tool, milliseconds(0U)).accepted() && drag.active(), "drag refused");
    require(drag.begin(tool, milliseconds(0U)).error == HostServiceError::invalid_argument,
            "a second drag accepted while one is active");
    for (std::uint64_t step = 1U; step <= 30U; ++step) {
        const double offset = static_cast<double>(step) * 4.0;
        windowing.set_pointer({start.x + 40.0 + offset, start.y - 10.0 + offset / 2.0});
        require(drag.move(milliseconds(step * 16U)).accepted(), "drag move refused");
    }
    // A move that does not change the target requests nothing.
    require(drag.move(milliseconds(31U * 16U)).accepted(), "still move refused");
    drag.end(milliseconds(31U * 16U));
    const Rect moved = tool.frame().frame.content;
    require(moved.x == start.x + 120.0 && moved.y == start.y + 60.0, "dragged window did not keep the grab offset");
    const WindowFrameDragStatistics statistics = drag.statistics();
    require(statistics.pointer_moves == 31U && statistics.frame_requests == 30U &&
                statistics.frame_reports == 30U && statistics.refused_requests == 0U,
            "drag statistics wrong");
    require(statistics.achieved_rate_hz() > 59.0 && statistics.achieved_rate_hz() < 62.0 &&
                statistics.keeps_up(),
            "headless drag rate not about 60 Hz");
    std::cout << "drag: pointer " << statistics.pointer_rate_hz() << " Hz, frames "
              << statistics.achieved_rate_hz() << " Hz, requests " << statistics.frame_requests
              << ", reports " << statistics.frame_reports << '\n';

    // The user dragged the floating window: that is its new offset, and the
    // main window carries it there.
    application.simulate_user_frame(main.id(), {150.0, 100.0, 800.0, 600.0});
    const Rect carried = tool.frame().frame.content;
    require(carried.x == moved.x + 50.0 && carried.y == moved.y, "drag did not set the floating offset");

    // Escape puts the window back where the drag began.
    const Rect before_cancel = tool.frame().frame.content;
    windowing.set_pointer({before_cancel.x + 10.0, before_cancel.y - 5.0});
    require(drag.begin(tool, milliseconds(1000U)).accepted(), "second drag refused");
    windowing.set_pointer({before_cancel.x + 210.0, before_cancel.y + 95.0});
    require(drag.move(milliseconds(1016U)).accepted(), "second move refused");
    require(drag.cancel(milliseconds(1020U)).accepted() && !drag.active(), "cancel refused");
    require(tool.frame().frame.content == before_cancel, "cancel did not restore the frame");
    require(drag.move(milliseconds(1030U)).error == HostServiceError::invalid_argument,
            "a move after cancel accepted");

    // A host that answers frame requests late (X11) reports fewer frames
    // during the drag; the statistics show it.
    windowing.set_deferred_frame_reports(true);
    windowing.set_pointer({before_cancel.x + 10.0, before_cancel.y - 5.0});
    require(drag.begin(tool, milliseconds(2000U)).accepted(), "third drag refused");
    for (std::uint64_t step = 1U; step <= 10U; ++step) {
        windowing.set_pointer({before_cancel.x + 10.0 + static_cast<double>(step), before_cancel.y - 5.0});
        require(drag.move(milliseconds(2000U + step * 16U)).accepted(), "deferred move refused");
    }
    require(drag.statistics().frame_reports == 0U, "deferred reports counted before they arrived");
    windowing.flush_frame_reports();
    drag.end(milliseconds(2160U));
    windowing.set_deferred_frame_reports(false);
    require(drag.statistics().frame_reports == 10U, "late reports not counted");

    // keeps_up: coalesced reports at a high pointer rate still keep up; too
    // few, too slowly, do not.
    WindowFrameDragStatistics coalesced;
    coalesced.pointer_moves = 240U;
    coalesced.frame_requests = 240U;
    coalesced.frame_reports = 120U;
    coalesced.first_timestamp_nanoseconds = 0U;
    coalesced.last_timestamp_nanoseconds = milliseconds(1000U);
    require(coalesced.keeps_up(), "coalesced 120 Hz reports judged too slow");
    WindowFrameDragStatistics slow = coalesced;
    slow.frame_reports = 12U;
    require(!slow.keeps_up(), "12 Hz judged fast enough");
    std::cout << "drag trace:\n" << windowing.trace();
}

void test_native_move_loop_fallback() {
    // A host without a desktop pointer cannot run the capture drag.
    const HostCapabilities limited{HostCapabilities::current_protocol_version, "limited",
                                   HostCapability::lifecycle | HostCapability::window_frames |
                                       HostCapability::pointer_capture | HostCapability::monitor_geometry};
    require(WindowFrameDrag::mode_for(limited) == WindowFrameDragMode::native_move_loop,
            "a host without the desktop pointer offered the capture drag");
    ApplicationWindowHandle main{};
    Window* main_model = nullptr;
    std::vector<ApplicationWindow> windows;
    ApplicationWindow primary = make_entry("main", "", HostWindowFloating::none,
                                           make_panel("main.root", {0.0, 0.0, 400.0, 300.0}), {400.0, 300.0}, {});
    primary.options.ready = KeepWindow{&main, &main_model};
    windows.push_back(std::move(primary));
    host::HeadlessApplication application(std::move(windows), limited);
    WindowFrameDrag drag;
    require(drag.begin(main, 0U).error == HostServiceError::unsupported && !drag.active(),
            "capture drag began without its capabilities");
    require(std::string(window_frame_drag_mode_name(WindowFrameDragMode::native_move_loop)) == "native_move_loop",
            "mode name wrong");
}
} // namespace

int main() {
    try {
        test_two_window_transaction();
        test_transaction_destructor_commits();
        test_refusal_after_the_host_took_the_entry();
        test_capture_drag();
        test_native_move_loop_fallback();
        std::cout << "gui_forms_workspace_transaction_tests: all tests passed\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "gui_forms_workspace_transaction_tests: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
