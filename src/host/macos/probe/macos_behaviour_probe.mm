// Native verification on macOS (stage DK decision 50): one behaviour of the
// AppKit host that only a real Mac can show, checked without anyone
// watching, written as one JSON document.
//
//   gui_forms_macos_behaviour_probe --check NAME --out FILE.json
//
// Checks (tools/native_verification/SIBLING_PROMPT.md says what each means):
//   fullscreen_auxiliary      floating windows join the main window's
//                             full-screen space
//   child_order_out           floating windows hide with their main window
//                             and return with it
//   child_miniaturize         floating windows leave with a miniaturized main
//                             window and return when it is restored
//   tool_hides_on_deactivate  a floating tool hides while the application is
//                             inactive and a floating document does not
//   context_menu_key          virtual key 0x6E reaches the model as the
//                             context-menu key (needs Accessibility)
//   ime_escape                Escape during composition cancels it and is not
//                             delivered as a key (needs Accessibility and an
//                             already enabled input method)
//
// It opens its own windows only, reads their AppKit state in-process, and
// exits 0 on pass or skipped, 1 on fail and 2 when the result is
// inconclusive. It never asks for a permission and never changes a setting;
// ime_escape selects an already enabled input source and restores the one
// that was selected.
// GUI.Forms first, and without AssertMacros' unprefixed check, verify and
// require macros: Carbon must not rewrite names in its headers.
#include "gui_forms/application.hpp"
#include "gui_forms/basic_controls.hpp"
#include "gui_forms/control.hpp"
#include "gui_forms/threading.hpp"
#include "gui_forms/timer.hpp"

#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// Carbon declares global Point and Rect types; every GUI.Forms type here is
// spelled with its namespace.
namespace {

std::uint64_t now_milliseconds() {
    const std::chrono::steady_clock::duration since = std::chrono::steady_clock::now().time_since_epoch();
    const std::uint64_t value = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(since).count());
    return value;
}

constexpr const char* main_title = "GUI.Forms behaviour probe";
constexpr const char* tool_title = "GUI.Forms behaviour probe tool";
constexpr const char* document_title = "GUI.Forms behaviour probe document";
constexpr std::uint64_t probe_deadline_ms = 60000U;

enum class Check : std::uint8_t {
    fullscreen_auxiliary,
    child_order_out,
    child_miniaturize,
    tool_hides_on_deactivate,
    context_menu_key,
    ime_escape,
    unknown,
};

Check parse_check(const char* name) {
    if (std::strcmp(name, "fullscreen_auxiliary") == 0) return Check::fullscreen_auxiliary;
    if (std::strcmp(name, "child_order_out") == 0) return Check::child_order_out;
    if (std::strcmp(name, "child_miniaturize") == 0) return Check::child_miniaturize;
    if (std::strcmp(name, "tool_hides_on_deactivate") == 0) return Check::tool_hides_on_deactivate;
    if (std::strcmp(name, "context_menu_key") == 0) return Check::context_menu_key;
    if (std::strcmp(name, "ime_escape") == 0) return Check::ime_escape;
    return Check::unknown;
}

// One named observation, its value already written as JSON.
struct Observation final {
    std::string name{};
    std::string json{};
};

std::string json_string(const std::string& text) {
    std::string result{"\""};
    for (const char character : text) {
        const unsigned char code = static_cast<unsigned char>(character);
        if (character == '"' || character == '\\') {
            result.push_back('\\');
            result.push_back(character);
        } else if (code < 0x20U) {
            char escaped[8];
            std::snprintf(escaped, sizeof(escaped), "\\u%04x", static_cast<unsigned int>(code));
            result.append(escaped);
        } else {
            result.push_back(character);
        }
    }
    result.push_back('"');
    return result;
}

std::string json_bool(bool value) {
    const std::string result = value ? "true" : "false";
    return result;
}

// Key and text events the main window's focused control received.
class Recorder final : public gui_forms::Control {
public:
    struct KeyRecord final {
        std::uint64_t at_ms{};
        std::uint32_t physical_key{};
        bool down{};
    };
    struct TextRecord final {
        std::uint64_t at_ms{};
        std::string text{};
        bool composing{};
    };

