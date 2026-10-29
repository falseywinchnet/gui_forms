// Theme packages, the installed index and `@import` closure (stage T3,
// phase P10): a package is verified whole (E601, E602, E603 against its
// manifest), and an `@import` resolves only among installed packs pinned by
// their manifests' hashes (E603, E604, E605, E506). Gate: a fixture for
// each code, the closure's cascade order, and each pack once.

#include "../../src/core/theme_language/package/theme_package.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using namespace gui_forms;
using namespace gui_forms::theme_language;

int failures = 0;

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

struct Pin final {
    std::string name{};
    std::string hash{};
};

std::string hex_of(std::string_view bytes) {
    return package_sha256_hex(package_sha256(bytes));
}

// A package named `name` of one sheet, its imports pinned by `pins`, with
// one resource where `resource` is not empty.
ThemePackageBytes package(std::string_view name, std::string_view sheet, const std::vector<Pin>& pins,
                          std::string_view resource = {}) {
    ThemePackageBytes bytes{};
    bytes.manifest = "{\"format\": \"gui-forms-theme/1\", \"name\": \"" + std::string(name) +
                     "\", \"version\": \"1.0.0\", \"engine\": \">=0.3\", \"sheet\": {\"path\": \"theme.theme.css\", "
                     "\"sha256\": \"" + hex_of(sheet) + "\"}";
    if (!pins.empty()) {
        bytes.manifest += ", \"imports\": {";
        for (std::size_t index = 0U; index < pins.size(); ++index) {
            if (index != 0U) bytes.manifest += ", ";
            bytes.manifest += "\"" + pins[index].name + "\": \"" + pins[index].hash + "\"";
        }
        bytes.manifest += "}";
    }
    if (!resource.empty()) {
        bytes.manifest += ", \"resources\": [{\"name\": \"icon.check\", \"path\": \"images/check.png\", "
                          "\"sha256\": \"" + hex_of(resource) + "\"}]";
        bytes.files.push_back(ThemePackageEntry{"images/check.png", std::string(resource)});
    }
    bytes.manifest += "}";
    bytes.files.push_back(ThemePackageEntry{"theme.theme.css", std::string(sheet)});
    return bytes;
}

struct Verified final {
    ThemePackageOutcome outcome{ThemePackageOutcome::refused};
    VerifiedThemePack pack{};
    ThemeDiagnostic problem{};
};

Verified verify(const ThemePackageBytes& bytes) {
    Verified result{};
    result.outcome = verify_theme_package(bytes, current_engine_version, result.pack, result.problem);
    return result;
}

VerifiedThemePack verified(const ThemePackageBytes& bytes) {
    Verified result = verify(bytes);
    require(result.outcome == ThemePackageOutcome::verified,
            "verified: " + bytes.manifest + " (" + std::string(diagnostic_code_name(result.problem.code)) + " " +
                result.problem.detail + ")");
    return std::move(result.pack);
}

bool refused_with(const Verified& result, DiagnosticCode code, std::string_view file, std::string_view detail) {
    const bool same = result.outcome == ThemePackageOutcome::refused && result.problem.code == code &&
                      result.problem.file == file && result.problem.detail == detail;
    if (!same) {
        std::cerr << "  got " << diagnostic_code_name(result.problem.code) << " in " << result.problem.file << " ("
                  << result.problem.detail << ")\n";
    }
    return same;
}

