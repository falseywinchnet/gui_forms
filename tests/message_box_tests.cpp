// show_message_box (stage W): the Windows Forms MessageBox.Show facade over
// the host's message dialog. The request it sends (caption, text, buttons,
// icon and the default button counted from the first), the chosen button as
// a DialogResult, a box closed without a choice, a host that refuses, no
// host, refused arguments and the wrong thread.

#include "gui_forms/gui_forms.hpp"
#include "headless_host.hpp"

#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

// What the scripted host saw and what it answers next. An answer queue that
// runs out answers with a cancelled box.
struct MessageScript final {
    std::vector<HostDialogRequest> requests{};
    std::deque<HostMessageDialogResult> answers{};
    HostServiceError error{HostServiceError::none};
};

struct ScriptedMessages final {
    MessageScript* script{};

    HostDialogResult operator()(const HostDialogRequest& request, host::HeadlessHostServices&) const {
        (*script).requests.push_back(request);
        HostDialogResult result;
        result.request_id = request.request_id;
        if ((*script).error != HostServiceError::none) {
            result.status.error = (*script).error;
            result.payload = HostMessageDialogResult{};
            return result;
        }
        if ((*script).answers.empty()) {
            result.payload = HostMessageDialogResult{HostDialogOutcome::cancelled, HostDialogChoice::none};
            return result;
        }
        result.payload = (*script).answers.front();
        (*script).answers.pop_front();
        return result;
    }
};

HostMessageDialogResult chose(HostDialogChoice choice) {
    const HostMessageDialogResult result{HostDialogOutcome::accepted, choice};
    return result;
}

const HostMessageDialogRequest* message_request(const HostDialogRequest& request) {
    const HostMessageDialogRequest* const result = std::get_if<HostMessageDialogRequest>(&request.payload);
    return result;
}

void without_host() {
    std::shared_ptr<Panel> root = make_control<Panel>(StableId("message.unhosted"));
    Window window(root, {320.0, 200.0});
    const MessageBoxResult result = show_message_box(window, {.text = "Saved."});
    require(!result.shown() && result.result() == DialogResult::none && result.error() == HostServiceError::unsupported,
            "without a host the box is not shown and the result is none");
}

struct ShowFromOtherThread final {
    Window* window{};
    bool* refused{};
    void operator()() const {
        try {
            static_cast<void>(show_message_box(*window, {.text = "Elsewhere"}));
        } catch (const std::logic_error&) {
            *refused = true;
        }
    }
};

void shown_through_host() {
    std::shared_ptr<Panel> root = make_control<Panel>(StableId("message.root"));
    Window window(root, {320.0, 200.0});
    host::HeadlessHost headless(window);
    host::HeadlessHostServices& services = static_cast<host::HeadlessHostServices&>(headless.services());
    MessageScript script;
    services.set_dialog_handler(ScriptedMessages{&script});

    script.answers.push_back(chose(HostDialogChoice::no));
    const MessageBoxResult answered = show_message_box(window, {.text = "Discard the changes?",
                                                                .caption = "Evidence",
                                                                .buttons = HostMessageButtons::yes_no_cancel,
                                                                .icon = HostMessageIcon::question,
                                                                .default_button = MessageBoxDefaultButton::button2});
    require(answered.shown() && answered.result() == DialogResult::no && answered.error() == HostServiceError::none, "the chosen button is the result");
    require(script.requests.size() == 1U && script.requests[0].owner_id == "message.root" &&
                script.requests[0].request_id != 0U,
            "the request names the window's root and a fresh identity");
    const HostMessageDialogRequest* const sent = message_request(script.requests[0]);
    require(sent != nullptr && (*sent).title == "Evidence" && (*sent).message == "Discard the changes?" &&
                (*sent).buttons == HostMessageButtons::yes_no_cancel && (*sent).icon == HostMessageIcon::question &&
                (*sent).default_choice == HostDialogChoice::no,
            "the caption, text, buttons, icon and second button as default reach the host");

    script.answers.push_back(chose(HostDialogChoice::yes));
    require(show_message_box(window, {.text = "Overwrite?",
                                      .buttons = HostMessageButtons::yes_no,
                                      .default_button = MessageBoxDefaultButton::button3})
                    .result() == DialogResult::yes &&
                (*message_request(script.requests[1])).default_choice == HostDialogChoice::yes,
            "a default past the last button is the first button");

    script.answers.push_back(chose(HostDialogChoice::retry));
    require(show_message_box(window, {.text = "Disk busy", .buttons = HostMessageButtons::retry_cancel,
                                      .icon = HostMessageIcon::error})
                    .result() == DialogResult::retry &&
                (*message_request(script.requests[2])).default_choice == HostDialogChoice::retry,
            "retry is the result of Retry");

    require(show_message_box(window, {.text = "Done."}).result() == DialogResult::ok,
            "an OK box closed without a choice returns ok");
    require(show_message_box(window, {.text = "Go on?", .buttons = HostMessageButtons::ok_cancel}).result() ==
                DialogResult::cancel,
            "a box with Cancel closed without a choice returns cancel");
    require(show_message_box(window, {.text = "Keep?", .buttons = HostMessageButtons::yes_no}).result() ==
                DialogResult::none,
            "a Yes/No box the host closed without a choice returns none");

    script.error = HostServiceError::backend_failure;
    const MessageBoxResult refused = show_message_box(window, {.text = "Busy"});
    require(!refused.shown() && refused.result() == DialogResult::none && refused.error() == HostServiceError::backend_failure,
            "a host that refuses leaves none and says why");
    script.error = HostServiceError::none;

    bool refused_none = false;
    try {
        static_cast<void>(MessageBoxResult::refused(HostServiceError::none));
    } catch (const std::invalid_argument&) {
        refused_none = true;
    }
    require(refused_none, "a refusal names an error");
    require(MessageBoxResult::closed(DialogResult::yes).shown() &&
                MessageBoxResult::closed(DialogResult::yes).error() == HostServiceError::none,
            "a closed box has no refusal");

    const std::size_t sent_before = script.requests.size();
    bool threw = false;
    try {
        static_cast<void>(show_message_box(window, {.text = std::string("bad\0text", 8U)}));
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw, "text with NUL is refused");
    threw = false;
    try {
        static_cast<void>(show_message_box(window, {.text = "x", .caption = std::string(1U, '\xff')}));
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw, "a caption that is not UTF-8 is refused");
    threw = false;
    try {
        static_cast<void>(show_message_box(window, {.text = "x", .buttons = static_cast<HostMessageButtons>(9U)}));
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw && script.requests.size() == sent_before, "unknown buttons are refused before the host is asked");

    bool wrong_thread = false;
    std::thread other(ShowFromOtherThread{&window, &wrong_thread});
    other.join();
    require(wrong_thread && script.requests.size() == sent_before, "showing off the UI thread is refused");
}

} // namespace

int main() {
    without_host();
    shown_through_host();
    std::cout << "show_message_box: requests, results, dismissal, refusals, no host and threads passed\n";
    return 0;
}
