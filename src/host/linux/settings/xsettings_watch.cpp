#include "xsettings_watch.hpp"

#include "../services/x11_foreign_window.hpp"
#include "xsettings_parse.hpp"

#include <X11/Xatom.h>

#include <cstddef>
#include <span>
#include <string>

namespace gui_forms::host::linux_detail {
namespace {

// The property is read whole up to this many 32-bit units (256 KiB); a
// larger one is not used.
constexpr long maximum_property_units = 65536L;

} // namespace

XSettingsWatch::XSettingsWatch(Display *display, int screen)
    : display_(display), root_(RootWindow(display, screen)) {
  const std::string name = "_XSETTINGS_S" + std::to_string(screen);
  selection_ = XInternAtom(display_, name.c_str(), False);
  settings_ = XInternAtom(display_, "_XSETTINGS_SETTINGS", False);
  manager_ = XInternAtom(display_, "MANAGER", False);
  // A new manager announces itself to StructureNotify on the root; the
  // root's other selections, made by this client, are kept.
  XWindowAttributes attributes{};
  long mask = StructureNotifyMask;
  if (XGetWindowAttributes(display_, root_, &attributes) != 0)
    mask |= attributes.your_event_mask;
  XSelectInput(display_, root_, mask);
  acquire_manager();
}

bool XSettingsWatch::settings_event(const XEvent &event) {
  if (event.type == ClientMessage && event.xclient.window == root_ &&
      event.xclient.message_type == manager_ && event.xclient.format == 32 &&
      static_cast<Atom>(event.xclient.data.l[1]) == selection_) {
    acquire_manager();
    return true;
  }
  if (owner_ == None)
    return false;
  if (event.type == PropertyNotify && event.xproperty.window == owner_ &&
      event.xproperty.atom == settings_) {
    read_settings();
    return true;
  }
  if (event.type == DestroyNotify && event.xdestroywindow.window == owner_) {
    owner_ = None;
    // A replacement may already own the selection; else the default.
    acquire_manager();
    return true;
  }
  return false;
}

void XSettingsWatch::acquire_manager() {
  // Grabbed, so that the owner cannot vanish between finding it and
  // selecting its events (XSETTINGS specification, "Reading settings").
  XGrabServer(display_);
  const ::Window owner = XGetSelectionOwner(display_, selection_);
  if (owner != None)
    XSelectInput(display_, owner, StructureNotifyMask | PropertyChangeMask);
  XUngrabServer(display_);
  XFlush(display_);
  owner_ = owner;
  read_settings();
}

void XSettingsWatch::read_settings() {
  double_click_milliseconds_ = default_double_click_milliseconds;
  double_click_origin_ = SettingOrigin::default_value;
  if (owner_ == None)
    return;
  Atom type = None;
  int format = 0;
  unsigned long items = 0UL;
  unsigned long after = 0UL;
  unsigned char *data = nullptr;
  int status = BadWindow;
  {
    // The manager may exit at any moment: its window is foreign.
    const ForeignWindowScope foreign(display_, owner_);
    status = XGetWindowProperty(display_, owner_, settings_, 0L, maximum_property_units, False,
                                settings_, &type, &format, &items, &after, &data);
  }
  const bool usable = status == Success && data != nullptr && type == settings_ &&
                      format == 8 && after == 0UL;
  XSettingsInteger found{};
  if (usable) {
    const std::span<const std::uint8_t> bytes(reinterpret_cast<const std::uint8_t *>(data),
                                              static_cast<std::size_t>(items));
    found = find_xsettings_integer(bytes, "Net/DoubleClickTime");
  }
  if (data != nullptr)
    XFree(data);
  const bool in_range =
      found.lookup == XSettingsLookup::found && found.value > 0 &&
      static_cast<std::uint32_t>(found.value) <= maximum_double_click_milliseconds;
  if (!in_range)
    return;
  double_click_milliseconds_ = static_cast<std::uint32_t>(found.value);
  double_click_origin_ = SettingOrigin::host;
}

} // namespace gui_forms::host::linux_detail
