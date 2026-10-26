// The layout fixture corpus against Chromium's checked-in reference boxes
// (planning/stages/L-layout/PLAN.md section 6, owner decisions L-Q9 and
// L-Q11). No Chromium runs here: every host compares GUI.Forms' own layout
// against tests/layout_corpus/references/, written by
// tools/layout_oracle/capture.py.
//
//   gui_forms_layout_corpus_tests <corpus directory> [--native-out <dir>]
//
// For each fixture of corpus.xml and each profile, the fixture's HTML
// fragment is read as XML, built into GUI.Forms controls, laid out in a
// window of the profile's size and scale, and every element with a
// data-gf-id is compared edge by edge with its reference box. A node over
// tolerance fails the test unless the fixture records the difference with
// data-gf-expected-difference="<reason>"; recorded differences are still
// printed and counted. --native-out writes the native boxes in the
// reference format, for tools/layout_oracle/compare.py's per-node report.
//
// The style reader here is deliberately small: it admits only the
// declarations the existing containers can express, refuses everything
// else by name, and is replaced by the layout value parser of phase L2.

#include "gui_forms/basic_controls.hpp"
#include "gui_forms/container_controls.hpp"
#include "gui_forms/window.hpp"
#include "gui_forms/xml.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {

using namespace gui_forms;

// ---------------------------------------------------------------------------
// Corpus records

struct Profile final {
    std::uint32_t width{};
    std::uint32_t height{};
    double scale{1.0};
};

struct Tolerances final {
    double box_edge{};
    double text_edge{};
};

struct Corpus final {
    std::vector<Profile> profiles;
    Tolerances tolerances;
    std::vector<std::string> fixture_names;
};

// One reference or native box in viewport coordinates, logical pixels.
struct NodeBox final {
    std::string identity;
    Rect bounds;
};

// Boxes of one profile, in document order.
struct ProfileBoxes final {
    Profile profile;
    std::vector<NodeBox> nodes;
};

// A fault in the corpus files themselves: the test stops and names it.
class CorpusFault final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

[[noreturn]] void fault(const std::string& where, const std::string& what) {
    throw CorpusFault(where + ": " + what);
}

std::string read_file(const std::filesystem::path& path) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) fault(path.string(), "cannot be opened");
    std::ostringstream contents;
    contents << stream.rdbuf();
    return contents.str();
}

XmlElement read_document(const std::filesystem::path& path) {
    const std::string text = read_file(path);
    XmlReadResult result = read_xml(text);
    if (!result) {
        fault(path.string(), "XML " + std::string(xml_error_name(result.error)) +
                                 " at line " + std::to_string(result.position.line) +
                                 ", column " + std::to_string(result.position.column));
    }
    return std::move(result.root);
}

const std::string* attribute(const XmlElement& element, std::string_view name) {
    const XmlAttribute* found = element.find_attribute(name);
    return found == nullptr ? nullptr : &(*found).value;
}

double parse_double(std::string_view text, const std::string& where) {
    double value = 0.0;
    const char* const first = text.data();
    const char* const last = text.data() + text.size();
    const std::from_chars_result parsed = std::from_chars(first, last, value);
    if (parsed.ec != std::errc{} || parsed.ptr != last || !std::isfinite(value)) {
        fault(where, "\"" + std::string(text) + "\" is not a finite number");
    }
    return value;
}

double number_attribute(const XmlElement& element, std::string_view name,
                        const std::string& where) {
    const std::string* text = attribute(element, name);
    if (text == nullptr) fault(where, "<" + element.name + "> has no " + std::string(name));
    return parse_double(*text, where);
}

Profile parse_profile(const XmlElement& element, const std::string& where) {
    const std::string* viewport = attribute(element, "viewport");
    if (viewport == nullptr) fault(where, "a profile has no viewport");
    const std::size_t separator = (*viewport).find('x');
    if (separator == std::string::npos) fault(where, "viewport is not WIDTHxHEIGHT");
    const double width = parse_double(std::string_view(*viewport).substr(0U, separator), where);
    const double height = parse_double(std::string_view(*viewport).substr(separator + 1U), where);
    const double scale = number_attribute(element, "scale", where);
    if (width < 1.0 || width > 8192.0 || height < 1.0 || height > 8192.0 ||
        width != std::floor(width) || height != std::floor(height) ||
        scale < 0.5 || scale > 4.0) {
        fault(where, "profile " + *viewport + " is out of range");
    }
    Profile profile;
    profile.width = static_cast<std::uint32_t>(width);
    profile.height = static_cast<std::uint32_t>(height);
    profile.scale = scale;
    return profile;
}

