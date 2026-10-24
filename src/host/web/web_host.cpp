// The browser host. It follows the X11 host's frame sequence (gather live
// frames, take damage, compose, paint, present the rectangle set, report) and
// replaces Xlib with a canvas: web_host.js forwards DOM input here through the
// exported gf_web_* entry points and presents raster rectangles with
// putImageData. The event loop is the browser's; ticks are requested through
// gf_web_js_schedule (requestAnimationFrame, or a timer for later deadlines),
// so a window with no damage, no live surface and no deadline costs nothing.
#include "gui_forms/platform/web_host.hpp"
#include "../../core/host/frame_capture/frame_capture.hpp"
#include "../../core/damage/device_damage/device_damage.hpp"
#include "../../render/skia/raster/skia_raster.hpp"
#include "gui_forms/live_surface.hpp"
#include "gui_forms/text/types/text_types.hpp"

#include <emscripten.h>
#include <emscripten/threading.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>

extern "C" {
// web_host.js
void gf_web_js_attach(double* width, double* height, double* scale);
void gf_web_js_schedule(int milliseconds);
void gf_web_js_present(const unsigned char* pixels, int stride, int left, int top,
                       int right, int bottom, int canvas_width, int canvas_height);
void gf_web_js_set_title(const char* title);
void gf_web_js_set_cursor(int kind);
void gf_web_js_pointer_capture(int captured, int pointer_id);
void gf_web_js_write_clipboard(const char* text);
void gf_web_js_full_screen();
void gf_web_js_set_visible(int visible);
void gf_web_js_screen(double* width, double* height);
}

namespace gui_forms::host::web_detail {
namespace {
std::uint64_t now_ns() {
  return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now().time_since_epoch()).count());
}

std::filesystem::path fonts_directory() {
  const char* override_path = std::getenv("GUI_FORMS_FONT_DIR");
  return override_path != nullptr ? std::filesystem::path(override_path)
                                  : std::filesystem::path("/gui_forms/fonts");
}

void register_fonts(render::SkiaRaster& raster) {
  struct Face final {
    const char* name;
    FontRole role;
    std::uint16_t weight;
    bool italic;
    bool fallback;
  };
  const Face faces[] = {
      {"PortsmouthRapids.ttf", FontRole::control, 400, false, false},
      {"PortsmouthRapids-Bold.ttf", FontRole::control, 700, false, false},
      {"Carlito-Regular.ttf", FontRole::content, 400, false, false},
      {"Carlito-Bold.ttf", FontRole::content, 700, false, false},
      {"Carlito-Italic.ttf", FontRole::content, 400, true, false},
      {"Carlito-BoldItalic.ttf", FontRole::content, 700, true, false},
      {"Cousine-Regular.ttf", FontRole::monospace, 400, false, false},
      {"Cousine-Bold.ttf", FontRole::monospace, 700, false, false},
      {"Cousine-Italic.ttf", FontRole::monospace, 400, true, false},
      {"Cousine-BoldItalic.ttf", FontRole::monospace, 700, true, false},
      {"NotoSansCJKjp-Regular.otf", FontRole::content, 400, false, true},
      {"NotoSansArabic-Regular.ttf", FontRole::content, 400, false, true},
      {"NotoSansHebrew-Regular.ttf", FontRole::content, 400, false, true},
      {"NotoSansDevanagari-Regular.ttf", FontRole::content, 400, false, true},
      {"NotoSansBengali-Regular.ttf", FontRole::content, 400, false, true},
      {"NotoSansGurmukhi-Regular.ttf", FontRole::content, 400, false, true},
      {"NotoEmoji-Regular.ttf", FontRole::content, 400, false, true}};
  const std::filesystem::path directory = fonts_directory();
  for (std::size_t index = 0; index < std::size(faces); ++index) {
    const Face& face = faces[index];
    const std::filesystem::path path = directory / face.name;
    // Optional Unicode and mono faces may be omitted by a page.
    if (!std::filesystem::exists(path) && index >= 6) continue;
    const std::string filename = path.string();
    const bool accepted = face.fallback
        ? raster.register_fallback_typeface_file(face.weight, face.italic, filename.c_str())
        : raster.register_typeface_file(face.role, face.weight, face.italic, filename.c_str());
    if (!accepted) throw std::runtime_error("Cannot load bundled GUI.Forms font: " + filename);
  }
}