    explicit Recorder(gui_forms::StableId id) : gui_forms::Control(std::move(id)) {
        set_focusable(true);
        keys.reserve(64U);
        texts.reserve(64U);
    }
    std::vector<KeyRecord> keys{};
    std::vector<TextRecord> texts{};

    void on_key(gui_forms::KeyEvent& event) override {
        if (keys.size() < keys.capacity()) {
            keys.push_back({now_milliseconds(), event.physical_key, event.action == gui_forms::KeyAction::down});
        }
        event.handled = true;
    }
    void on_text_input(gui_forms::TextInputEvent& event) override {
        if (texts.size() < texts.capacity()) {
            texts.push_back({now_milliseconds(), event.text_utf8, event.composing});
        }
        event.handled = true;
    }
};

struct Probe final {
    Check check{Check::unknown};
    std::string check_name{};
    std::string out_path{};
    gui_forms::ApplicationWindowHandle main{};
    gui_forms::ApplicationWindowHandle tool{};
    gui_forms::ApplicationWindowHandle document{};
    gui_forms::Window* main_model{};
    std::shared_ptr<Recorder> recorder{};
    std::unique_ptr<gui_forms::Timer> timer{};
    gui_forms::SubscriptionToken tick{};
    int step{};
    std::uint64_t wait_until_ms{};
    std::uint64_t step_deadline_ms{};
    std::uint64_t started_ms{};
    bool finished{};
    std::string result{"inconclusive"};
    std::string reason{};
    std::vector<Observation> observations{};
    std::size_t keys_before{};
    std::size_t texts_before{};
    TISInputSourceRef saved_source{};
};

NSWindow* native_window(const char* title) {
    NSString* const wanted = [NSString stringWithUTF8String:title];
    for (NSWindow* window in [NSApp windows]) {
        if ([[window title] isEqualToString:wanted]) return window;
    }
    return nil;
}

void observe(Probe& probe, const std::string& name, const std::string& json) {
    probe.observations.push_back({name, json});
}

// Visibility and space membership of one window, as observations.
void observe_window(Probe& probe, const std::string& prefix, const char* title) {
    NSWindow* const window = native_window(title);
    observe(probe, prefix + ".found", json_bool(window != nil));
    if (window == nil) return;
    observe(probe, prefix + ".visible", json_bool([window isVisible] == YES));
    observe(probe, prefix + ".on_active_space", json_bool([window isOnActiveSpace] == YES));
    observe(probe, prefix + ".miniaturized", json_bool([window isMiniaturized] == YES));
    observe(probe, prefix + ".panel", json_bool([window isKindOfClass:[NSPanel class]] == YES));
    observe(probe, prefix + ".hides_on_deactivate", json_bool([window hidesOnDeactivate] == YES));
    const bool auxiliary = ([window collectionBehavior] & NSWindowCollectionBehaviorFullScreenAuxiliary) != 0;
    observe(probe, prefix + ".fullscreen_auxiliary", json_bool(auxiliary));
    const bool full_screen = ([window styleMask] & NSWindowStyleMaskFullScreen) != 0;
    observe(probe, prefix + ".full_screen", json_bool(full_screen));
    NSWindow* const parent = [window parentWindow];
    const std::string parent_title = parent == nil ? std::string{} : std::string([[parent title] UTF8String]);
    observe(probe, prefix + ".parent", json_string(parent_title));
}

bool visible(const char* title) {
    NSWindow* const window = native_window(title);
    const bool result = window != nil && [window isVisible] == YES;
    return result;
}

bool write_result(const Probe& probe) {
    std::FILE* const file = std::fopen(probe.out_path.c_str(), "wb");
    if (file == nullptr) return false;
    NSString* const version = [[NSProcessInfo processInfo] operatingSystemVersionString];
    const bool trusted = AXIsProcessTrusted() != false;
    std::fprintf(file, "{\"probe\":\"macos_behaviour\",\"schema\":1,\"check\":%s,\"result\":%s,\"reason\":%s,"
                       "\"macos\":%s,\"accessibility_trusted\":%s,\"observations\":{",
                 json_string(probe.check_name).c_str(), json_string(probe.result).c_str(),
                 json_string(probe.reason).c_str(), json_string(std::string([version UTF8String])).c_str(),
                 trusted ? "true" : "false");
    for (std::size_t index = 0U; index < probe.observations.size(); ++index) {
        const Observation& observation = probe.observations[index];
        std::fprintf(file, "%s%s:%s", index == 0U ? "" : ",", json_string(observation.name).c_str(),
                     observation.json.c_str());
    }
    std::fputs("}}\n", file);
    const bool written = std::ferror(file) == 0;
    const bool closed = std::fclose(file) == 0;
    return written && closed;
}