bool same_profile(const Profile& left, const Profile& right) noexcept {
    return left.width == right.width && left.height == right.height &&
           left.scale == right.scale;
}

std::string profile_name(const Profile& profile) {
    std::ostringstream name;
    name << profile.width << 'x' << profile.height << '@' << profile.scale;
    return name.str();
}

Corpus read_corpus(const std::filesystem::path& root) {
    const std::filesystem::path manifest_path = root / "corpus.xml";
    const std::string where = manifest_path.string();
    const XmlElement manifest = read_document(manifest_path);
    if (manifest.name != "layout-corpus") fault(where, "root must be <layout-corpus>");
    Corpus corpus;
    for (const XmlElement& section : manifest.children) {
        if (section.name == "profiles") {
            for (const XmlElement& profile : section.children) {
                corpus.profiles.push_back(parse_profile(profile, where));
            }
        } else if (section.name == "tolerance") {
            corpus.tolerances.box_edge = number_attribute(section, "box-edge", where);
            corpus.tolerances.text_edge = number_attribute(section, "text-edge", where);
        } else if (section.name == "fixtures") {
            for (const XmlElement& fixture : section.children) {
                const std::string* name = attribute(fixture, "name");
                if (name == nullptr || (*name).empty() ||
                    (*name).find("..") != std::string::npos) {
                    fault(where, "a fixture name is missing or unsafe");
                }
                corpus.fixture_names.push_back(*name);
            }
        } else {
            fault(where, "unknown section <" + section.name + ">");
        }
    }
    if (corpus.profiles.empty() || corpus.fixture_names.empty()) {
        fault(where, "the corpus needs profiles and fixtures");
    }
    return corpus;
}

std::vector<ProfileBoxes> read_boxes(const std::filesystem::path& path) {
    const std::string where = path.string();
    const XmlElement document = read_document(path);
    if (document.name != "layout-reference") fault(where, "not a layout reference");
    std::vector<ProfileBoxes> profiles;
    for (const XmlElement& profile_element : document.children) {
        ProfileBoxes boxes;
        boxes.profile = parse_profile(profile_element, where);
        for (const XmlElement& node : profile_element.children) {
            const std::string* identity = attribute(node, "id");
            if (identity == nullptr) fault(where, "a node has no id");
            NodeBox box;
            box.identity = *identity;
            box.bounds = Rect{number_attribute(node, "x", where), number_attribute(node, "y", where),
                              number_attribute(node, "width", where),
                              number_attribute(node, "height", where)};
            boxes.nodes.push_back(std::move(box));
        }
        profiles.push_back(std::move(boxes));
    }
    return profiles;
}

// ---------------------------------------------------------------------------
// The minimal style reader (replaced by phase L2's layout value parser)

struct StyleDeclaration final {
    std::string name;
    std::string value;
};

std::string_view trimmed(std::string_view text) noexcept {
    const std::string_view spaces = " \t\r\n";
    const std::size_t first = text.find_first_not_of(spaces);
    if (first == std::string_view::npos) return {};
    const std::size_t last = text.find_last_not_of(spaces);
    return text.substr(first, last - first + 1U);
}

// Splits on whitespace outside parentheses, so "minmax(0, 1fr) 40px" is two
// components.
std::vector<std::string> value_components(std::string_view value) {
    std::vector<std::string> components;
    std::string current;
    std::uint32_t depth = 0U;
    for (const char character : value) {
        const bool space = character == ' ' || character == '\t' || character == '\r' ||
                           character == '\n';
        if (character == '(') ++depth;
        if (character == ')' && depth > 0U) --depth;
        if (space && depth == 0U) {
            if (!current.empty()) components.push_back(std::move(current));
            current.clear();
            continue;
        }
        current.push_back(character);
    }
    if (!current.empty()) components.push_back(std::move(current));
    return components;
}

std::vector<StyleDeclaration> read_style(std::string_view style, const std::string& where) {
    std::vector<StyleDeclaration> declarations;
    std::size_t start = 0U;
    while (start <= style.size()) {
        const std::size_t end = std::min(style.find(';', start), style.size());
        const std::string_view text = trimmed(style.substr(start, end - start));
        start = end + 1U;
        if (text.empty()) continue;
        const std::size_t colon = text.find(':');
        if (colon == std::string_view::npos) fault(where, "declaration without ':'");
        StyleDeclaration declaration;
        declaration.name = std::string(trimmed(text.substr(0U, colon)));
        declaration.value = std::string(trimmed(text.substr(colon + 1U)));
        if (declaration.name.empty() || declaration.value.empty()) {
            fault(where, "empty declaration name or value");
        }
        declarations.push_back(std::move(declaration));
    }
    return declarations;
}