// Wake handlers may run on worker threads; a tick is always requested on the
// browser's main thread.
void schedule_now() { gf_web_js_schedule(0); }
struct Wake final {
  void operator()() const noexcept {
    if (emscripten_is_main_runtime_thread()) schedule_now();
    else emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_V, &schedule_now);
  }
};
} // namespace

class NativeWindow;

class Services final : public HostServices {
public:
  explicit Services(NativeWindow& window) : HostServices(web_capabilities()), window_(window) {}
  ~Services() override { shutdown(); }
  std::string clipboard;
  std::uint64_t clipboard_generation{};

protected:
  HostMonitorResult query_monitors_impl() override {
    double width = 0.0, height = 0.0;
    gf_web_js_screen(&width, &height);
    return {{}, {{"web.screen", {0.0, 0.0, width, height}, {0.0, 0.0, width, height}, 1.0, true}}};
  }
  HostServiceStatus set_cursor_impl(CursorKind cursor) override {
    gf_web_js_set_cursor(static_cast<int>(cursor));
    return {};
  }
  HostServiceStatus set_pointer_capture_impl(bool captured, std::uint64_t pointer_id) override {
    gf_web_js_pointer_capture(captured ? 1 : 0, static_cast<int>(pointer_id));
    return {};
  }
  HostClipboardTextResult read_clipboard_text_impl() override {
    // The page mirrors the system clipboard from paste events; a browser
    // grants no synchronous read.
    return {{}, clipboard, clipboard_generation, !clipboard.empty()};
  }
  HostServiceStatus write_clipboard_text_impl(std::string_view text) override {
    clipboard.assign(text);
    ++clipboard_generation;
    gf_web_js_write_clipboard(clipboard.c_str());
    return {};
  }
  HostDialogResult show_dialog_impl(const HostDialogRequest& request) override {
    // Native dialogs would need a nested event loop, which a page cannot run.
    HostDialogResult result;
    result.status.error = HostServiceError::unsupported;
    result.request_id = request.request_id;
    return result;
  }
  HostServiceStatus play_sound_cue_impl(const HostSoundCueRequest&) override {
    return {HostServiceError::unsupported};
  }

private:
  [[maybe_unused]] NativeWindow& window_;
};

class NativeWindow final : public std::enable_shared_from_this<NativeWindow> {
public:
  explicit NativeWindow(WebApplicationWindow source);
  ~NativeWindow();
  void attach();
  void ready();
  void update();
  void close(HostCloseReason reason, bool force = false);
  bool gather_live_frames();
  void compose_live_frames();
  bool report_presented(const std::optional<PaintReceipt>& receipt, std::uint64_t elapsed,
                        PresentedArea copied);
  bool present(std::span<const Rect> requested, PresentedArea& copied);
  HostDispatchResult dispatch(HostEventPayload payload);
  void pointer(PointerEvent event);
  void update_cursor(Point position);

  WebApplicationWindow entry;
  Size size;
  double scale{1.0};
  bool closed{};
  bool visible{};
  std::unique_ptr<Services> services;
  std::unique_ptr<HostSession> session;
  render::SkiaRaster raster;
  DamageRegion damage;
  SubscriptionToken closing_token;
  std::uint64_t sequence{};
  double last_click{};
  Point last_click_position{};
  PointerButton last_click_button{};
  unsigned click_count{};
  std::vector<LiveSurfacePresentation> live_updates;
  std::vector<LiveSurfaceFrame> live_frames;
  std::vector<std::size_t> live_frame_sources;
  std::vector<Rect> live_regions;
  std::vector<std::uint64_t> live_frame_pixels;
  std::vector<std::uint64_t> live_frame_rectangles;
  bool live_region_lost{};
};

