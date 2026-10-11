#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>

namespace gui_forms::host {
void run_windows_tooltip_fixture(const std::filesystem::path& fonts);
}

int main(const int argc, char** const argv) {
    if (argc != 2) {
        std::fputs("Pass fixture font directory\n", stderr);
        return EXIT_FAILURE;
    }
    try {
        gui_forms::host::run_windows_tooltip_fixture(std::filesystem::path(argv[1]));
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return EXIT_FAILURE;
    }
}