void finish(Probe& probe, const char* result, const std::string& reason) {
    if (probe.finished) return;
    probe.finished = true;
    probe.result = result;
    probe.reason = reason;
    if (probe.saved_source != nullptr) {
        static_cast<void>(TISSelectInputSource(probe.saved_source));
        CFRelease(probe.saved_source);
        probe.saved_source = nullptr;
    }
    if (probe.timer) (*probe.timer).stop();
    if (!write_result(probe)) probe.result = "error";
    std::printf("%s: %s%s%s\n", probe.check_name.c_str(), probe.result.c_str(), probe.reason.empty() ? "" : ": ",
                probe.reason.c_str());
    std::fflush(stdout);
    static_cast<void>(probe.main.request_close());
}

void wait_for(Probe& probe, std::uint64_t milliseconds) {
    probe.wait_until_ms = now_milliseconds() + milliseconds;
}

void next(Probe& probe, std::uint64_t wait_ms, std::uint64_t deadline_ms) {
    ++probe.step;
    wait_for(probe, wait_ms);
    probe.step_deadline_ms = now_milliseconds() + wait_ms + deadline_ms;
}

bool past_deadline(const Probe& probe) {
    const bool past = now_milliseconds() > probe.step_deadline_ms;
    return past;
}

void post_key(CGKeyCode code) {
    const CGEventRef down = CGEventCreateKeyboardEvent(nullptr, code, true);
    const CGEventRef up = CGEventCreateKeyboardEvent(nullptr, code, false);
    if (down != nullptr) {
        CGEventPost(kCGHIDEventTap, down);
        CFRelease(down);
    }
    if (up != nullptr) {
        CGEventPost(kCGHIDEventTap, up);
        CFRelease(up);
    }
}

void focus_recorder(Probe& probe) {
    NSWindow* const window = native_window(main_title);
    [NSApp activate];
    if (window != nil) [window makeKeyAndOrderFront:nil];
    if (probe.main_model != nullptr) static_cast<void>((*probe.main_model).request_focus(probe.recorder));
}

// An enabled input source that composes (an input method's mode), or null.
// The caller releases it.
TISInputSourceRef enabled_input_method(std::string& identifier) {
    NSDictionary* const filter = @{
        (__bridge NSString*)kTISPropertyInputSourceCategory : (__bridge NSString*)kTISCategoryKeyboardInputSource,
        (__bridge NSString*)kTISPropertyInputSourceIsSelectCapable : @YES,
    };
    // false: enabled sources only; nothing is installed or enabled here.
    const CFArrayRef sources = TISCreateInputSourceList((__bridge CFDictionaryRef)filter, false);
    if (sources == nullptr) return nullptr;
    TISInputSourceRef found = nullptr;
    const CFIndex count = CFArrayGetCount(sources);
    for (CFIndex index = 0; index < count && found == nullptr; ++index) {
        TISInputSourceRef const source = (TISInputSourceRef)CFArrayGetValueAtIndex(sources, index);
        const CFStringRef type = (CFStringRef)TISGetInputSourceProperty(source, kTISPropertyInputSourceType);
        if (type == nullptr || CFStringCompare(type, kTISTypeKeyboardInputMode, 0) != kCFCompareEqualTo) continue;
        const CFStringRef name = (CFStringRef)TISGetInputSourceProperty(source, kTISPropertyInputSourceID);
        if (name != nullptr) identifier = std::string([(__bridge NSString*)name UTF8String]);
        CFRetain(source);
        found = source;
    }
    CFRelease(sources);
    return found;
}

