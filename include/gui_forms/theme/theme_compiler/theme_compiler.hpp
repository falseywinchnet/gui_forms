#pragma once

// Theme packages compiled into themes (stage T3, PLAN.md P10): the one
// public seam of the theme language. A package is a `theme.json` manifest
// and the files it lists, given as bytes; nothing is read from a file
// system or a network. Compiling a package gives a Theme, or a refusal with
// its diagnostics; the caller applies the theme through the atomic
// Window::set_theme or ApplicationResources::set_theme, so a refused theme
// never half-applies and the running one stays (B2, RC003).

#include "gui_forms/anatomy/types/anatomy_types.hpp"
#include "gui_forms/resources/image_registry/image_registry.hpp"
#include "gui_forms/theme/theme/theme.hpp"

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace gui_forms {

// One file of a theme package: its path inside the package, as the
// manifest lists it, and its bytes.
struct ThemePackageFile final {
    std::string path{};
    std::string bytes{};
};

// A theme package: its manifest's bytes and its files.
struct ThemePackage final {
    std::string manifest{};
    std::vector<ThemePackageFile> files{};
};

enum class ThemeDiagnosticSeverity : std::uint8_t {
    // The theme is refused.
    error,
    // One item is dropped; the rest applies.
    warning,
};

// One finding of an install or a compilation.
struct ThemeCompileDiagnostic final {
    // As `E601` or `W106`.
    std::string code{};
    // As `manifest_invalid`.
    std::string name{};
    ThemeDiagnosticSeverity severity{ThemeDiagnosticSeverity::error};
    // The file at fault: `theme.json`, or a sheet as `<theme>/<path>`.
    std::string file{};
    // Counting from 1; 0 where the finding has no place in a file.
    std::uint32_t line{};
    std::uint32_t column{};
    // The rule and property, where the finding is inside one.
    std::string rule{};
    std::string property{};
    std::string detail{};
};

// The installed theme packages an `@import` may name (D2): each verified
// whole when installed (its manifest, every file's hash, and every
// `@import` of its sheet listed in its manifest), and found by its name.
class ThemeLibrary final {
public:
    ThemeLibrary();
    ~ThemeLibrary();
    ThemeLibrary(ThemeLibrary&&) noexcept;
    ThemeLibrary& operator=(ThemeLibrary&&) noexcept;
    ThemeLibrary(const ThemeLibrary&) = delete;
    ThemeLibrary& operator=(const ThemeLibrary&) = delete;

    // Verifies `package` and installs it, replacing an installed package of
    // its name. Returns the refusal's diagnostics, or none when installed;
    // a refused package changes nothing. A library holds at most 256
    // packages.
    [[nodiscard]] std::vector<ThemeCompileDiagnostic> install(const ThemePackage& package);
    // Removes the package named `name`; whether there was one.
    bool remove(std::string_view name) noexcept;
    [[nodiscard]] bool contains(std::string_view name) const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;

private:
    friend struct ThemeLibraryAccess;
    struct State;
    std::unique_ptr<State> state_;
};

// What a compilation reads besides the package.
struct ThemeCompileOptions final {
    // The control types a selector may name (the stock ones and the
    // application's own); a name outside them is a warning that drops its
    // rule.
    std::span<const ControlAnatomy> controls{};
    // The application values the application publishes; a package listing
    // another is refused (E606).
    std::span<const std::string> published_values{};
};

// What setting a control's inline style gave (Control::set_inline_style):
// applied, with any warnings (none for empty text, which clears the
// style), or refused with its diagnostics, the control keeping its
// previous inline style.
class InlineStyleResult final {
public:
    [[nodiscard]] static InlineStyleResult applied(std::vector<ThemeCompileDiagnostic> warnings) noexcept;
    [[nodiscard]] static InlineStyleResult refused(std::vector<ThemeCompileDiagnostic> diagnostics) noexcept;

    [[nodiscard]] bool succeeded() const noexcept;
    // The warnings of a style applied, or the diagnostics of one refused.
    [[nodiscard]] std::span<const ThemeCompileDiagnostic> diagnostics() const noexcept;

private:
    struct Applied final {
        std::vector<ThemeCompileDiagnostic> warnings{};
    };
    struct Refused final {
        std::vector<ThemeCompileDiagnostic> diagnostics{};
    };
    explicit InlineStyleResult(std::variant<Applied, Refused> state) noexcept;

    std::variant<Applied, Refused> state_;
};

// A theme, or the refusal's diagnostics. Warnings come with a theme too.
struct ThemeCompileResult final {
    std::shared_ptr<const Theme> theme{};
    std::vector<ThemeCompileDiagnostic> diagnostics{};

    [[nodiscard]] explicit operator bool() const noexcept { return theme != nullptr; }
};

// Compiles `package` with its `@import` closure among `library`'s packages
// (one cascade: imported sheets first, and the later definition of a
// colour role or `@keyframes` wins) onto the built-in professional theme.
// Its image resources are loaded into `images` and owned by the compiled
// theme: each is removed from `images` when the last theme holding it is
// released, so replacing or dropping a theme removes its images. Themes
// compiled through one library that load the same bytes into one
// registry share one image. A refusal leaves none of its own. The package
// itself need not be installed. Compiles through one library, and the
// release of their themes, are not safe to run concurrently.
[[nodiscard]] ThemeCompileResult compile_theme_package(const ThemePackage& package, const ThemeLibrary& library,
                                                       ImageRegistry& images, const ThemeCompileOptions& options);

// As compile_theme_package, for the installed package named `name`; an
// unknown name is refused (E603).
[[nodiscard]] ThemeCompileResult compile_installed_theme(std::string_view name, const ThemeLibrary& library,
                                                         ImageRegistry& images, const ThemeCompileOptions& options);

} // namespace gui_forms