// A length in px; a bare zero is admitted, as CSS does.
double parse_length(std::string_view text, const std::string& where) {
    if (text == "0") return 0.0;
    if (text.size() < 3U || text.substr(text.size() - 2U) != "px") {
        fault(where, "\"" + std::string(text) + "\" is not a px length");
    }
    const double value = parse_double(text.substr(0U, text.size() - 2U), where);
    if (value < 0.0) fault(where, "a negative length is not admitted");
    return value;
}

// CSS's one-to-four value box shorthand: top, right, bottom, left.
Insets parse_box_shorthand(std::string_view value, const std::string& where) {
    const std::vector<std::string> components = value_components(value);
    if (components.empty() || components.size() > 4U) fault(where, "expects one to four lengths");
    std::vector<double> lengths;
    for (const std::string& component : components) {
        lengths.push_back(parse_length(component, where));
    }
    const double top = lengths[0];
    const double right = lengths.size() > 1U ? lengths[1] : top;
    const double bottom = lengths.size() > 2U ? lengths[2] : top;
    const double left = lengths.size() > 3U ? lengths[3] : right;
    return Insets{left, top, right, bottom};
}

// ---------------------------------------------------------------------------
// Building controls from a fixture

enum class FixtureKind : std::uint8_t {
    box,
    flow_layout_panel,
    table_layout_panel,
};

FixtureKind parse_kind(const std::string& kind, const std::string& where) {
    if (kind == "box") return FixtureKind::box;
    if (kind == "flow-layout-panel") return FixtureKind::flow_layout_panel;
    if (kind == "table-layout-panel") return FixtureKind::table_layout_panel;
    fault(where, "data-gf-kind \"" + kind + "\" is not built by the corpus harness yet");
}

// Self alignment on one axis of a table cell, as CSS justify-self or
// align-self.
enum class CellAlignment : std::uint8_t {
    start,
    end,
    center,
    stretch,
};

CellAlignment parse_cell_alignment(std::string_view value, const std::string& where) {
    if (value == "start") return CellAlignment::start;
    if (value == "end") return CellAlignment::end;
    if (value == "center") return CellAlignment::center;
    if (value == "stretch" || value == "normal") return CellAlignment::stretch;
    fault(where, "alignment \"" + std::string(value) + "\" is not admitted");
}

// Grid placement on one axis: a one-based start line and a span.
struct GridPlacement final {
    std::optional<std::size_t> start_line;
    std::size_t span{1U};
};

GridPlacement parse_grid_placement(std::string_view value, const std::string& where) {
    const std::vector<std::string> components = value_components(value);
    GridPlacement placement;
    std::size_t index = 0U;
    if (index < components.size() && components[index] != "span" && components[index] != "/") {
        const double line = parse_double(components[index], where);
        if (line < 1.0 || line > 64.0 || line != std::floor(line)) {
            fault(where, "a grid line must be 1 through 64");
        }
        placement.start_line = static_cast<std::size_t>(line);
        ++index;
    }
    if (index < components.size() && components[index] == "/") ++index;
    if (index < components.size() && components[index] == "span") {
        if (index + 1U >= components.size()) fault(where, "span needs a count");
        const double span = parse_double(components[index + 1U], where);
        if (span < 1.0 || span > 64.0 || span != std::floor(span)) {
            fault(where, "a grid span must be 1 through 64");
        }
        placement.span = static_cast<std::size_t>(span);
        index += 2U;
    }
    if (index != components.size()) fault(where, "grid placement \"" + std::string(value) +
                                                     "\" is not admitted");
    return placement;
}

TableLayoutStyle parse_track(std::string_view component, const std::string& where) {
    TableLayoutStyle style;
    if (component == "auto") {
        style.size_mode = TableSizeMode::auto_size;
        return style;
    }
    const std::string_view prefix = "minmax(0,";
    if (component.substr(0U, prefix.size()) == prefix && component.size() > prefix.size() + 3U &&
        component.substr(component.size() - 3U) == "fr)") {
        // minmax(0, Nfr) is the share of what is left after absolute and
        // content-sized tracks, ignoring content: TableLayoutPanel's percent.
        style.size_mode = TableSizeMode::percent;
        style.size = parse_double(
            trimmed(component.substr(prefix.size(), component.size() - prefix.size() - 3U)),
            where);
        return style;
    }
    style.size_mode = TableSizeMode::absolute;
    style.size = parse_length(component, where);
    return style;
}