struct Runtime final {
  std::vector<std::shared_ptr<NativeWindow>> windows;
  void step();
};

Runtime* runtime = nullptr;

namespace {
NativeWindow* primary() {
  if (runtime == nullptr) return nullptr;
  for (const std::shared_ptr<NativeWindow>& window : (*runtime).windows)
    if (!(*window).closed) return window.get();
  return nullptr;
}

struct WindowAction final {
  std::weak_ptr<NativeWindow> window;
  int kind;  // 0 close, 1 show, 2 hide, 3 full screen
  void operator()() const {
    const std::shared_ptr<NativeWindow> target = window.lock();
    if (!target || (*target).closed) return;
    if (kind == 0) (*target).close(HostCloseReason::application);
    else if (kind == 1 || kind == 2) {
      (*target).visible = kind == 1;
      gf_web_js_set_visible((*target).visible ? 1 : 0);
      if ((*target).visible) (*target).damage.add({0, 0, (*target).size.width, (*target).size.height});
      static_cast<void>((*target).dispatch(HostOcclusionEvent{!(*target).visible}));
    } else gf_web_js_full_screen();
    schedule_now();
  }
};
struct WindowTitle final {
  std::weak_ptr<NativeWindow> owner{};
  HostServiceStatus operator()(const std::string_view text) const {
    if (!emscripten_is_main_runtime_thread()) return {HostServiceError::wrong_thread};
    const std::shared_ptr<NativeWindow> window = owner.lock();
    if (!window || (*window).closed) return {HostServiceError::after_shutdown};
    if (text.size() > 65536U || text.find('\0') != std::string_view::npos ||
        !validate_utf8(text).valid()) return {HostServiceError::invalid_argument};
    const std::string title(text);
    gf_web_js_set_title(title.c_str());
    return {};
  }
};
// The page decides the canvas size; a window cannot resize the browser.
struct WindowClientSize final {
  HostServiceStatus operator()(const Size) const { return {HostServiceError::unsupported}; }
};
struct Dialog final {
  NativeWindow* window;
  HostDialogResult operator()(const HostDialogRequest& request) const {
    return (*(*window).services).show_dialog(request);
  }
};
struct ReadText final {
  NativeWindow* window;
  HostClipboardTextResult operator()() const { return (*(*window).services).read_clipboard_text(); }
};
struct WriteText final {
  NativeWindow* window;
  HostServiceStatus operator()(std::string_view text) const {
    return (*(*window).services).write_clipboard_text(text);
  }
};
struct Tooltip final {
  HostServiceStatus operator()(const HostTooltipRequest&) const { return {HostServiceError::unsupported}; }
};
struct NoOperation final {
  void operator()() const {}
};
struct Closing final {
  NativeWindow* window;
  void operator()(HostCloseRequest& request) const {
    if ((*window).entry.options.close_request) (*window).entry.options.close_request(request);
  }
};
} // namespace

NativeWindow::NativeWindow(WebApplicationWindow source)
    : entry(std::move(source)), size(entry.options.initial_size) {
  gf_web_js_attach(&size.width, &size.height, &scale);
  if (!(size.width > 0.0) || !(size.height > 0.0)) size = entry.options.initial_size;
  register_fonts(raster);
  (*entry.model).set_text_metrics_provider(&raster);
  (*entry.model).set_framebuffer_painter(&raster);
  (*entry.model).metrics().set_renderer("Skia CPU / HarfBuzz / FreeType / Web canvas", true);
  services = std::make_unique<Services>(*this);
  PresentationDeclaration presentation;
  presentation.contract_version = paint_contract_version;
  presentation.extensions.partial_present = true;
  (*entry.model).declare_presentation(presentation);
  session = std::make_unique<HostSession>(*entry.model, web_capabilities(), services.get());
  closing_token = (*session).closing().subscribe(Closing{this});
  gf_web_js_set_title(entry.options.title.c_str());
}