void run_fullscreen_auxiliary(Probe& probe) {
    switch (probe.step) {
    case 0:
        observe_window(probe, "before.tool", tool_title);
        observe_window(probe, "before.document", document_title);
        if (!visible(tool_title) || !visible(document_title)) {
            finish(probe, "inconclusive", "the floating windows were not visible before full screen");
            return;
        }
        static_cast<void>(probe.main.toggle_full_screen());
        next(probe, 0U, 8000U);
        return;
    case 1: {
        NSWindow* const window = native_window(main_title);
        const bool full = window != nil && ([window styleMask] & NSWindowStyleMaskFullScreen) != 0;
        if (!full) {
            if (past_deadline(probe)) finish(probe, "inconclusive", "the main window never entered full screen");
            return;
        }
        // Let the space transition finish.
        next(probe, 1500U, 1000U);
        return;
    }
    case 2: {
        observe_window(probe, "full_screen.main", main_title);
        observe_window(probe, "full_screen.tool", tool_title);
        observe_window(probe, "full_screen.document", document_title);
        NSWindow* const tool = native_window(tool_title);
        NSWindow* const document = native_window(document_title);
        const bool joined = tool != nil && document != nil && [tool isVisible] == YES &&
            [document isVisible] == YES && [tool isOnActiveSpace] == YES && [document isOnActiveSpace] == YES;
        probe.result = joined ? "pass" : "fail";
        probe.reason = joined ? "" : "a floating window is not on the full-screen space";
        static_cast<void>(probe.main.toggle_full_screen());
        next(probe, 0U, 8000U);
        return;
    }
    default: {
        NSWindow* const window = native_window(main_title);
        const bool full = window != nil && ([window styleMask] & NSWindowStyleMaskFullScreen) != 0;
        if (full && !past_deadline(probe)) return;
        const std::string result = probe.result;
        finish(probe, result.c_str(), probe.reason);
        return;
    }
    }
}

void run_child_order_out(Probe& probe) {
    switch (probe.step) {
    case 0:
        observe_window(probe, "before.tool", tool_title);
        observe_window(probe, "before.document", document_title);
        if (!visible(tool_title) || !visible(document_title)) {
            finish(probe, "inconclusive", "the floating windows were not visible to begin with");
            return;
        }
        static_cast<void>(probe.main.hide());
        next(probe, 800U, 0U);
        return;
    case 1:
        observe_window(probe, "hidden.main", main_title);
        observe_window(probe, "hidden.tool", tool_title);
        observe_window(probe, "hidden.document", document_title);
        probe.result = !visible(tool_title) && !visible(document_title) ? "pass" : "fail";
        if (probe.result == "fail") probe.reason = "a floating window stayed on screen after its main window was hidden";
        static_cast<void>(probe.main.show());
        next(probe, 800U, 0U);
        return;
    default: {
        observe_window(probe, "shown.tool", tool_title);
        observe_window(probe, "shown.document", document_title);
        const bool returned = visible(tool_title) && visible(document_title);
        if (probe.result == "pass" && !returned) {
            probe.result = "fail";
            probe.reason = "a floating window did not return when its main window was shown";
        }
        const std::string result = probe.result;
        finish(probe, result.c_str(), probe.reason);
        return;
    }
    }
}

void run_child_miniaturize(Probe& probe) {
    NSWindow* const window = native_window(main_title);
    switch (probe.step) {
    case 0:
        if (window == nil || !visible(tool_title) || !visible(document_title)) {
            finish(probe, "inconclusive", "the windows were not visible to begin with");
            return;
        }
        [window miniaturize:nil];
        next(probe, 0U, 5000U);
        return;
    case 1:
        if (window == nil || [window isMiniaturized] == NO) {
            if (past_deadline(probe)) finish(probe, "inconclusive", "the main window never miniaturized");
            return;
        }
        next(probe, 800U, 0U);
        return;
    case 2:
        observe_window(probe, "miniaturized.main", main_title);
        observe_window(probe, "miniaturized.tool", tool_title);
        observe_window(probe, "miniaturized.document", document_title);
        probe.result = !visible(tool_title) && !visible(document_title) ? "pass" : "fail";
        if (probe.result == "fail") probe.reason = "a floating window stayed on screen with its main window miniaturized";
        [window deminiaturize:nil];
        next(probe, 0U, 5000U);
        return;
    case 3:
        if ([window isMiniaturized] == YES) {
            if (past_deadline(probe)) finish(probe, "inconclusive", "the main window never deminiaturized");
            return;
        }
        next(probe, 1000U, 0U);
        return;
    default: {
        observe_window(probe, "restored.tool", tool_title);
        observe_window(probe, "restored.document", document_title);
        if (probe.result == "pass" && !(visible(tool_title) && visible(document_title))) {
            probe.result = "fail";
            probe.reason = "a floating window did not return with its restored main window";
        }
        const std::string result = probe.result;
        finish(probe, result.c_str(), probe.reason);
        return;
    }
    }
}