// Splits a track list into components, joining "minmax(0," with "1fr)".
std::vector<std::string> track_components(std::string_view value) {
    std::string compact;
    std::uint32_t depth = 0U;
    for (const char character : value) {
        if (character == '(') ++depth;
        if (character == ')' && depth > 0U) --depth;
        if (depth > 0U && character == ' ') continue;
        compact.push_back(character);
    }
    return value_components(compact);
}

// What a child declares for its parent container to read.
struct ChildLayout final {
    // Flow children: the grow factor.
    double flow_grow{};
    // Table children: placement and self alignment.
    GridPlacement column;
    GridPlacement row;
    std::optional<CellAlignment> justify_self;
    std::optional<CellAlignment> align_self;
    bool explicit_width{};
    bool explicit_height{};
};

struct ContainerDefaults final {
    CellAlignment justify_items{CellAlignment::stretch};
    CellAlignment align_items{CellAlignment::stretch};
};

AnchorStyles cell_anchor(CellAlignment justify, CellAlignment align, const ChildLayout& child) {
    // CSS stretches only an item whose size on that axis is not given; a
    // given size with stretch behaves as start.
    const CellAlignment horizontal =
        justify == CellAlignment::stretch && child.explicit_width ? CellAlignment::start : justify;
    const CellAlignment vertical =
        align == CellAlignment::stretch && child.explicit_height ? CellAlignment::start : align;
    AnchorStyles anchor = AnchorStyles::none;
    if (horizontal == CellAlignment::start || horizontal == CellAlignment::stretch) {
        anchor |= AnchorStyles::left;
    }
    if (horizontal == CellAlignment::end || horizontal == CellAlignment::stretch) {
        anchor |= AnchorStyles::right;
    }
    if (vertical == CellAlignment::start || vertical == CellAlignment::stretch) {
        anchor |= AnchorStyles::top;
    }
    if (vertical == CellAlignment::end || vertical == CellAlignment::stretch) {
        anchor |= AnchorStyles::bottom;
    }
    return anchor;
}

// One element of the fixture that carries a data-gf-id, in document order.
struct BuiltIdentity final {
    std::string identity;
    Control::Ptr control;
};

// A built element and the layout its parent container reads from it.
struct BuiltElement final {
    Control::Ptr control;
    ChildLayout layout;
};

class FixtureBuilder final {
public:
    explicit FixtureBuilder(std::string fixture_name) : fixture_name_(std::move(fixture_name)) {}

    Control::Ptr build(const XmlElement& root) {
        identities_.clear();
        const BuiltElement built = build_element(root, std::nullopt);
        return built.control;
    }

    // Every built element, in document order. The span borrows this
    // builder's storage, so a temporary builder is refused.
    [[nodiscard]] std::span<const BuiltIdentity> identities() const& noexcept {
        return identities_;
    }
    std::span<const BuiltIdentity> identities() const&& = delete;

private:
    BuiltElement build_element(const XmlElement& element, std::optional<FixtureKind> parent_kind) {
        const std::string* identity = attribute(element, "data-gf-id");
        const std::string* kind_name = attribute(element, "data-gf-kind");
        if (identity == nullptr || kind_name == nullptr) {
            fault(fixture_name_, "<" + element.name + "> needs data-gf-id and data-gf-kind");
        }
        const std::string where = fixture_name_ + " #" + *identity;
        if (element.name != "div") fault(where, "only <div> elements are admitted in L0");
        const FixtureKind kind = parse_kind(*kind_name, where);
        BuiltElement built;
        built.control = make_kind(kind, *identity);
        Control& control = *built.control;
        control.set_margin({0.0, 0.0, 0.0, 0.0});
        identities_.push_back(BuiltIdentity{*identity, built.control});

        const std::string* style_text = attribute(element, "style");
        const std::vector<StyleDeclaration> declarations =
            style_text == nullptr ? std::vector<StyleDeclaration>{} : read_style(*style_text, where);
        ContainerDefaults defaults;
        std::vector<TableLayoutStyle> columns;
        std::vector<TableLayoutStyle> rows;
        for (const StyleDeclaration& declaration : declarations) {
            const std::string declaration_where = where + " " + declaration.name;
            if (apply_common(control, declaration, built.layout, declaration_where)) continue;
            if (kind == FixtureKind::flow_layout_panel &&
                apply_flow(static_cast<FlowLayoutPanel&>(control), declaration,
                           declaration_where)) {
                continue;
            }
            if (kind == FixtureKind::table_layout_panel &&
                apply_table(static_cast<TableLayoutPanel&>(control), declaration, defaults,
                            columns, rows, declaration_where)) {
                continue;
            }
            if (parent_kind == FixtureKind::flow_layout_panel &&
                apply_flow_child(declaration, built.layout, declaration_where)) {
                continue;
            }
            if (parent_kind == FixtureKind::table_layout_panel &&
                apply_table_child(declaration, built.layout, declaration_where)) {
                continue;
            }
            fault(declaration_where, "declaration \"" + declaration.name + ": " +
                                         declaration.value + "\" is not admitted here");
        }

        if (kind == FixtureKind::table_layout_panel) {
            configure_table(static_cast<TableLayoutPanel&>(control), columns, rows, where);
        }
        for (const XmlElement& child_element : element.children) {
            const BuiltElement child = build_element(child_element, kind);
            control.add_child(child.control);
            if (kind == FixtureKind::flow_layout_panel && child.layout.flow_grow != 0.0) {
                static_cast<FlowLayoutPanel&>(control).set_flex_grow(*child.control,
                                                                     child.layout.flow_grow);
            }
            if (kind == FixtureKind::table_layout_panel) {
                place_table_child(static_cast<TableLayoutPanel&>(control), child, defaults,
                                  where);
            }
        }
        return built;
    }