void test_verify() {
    const ThemePackageBytes good =
        package("top", "@import \"base\";\n@import \"extra\";\n#a { opacity: 0.5 }", {{"base", hex_of("x")},
                                                                                     {"extra", hex_of("y")}},
                "PNG");
    const Verified read = verify(good);
    require(read.outcome == ThemePackageOutcome::verified, "a package is verified");
    require(read.pack.identity == package_sha256(good.manifest), "its identity is its manifest's hash");
    require(read.pack.sheet == good.files[1].bytes && read.pack.resources.size() == 1U &&
                read.pack.resources[0] == "PNG",
            "its sheet and resources");
    require(read.pack.imports.size() == 2U && read.pack.imports[0].name == "base" &&
                read.pack.imports[1].name == "extra" && read.pack.imports[1].position.line.value == 2U &&
                read.pack.imports[1].position.column.value == 9U,
            "its imports, in order, where they are written");

    ThemePackageBytes edited = good;
    edited.files[1].bytes += " ";
    require(refused_with(verify(edited), DiagnosticCode::file_hash_mismatch, "theme.theme.css", "theme.theme.css"),
            "an edited sheet is E602");
    ThemePackageBytes missing = good;
    missing.files.erase(missing.files.begin());
    require(refused_with(verify(missing), DiagnosticCode::file_hash_mismatch, "images/check.png",
                         "images/check.png"),
            "a missing resource is E602");
    ThemePackageBytes swapped = good;
    swapped.files[0].bytes = "GIF";
    require(refused_with(verify(swapped), DiagnosticCode::file_hash_mismatch, "images/check.png",
                         "images/check.png"),
            "a resource with other bytes is E602");
    ThemePackageBytes extra = good;
    extra.files.push_back(ThemePackageEntry{"notes.txt", "hello"});
    require(refused_with(verify(extra), DiagnosticCode::manifest_invalid, "notes.txt", "notes.txt"),
            "a file the manifest does not list is E601");
    ThemePackageBytes twice = good;
    twice.files.push_back(twice.files[1]);
    require(refused_with(verify(twice), DiagnosticCode::manifest_invalid, "theme.theme.css", "theme.theme.css"),
            "a file given twice is E601");

    const Verified unlisted = verify(package("top", "@import \"other\";", {{"base", hex_of("x")}}));
    require(refused_with(unlisted, DiagnosticCode::import_not_installed, "theme.theme.css", "other") &&
                unlisted.problem.position.line.value == 1U && unlisted.problem.position.column.value == 9U,
            "an @import the manifest does not list is E603, where it is written");
    const Verified late = verify(package("top", "#a { opacity: 1 }\n@import \"base\";", {{"base", hex_of("x")}}));
    require(late.outcome == ThemePackageOutcome::refused && late.problem.code == DiagnosticCode::import_after_rules &&
                late.problem.file == "theme.theme.css",
            "a sheet that does not parse is refused with its error");
    ThemePackageBytes broken = good;
    broken.manifest = "{\n\"format\": }";
    const Verified unread = verify(broken);
    require(refused_with(unread, DiagnosticCode::manifest_invalid, "theme.json", "syntax_error") &&
                unread.problem.position.line.value == 2U,
            "a manifest that is not JSON is E601 in theme.json, where it was found");
}

void test_index() {
    InstalledThemeIndex index{};
    require(index.install(verified(package("base", "#a { opacity: 1 }", {}))) && index.size() == 1U,
            "a pack is installed");
    const VerifiedThemePack* const first = index.find("base");
    require(first != nullptr && (*first).sheet == "#a { opacity: 1 }", "and found by name");
    require(index.install(verified(package("base", "#a { opacity: 0.5 }", {}))) && index.size() == 1U &&
                (*index.find("base")).sheet == "#a { opacity: 0.5 }",
            "a pack of the same name replaces it");
    require(index.find("other") == nullptr && !index.remove("other"), "an unknown name");
    require(index.remove("base") && index.size() == 0U && index.find("base") == nullptr, "a pack is removed");

    const VerifiedThemePack one = verified(package("p", "", {}));
    for (std::size_t count = 0U; count < maximum_installed_themes; ++count) {
        VerifiedThemePack copy = one;
        copy.manifest.name = "p" + std::to_string(count);
        require(index.install(std::move(copy)), "installed up to the limit");
    }
    VerifiedThemePack past = one;
    past.manifest.name = "past";
    require(!index.install(std::move(past)) && index.size() == maximum_installed_themes,
            "the index holds at most 256 packs");
    VerifiedThemePack again = one;
    again.manifest.name = "p0";
    require(index.install(std::move(again)), "a full index still replaces");
}

struct Resolved final {
    ThemeImportOutcome outcome{ThemeImportOutcome::refused};
    std::vector<std::string> names{};
    ThemeDiagnostic problem{};
};

Resolved resolve(const VerifiedThemePack& root, const InstalledThemeIndex& index) {
    Resolved result{};
    std::vector<const VerifiedThemePack*> order{};
    result.outcome = resolve_theme_imports(root, index, order, result.problem);
    for (const VerifiedThemePack* const pack : order) result.names.push_back((*pack).manifest.name);
    return result;
}

std::string identity_of(const ThemePackageBytes& bytes) {
    return hex_of(bytes.manifest);
}