void run_tool_hides_on_deactivate(Probe& probe) {
    switch (probe.step) {
    case 0:
        observe_window(probe, "active.tool", tool_title);
        observe_window(probe, "active.document", document_title);
        if (!visible(tool_title) || !visible(document_title)) {
            finish(probe, "inconclusive", "the floating windows were not visible while active");
            return;
        }
        [NSApp deactivate];
        next(probe, 0U, 4000U);
        return;
    case 1:
        if ([NSApp isActive] == YES) {
            if (past_deadline(probe)) {
                finish(probe, "inconclusive",
                       "the application stayed active after [NSApp deactivate]; activate another application by hand "
                       "and rerun only if the owner asks");
            }
            return;
        }
        next(probe, 800U, 0U);
        return;
    case 2:
        observe_window(probe, "inactive.tool", tool_title);
        observe_window(probe, "inactive.document", document_title);
        probe.result = !visible(tool_title) && visible(document_title) ? "pass" : "fail";
        if (probe.result == "fail") {
            probe.reason = "while inactive the tool must be hidden and the document visible";
        }
        [NSApp activate];
        next(probe, 0U, 4000U);
        return;
    case 3:
        if ([NSApp isActive] == NO) {
            if (past_deadline(probe)) finish(probe, "inconclusive", "the application could not be reactivated");
            return;
        }
        next(probe, 800U, 0U);
        return;
    default: {
        observe_window(probe, "reactivated.tool", tool_title);
        if (probe.result == "pass" && !visible(tool_title)) {
            probe.result = "fail";
            probe.reason = "the tool did not return when the application became active";
        }
        const std::string result = probe.result;
        finish(probe, result.c_str(), probe.reason);
        return;
    }
    }
}

std::string key_list(const Recorder& recorder, std::size_t from) {
    std::string list{"["};
    for (std::size_t index = from; index < recorder.keys.size(); ++index) {
        const Recorder::KeyRecord& key = recorder.keys[index];
        char item[48];
        std::snprintf(item, sizeof(item), "%s{\"key\":%u,\"down\":%s}", index == from ? "" : ",", key.physical_key,
                      key.down ? "true" : "false");
        list.append(item);
    }
    list.push_back(']');
    return list;
}

std::string text_list(const Recorder& recorder, std::size_t from) {
    std::string list{"["};
    for (std::size_t index = from; index < recorder.texts.size(); ++index) {
        const Recorder::TextRecord& text = recorder.texts[index];
        if (index != from) list.push_back(',');
        list.append("{\"text\":");
        list.append(json_string(text.text));
        list.append(",\"composing\":");
        list.append(json_bool(text.composing));
        list.push_back('}');
    }
    list.push_back(']');
    return list;
}

void run_context_menu_key(Probe& probe) {
    const Recorder& recorder = *probe.recorder;
    switch (probe.step) {
    case 0:
        if (AXIsProcessTrusted() == false) {
            finish(probe, "skipped", "posting key events needs the Accessibility permission");
            return;
        }
        focus_recorder(probe);
        next(probe, 600U, 0U);
        return;
    case 1:
        probe.keys_before = recorder.keys.size();
        // kVK_ContextualMenu, the key a PC keyboard's context-menu key
        // reports on macOS.
        post_key(static_cast<CGKeyCode>(0x6E));
        next(probe, 600U, 0U);
        return;
    default: {
        observe(probe, "keys", key_list(recorder, probe.keys_before));
        bool context_menu = false;
        for (std::size_t index = probe.keys_before; index < recorder.keys.size(); ++index) {
            if (recorder.keys[index].down && recorder.keys[index].physical_key == 0x65U) context_menu = true;
        }
        if (recorder.keys.size() == probe.keys_before) {
            finish(probe, "fail", "no key event reached the focused control");
            return;
        }
        finish(probe, context_menu ? "pass" : "fail",
               context_menu ? std::string{} : "virtual key 0x6E did not arrive as PhysicalKey 0x65 (context menu)");
        return;
    }
    }
}

