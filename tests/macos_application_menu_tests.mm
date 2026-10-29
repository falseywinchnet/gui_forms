#import <AppKit/AppKit.h>
#include "application_menu.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h>

// The library installs the application menu for every program, so its Quit
// item must name the running program: a bundle's own display or bundle name,
// and an unbundled executable's process name, never the Gallery's.
namespace {

namespace detail = gui_forms::host::detail;

int failures = 0;

void expect_equal(std::string_view what, const std::string& actual, std::string_view expected) {
    if (actual == expected) { return; }
    std::cerr << what << ": expected \"" << expected << "\", got \"" << actual << "\"\n";
    ++failures;
}

// A minimal bundle directory whose Info.plist holds `info`.
std::filesystem::path make_bundle(const std::filesystem::path& root, std::string_view name, NSDictionary* info) {
    const std::filesystem::path bundle = root / std::string(name);
    std::filesystem::create_directories(bundle / "Contents");
    const std::filesystem::path plist = bundle / "Contents" / "Info.plist";
    if (![info writeToURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:plist.c_str()]] error:nil]) {
        std::cerr << "could not write " << plist << '\n';
        std::exit(1);
    }
    return bundle;
}

void check_names(const std::filesystem::path& root, const std::string& process_name) {
    const std::filesystem::path named = make_bundle(root, "Named.app",
        @{@"CFBundleIdentifier" : @"local.guiforms.menu-probe.named", @"CFBundleName" : @"Menu Probe"});
    expect_equal("bundle name", detail::application_menu_name(named), "Menu Probe");

    const std::filesystem::path displayed = make_bundle(root, "Displayed.app",
        @{@"CFBundleIdentifier" : @"local.guiforms.menu-probe.displayed",
          @"CFBundleName" : @"Menu Probe",
          @"CFBundleDisplayName" : @"Menu Probe Display"});
    expect_equal("display name before bundle name", detail::application_menu_name(displayed), "Menu Probe Display");

    const std::filesystem::path unnamed = make_bundle(root, "Unnamed.app",
        @{@"CFBundleIdentifier" : @"local.guiforms.menu-probe.unnamed", @"CFBundleName" : @"", @"CFBundleDisplayName" : @7});
    expect_equal("empty and non-string names fall back to the process", detail::application_menu_name(unnamed),
                 process_name);

    expect_equal("quit title", detail::quit_menu_title("Menu Probe"), "Quit Menu Probe");
}

void check_installed_menu(const std::string& process_name) {
    [NSApplication sharedApplication];
    detail::install_application_menu();
    NSMenu* application_menu = [[[NSApp mainMenu] itemAtIndex:0] submenu];
    NSMenuItem* quit = [application_menu itemWithTitle:[NSString stringWithUTF8String:("Quit " + process_name).c_str()]];
    if (quit == nil) {
        std::cerr << "installed menu has no \"Quit " << process_name << "\" item; first item is \""
                  << [[[application_menu itemAtIndex:0] title] UTF8String] << "\"\n";
        ++failures;
        return;
    }
    expect_equal("quit key", [[quit keyEquivalent] UTF8String], "q");
    if ([quit action] != @selector(terminate:)) {
        std::cerr << "quit item does not terminate\n";
        ++failures;
    }
}

} // namespace

int main() {
    @autoreleasepool {
        // This test is an unbundled executable, so it is named by its process.
        const std::string process_name = [[[NSProcessInfo processInfo] processName] UTF8String];
        expect_equal("unbundled program", detail::application_menu_name(), process_name);
        if (process_name.find("Gallery") != std::string::npos) {
            std::cerr << "test process unexpectedly named like the Gallery\n";
            ++failures;
        }

        char pattern[] = "/tmp/gui_forms_application_menu.XXXXXX";
        if (mkdtemp(pattern) == nullptr) {
            std::cerr << "mkdtemp failed\n";
            return 1;
        }
        const std::filesystem::path root{pattern};
        check_names(root, process_name);
        std::filesystem::remove_all(root);

        check_installed_menu(process_name);
    }
    if (failures != 0) { return 1; }
    std::cout << "macos application menu: ok\n";
    return 0;
}