void test_closure() {
    InstalledThemeIndex index{};
    const ThemePackageBytes base = package("base", "#a { opacity: 1 }", {});
    const ThemePackageBytes left = package("left", "@import \"base\";", {{"base", identity_of(base)}});
    const ThemePackageBytes right = package("right", "@import \"base\";", {{"base", identity_of(base)}});
    require(index.install(verified(base)) && index.install(verified(left)) && index.install(verified(right)),
            "the closure's packs are installed");
    const VerifiedThemePack top = verified(package("top", "@import \"left\"; @import \"right\";",
                                                   {{"left", identity_of(left)}, {"right", identity_of(right)}}));
    const Resolved diamond = resolve(top, index);
    require(diamond.outcome == ThemeImportOutcome::resolved &&
                diamond.names == std::vector<std::string>({"base", "left", "right", "top"}),
            "imported packs come first, each once, the root last");
    const Resolved alone = resolve(verified(package("solo", "#a { opacity: 1 }", {})), index);
    require(alone.outcome == ThemeImportOutcome::resolved && alone.names == std::vector<std::string>({"solo"}),
            "a pack without imports is its own closure");

    const Resolved absent =
        resolve(verified(package("top", "\n@import \"gone\";", {{"gone", identity_of(base)}})), index);
    require(absent.outcome == ThemeImportOutcome::refused && absent.names.empty() &&
                absent.problem.code == DiagnosticCode::import_not_installed && absent.problem.detail == "gone" &&
                absent.problem.file == "top/theme.theme.css" && absent.problem.position.line.value == 2U,
            "a theme not installed is E603 at its @import");
    const Resolved stale =
        resolve(verified(package("top", "@import \"base\";", {{"base", hex_of("an older base")}})), index);
    require(stale.problem.code == DiagnosticCode::import_hash_mismatch && stale.problem.detail == "base",
            "an installed pack other than the pinned one is E604");

    // A pack reached a second time is checked against that importer's pin.
    const ThemePackageBytes stale_right =
        package("right", "@import \"base\";", {{"base", hex_of("an older base")}});
    InstalledThemeIndex mixed = index;
    VerifiedThemePack unchecked = verified(left);
    unchecked.manifest.name = "right";
    unchecked.identity = package_sha256(right.manifest);
    unchecked.manifest.imports = verified(stale_right).manifest.imports;
    require(mixed.install(std::move(unchecked)), "a pack pinning another base");
    const Resolved second = resolve(top, mixed);
    require(second.problem.code == DiagnosticCode::import_hash_mismatch, "the second pin is checked too");

    const ThemePackageBytes old_a = package("a", "#a { opacity: 1 }", {});
    const ThemePackageBytes b = package("b", "@import \"a\";", {{"a", identity_of(old_a)}});
    InstalledThemeIndex looped{};
    require(looped.install(verified(old_a)) && looped.install(verified(b)), "a and b are installed");
    const Resolved cycle = resolve(verified(package("a", "@import \"b\";", {{"b", identity_of(b)}})), looped);
    require(cycle.problem.code == DiagnosticCode::import_cycle && cycle.problem.detail == "a" &&
                cycle.problem.file == "b/theme.theme.css",
            "a pack importing itself through another is E605");

    InstalledThemeIndex wide{};
    const ThemePackageBytes leaf = package("leaf", "", {});
    require(wide.install(verified(leaf)), "a leaf");
    std::vector<Pin> pins{};
    std::string sheet{};
    for (std::size_t count = 0U; count < maximum_imports; ++count) {
        const std::string name = "p" + std::to_string(count);
        const ThemePackageBytes member = count == 0U ? package(name, "@import \"leaf\";", {{"leaf", identity_of(leaf)}})
                                                     : package(name, "", {});
        require(wide.install(verified(member)), "a member");
        pins.push_back(Pin{name, identity_of(member)});
        sheet += "@import \"" + name + "\";";
    }
    const Resolved crowded = resolve(verified(package("root", sheet, pins)), wide);
    require(crowded.problem.code == DiagnosticCode::too_many_imports && crowded.problem.detail == "16",
            "17 packs over the closure is E506");
    pins.pop_back();
    sheet = sheet.substr(0U, sheet.rfind("@import"));
    const Resolved full = resolve(verified(package("root", sheet, pins)), wide);
    require(full.outcome == ThemeImportOutcome::resolved && full.names.size() == maximum_imports + 1U &&
                full.names[0] == "leaf" && full.names[1] == "p0",
            "16 packs over the closure resolve");
}

} // namespace

int main() {
    test_verify();
    test_index();
    test_closure();
    if (failures != 0) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "theme package tests passed\n";
    return 0;
}