NativeWindow::~NativeWindow() {
  (*entry.model).set_dispatch_wake_handler({});
  (*entry.model).set_paint_wake_handler({});
  (*entry.model).set_text_metrics_provider(nullptr);
  (*entry.model).set_framebuffer_painter(nullptr);
  if (session) (*session).shutdown();
}

HostDispatchResult NativeWindow::dispatch(HostEventPayload payload) {
  const HostDispatchResult result = (*session).dispatch({++sequence, now_ns(), std::move(payload)});
  schedule_now();
  return result;
}

void NativeWindow::attach() {
  (*entry.model).set_dispatch_wake_handler(Wake{});
  (*entry.model).set_paint_wake_handler(Wake{});
  if (!dispatch(HostAttachEvent{size, scale}).accepted())
    throw std::runtime_error("Web host attach rejected");
  if (entry.options.initially_visible) WindowAction{weak_from_this(), 1}();
}

void NativeWindow::ready() {
  if (entry.options.host_ready)
    entry.options.host_ready(Wake{}, WindowAction{weak_from_this(), 0}, Dialog{this}, Tooltip{},
                             NoOperation{}, ReadText{this}, WriteText{this});
  if (entry.options.full_screen_ready) entry.options.full_screen_ready(WindowAction{weak_from_this(), 3});
  if (entry.options.title_ready) entry.options.title_ready(WindowTitle{weak_from_this()});
  if (entry.options.client_size_ready) entry.options.client_size_ready(WindowClientSize{});
  if (entry.options.visibility_ready)
    entry.options.visibility_ready(WindowAction{weak_from_this(), 1}, WindowAction{weak_from_this(), 2});
}

void NativeWindow::close(HostCloseReason reason, bool force) {
  if (closed) return;
  if (!force) {
    const HostDispatchResult result =
        dispatch(HostCloseRequest{reason, false, entry.options.hide_on_close});
    if (!result.accepted() || !result.close_allowed) return;
    if (entry.options.hide_on_close) {
      WindowAction{weak_from_this(), 2}();
      return;
    }
  }
  closed = true;
  visible = false;
  static_cast<void>(dispatch(HostClosedEvent{reason}));
  (*entry.model).set_dispatch_wake_handler({});
  (*entry.model).set_paint_wake_handler({});
  (*session).shutdown();
  gf_web_js_set_visible(0);
  if (entry.options.print_metrics_on_close)
    std::cout << "{\"window\":" << (*entry.model).metrics_snapshot().to_json()
              << ",\"host\":" << (*session).snapshot().to_json() << "}\n";
  if (entry.options.closed) entry.options.closed();
}

void NativeWindow::update_cursor(const Point position) {
  const Control::Ptr captured = (*entry.model).captured_control();
  const Control::Ptr target = captured ? captured : (*entry.model).hit_test(position);
  static_cast<void>((*services).set_custom_cursor(
      target ? (*target).effective_cursor_images() : CursorImagesPtr{}, scale,
      target ? (*target).effective_cursor() : CursorKind::arrow));
}

void NativeWindow::pointer(PointerEvent event) {
  if (event.action == PointerAction::down) {
    const double now = emscripten_get_now();
    const double dx = event.position.x - last_click_position.x;
    const double dy = event.position.y - last_click_position.y;
    click_count = event.button == last_click_button && now - last_click < 500.0 &&
                          dx * dx + dy * dy < 25.0
                      ? click_count % 3 + 1
                      : 1;
    last_click = now;
    last_click_button = event.button;
    last_click_position = event.position;
  }
  if (event.action == PointerAction::down || event.action == PointerAction::up)
    event.click_count = click_count;
  const Point position = event.position;
  static_cast<void>(dispatch(std::move(event)));
  if (!closed) update_cursor(position);
}

