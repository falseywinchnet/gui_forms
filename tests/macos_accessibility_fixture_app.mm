#import <AppKit/AppKit.h>

#include "macos_host.hpp"

#include "gui_forms/gui_forms.hpp"
#include "support/stock_control_fixture.hpp"

#if defined(GUI_FORMS_AX_GALLERY)
#include "gallery.hpp"
#endif

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unistd.h>

// Stage A: the application the out-of-process AX client
// (macos_accessibility_client_tests) inspects. It shows the stock-control fixture, or,
// built as the Gallery variant (GUI_FORMS_AX_GALLERY), the GUI.Forms
// Gallery. While it runs it writes a dump the client compares against:
//
//   {"pid":N,"generation":G,"effects":E,"types":{stable id: manifest id},
//    "snapshot":<SemanticSnapshot::to_json()>}
//
// to the path after --semantic-dump, whenever its content changes (checked
// every 50 ms), by writing a temporary file and renaming it over the
// dump. The fixture variant adds two controls: "ax.disposable" and the
// button "ax.dispose", whose click removes and destroys "ax.disposable", so
// the client can hold an element of a disposed control (interview GF017).

namespace {

using gui_forms::Control;
using gui_forms::SemanticNode;
using gui_forms::SemanticSnapshot;
using gui_forms::test_support::StockControlFixture;
using gui_forms::test_support::StockEffectCounter;

constexpr const char* fixture_title = "GUI.Forms AX fixture";

void append_json_string(std::string& out, std::string_view text) {
    out += '"';
    for (const char character : text) {
        if (character == '"' || character == '\\') out += '\\';
        out += character;
    }
    out += '"';
}

// Each node's control type, by stable id: the control's manifest id
// (anatomy().control_id) for a node that is a control.
void append_types(const gui_forms::Window& window, const SemanticNode& node, std::string& out,
                  bool& first) {
    const Control::Ptr control = window.find(node.stable_id);
    if (control) {
        if (!first) out += ',';
        first = false;
        append_json_string(out, node.stable_id);
        out += ':';
        append_json_string(out, (*control).anatomy().control_id);
    }
    for (const SemanticNode& child : node.children) append_types(window, child, out, first);
}

class SemanticDumpWriter final {
public:
    SemanticDumpWriter(gui_forms::Window& window, const StockEffectCounter* effects, std::string path)
        : window_(window), effects_(effects), path_(std::move(path)) {}

    // Rewrites the dump when its content changed: a new generation, a new
    // effect count, or a value that changed without a new generation (a
    // diagnostics control's live counters).
    void tick() {
        const std::uint64_t effects = effects_ == nullptr ? 0U : (*effects_).total();
        const std::string text = dump_text(effects);
        if (text == written_text_) return;
        write(text);
        written_text_ = text;
    }

private:
    std::string dump_text(std::uint64_t effects) const {
        const SemanticSnapshot snapshot = window_.semantic_snapshot();
        std::string text = "{\"pid\":" + std::to_string(static_cast<long long>(getpid())) +
                           ",\"generation\":" + std::to_string(snapshot.generation) +
                           ",\"effects\":" + std::to_string(effects) + ",\"types\":{";
        bool first = true;
        for (const SemanticNode& root : snapshot.roots) append_types(window_, root, text, first);
        text += "},\"snapshot\":";
        text += snapshot.to_json();
        text += '}';
        return text;
    }

    void write(const std::string& text) const {
        const std::string temporary = path_ + ".tmp";
        std::FILE* file = std::fopen(temporary.c_str(), "w");
        if (file == nullptr) return;
        const std::size_t written = std::fwrite(text.data(), 1U, text.size(), file);
        const int closed = std::fclose(file);
        if (written == text.size() && closed == 0) std::rename(temporary.c_str(), path_.c_str());
    }

    gui_forms::Window& window_;
    const StockEffectCounter* effects_;
    std::string path_;
    std::string written_text_;
};

// Removes and destroys the disposable sample when "ax.dispose" is clicked.
class DisposeSample final {
public:
    DisposeSample(std::weak_ptr<Control> parent, std::weak_ptr<Control> sample)
        : parent_(std::move(parent)), sample_(std::move(sample)) {}

