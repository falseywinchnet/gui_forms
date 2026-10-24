#pragma once

#include "gui_forms/host.hpp"
#include "gui_forms/window.hpp"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms::host {

// The browser host (Emscripten). One window fills the page's canvas element
// (`#gui-forms-canvas` unless the page sets Module.guiFormsCanvas); its client
// size is the canvas's CSS size and its scale is devicePixelRatio. The
// callbacks mirror LinuxHostOptions and are published the same way.
struct WebHostOptions final {
  std::string title{"GUI.Forms Application"};
  Size initial_size{1120.0, 680.0};
  Size minimum_size{150.0, 150.0};
  bool print_metrics_on_close{true};
  bool initially_visible{true};
  bool hide_on_close{};
  bool minimizable{true};
  std::function<void(std::function<void()> show, std::function<void()> hide)>
      visibility_ready;
  std::function<void(std::function<void()> toggle)> full_screen_ready;
  std::function<void(std::function<HostServiceStatus(std::string_view)>)> title_ready;
  std::function<void(std::function<HostServiceStatus(Size)>)> client_size_ready;
  std::function<void(HostCloseRequest &)> close_request;
  std::function<void(
      std::function<void()> wake, std::function<void()> request_close,
      std::function<HostDialogResult(const HostDialogRequest &)> show_dialog,
      std::function<HostServiceStatus(const HostTooltipRequest &)> show_tooltip,
      std::function<void()> hide_tooltip,
      std::function<HostClipboardTextResult()> read_clipboard_text,
      std::function<HostServiceStatus(std::string_view)> write_clipboard_text)>
      host_ready;
  std::function<void()> dispatch_pending;
  std::function<void()> closed;
};

struct WebApplicationWindow final {
  std::string stable_id;
  std::string owner_id;
  std::unique_ptr<Window> model;
  WebHostOptions options;
  bool tool_window{};
};

[[nodiscard]] HostCapabilities web_capabilities();
// Starts the browser event loop and never returns to the caller: the stack is
// unwound to the JavaScript event loop (destructors on it do not run) and the
// windows live on the heap until the page closes. Only the primary window is
// shown; tool windows are refused.
int run_web_application(std::vector<WebApplicationWindow> windows);

} // namespace gui_forms::host
