// The theme.json manifest (stage T3, phase P10, owner decisions of
// 2026-10-28): one sheet a pack, imports pinned by their manifests'
// hashes, the engine bound read first (E607), every other fault of the
// schema E601, published values E606. Gate: a full manifest read whole, a
// minimal one, and a fixture for each refusal.

#include "../../src/core/theme_language/package/theme_manifest.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

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

constexpr std::string_view hash_a = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
constexpr std::string_view hash_b = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

std::string minimal(std::string_view extra = {}) {
    std::string text = "{\"format\": \"gui-forms-theme/1\", \"name\": \"harbour-dark\", \"version\": \"1.4.0\", "
                       "\"engine\": \">=0.3\", \"sheet\": {\"path\": \"theme.theme.css\", \"sha256\": \"" +
                       std::string(hash_a) + "\"}";
    text += extra;
    text += "}";
    return text;
}

struct Read final {
    ThemeManifestOutcome outcome{ThemeManifestOutcome::refused};
    ThemeManifest manifest{};
    ThemeManifestProblem problem{};
};

Read read(std::string_view text, EngineVersion running = current_engine_version) {
    Read result{};
    result.outcome = read_theme_manifest(text, running, result.manifest, result.problem);
    return result;
}

void test_full() {
    const std::string text = minimal(", \"author\": \"rainstar\", \"provenance\": \"first-party\", "
                                     "\"imports\": {\"harbour-base\": \"" + std::string(hash_b) + "\"}, "
                                     "\"resources\": [{\"name\": \"icon.check\", \"path\": \"images/check.png\", "
                                     "\"sha256\": \"" + std::string(hash_b) + "\"}], "
                                     "\"published_values\": [\"progress\", \"level-2\"], "
                                     "\"coverage\": [\"button\", \"check-box\"]");
    const Read full = read(text);
    const ThemeManifest& manifest = full.manifest;
    require(full.outcome == ThemeManifestOutcome::read, "a full manifest is read");
    require(manifest.name == "harbour-dark" && manifest.version == "1.4.0" && manifest.engine == EngineVersion{0, 3} &&
                manifest.author == "rainstar" && manifest.provenance == "first-party",
            "its identity");
    require(manifest.sheet.path == "theme.theme.css" && package_sha256_hex(manifest.sheet.sha256) == hash_a,
            "its sheet and the sheet's hash");
    require(manifest.imports.size() == 1U && manifest.imports[0].name == "harbour-base" &&
                package_sha256_hex(manifest.imports[0].manifest_sha256) == hash_b,
            "an import pinned by its manifest's hash");
    require(manifest.resources.size() == 1U && manifest.resources[0].name == "icon.check" &&
                manifest.resources[0].file.path == "images/check.png",
            "a resource");
    require(manifest.published_values.size() == 2U && manifest.published_values[1] == "level-2",
            "published values");
    require(manifest.coverage.size() == 2U && manifest.coverage[1] == "check-box", "coverage, in order");

    const Read small = read(minimal());
    require(small.outcome == ThemeManifestOutcome::read && small.manifest.imports.empty() &&
                small.manifest.resources.empty() && small.manifest.author.empty(),
            "the optional keys may be left out");
}

struct Refusal final {
    std::string text{};
    DiagnosticCode code{DiagnosticCode::manifest_invalid};
    std::string_view detail{};
};