    static Control::Ptr make_kind(FixtureKind kind, const std::string& identity) {
        switch (kind) {
        case FixtureKind::box:
            return make_control<Panel>(StableId(identity));
        case FixtureKind::flow_layout_panel:
            return make_control<FlowLayoutPanel>(StableId(identity));
        case FixtureKind::table_layout_panel:
            return make_control<TableLayoutPanel>(StableId(identity));
        }
        throw std::logic_error("unknown fixture kind");
    }

    static bool apply_common(Control& control, const StyleDeclaration& declaration,
                             ChildLayout& child, const std::string& where) {
        const Rect requested = control.requested_bounds();
        if (declaration.name == "width") {
            control.set_requested_bounds({requested.x, requested.y,
                                          parse_length(declaration.value, where), requested.height});
            child.explicit_width = true;
            return true;
        }
        if (declaration.name == "height") {
            control.set_requested_bounds({requested.x, requested.y, requested.width,
                                          parse_length(declaration.value, where)});
            child.explicit_height = true;
            return true;
        }
        if (declaration.name == "margin") {
            control.set_margin(parse_box_shorthand(declaration.value, where));
            return true;
        }
        if (declaration.name == "padding") {
            control.set_padding(parse_box_shorthand(declaration.value, where));
            return true;
        }
        return false;
    }

    static bool apply_flow(FlowLayoutPanel& flow, const StyleDeclaration& declaration,
                           const std::string& where) {
        const std::string& value = declaration.value;
        if (declaration.name == "display") {
            if (value != "flex") fault(where, "a flow-layout-panel is display: flex");
            return true;
        }
        if (declaration.name == "flex-direction") {
            if (value == "row") flow.set_flow_direction(FlowDirection::left_to_right);
            else if (value == "row-reverse") flow.set_flow_direction(FlowDirection::right_to_left);
            else if (value == "column") flow.set_flow_direction(FlowDirection::top_down);
            else if (value == "column-reverse") flow.set_flow_direction(FlowDirection::bottom_up);
            else fault(where, "unknown direction");
            return true;
        }
        if (declaration.name == "flex-wrap") {
            if (value == "wrap") flow.set_wrap_contents(true);
            else if (value == "nowrap") flow.set_wrap_contents(false);
            else fault(where, "FlowLayoutPanel has no " + value);
            return true;
        }
        if (declaration.name == "justify-content") {
            if (value == "flex-start" || value == "start") {
                flow.set_main_alignment(FlowMainAlignment::start);
            } else if (value == "center") {
                flow.set_main_alignment(FlowMainAlignment::center);
            } else if (value == "flex-end" || value == "end") {
                flow.set_main_alignment(FlowMainAlignment::end);
            } else if (value == "space-between") {
                flow.set_main_alignment(FlowMainAlignment::space_between);
            } else if (value == "space-around") {
                flow.set_main_alignment(FlowMainAlignment::space_around);
            } else if (value == "space-evenly") {
                flow.set_main_alignment(FlowMainAlignment::space_evenly);
            } else {
                fault(where, "unknown justify-content");
            }
            return true;
        }
        if (declaration.name == "align-items") {
            if (value == "flex-start" || value == "start") {
                flow.set_cross_alignment(FlowCrossAlignment::start);
            } else if (value == "center") {
                flow.set_cross_alignment(FlowCrossAlignment::center);
            } else if (value == "flex-end" || value == "end") {
                flow.set_cross_alignment(FlowCrossAlignment::end);
            } else if (value == "stretch") {
                flow.set_cross_alignment(FlowCrossAlignment::stretch);
            } else {
                fault(where, "unknown align-items");
            }
            return true;
        }
        if (declaration.name == "align-content") {
            // FlowLayoutPanel packs its lines at the start.
            if (value != "flex-start" && value != "start") {
                fault(where, "FlowLayoutPanel packs lines at the start only");
            }
            return true;
        }
        if (declaration.name == "gap") {
            const std::vector<std::string> components = value_components(value);
            if (components.empty() || components.size() > 2U) fault(where, "gap takes one or two");
            const double row_gap = parse_length(components[0], where);
            const double column_gap =
                components.size() == 2U ? parse_length(components[1], where) : row_gap;
            flow.set_item_spacing({column_gap, row_gap});
            return true;
        }
        return false;
    }

