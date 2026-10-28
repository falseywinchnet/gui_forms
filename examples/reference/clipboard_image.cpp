#include <gui_forms/host/services/host_services.hpp>

#include <cstddef>
#include <utility>

// Call on the thread that owns services. The application's document stays
// straight RGBA; no display bitmap participates in copying or pasting.
gui_forms::HostServiceStatus copy_pixels(gui_forms::HostServices& services,
                                        gui_forms::HostImageView selection) {
    return services.write_clipboard_image(selection);
}

gui_forms::HostServiceStatus paste_pixels(gui_forms::HostServices& services,
                                         gui_forms::HostImage& document,
                                         bool& changed) {
    changed = false;
    gui_forms::HostClipboardImageResult result = services.read_clipboard_image();
    const gui_forms::HostServiceStatus status = result.status();
    if (!status.accepted() || result.image() == nullptr) return status;
    document = *std::move(result).release_image();
    changed = true;
    return status;
}
