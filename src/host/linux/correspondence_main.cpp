#include "correspondence.hpp"
#include "gui_forms/platform/linux_host.hpp"
#include "program_options.hpp"

#include <optional>
#include <utility>

int main(int argc, char** argv) {
    const std::optional<gui_forms::demo::ProgramOptions> program =
        gui_forms::demo::parse_program_options(argc, argv);
    if (!program) return 2;
    gui_forms::host::LinuxHostOptions options;
    options.title = "GUI.Forms Correspondence";
    options.initial_size = {1120.0, 720.0};
    options.minimum_size = {600.0, 420.0};
    gui_forms::demo::apply_program_options(*program, options);
    // --automation is accepted for a uniform CI command line; the X11 host has
    // no WM_COPYDATA-style automation channel, so the flag has no effect here.
    return gui_forms::host::run_linux(
        gui_forms::correspondence::make_correspondence(), std::move(options));
}