    static bool apply_flow_child(const StyleDeclaration& declaration, ChildLayout& child,
                                 const std::string& where) {
        double& grow = child.flow_grow;
        const std::string& value = declaration.value;
        if (declaration.name == "flex") {
            if (value == "none") {
                grow = 0.0;
                return true;
            }
            const std::vector<std::string> components = value_components(value);
            if (components.size() != 3U || components[1] != "0" || components[2] != "auto") {
                fault(where, "FlowLayoutPanel admits flex: none or flex: <grow> 0 auto");
            }
            grow = parse_double(components[0], where);
            return true;
        }
        if (declaration.name == "flex-grow") {
            grow = parse_double(value, where);
            return true;
        }
        if (declaration.name == "flex-shrink") {
            if (value != "0") fault(where, "FlowLayoutPanel never shrinks items");
            return true;
        }
        if (declaration.name == "flex-basis") {
            if (value != "auto") fault(where, "FlowLayoutPanel items have basis auto");
            return true;
        }
        return false;
    }

    static bool apply_table(TableLayoutPanel& table, const StyleDeclaration& declaration,
                            ContainerDefaults& defaults, std::vector<TableLayoutStyle>& columns,
                            std::vector<TableLayoutStyle>& rows, const std::string& where) {
        const std::string& value = declaration.value;
        if (declaration.name == "display") {
            if (value != "grid") fault(where, "a table-layout-panel is display: grid");
            return true;
        }
        if (declaration.name == "grid-template-columns" || declaration.name == "grid-template-rows") {
            std::vector<TableLayoutStyle>& tracks =
                declaration.name == "grid-template-columns" ? columns : rows;
            for (const std::string& component : track_components(value)) {
                tracks.push_back(parse_track(component, where));
            }
            return true;
        }
        if (declaration.name == "gap") {
            const std::vector<std::string> components = value_components(value);
            if (components.empty() || components.size() > 2U) fault(where, "gap takes one or two");
            const double row_gap = parse_length(components[0], where);
            const double column_gap =
                components.size() == 2U ? parse_length(components[1], where) : row_gap;
            table.set_track_spacing({column_gap, row_gap});
            return true;
        }
        if (declaration.name == "justify-items") {
            defaults.justify_items = parse_cell_alignment(value, where);
            return true;
        }
        if (declaration.name == "align-items") {
            defaults.align_items = parse_cell_alignment(value, where);
            return true;
        }
        if (declaration.name == "justify-content" || declaration.name == "align-content") {
            // TableLayoutPanel packs its tracks at the start; CSS's default
            // (normal) stretches auto tracks into free space, which a
            // fixture records as a difference where it shows.
            if (value != "start" && value != "normal") {
                fault(where, "TableLayoutPanel packs tracks at the start only");
            }
            return true;
        }
        if (declaration.name == "grid-auto-flow") {
            if (value == "row") table.set_grow_style(TableLayoutGrowStyle::add_rows);
            else if (value == "column") table.set_grow_style(TableLayoutGrowStyle::add_columns);
            else fault(where, "TableLayoutPanel grows by rows or columns only");
            return true;
        }
        return false;
    }

    static bool apply_table_child(const StyleDeclaration& declaration, ChildLayout& child,
                                  const std::string& where) {
        if (declaration.name == "grid-column") {
            child.column = parse_grid_placement(declaration.value, where);
            return true;
        }
        if (declaration.name == "grid-row") {
            child.row = parse_grid_placement(declaration.value, where);
            return true;
        }
        if (declaration.name == "justify-self") {
            child.justify_self = parse_cell_alignment(declaration.value, where);
            return true;
        }
        if (declaration.name == "align-self") {
            child.align_self = parse_cell_alignment(declaration.value, where);
            return true;
        }
        return false;
    }