void test_refusals() {
    const std::string hash = std::string(hash_b);
    const std::array<Refusal, 34> refusals{{
        {"{\"format\": ", DiagnosticCode::manifest_invalid, "unexpected_end"},
        {"[]", DiagnosticCode::manifest_invalid, "the manifest is not an object"},
        {"{\"format\": \"gui-forms-theme/1\", \"format\": \"x\"}", DiagnosticCode::manifest_invalid, "duplicate_name"},
        {"{\"engine\": \">=9.0\", \"colour\": 1}", DiagnosticCode::engine_too_old,
         "this theme needs engine >= 9.0; upgrade to run it"},
        {"{\"engine\": \">=0.4\"}", DiagnosticCode::engine_too_old, "this theme needs engine >= 0.4; upgrade to run it"},
        {"{\"format\": \"gui-forms-theme/1\"}", DiagnosticCode::manifest_invalid, "engine"},
        {"{\"engine\": \"0.3\"}", DiagnosticCode::manifest_invalid, "engine"},
        {"{\"engine\": \"<=0.3\"}", DiagnosticCode::manifest_invalid, "engine"},
        {"{\"engine\": \">=0.3 <1.0\"}", DiagnosticCode::manifest_invalid, "engine"},
        {"{\"engine\": \">=0.03\"}", DiagnosticCode::manifest_invalid, "engine"},
        {"{\"engine\": 0.3}", DiagnosticCode::manifest_invalid, "engine"},
        {minimal(", \"colour\": 1"), DiagnosticCode::manifest_invalid, "colour"},
        {"{\"engine\": \">=0.3\", \"format\": \"gui-forms-theme/2\"}", DiagnosticCode::manifest_invalid, "format"},
        {"{\"engine\": \">=0.3\", \"format\": \"gui-forms-theme/1\", \"name\": \"Harbour\"}",
         DiagnosticCode::manifest_invalid, "name"},
        {"{\"engine\": \">=0.3\", \"format\": \"gui-forms-theme/1\", \"name\": \"-a\"}",
         DiagnosticCode::manifest_invalid, "name"},
        {"{\"engine\": \">=0.3\", \"format\": \"gui-forms-theme/1\", \"name\": \"a\", \"version\": \"1.4\"}",
         DiagnosticCode::manifest_invalid, "version"},
        {"{\"engine\": \">=0.3\", \"format\": \"gui-forms-theme/1\", \"name\": \"a\", \"version\": \"1.04.0\"}",
         DiagnosticCode::manifest_invalid, "version"},
        {"{\"engine\": \">=0.3\", \"format\": \"gui-forms-theme/1\", \"name\": \"a\", \"version\": \"1.0.0\"}",
         DiagnosticCode::manifest_invalid, "sheet"},
        {minimal(", \"author\": 7"), DiagnosticCode::manifest_invalid, "author"},
        {"{\"engine\": \">=0.3\", \"format\": \"gui-forms-theme/1\", \"name\": \"a\", \"version\": \"1.0.0\", "
         "\"sheet\": {\"path\": \"../theme.theme.css\", \"sha256\": \"" + hash + "\"}}",
         DiagnosticCode::manifest_invalid, "sheet.path"},
        {"{\"engine\": \">=0.3\", \"format\": \"gui-forms-theme/1\", \"name\": \"a\", \"version\": \"1.0.0\", "
         "\"sheet\": {\"path\": \"theme.theme.css\", \"sha256\": \"" + std::string(64U, 'A') + "\"}}",
         DiagnosticCode::manifest_invalid, "sheet.sha256"},
        {"{\"engine\": \">=0.3\", \"format\": \"gui-forms-theme/1\", \"name\": \"a\", \"version\": \"1.0.0\", "
         "\"sheet\": {\"path\": \"theme.theme.css\", \"sha256\": \"" + hash + "\", \"size\": 3}}",
         DiagnosticCode::manifest_invalid, "sheet.size"},
        {minimal(", \"imports\": {\"Base\": \"" + hash + "\"}"), DiagnosticCode::manifest_invalid, "imports.Base"},
        {minimal(", \"imports\": {\"base\": \"abc\"}"), DiagnosticCode::manifest_invalid, "imports.base"},
        {minimal(", \"imports\": {\"harbour-dark\": \"" + hash + "\"}"), DiagnosticCode::import_cycle, "harbour-dark"},
        {minimal(", \"imports\": [\"base\"]"), DiagnosticCode::manifest_invalid, "imports"},
        {minimal(", \"resources\": [{\"name\": \"icon.check\", \"path\": \"a.png\", \"sha256\": \"" + hash +
                 "\"}, {\"name\": \"icon.check\", \"path\": \"b.png\", \"sha256\": \"" + hash + "\"}]"),
         DiagnosticCode::manifest_invalid, "resources[1].name"},
        {minimal(", \"resources\": [{\"name\": \"icon\", \"path\": \"/a.png\", \"sha256\": \"" + hash + "\"}]"),
         DiagnosticCode::manifest_invalid, "resources[0].path"},
        {minimal(", \"resources\": [{\"name\": \"icon\", \"path\": \"a.png\"}]"), DiagnosticCode::manifest_invalid,
         "resources[0].sha256"},
        {minimal(", \"published_values\": [\"--progress\"]"), DiagnosticCode::published_value_invalid,
         "--progress"},
        {minimal(", \"published_values\": [\"progress\", \"progress\"]"), DiagnosticCode::published_value_invalid,
         "progress"},
        {minimal(", \"published_values\": [3]"), DiagnosticCode::manifest_invalid, "published_values"},
        {minimal(", \"coverage\": [\"button\", \"button\"]"), DiagnosticCode::manifest_invalid, "coverage[1]"},
        {minimal(", \"coverage\": \"button\""), DiagnosticCode::manifest_invalid, "coverage"},
    }};
    for (const Refusal& refusal : refusals) {
        const Read refused = read(refusal.text);
        const bool same = refused.outcome == ThemeManifestOutcome::refused && refused.problem.code == refusal.code &&
                          refused.problem.detail == refusal.detail;
        if (!same) {
            std::cerr << "  got " << diagnostic_code_name(refused.problem.code) << " (" << refused.problem.detail
                      << ")\n";
        }
        require(same, "refused: " + refusal.text);
    }

    std::string many = ", \"imports\": {";
    for (std::size_t index = 0U; index <= maximum_imports; ++index) {
        if (index != 0U) many += ", ";
        many += "\"base-" + std::to_string(index) + "\": \"" + hash + "\"";
    }
    many += "}";
    const Read crowded = read(minimal(many));
    require(crowded.outcome == ThemeManifestOutcome::refused &&
                crowded.problem.code == DiagnosticCode::too_many_imports,
            "more than 16 imports is E506");
    const Read where = read("{\n  \"format\": tru }");
    require(where.problem.position.line == 2U && where.problem.position.column == 13U,
            "a JSON error says where it was found");
    const Read large = read(std::string(maximum_manifest_bytes + 1U, ' '));
    require(large.problem.code == DiagnosticCode::manifest_invalid && large.problem.detail == "input_too_large",
            "a manifest is at most 64 KiB");
    const Read newer = read(minimal(), EngineVersion{0, 2});
    require(newer.outcome == ThemeManifestOutcome::refused && newer.problem.code == DiagnosticCode::engine_too_old,
            "an older running engine refuses the bound");
}