void run_ime_escape(Probe& probe) {
    const Recorder& recorder = *probe.recorder;
    switch (probe.step) {
    case 0: {
        if (AXIsProcessTrusted() == false) {
            finish(probe, "skipped", "posting key events needs the Accessibility permission");
            return;
        }
        std::string identifier{};
        TISInputSourceRef const method = enabled_input_method(identifier);
        if (method == nullptr) {
            finish(probe, "skipped",
                   "no input method is enabled; enabling one changes System Settings, which the rules forbid");
            return;
        }
        observe(probe, "input_method", json_string(identifier));
        probe.saved_source = TISCopyCurrentKeyboardInputSource();
        const OSStatus selected = TISSelectInputSource(method);
        CFRelease(method);
        if (selected != noErr) {
            finish(probe, "inconclusive", "the enabled input method could not be selected");
            return;
        }
        focus_recorder(probe);
        next(probe, 800U, 0U);
        return;
    }
    case 1:
        probe.keys_before = recorder.keys.size();
        probe.texts_before = recorder.texts.size();
        post_key(static_cast<CGKeyCode>(kVK_ANSI_K));
        post_key(static_cast<CGKeyCode>(kVK_ANSI_A));
        next(probe, 800U, 0U);
        return;
    case 2: {
        observe(probe, "composing.texts", text_list(recorder, probe.texts_before));
        bool composing = false;
        for (std::size_t index = probe.texts_before; index < recorder.texts.size(); ++index) {
            if (recorder.texts[index].composing && !recorder.texts[index].text.empty()) composing = true;
        }
        if (!composing) {
            finish(probe, "inconclusive", "the input method did not start composing on k, a");
            return;
        }
        probe.keys_before = recorder.keys.size();
        probe.texts_before = recorder.texts.size();
        post_key(static_cast<CGKeyCode>(kVK_Escape));
        next(probe, 800U, 0U);
        return;
    }
    default: {
        observe(probe, "after_escape.texts", text_list(recorder, probe.texts_before));
        observe(probe, "after_escape.keys", key_list(recorder, probe.keys_before));
        bool committed = false;
        bool cleared = false;
        bool escape_delivered = false;
        for (std::size_t index = probe.texts_before; index < recorder.texts.size(); ++index) {
            const Recorder::TextRecord& text = recorder.texts[index];
            if (!text.composing && !text.text.empty()) committed = true;
            if (text.text.empty()) cleared = true;
        }
        for (std::size_t index = probe.keys_before; index < recorder.keys.size(); ++index) {
            if (recorder.keys[index].down && recorder.keys[index].physical_key == 0x29U) escape_delivered = true;
        }
        observe(probe, "after_escape.committed", json_bool(committed));
        observe(probe, "after_escape.cleared", json_bool(cleared));
        observe(probe, "after_escape.escape_delivered", json_bool(escape_delivered));
        const bool passed = cleared && !committed && !escape_delivered;
        finish(probe, passed ? "pass" : "fail",
               passed ? std::string{} : "Escape must clear the composition, commit nothing and not arrive as a key");
        return;
    }
    }
}

struct Tick final {
    Probe* state{};
    void operator()() const {
        Probe& probe = *state;
        if (probe.finished) return;
        if (now_milliseconds() - probe.started_ms > probe_deadline_ms) {
            finish(probe, "inconclusive", "the check did not finish within 60 seconds");
            return;
        }
        if (now_milliseconds() < probe.wait_until_ms) return;
        switch (probe.check) {
        case Check::fullscreen_auxiliary: run_fullscreen_auxiliary(probe); return;
        case Check::child_order_out: run_child_order_out(probe); return;
        case Check::child_miniaturize: run_child_miniaturize(probe); return;
        case Check::tool_hides_on_deactivate: run_tool_hides_on_deactivate(probe); return;
        case Check::context_menu_key: run_context_menu_key(probe); return;
        case Check::ime_escape: run_ime_escape(probe); return;
        case Check::unknown: finish(probe, "error", "unknown check"); return;
        }
    }
};

struct MainClosed final {
    Probe* state{};
    void operator()() const {
        Probe& probe = *state;
        probe.tick.disconnect();
        if (probe.timer) (*probe.timer).dispose();
        probe.timer.reset();
    }
};

