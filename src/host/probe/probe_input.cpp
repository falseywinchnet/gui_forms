#include "probe/probe_input.hpp"

namespace gui_forms::probe {

const char* input_path_name(InputPath path) noexcept {
    switch (path) {
    case InputPath::os_injection: return "os_injection";
    case InputPath::in_process: return "in_process";
    }
    return "unknown";
}

} // namespace gui_forms::probe