void test_forms() {
    require(parse_engine_bound(">=0.3") == EngineVersion{0, 3} &&
                parse_engine_bound(">=12.65535") == EngineVersion{12, 65535},
            "an engine bound");
    require(!parse_engine_bound(">=65536.0") && !parse_engine_bound(">= 0.3") && !parse_engine_bound(">=0") &&
                !parse_engine_bound(">=0.3.1") && !parse_engine_bound(">=+1.0") && !parse_engine_bound("^0.3"),
            "only >=X.Y");
    require(package_name_valid("a") && package_name_valid("harbour-dark-2") &&
                package_name_valid(std::string(maximum_package_name_bytes, 'a')),
            "names");
    require(!package_name_valid("") && !package_name_valid("-a") && !package_name_valid("a_b") &&
                !package_name_valid("A") && !package_name_valid(std::string(maximum_package_name_bytes + 1U, 'a')),
            "not names");
    require(package_path_valid("theme.theme.css") && package_path_valid("a/b/c.png") &&
                package_path_valid(".hidden/a.png"),
            "paths");
    const std::array<std::string_view, 10> bad{
        "", "/a", "a/", "a//b", "./a", "a/../b", "a\\b", "c:/a", "a/b/c/d/e/f/g/h/i", "a\tb",
    };
    for (const std::string_view path : bad) {
        require(!package_path_valid(path), std::string("not a path: ") + std::string(path));
    }
    require(!package_path_valid(std::string(maximum_package_path_bytes + 1U, 'a')), "at most 255 bytes");
}

} // namespace

int main() {
    test_full();
    test_refusals();
    test_forms();
    if (failures != 0) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "theme manifest tests passed\n";
    return 0;
}