gui_forms::ApplicationWindow floating_window(const char* stable_id, const char* title,
                                             gui_forms::HostWindowFloating kind, gui_forms::Window& owner) {
    gui_forms::ApplicationWindow window{};
    window.stable_id = stable_id;
    window.owner_id = "main";
    window.floating = kind;
    const std::shared_ptr<gui_forms::Panel> root = gui_forms::make_control<gui_forms::Panel>(gui_forms::StableId(stable_id));
    window.model = std::make_unique<gui_forms::Window>(root, gui_forms::Size{220.0, 140.0}, owner.resources());
    window.options.title = title;
    window.options.initial_size = {220.0, 140.0};
    return window;
}

struct MainReady final {
    Probe* state{};
    void operator()(gui_forms::Window& window, gui_forms::ApplicationWindowHandle handle) const {
        Probe& probe = *state;
        probe.main = handle;
        probe.main_model = &window;
        probe.started_ms = now_milliseconds();
        probe.timer = std::make_unique<gui_forms::Timer>(window, std::chrono::milliseconds(50));
        probe.tick = (*probe.timer).tick().subscribe(Tick{state});
        const gui_forms::ApplicationWindows windows = handle.windows();
        const gui_forms::ApplicationWindowOpenResult tool = windows.open_window(
            floating_window("tool", tool_title, gui_forms::HostWindowFloating::tool, window));
        const gui_forms::ApplicationWindowOpenResult document = windows.open_window(
            floating_window("document", document_title, gui_forms::HostWindowFloating::document, window));
        probe.tool = tool.window;
        probe.document = document.window;
        observe(probe, "open.tool", json_bool(tool.status.accepted()));
        observe(probe, "open.document", json_bool(document.status.accepted()));
        // The floating windows are shown by the policy once the main window
        // is active; wait for that before the first step.
        wait_for(probe, 1200U);
        (*probe.timer).start();
    }
};

struct Watchdog final {
    std::uint64_t deadline_ms{};
};

void run_watchdog(const gui_forms::CancellationFlag& cancellation, void* context) {
    const Watchdog& watchdog = *static_cast<const Watchdog*>(context);
    while (!cancellation.requested()) {
        if (now_milliseconds() >= watchdog.deadline_ms) {
            std::fprintf(stderr, "behaviour probe: deadline passed; abandoning the check\n");
            std::fflush(stderr);
            std::_Exit(2);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

} // namespace

int main(int argc, char** argv) {
    Probe probe{};
    for (int index = 1; index + 1 < argc; index += 2) {
        if (std::strcmp(argv[index], "--check") == 0) probe.check_name = argv[index + 1];
        else if (std::strcmp(argv[index], "--out") == 0) probe.out_path = argv[index + 1];
    }
    probe.check = parse_check(probe.check_name.c_str());
    if (probe.check == Check::unknown || probe.out_path.empty()) {
        std::fprintf(stderr, "usage: gui_forms_macos_behaviour_probe --check NAME --out FILE.json\n"
                             "checks: fullscreen_auxiliary child_order_out child_miniaturize "
                             "tool_hides_on_deactivate context_menu_key ime_escape\n");
        return 2;
    }
    Watchdog watchdog{};
    watchdog.deadline_ms = now_milliseconds() + probe_deadline_ms + 30000U;
    gui_forms::Worker watchdog_worker(run_watchdog, &watchdog);
    probe.recorder = gui_forms::make_control<Recorder>(gui_forms::StableId("probe.recorder"));
    gui_forms::ApplicationWindowOptions options{};
    options.title = main_title;
    options.initial_size = {420.0, 280.0};
    options.ready = MainReady{&probe};
    options.closed = MainClosed{&probe};
    const gui_forms::ApplicationResult run =
        gui_forms::Application::run(std::make_unique<gui_forms::Window>(probe.recorder, gui_forms::Size{420.0, 280.0}),
                                    std::move(options));
    watchdog_worker.request_cancel();
    watchdog_worker.join();
    if (!run.accepted() || !probe.finished) return 2;
    if (probe.result == "pass" || probe.result == "skipped") return 0;
    if (probe.result == "fail") return 1;
    return 2;
}
