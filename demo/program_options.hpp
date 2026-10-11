#pragma once

// Shared command-line contract for the GUI.Forms demonstration programs
// (Gallery, Correspondence and the labs) on every host:
//
//   --automation        Windows: enable the WM_COPYDATA automation channel.
//                       Other hosts accept and ignore the flag so one CI
//                       command line works everywhere.
//   --capture <path>    After the first presented frame that covers the whole
//                       window, write the framebuffer as PNG to <path>, print
//                       {"automation":"capture","saved":true|false,"path":"..."}
//                       to stdout and close. Exit code 0 only when saved.
//   --size <W>x<H>      Create the window at this logical size.
//   --scale <factor>    Recorded as a hint only; hosts take the device scale
//                       from the display.
//
// Header-only so every demo executable can include it without a new library.
// It is demo-facing code: no host, renderer or platform type appears here.

#include "gui_forms/types.hpp"

#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>

namespace gui_forms::demo {

struct ProgramOptions final {
    bool automation{};
    std::string capture_path;
    std::optional<Size> capture_size;
    double scale_hint{};
};

inline void print_program_usage(const char* program) {
    std::fprintf(stderr,
                 "usage: %s [--automation] [--capture <path.png>] [--size <W>x<H>] "
                 "[--scale <factor>]\n",
                 program != nullptr && *program != '\0' ? program : "gui-forms-demo");
}

namespace detail {

inline bool parse_positive_double(std::string_view text, double& value) {
    if (text.empty()) return false;
    const std::string buffer(text);
    char* end = nullptr;
    errno = 0;
    const double parsed = std::strtod(buffer.c_str(), &end);
    if (errno != 0 || end == nullptr || *end != '\0' || !std::isfinite(parsed) || parsed <= 0.0) {
        return false;
    }
    value = parsed;
    return true;
}

// Accepts "<W>x<H>" with positive finite numbers, e.g. 1280x800 or 1280.5x800.
inline bool parse_size(std::string_view text, Size& size) {
    const std::size_t separator = text.find_first_of("xX");
    if (separator == std::string_view::npos) return false;
    double width{};
    double height{};
    if (!parse_positive_double(text.substr(0, separator), width) ||
        !parse_positive_double(text.substr(separator + 1U), height)) {
        return false;
    }
    size = {width, height};
    return true;
}

} // namespace detail

// Returns nullopt after printing usage to stderr when an argument is unknown,
// a value is missing, or a value is malformed.
inline std::optional<ProgramOptions> parse_program_options(int argc, char** argv) {
    ProgramOptions options;
    const char* program = argc > 0 && argv != nullptr ? argv[0] : "";
    for (int index = 1; index < argc; ++index) {
        if (argv[index] == nullptr) {
            print_program_usage(program);
            return std::nullopt;
        }
        const std::string_view argument(argv[index]);
        if (argument == "--automation") {
            options.automation = true;
            continue;
        }
        const bool wants_value = argument == "--capture" || argument == "--size" ||
                                 argument == "--scale";
        if (!wants_value) {
            print_program_usage(program);
            return std::nullopt;
        }
        if (index + 1 >= argc || argv[index + 1] == nullptr) {
            print_program_usage(program);
            return std::nullopt;
        }
        const std::string_view value(argv[++index]);
        if (argument == "--capture") {
            if (value.empty()) {
                print_program_usage(program);
                return std::nullopt;
            }
            options.capture_path = std::string(value);
        } else if (argument == "--size") {
            Size size{};
            if (!detail::parse_size(value, size)) {
                print_program_usage(program);
                return std::nullopt;
            }
            options.capture_size = size;
        } else {
            double scale{};
            if (!detail::parse_positive_double(value, scale)) {
                print_program_usage(program);
                return std::nullopt;
            }
            options.scale_hint = scale;
        }
    }
    return options;
}

// Copies the portable capture fields into any host option struct that carries
// them (MacHostOptions, WindowsHostOptions, LinuxHostOptions). Host-specific
// fields such as automation_enabled are set by the main that owns them.
template <class HostOptions>
inline void apply_program_options(const ProgramOptions& program, HostOptions& host) {
    host.capture_path = program.capture_path;
    host.capture_size = program.capture_size;
}

} // namespace gui_forms::demo