    static void configure_table(TableLayoutPanel& table, const std::vector<TableLayoutStyle>& columns,
                                const std::vector<TableLayoutStyle>& rows, const std::string& where) {
        if (columns.empty()) fault(where, "a table-layout-panel needs grid-template-columns");
        table.set_grow_style(TableLayoutGrowStyle::add_rows);
        table.set_column_count(columns.size());
        table.set_row_count(rows.empty() ? 1U : rows.size());
        for (std::size_t index = 0U; index < columns.size(); ++index) {
            table.set_column_style(index, columns[index]);
        }
        for (std::size_t index = 0U; index < rows.size(); ++index) {
            table.set_row_style(index, rows[index]);
        }
    }

    static void place_table_child(TableLayoutPanel& table, const BuiltElement& child,
                                  const ContainerDefaults& defaults, const std::string& where) {
        const ChildLayout& layout = child.layout;
        Control& control = *child.control;
        if (layout.column.start_line.has_value() != layout.row.start_line.has_value()) {
            fault(where, "TableLayoutPanel places a child on both axes or neither");
        }
        if (layout.column.start_line) {
            table.set_cell_position(control, TableLayoutCellPosition{*layout.column.start_line - 1U,
                                                                     *layout.row.start_line - 1U});
        }
        table.set_column_span(control, layout.column.span);
        table.set_row_span(control, layout.row.span);
        const CellAlignment justify = layout.justify_self.value_or(defaults.justify_items);
        const CellAlignment align = layout.align_self.value_or(defaults.align_items);
        control.set_dock(DockStyle::none);
        control.set_anchor(cell_anchor(justify, align, layout));
    }

    std::string fixture_name_;
    std::vector<BuiltIdentity> identities_;
};

// ---------------------------------------------------------------------------
// Comparison

double edge_difference(Rect reference, Rect observed) noexcept {
    const double left = std::abs(reference.x - observed.x);
    const double top = std::abs(reference.y - observed.y);
    const double right = std::abs(reference.right() - observed.right());
    const double bottom = std::abs(reference.bottom() - observed.bottom());
    return std::max(std::max(left, top), std::max(right, bottom));
}

std::string format_number(double value) {
    if (value == 0.0) return "0";
    if (value == std::floor(value) && std::abs(value) < 1.0e15) {
        return std::to_string(static_cast<long long>(value));
    }
    char buffer[64]{};
    const std::to_chars_result written = std::to_chars(buffer, buffer + sizeof(buffer), value);
    return std::string(buffer, written.ptr);
}

struct ProfileMatches final {
    Profile profile;
    bool operator()(const ProfileBoxes& boxes) const noexcept {
        return same_profile(boxes.profile, profile);
    }
};

struct IdentityMatches final {
    std::string_view identity;
    bool operator()(const BuiltIdentity& built) const noexcept {
        return built.identity == identity;
    }
};

struct FixtureTally final {
    std::uint64_t nodes{};
    std::uint64_t over_tolerance{};
    std::uint64_t recorded{};
    double largest_edge{};
};

std::map<std::string, std::string> expected_differences(const XmlElement& element) {
    std::map<std::string, std::string> expected;
    std::vector<const XmlElement*> pending{&element};
    while (!pending.empty()) {
        const XmlElement* current = pending.back();
        pending.pop_back();
        const std::string* identity = attribute(*current, "data-gf-id");
        const std::string* reason = attribute(*current, "data-gf-expected-difference");
        if (identity != nullptr && reason != nullptr) expected[*identity] = *reason;
        for (const XmlElement& child : (*current).children) pending.push_back(&child);
    }
    return expected;
}

