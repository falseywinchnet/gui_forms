#pragma once

#include "gui_forms/events/key_commands/key_commands.hpp"
#include "gui_forms/host/types/host_capability/host_capability.hpp"

#include <cstdint>
#include <string>

namespace gui_forms {

struct HostCapabilities final {
    // 8: the host's keyboard-command defaults (`key_map`).
    static constexpr std::uint32_t current_protocol_version = 8;

    std::uint32_t protocol_version{current_protocol_version};
    std::string platform{"unknown"};
    HostCapability available{HostCapability::none};
    // The keys this host's convention gives the named keyboard commands
    // (owner decision DK-30, interview G5-1): host data, not core policy.
    // The first HostSession of an application seeds the application's
    // KeyBindings with it (owner decision DK-44); the application's
    // rebindings stay over it.
    KeyMap key_map{pc_key_map()};

    [[nodiscard]] bool supports(HostCapability capability) const noexcept {
        return has_capability(available, capability);
    }
    [[nodiscard]] std::string to_json() const;
};

} // namespace gui_forms