bool NativeWindow::present(const std::span<const Rect> requested, PresentedArea& copied) {
  const std::uint32_t width = raster.pixel_width();
  const std::uint32_t height = raster.pixel_height();
  const unsigned char* const source = static_cast<const unsigned char*>(raster.pixels());
  if (source == nullptr || width == 0 || height == 0 || width > 16384 || height > 16384) return false;
  const std::chrono::steady_clock::time_point started = std::chrono::steady_clock::now();
  const Rect extent{0.0, 0.0, static_cast<double>(width), static_cast<double>(height)};
  for (const Rect rectangle : requested) {
    if (!rectangle.finite()) continue;
    const Rect device{rectangle.x * scale, rectangle.y * scale, rectangle.width * scale,
                      rectangle.height * scale};
    const Rect clipped = Rect::intersection(device, extent);
    if (clipped.empty()) continue;
    const int left = static_cast<int>(std::floor(clipped.x));
    const int top = static_cast<int>(std::floor(clipped.y));
    const int right = static_cast<int>(std::ceil(clipped.right()));
    const int bottom = static_cast<int>(std::ceil(clipped.bottom()));
    gf_web_js_present(source, static_cast<int>(raster.row_bytes()), left, top, right, bottom,
                      static_cast<int>(width), static_cast<int>(height));
    copied.device_pixels += static_cast<std::uint64_t>(right - left) *
                            static_cast<std::uint64_t>(bottom - top);
    ++copied.rectangles;
  }
  copied.copy_nanoseconds += static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now() - started).count());
  return true;
}

void NativeWindow::update() {
  if (closed) return;
  if (entry.options.dispatch_pending) entry.options.dispatch_pending();
  if (closed) return;
  static_cast<void>((*entry.model).drain_posted_work());
  static_cast<void>((*entry.model).poll_frame_schedule(std::chrono::steady_clock::now()));
  const bool regions_taken = gather_live_frames();
  const DamageRegion fresh = (*entry.model).take_damage();
  for (std::size_t i = 0; i < fresh.rectangles().size(); ++i) damage.add(fresh.rectangles()[i]);
  if (regions_taken) live_region_lost = true;
  DamageRegion screen = damage;
  for (std::size_t index = 0; index < live_regions.size(); ++index)
    if (!live_regions[index].empty()) screen.add(live_regions[index]);
  if (!visible || screen.empty()) {
    live_frames.clear();
    return;
  }
  raster.resize(size, scale);
  static_cast<void>(raster.synchronize_images((*entry.model).image_resources()));
  const std::chrono::steady_clock::time_point started = std::chrono::steady_clock::now();
  raster.begin_frame(screen);
  compose_live_frames();
  std::optional<PaintReceipt> receipt{};
  if (!damage.empty()) {
    receipt = (*entry.model).paint(raster, damage.rectangles());
    const std::span<const Rect> painted = damage.rectangles();
    for (std::size_t index = 0; index < painted.size(); ++index) screen.add(painted[index]);
  }
  raster.end_frame();
  live_frames.clear();
  if (!damage.empty() && !receipt) return;
  PresentedArea copied;
  if (!present(screen.rectangles(), copied)) return;
  const std::uint64_t elapsed = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now() - started).count());
  if (regions_taken) live_region_lost = false;
  const bool acknowledged = report_presented(receipt, elapsed, copied);
  if (!receipt) return;
  const PaintLeaseSnapshot lease = (*entry.model).paint_lease_snapshot();
  const bool shown = acknowledged || (lease.surface_epoch == (*receipt).surface_epoch &&
                                      lease.presented_revision == (*receipt).rendered_revision);
  if (shown) damage.clear();
}

