#include "macos_host.hpp"
#include "gallery.hpp"
#include "program_options.hpp"

#include <optional>
#include <string>
#include <utility>

int main(int argc, char** argv) {
    const std::optional<gui_forms::demo::ProgramOptions> program =
        gui_forms::demo::parse_program_options(argc, argv);
    if (!program) return 2;
    gui_forms::host::MacHostOptions options;
    options.title = std::string(gui_forms::gallery::gallery_program_name);
    options.initial_size = gui_forms::gallery::gallery_default_size;
    options.minimum_size = gui_forms::gallery::gallery_minimum_size;
    gui_forms::demo::apply_program_options(*program, options);
    return gui_forms::host::run_macos(gui_forms::gallery::make_gallery(),
                                      std::move(options));
}
