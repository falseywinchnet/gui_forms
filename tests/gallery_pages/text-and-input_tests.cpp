#include "support.hpp"

namespace gallery_pages_tests {

void test_text_and_input(gui_forms::Window& window,
                         gui_forms::host::HeadlessHost& host) {
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
}

} // namespace gallery_pages_tests