bool NativeWindow::gather_live_frames() {
  live_updates.clear();
  live_frames.clear();
  live_frame_sources.clear();
  live_regions.clear();
  live_frame_pixels.clear();
  live_frame_rectangles.clear();
  if (!(*entry.model).has_live_surface_presentations()) return false;
  live_updates = (*entry.model).take_live_surface_presentations();
  const bool regions_usable = !live_region_lost;
  bool regions_taken = false;
  std::vector<Rect> changed_regions;
  changed_regions.reserve(live_updates.size());
  for (std::size_t index = 0; index < live_updates.size(); ++index) {
    const LiveSurfacePresentation& update = live_updates[index];
    std::size_t source = live_frames.size();
    for (std::size_t earlier = 0; earlier < index; ++earlier) {
      if (live_updates[earlier].surface == update.surface) {
        source = live_frame_sources[earlier];
        break;
      }
    }
    if (source == live_frames.size()) {
      LiveSurfaceFrame taken{};
      if (update.surface) taken = (*update.surface).acquire_for_presentation(this);
      if (taken) regions_taken = true;
      Rect changed = update.clip;
      if (!taken) changed = {};
      else if (update.damage_limited && regions_usable)
        changed = gui_forms::detail::live_damage_in_window(taken.damage(), update.destination,
                                                           taken.width(), taken.height(), scale);
      live_frames.push_back(std::move(taken));
      changed_regions.push_back(changed);
      live_frame_pixels.push_back(0U);
      live_frame_rectangles.push_back(0U);
    }
    live_frame_sources.push_back(source);
    live_regions.push_back(Rect{});
    const Rect changed = changed_regions[source];
    const Rect region = update.damage_limited && regions_usable
        ? Rect::intersection(update.clip, changed) : (changed.empty() ? Rect{} : update.clip);
    if (region.empty()) continue;
    if (live_frames[source].opaque()) live_regions[index] = region;
    else damage.add(region);
    const Rect device = gui_forms::detail::align_damage_outward(region, scale);
    live_frame_pixels[source] += static_cast<std::uint64_t>(device.width * scale) *
                                 static_cast<std::uint64_t>(device.height * scale);
    ++live_frame_rectangles[source];
  }
  return regions_taken;
}

void NativeWindow::compose_live_frames() {
  for (std::size_t index = 0; index < live_regions.size(); ++index) {
    const Rect region = live_regions[index];
    if (region.empty()) continue;
    raster.save();
    raster.clip_rect(region);
    const bool drawn = raster.draw_live_surface_frame(
        live_frames[live_frame_sources[index]], live_updates[index].destination, scale);
    raster.restore();
    if (drawn) continue;
    live_regions[index] = {};
    damage.add(region);
  }
}

bool NativeWindow::report_presented(const std::optional<PaintReceipt>& receipt,
                                    const std::uint64_t elapsed, const PresentedArea copied) {
  std::uint64_t live_pixels = 0U;
  std::uint64_t live_frames_copied = 0U;
  for (std::size_t index = 0; index < live_frame_rectangles.size(); ++index) {
    if (live_frame_rectangles[index] == 0U) continue;
    live_pixels += live_frame_pixels[index];
    ++live_frames_copied;
  }
  if (live_pixels > copied.device_pixels) live_pixels = copied.device_pixels;
  bool acknowledged = false;
  std::uint64_t live_elapsed = elapsed;
  std::uint64_t unreported_copy = copied.copy_nanoseconds;
  if (receipt) {
    PresentedArea retained;
    retained.device_pixels = copied.device_pixels - live_pixels;
    retained.rectangles = copied.rectangles;
    retained.copy_nanoseconds = copied.copy_nanoseconds;
    unreported_copy = 0U;
    live_elapsed = copied.device_pixels == 0U ? 0U :
        static_cast<std::uint64_t>(static_cast<double>(elapsed) *
            static_cast<double>(live_pixels) / static_cast<double>(copied.device_pixels));
    acknowledged = (*entry.model).notify_presented(*receipt, elapsed - live_elapsed, retained);
  }
  const std::uint64_t live_duration = live_frames_copied == 0U ? 0U : live_elapsed / live_frames_copied;
  for (std::size_t index = 0; index < live_frame_rectangles.size(); ++index) {
    if (live_frame_rectangles[index] == 0U) continue;
    PresentedArea live;
    live.device_pixels = live_frame_pixels[index];
    live.rectangles = live_frame_rectangles[index];
    live.copy_nanoseconds = unreported_copy;
    unreported_copy = 0U;
    (*entry.model).notify_live_presented(live_duration, live);
  }
  live_frame_rectangles.clear();
  return acknowledged;
}

