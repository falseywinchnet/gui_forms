#include "headless_capabilities.hpp"

#include <sstream>

namespace gui_forms::host {

HostCapabilities headless_capabilities() {
    return {HostCapabilities::current_protocol_version,
            "headless-reference",
            HostCapability::lifecycle |
                HostCapability::scale_notifications |
                HostCapability::occlusion |
                HostCapability::scheduled_wake |
                HostCapability::pointer_input |
                HostCapability::keyboard_input |
                HostCapability::text_composition |
                HostCapability::monitor_geometry |
                HostCapability::pointer_capture |
                HostCapability::cursor |
                HostCapability::clipboard |
                HostCapability::clipboard_images |
                HostCapability::typed_drag_destination |
                HostCapability::dialogs |
                HostCapability::sound_cues |
                HostCapability::notification_area |
                HostCapability::notification_area_messages |
                HostCapability::notification_area_menus,
            // The reference host reports the PC convention.
            pc_key_map()};
}
} // namespace gui_forms::host