    void clicked(gui_forms::ButtonBase&) {
        const Control::Ptr parent = parent_.lock();
        const Control::Ptr sample = sample_.lock();
        if (!parent || !sample) return;
        static_cast<void>((*parent).remove_child((*sample).runtime_id()));
    }

private:
    std::weak_ptr<Control> parent_;
    std::weak_ptr<Control> sample_;
};

// What the app owns for its run. The writer and its timer are made on the UI
// thread when the host is ready, and the timer is declared after the writer
// it reports into.
struct AppState final {
    gui_forms::Window* window{};
    const StockEffectCounter* effects{};
    std::string dump_path;
    std::unique_ptr<SemanticDumpWriter> writer;
    std::unique_ptr<gui_forms::Timer> timer;
    gui_forms::SubscriptionToken tick_subscription;
};

using VoidCallback = std::function<void()>;
using ShowDialogCallback = std::function<gui_forms::HostDialogResult(const gui_forms::HostDialogRequest&)>;
using ShowTooltipCallback = std::function<gui_forms::HostServiceStatus(const gui_forms::HostTooltipRequest&)>;
using ReadClipboardCallback = std::function<gui_forms::HostClipboardTextResult()>;
using WriteClipboardCallback = std::function<gui_forms::HostServiceStatus(std::string_view)>;

class StartDumping final {
public:
    explicit StartDumping(AppState& state) noexcept : state_(state) {}

    void operator()(VoidCallback, VoidCallback, ShowDialogCallback, ShowTooltipCallback, VoidCallback,
                    ReadClipboardCallback, WriteClipboardCallback) const {
        if (state_.dump_path.empty()) return;
        gui_forms::Window& window = *state_.window;
        state_.writer = std::make_unique<SemanticDumpWriter>(window, state_.effects, state_.dump_path);
        state_.timer = std::make_unique<gui_forms::Timer>(window, std::chrono::milliseconds(50));
        state_.tick_subscription = (*state_.timer).tick().subscribe(
            gui_forms::Delegate<>::bind<SemanticDumpWriter, &SemanticDumpWriter::tick>(*state_.writer));
        (*state_.timer).start();
        (*state_.writer).tick();
    }

private:
    AppState& state_;
};

std::string argument_after(int argc, char** argv, std::string_view name) {
    for (int index = 1; index + 1 < argc; ++index) {
        if (std::string_view(argv[index]) == name) return argv[index + 1];
    }
    return {};
}

} // namespace

int main(int argc, char** argv) {
    AppState state;
    state.dump_path = argument_after(argc, argv, "--semantic-dump");
    gui_forms::host::MacHostOptions options;
    options.print_metrics_on_close = false;
    options.host_ready = StartDumping(state);
#if defined(GUI_FORMS_AX_GALLERY)
    std::unique_ptr<gui_forms::Window> window = gui_forms::gallery::make_gallery();
    options.title = std::string(gui_forms::gallery::gallery_program_name);
    options.initial_size = gui_forms::gallery::gallery_default_size;
    options.minimum_size = gui_forms::gallery::gallery_minimum_size;
    state.window = window.get();
    return gui_forms::host::run_macos(std::move(window), std::move(options));
#else
    StockControlFixture fixture = gui_forms::test_support::make_stock_control_fixture();
    std::shared_ptr<gui_forms::Button> disposable =
        gui_forms::make_control<gui_forms::Button>(gui_forms::StableId("ax.disposable"), "Disposable sample");
    (*disposable).set_requested_bounds({1500.0, 1100.0, 180.0, 28.0});
    (*fixture.root).add_child(disposable);
    std::shared_ptr<gui_forms::Button> dispose =
        gui_forms::make_control<gui_forms::Button>(gui_forms::StableId("ax.dispose"), "Dispose sample");
    (*dispose).set_requested_bounds({1700.0, 1100.0, 180.0, 28.0});
    (*fixture.root).add_child(dispose);
    DisposeSample dispose_sample(fixture.root, disposable);
    const gui_forms::SubscriptionToken dispose_subscription = (*dispose).clicked().subscribe(
        gui_forms::Delegate<gui_forms::ButtonBase&>::bind<DisposeSample, &DisposeSample::clicked>(dispose_sample));
    disposable.reset();
    std::unique_ptr<gui_forms::Window> window =
        std::make_unique<gui_forms::Window>(fixture.root, gui_forms::test_support::stock_control_fixture_size);
    options.title = fixture_title;
    options.initial_size = gui_forms::test_support::stock_control_fixture_size;
    state.window = window.get();
    state.effects = fixture.effects.get();
    return gui_forms::host::run_macos(std::move(window), std::move(options));
#endif
}