void Runtime::step() {
  const std::vector<std::shared_ptr<NativeWindow>> snapshot = windows;
  int wait = -1;
  for (const std::shared_ptr<NativeWindow>& entry : snapshot) {
    NativeWindow& window = *entry;
    window.update();
    if (window.closed) continue;
    // The live clock: a registered live surface is sampled every display frame.
    if ((*window.entry.model).has_live_surface_presentations()) wait = 0;
    if (!window.damage.empty()) wait = 0;
    const std::optional<FrameTime> deadline = (*window.entry.model).next_wake();
    if (deadline) {
      const long long delay = std::chrono::duration_cast<std::chrono::milliseconds>(
          *deadline - std::chrono::steady_clock::now()).count();
      const int milliseconds = static_cast<int>(std::clamp(
          delay, 0LL, static_cast<long long>(std::numeric_limits<int>::max())));
      wait = wait < 0 ? milliseconds : std::min(wait, milliseconds);
    }
  }
  if (wait >= 0) gf_web_js_schedule(wait);
}
} // namespace gui_forms::host::web_detail

using gui_forms::host::web_detail::primary;
namespace gf = gui_forms;

extern "C" {
EMSCRIPTEN_KEEPALIVE void gf_web_tick() {
  if (gf::host::web_detail::runtime != nullptr) (*gf::host::web_detail::runtime).step();
}

// kind: 0 move, 1 down, 2 up, 3 enter, 4 leave, 5 wheel, 6 cancel.
// modifiers: shift 1, control 2, alt 4, meta 8 (gui_forms::Modifier bits).
// button: 0 primary, 1 middle, 2 secondary (DOM numbering).
EMSCRIPTEN_KEEPALIVE void gf_web_pointer(int kind, double x, double y, int button, int modifiers,
                                         double wheel_x, double wheel_y, int pointer_id, int touch) {
  gf::host::web_detail::NativeWindow* window = primary();
  if (window == nullptr) return;
  gf::PointerEvent event;
  event.pointer_id = static_cast<std::uint64_t>(pointer_id);
  event.position = {x, y};
  event.modifiers = static_cast<gf::Modifier>(modifiers);
  static_cast<void>(touch);
  switch (kind) {
  case 1: event.action = gf::PointerAction::down; break;
  case 2: event.action = gf::PointerAction::up; break;
  case 3: event.action = gf::PointerAction::enter; break;
  case 4: event.action = gf::PointerAction::leave; break;
  case 5:
    event.action = gf::PointerAction::wheel;
    event.wheel_delta = {wheel_x, wheel_y};
    break;
  // A cancelled touch ends its press where it was.
  case 6: event.action = gf::PointerAction::up; break;
  default: event.action = gf::PointerAction::move; break;
  }
  if (kind == 1 || kind == 2 || kind == 6)
    event.button = button == 2 ? gf::PointerButton::secondary
                 : button == 1 ? gf::PointerButton::middle
                               : gf::PointerButton::primary;
  (*window).pointer(std::move(event));
}

// Returns 1 when the window handled the key, so the page can stop the
// browser's own action.
EMSCRIPTEN_KEEPALIVE int gf_web_key(int down, int usage, int repeat, int modifiers) {
  gf::host::web_detail::NativeWindow* window = primary();
  if (window == nullptr) return 0;
  gf::KeyEvent key;
  key.action = down != 0 ? gf::KeyAction::down : gf::KeyAction::up;
  key.physical_key = static_cast<std::uint32_t>(usage);
  key.repeat = repeat != 0;
  key.modifiers = static_cast<gf::Modifier>(modifiers);
  return (*window).dispatch(key).handled ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE void gf_web_text(const char* utf8) {
  gf::host::web_detail::NativeWindow* window = primary();
  if (window == nullptr || utf8 == nullptr || utf8[0] == '\0') return;
  gf::TextInputEvent text;
  text.text_utf8 = utf8;
  static_cast<void>((*window).dispatch(std::move(text)));
}

EMSCRIPTEN_KEEPALIVE void gf_web_paste(const char* utf8) {
  gf::host::web_detail::NativeWindow* window = primary();
  if (window == nullptr || utf8 == nullptr) return;
  (*(*window).services).clipboard = utf8;
  ++(*(*window).services).clipboard_generation;
}

EMSCRIPTEN_KEEPALIVE void gf_web_resize(double width, double height, double scale) {
  gf::host::web_detail::NativeWindow* window = primary();
  if (window == nullptr || !(width > 0.0) || !(height > 0.0)) return;
  if (scale > 0.0 && scale != (*window).scale) {
    (*window).scale = scale;
    static_cast<void>((*window).dispatch(gf::HostScaleEvent{scale}));
  }
  const gf::Size resized{width, height};
  if (resized != (*window).size) {
    (*window).size = resized;
    static_cast<void>((*window).dispatch(gf::HostResizeEvent{resized}));
  }
  (*window).damage.add({0, 0, width, height});
}

EMSCRIPTEN_KEEPALIVE void gf_web_focus(int active) {
  gf::host::web_detail::NativeWindow* window = primary();
  if (window != nullptr) static_cast<void>((*window).dispatch(gf::HostActivationEvent{active != 0}));
}

EMSCRIPTEN_KEEPALIVE void gf_web_occluded(int occluded) {
  gf::host::web_detail::NativeWindow* window = primary();
  if (window == nullptr) return;
  static_cast<void>((*window).dispatch(gf::HostOcclusionEvent{occluded != 0}));
  if (occluded == 0) (*window).damage.add({0, 0, (*window).size.width, (*window).size.height});
}

EMSCRIPTEN_KEEPALIVE void gf_web_close() {
  gf::host::web_detail::NativeWindow* window = primary();
  if (window != nullptr) (*window).close(gf::HostCloseReason::user);
}
}

namespace gui_forms::host {
HostCapabilities web_capabilities() {
  return {HostCapabilities::current_protocol_version, "web-canvas",
          HostCapability::lifecycle | HostCapability::monitor_geometry |
              HostCapability::occlusion | HostCapability::scheduled_wake |
              HostCapability::pointer_input | HostCapability::keyboard_input |
              HostCapability::pointer_capture | HostCapability::cursor |
              HostCapability::clipboard};
}

int run_web_application(std::vector<WebApplicationWindow> windows) {
  try {
    web_detail::runtime = new web_detail::Runtime();
    for (WebApplicationWindow& entry : windows) {
      if (entry.tool_window || !entry.owner_id.empty()) continue;
      const std::shared_ptr<web_detail::NativeWindow> window =
          std::make_shared<web_detail::NativeWindow>(std::move(entry));
      (*web_detail::runtime).windows.push_back(window);
      (*window).attach();
      break;
    }
    if ((*web_detail::runtime).windows.empty()) return 1;
    for (const std::shared_ptr<web_detail::NativeWindow>& window : (*web_detail::runtime).windows)
      (*window).ready();
    gf_web_js_schedule(0);
  } catch (const std::exception& error) {
    std::cerr << "GUI.Forms web host: " << error.what() << '\n';
    return 1;
  }
  // The windows now belong to the page. Callers' stack frames are abandoned
  // without running their destructors, as emscripten_set_main_loop does.
  emscripten_unwind_to_js_event_loop();
}
} // namespace gui_forms::host