bool run_fixture(const std::filesystem::path& corpus_root, const Corpus& corpus,
                 const std::string& name, const std::optional<std::filesystem::path>& native_out,
                 FixtureTally& tally) {
    const XmlElement fixture = read_document(corpus_root / "fixtures" / (name + ".html"));
    const std::vector<ProfileBoxes> references =
        read_boxes(corpus_root / "references" / (name + ".xml"));
    const std::map<std::string, std::string> expected = expected_differences(fixture);
    const double tolerance = corpus.tolerances.box_edge;
    bool passed = true;
    std::ostringstream native;
    native << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<layout-native fixture=\"" << name
           << "\">\n";

    for (const Profile& profile : corpus.profiles) {
        const std::vector<ProfileBoxes>::const_iterator reference = std::find_if(
            references.begin(), references.end(), ProfileMatches{profile});
        if (reference == references.end()) {
            std::cout << name << " " << profile_name(profile) << ": no reference profile\n";
            passed = false;
            continue;
        }
        FixtureBuilder builder(name);
        const Control::Ptr root = builder.build(fixture);
        const std::unique_ptr<Window> held_window = std::make_unique<Window>(
            root, Size{static_cast<double>(profile.width), static_cast<double>(profile.height)});
        Window& window = *held_window;
        window.set_scale(profile.scale);
        window.perform_layout();
        native << "  <profile viewport=\"" << profile.width << 'x' << profile.height
               << "\" scale=\"" << format_number(profile.scale) << "\">\n";
        const std::span<const BuiltIdentity> built = builder.identities();
        for (const BuiltIdentity& entry : built) {
            const Rect observed = (*entry.control).absolute_bounds();
            native << "    <node id=\"" << entry.identity << "\" x=\"" << format_number(observed.x)
                   << "\" y=\"" << format_number(observed.y) << "\" width=\""
                   << format_number(observed.width) << "\" height=\""
                   << format_number(observed.height) << "\"/>\n";
        }
        native << "  </profile>\n";
        for (const NodeBox& node : (*reference).nodes) {
            ++tally.nodes;
            const std::span<const BuiltIdentity>::iterator found =
                std::find_if(built.begin(), built.end(), IdentityMatches{node.identity});
            const std::map<std::string, std::string>::const_iterator reason =
                expected.find(node.identity);
            if (found == built.end()) {
                std::cout << name << " " << profile_name(profile) << " #" << node.identity
                          << ": missing on the GUI.Forms side\n";
                passed = false;
                continue;
            }
            const Rect observed = (*(*found).control).absolute_bounds();
            const double edge = edge_difference(node.bounds, observed);
            tally.largest_edge = std::max(tally.largest_edge, edge);
            if (edge <= tolerance) continue;
            std::cout << name << " " << profile_name(profile) << " #" << node.identity
                      << ": edge difference " << edge << " (Chromium " << node.bounds.x << ','
                      << node.bounds.y << ' ' << node.bounds.width << 'x' << node.bounds.height
                      << ", GUI.Forms " << observed.x << ',' << observed.y << ' '
                      << observed.width << 'x' << observed.height << ')';
            if (reason != expected.end()) {
                ++tally.recorded;
                std::cout << " recorded: " << (*reason).second << '\n';
            } else {
                ++tally.over_tolerance;
                passed = false;
                std::cout << " OVER TOLERANCE\n";
            }
        }
    }
    native << "</layout-native>\n";
    if (native_out) {
        const std::filesystem::path path = *native_out / (name + ".xml");
        std::filesystem::create_directories(path.parent_path());
        std::ofstream stream(path, std::ios::binary);
        stream << native.str();
    }
    return passed;
}

} // namespace

int main(int argument_count, char** arguments) {
    if (argument_count != 2 && argument_count != 4) {
        std::cerr << "usage: gui_forms_layout_corpus_tests <corpus> [--native-out <dir>]\n";
        return 2;
    }
    const std::filesystem::path corpus_root(arguments[1]);
    std::optional<std::filesystem::path> native_out;
    if (argument_count == 4) {
        if (std::string_view(arguments[2]) != "--native-out") {
            std::cerr << "unknown option " << arguments[2] << '\n';
            return 2;
        }
        native_out = std::filesystem::path(arguments[3]);
    }
    try {
        const Corpus corpus = read_corpus(corpus_root);
        bool passed = true;
        FixtureTally total;
        for (const std::string& name : corpus.fixture_names) {
            FixtureTally tally;
            const bool fixture_passed = run_fixture(corpus_root, corpus, name, native_out, tally);
            std::cout << name << ": " << tally.nodes << " nodes, largest edge difference "
                      << tally.largest_edge << ", " << tally.over_tolerance
                      << " over tolerance, " << tally.recorded << " recorded\n";
            passed = passed && fixture_passed;
            total.nodes += tally.nodes;
            total.over_tolerance += tally.over_tolerance;
            total.recorded += tally.recorded;
            total.largest_edge = std::max(total.largest_edge, tally.largest_edge);
        }
        std::cout << "corpus: " << corpus.fixture_names.size() << " fixtures, "
                  << corpus.profiles.size() << " profiles, " << total.nodes << " nodes, "
                  << total.over_tolerance << " over tolerance, " << total.recorded
                  << " recorded differences, largest edge difference " << total.largest_edge
                  << '\n';
        if (!passed) return EXIT_FAILURE;
    } catch (const CorpusFault& error) {
        std::cerr << "layout corpus fault: " << error.what() << '\n';
        return EXIT_FAILURE;
    } catch (const std::exception& error) {
        std::cerr << "layout corpus failure: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
