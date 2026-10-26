// The layout fixture corpus against Chromium's checked-in reference boxes
// (planning/stages/L-layout/PLAN.md section 6, owner decisions L-Q9 and
// L-Q11). No Chromium runs here: every host compares GUI.Forms' own layout
// against tests/layout_corpus/references/, written by
// tools/layout_oracle/capture.py.
//
//   gui_forms_layout_corpus_tests <corpus directory> [--native-out <dir>]
//
// For each fixture of corpus.xml and each profile that applies to it, the
// fixture's HTML fragment is read as XML, built into GUI.Forms controls,
// laid out in a window of the profile's size and scale (a right-to-left
// profile, which applies only to a fixture marked bidirectional, sets the
// root's LayoutDirection), and every element with a
// data-gf-id is compared edge by edge with its reference box. A node over
// tolerance fails the test unless the fixture records the difference with
// data-gf-expected-difference="<reason>"; recorded differences are still
// printed and counted. --native-out writes the native boxes in the
// reference format, for tools/layout_oracle/compare.py's per-node report.
//
// The flow and table fixtures go through a deliberately small style reader
// that admits only the declarations the Windows Forms containers can
// express. Flex and grid fixtures (phases L3 and L4) are read whole by the
// layout value reader of phase L2: a container's declarations become
// FlexPanel or GridPanel properties, an item's its
// FlexItemLayout or GridItemLayout, and every other declaration is refused
// by name.

#include "../src/core/layout/style_reader/layout_style_reader.hpp"

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
    // The root's direction; right to left applies only to bidirectional
    // fixtures.
    LayoutDirection direction{LayoutDirection::left_to_right};
};

struct FixtureEntry final {
    std::string name;
    bool bidirectional{};
};

struct Tolerances final {
    double box_edge{};
    double text_edge{};
};

struct Corpus final {
    std::vector<Profile> profiles;
    Tolerances tolerances;
    std::vector<FixtureEntry> fixtures;
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
    const std::string* direction = attribute(element, "direction");
    if (direction != nullptr) {
        if (*direction == "rtl") {
            profile.direction = LayoutDirection::right_to_left;
        } else if (*direction != "ltr") {
            fault(where, "profile direction " + *direction + " is not ltr or rtl");
        }
    }
    return profile;
}

bool same_profile(const Profile& left, const Profile& right) noexcept {
    return left.width == right.width && left.height == right.height &&
           left.scale == right.scale && left.direction == right.direction;
}

bool right_to_left(const Profile& profile) noexcept {
    return profile.direction == LayoutDirection::right_to_left;
}

std::string profile_name(const Profile& profile) {
    std::ostringstream name;
    name << profile.width << 'x' << profile.height << '@' << profile.scale;
    if (right_to_left(profile)) name << "/rtl";
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
                FixtureEntry entry;
                entry.name = *name;
                const std::string* marked = attribute(fixture, "bidirectional");
                if (marked != nullptr) {
                    if (*marked != "true" && *marked != "false") {
                        fault(where, *name + ": bidirectional is true or false");
                    }
                    entry.bidirectional = *marked == "true";
                }
                corpus.fixtures.push_back(std::move(entry));
            }
        } else {
            fault(where, "unknown section <" + section.name + ">");
        }
    }
    if (corpus.profiles.empty() || corpus.fixtures.empty()) {
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
    flow_layout_panel_compat,
    table_layout_panel_compat,
    flex_panel,
    grid_panel,
    block_panel,
};

FixtureKind parse_kind(const std::string& kind, const std::string& where) {
    if (kind == "box") return FixtureKind::box;
    if (kind == "flow-layout-panel-compat") return FixtureKind::flow_layout_panel_compat;
    if (kind == "table-layout-panel-compat") return FixtureKind::table_layout_panel_compat;
    if (kind == "flex-panel") return FixtureKind::flex_panel;
    if (kind == "grid-panel") return FixtureKind::grid_panel;
    if (kind == "block-panel") return FixtureKind::block_panel;
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
        // content-sized tracks, ignoring content: TableLayoutPanelCompat's percent.
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
    // Flex children: the whole item record.
    FlexItemLayout flex_item;
    // Grid children: the whole item record.
    GridItemLayout grid_item;
    // Block flow children: the whole item record.
    BlockFlowItemLayout block_item;
};

// What the layout value reader may set on one element: the container and
// item properties of its own kind and its parent's.
struct ReaderRoles final {
    bool flex_container{};
    bool grid_container{};
    bool flex_item{};
    bool grid_item{};
    bool block_container{};
    bool block_item{};
};

bool reader_kind(FixtureKind kind) {
    return kind == FixtureKind::flex_panel || kind == FixtureKind::grid_panel ||
           kind == FixtureKind::block_panel;
}

// What a block flow panel's line boxes inherit from the fixture's text
// declarations, which are not layout values: the corpus sets inline content
// at `font-size: 0`, so a strut has no font metrics, and a `line-height`
// in px then gives it half that above and half below the baseline.
struct InheritedText final {
    bool zero_font{};
    // Negative for `normal`.
    double line_height{-1.0};
};

// The text declarations taken out of an inline style before the layout
// value reader sees it.
struct TextDeclarations final {
    std::optional<bool> zero_font;
    std::optional<double> line_height;
    std::optional<LayoutDirection> direction;
};

TextDeclarations take_text_declarations(std::string& style, const std::string& where) {
    TextDeclarations taken;
    std::string kept;
    std::size_t start = 0U;
    while (start <= style.size()) {
        std::size_t end = style.find(';', start);
        if (end == std::string::npos) end = style.size();
        const std::string declaration = style.substr(start, end - start);
        start = end + 1U;
        const std::size_t colon = declaration.find(':');
        const std::string_view name =
            colon == std::string::npos ? std::string_view{} : trimmed(std::string_view(declaration).substr(0U, colon));
        const std::string value =
            colon == std::string::npos ? std::string{} : std::string(trimmed(std::string_view(declaration).substr(colon + 1U)));
        if (name == "font-size") {
            if (value != "0" && value != "0px") fault(where, "font-size is admitted only as 0 (the corpus's strut)");
            taken.zero_font = true;
        } else if (name == "line-height") {
            taken.line_height = parse_length(value, where);
        } else if (name == "direction") {
            if (value == "rtl") {
                taken.direction = LayoutDirection::right_to_left;
            } else if (value == "ltr") {
                taken.direction = LayoutDirection::left_to_right;
            } else {
                fault(where, "direction is ltr or rtl");
            }
        } else if (!trimmed(declaration).empty()) {
            kept += declaration;
            kept += ';';
        }
    }
    style = kept;
    return taken;
}

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
        const BuiltElement built = build_element(root, std::nullopt, InheritedText{});
        return built.control;
    }

    // Every built element, in document order. The span borrows this
    // builder's storage, so a temporary builder is refused.
    [[nodiscard]] std::span<const BuiltIdentity> identities() const& noexcept {
        return identities_;
    }
    std::span<const BuiltIdentity> identities() const&& = delete;

private:
    BuiltElement build_element(const XmlElement& element, std::optional<FixtureKind> parent_kind,
                               const InheritedText& inherited) {
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
        std::string layout_style = style_text == nullptr ? std::string{} : *style_text;
        const TextDeclarations text = take_text_declarations(layout_style, where);
        const bool text_declared = text.zero_font || text.line_height || text.direction;
        if (text_declared && kind != FixtureKind::block_panel) {
            fault(where, "font-size, line-height and direction are built on a block-panel only");
        }
        InheritedText own = inherited;
        if (text.zero_font) own.zero_font = true;
        if (text.line_height) own.line_height = *text.line_height;
        if (text.direction) control.set_layout_direction(*text.direction);
        if (style_text != nullptr) style_text = &layout_style;
        const bool reader_context = reader_kind(kind) || (parent_kind && reader_kind(*parent_kind));
        if (reader_context) {
            apply_reader_style(control, kind, parent_kind, style_text, built.layout, where);
        }
        if (kind == FixtureKind::block_panel && own.zero_font) {
            const double half = own.line_height < 0.0 ? 0.0 : own.line_height / 2.0;
            static_cast<BlockPanel&>(control).set_line_strut(LineStrut{half, half, 0.0});
        }
        const std::vector<StyleDeclaration> declarations =
            style_text == nullptr || reader_context ? std::vector<StyleDeclaration>{}
                                                    : read_style(*style_text, where);
        ContainerDefaults defaults;
        std::vector<TableLayoutStyle> columns;
        std::vector<TableLayoutStyle> rows;
        for (const StyleDeclaration& declaration : declarations) {
            const std::string declaration_where = where + " " + declaration.name;
            if (apply_common(control, declaration, built.layout, declaration_where)) continue;
            if (kind == FixtureKind::flow_layout_panel_compat &&
                apply_flow(static_cast<FlowLayoutPanelCompat&>(control), declaration,
                           declaration_where)) {
                continue;
            }
            if (kind == FixtureKind::table_layout_panel_compat &&
                apply_table(static_cast<TableLayoutPanelCompat&>(control), declaration, defaults,
                            columns, rows, declaration_where)) {
                continue;
            }
            if (parent_kind == FixtureKind::flow_layout_panel_compat &&
                apply_flow_child(declaration, built.layout, declaration_where)) {
                continue;
            }
            if (parent_kind == FixtureKind::table_layout_panel_compat &&
                apply_table_child(declaration, built.layout, declaration_where)) {
                continue;
            }
            fault(declaration_where, "declaration \"" + declaration.name + ": " +
                                         declaration.value + "\" is not admitted here");
        }

        if (kind == FixtureKind::table_layout_panel_compat) {
            configure_table(static_cast<TableLayoutPanelCompat&>(control), columns, rows, where);
        }
        for (const XmlElement& child_element : element.children) {
            const BuiltElement child = build_element(child_element, kind, own);
            control.add_child(child.control);
            if (kind == FixtureKind::flow_layout_panel_compat && child.layout.flow_grow != 0.0) {
                static_cast<FlowLayoutPanelCompat&>(control).set_flex_grow(*child.control,
                                                                     child.layout.flow_grow);
            }
            if (kind == FixtureKind::table_layout_panel_compat) {
                place_table_child(static_cast<TableLayoutPanelCompat&>(control), child, defaults,
                                  where);
            }
            if (kind == FixtureKind::flex_panel) {
                static_cast<FlexPanel&>(control).set_item_layout(*child.control,
                                                                       child.layout.flex_item);
            }
            if (kind == FixtureKind::grid_panel) {
                try {
                    static_cast<GridPanel&>(control).set_item_layout(*child.control,
                                                                           child.layout.grid_item);
                } catch (const std::invalid_argument& error) {
                    fault(where, std::string("GridPanel refused an item layout: ") + error.what());
                }
            }
            if (kind == FixtureKind::block_panel) {
                if (child.layout.block_item.level == BlockFlowLevel::inline_level && !own.zero_font) {
                    fault(where, "inline content needs font-size: 0 (the corpus's strut has no font metrics)");
                }
                try {
                    static_cast<BlockPanel&>(control).set_item_layout(*child.control,
                                                                          child.layout.block_item);
                } catch (const std::invalid_argument& error) {
                    fault(where, std::string("BlockPanel refused an item layout: ") + error.what());
                }
            }
        }
        return built;
    }

    // A flex or grid container's, or a flex or grid item's, whole inline
    // style through the layout value reader. On a fault the fixture is
    // abandoned, so the partly configured control is never laid out.
    void apply_reader_style(Control& control, FixtureKind kind, std::optional<FixtureKind> parent_kind,
                            const std::string* style_text, ChildLayout& child, const std::string& where) {
        LayoutStyle base{};
        base.display = DisplayKind::block;
        LayoutStyle style = base;
        if (style_text != nullptr) {
            diagnostics_.clear();
            const gui_forms::theme_language::ThemeSource source{where, *style_text};
            const gui_forms::layout_style::StyleReadOutcome outcome =
                style_reader_.read_inline_style(source, style, diagnostics_);
            if (outcome != gui_forms::layout_style::StyleReadOutcome::applied ||
                !diagnostics_.records().empty()) {
                std::string reported;
                for (const gui_forms::theme_language::ThemeDiagnostic& diagnostic : diagnostics_.records()) {
                    reported += " ";
                    reported += gui_forms::theme_language::diagnostic_code_label(diagnostic.code);
                    reported += " ";
                    reported += diagnostic.property;
                    reported += ": ";
                    reported += diagnostic.detail;
                }
                fault(where, "the layout value reader reported" + reported);
            }
        }
        ReaderRoles roles{};
        roles.flex_container = kind == FixtureKind::flex_panel;
        roles.grid_container = kind == FixtureKind::grid_panel;
        roles.flex_item = parent_kind == FixtureKind::flex_panel;
        roles.grid_item = parent_kind == FixtureKind::grid_panel;
        roles.block_container = kind == FixtureKind::block_panel;
        roles.block_item = parent_kind == FixtureKind::block_panel;
        refuse_unbuilt_values(style, base, roles, where);

        // Margins: a length, or `auto` on an item's side.
        const BoxSides<AutoLengthPercentage>& margin = style.margin;
        const BoxSides<bool> automatic{margin.top.automatic, margin.right.automatic, margin.bottom.automatic,
                                       margin.left.automatic};
        child.flex_item.auto_margin = automatic;
        child.grid_item.auto_margin = automatic;
        child.block_item.auto_margin = automatic;
        Insets margins{};
        margins.left = margin_length(margin.left, where);
        margins.top = margin_length(margin.top, where);
        margins.right = margin_length(margin.right, where);
        margins.bottom = margin_length(margin.bottom, where);
        control.set_margin(margins);
        const BoxSides<LengthPercentage>& padding = style.padding;
        if (padding.top.percent != 0.0 || padding.right.percent != 0.0 || padding.bottom.percent != 0.0 ||
            padding.left.percent != 0.0) {
            fault(where, "a percentage padding is not built by the corpus harness yet");
        }
        Insets paddings{};
        paddings.left = padding.left.length.value();
        paddings.top = padding.top.length.value();
        paddings.right = padding.right.length.value();
        paddings.bottom = padding.bottom.length.value();
        control.set_padding(paddings);

        if (roles.flex_container) {
            FlexPanel& panel = static_cast<FlexPanel&>(control);
            panel.set_flex_direction(style.flex.direction);
            panel.set_flex_wrap(style.flex.wrap);
            panel.set_justify_content(style.alignment.justify_content);
            panel.set_align_content(style.alignment.align_content);
            panel.set_align_items(style.alignment.align_items);
            panel.set_row_gap(style.row_gap);
            panel.set_column_gap(style.column_gap);
        }
        if (roles.grid_container) {
            configure_grid(static_cast<GridPanel&>(control), style, where);
        }
        if (roles.block_container) {
            static_cast<BlockPanel&>(control).set_text_align(style.text_align);
        }
        if (roles.block_item) {
            BlockFlowItemLayout& layout = child.block_item;
            layout.level = style.display == DisplayKind::inline_block ? BlockFlowLevel::inline_level
                                                                      : BlockFlowLevel::block;
            layout.width = style.width;
            layout.height = style.height;
            layout.min_width = style.min_width;
            layout.min_height = style.min_height;
            layout.max_width = style.max_width;
            layout.max_height = style.max_height;
            layout.vertical_align = style.vertical_align;
            layout.positioning = ItemPositioning{style.position, style.inset};
        }
        if (roles.flex_item) {
            FlexItemLayout& layout = child.flex_item;
            layout.flex = style.flex_item;
            layout.align_self = style.alignment.align_self;
            layout.width = style.width;
            layout.height = style.height;
            layout.min_width = style.min_width;
            layout.min_height = style.min_height;
            layout.max_width = style.max_width;
            layout.max_height = style.max_height;
            layout.positioning = ItemPositioning{style.position, style.inset};
        }
        if (roles.grid_item) {
            GridItemLayout& layout = child.grid_item;
            layout.placement = style.grid_item;
            layout.justify_self = style.alignment.justify_self;
            layout.align_self = style.alignment.align_self;
            layout.width = style.width;
            layout.height = style.height;
            layout.min_width = style.min_width;
            layout.min_height = style.min_height;
            layout.max_width = style.max_width;
            layout.max_height = style.max_height;
            layout.positioning = ItemPositioning{style.position, style.inset};
        }
    }

    // A grid container's properties; a value the panel refuses abandons
    // the fixture.
    static void configure_grid(GridPanel& panel, const LayoutStyle& style, const std::string& where) {
        try {
            panel.set_template_columns(style.grid.template_columns);
            panel.set_template_rows(style.grid.template_rows);
            panel.set_template_areas(style.grid.template_areas);
            panel.set_auto_flow(style.grid.auto_flow);
            panel.set_auto_columns(style.grid.auto_columns);
            panel.set_auto_rows(style.grid.auto_rows);
            panel.set_justify_content(style.alignment.justify_content);
            panel.set_align_content(style.alignment.align_content);
            panel.set_justify_items(style.alignment.justify_items);
            panel.set_align_items(style.alignment.align_items);
            panel.set_row_gap(style.row_gap);
            panel.set_column_gap(style.column_gap);
        } catch (const std::invalid_argument& error) {
            fault(where, std::string("GridPanel refused a container value: ") + error.what());
        }
    }

    static double margin_length(const AutoLengthPercentage& side, const std::string& where) {
        if (side.automatic) return 0.0;
        if (side.value.percent != 0.0) fault(where, "a percentage margin is not built by the corpus harness yet");
        return side.value.length.value();
    }

    // The values a flex or grid fixture may not use: those of a layout not
    // built yet, and container or item values on an element that is not
    // one.
    static void refuse_unbuilt_values(const LayoutStyle& style, const LayoutStyle& base, const ReaderRoles& roles,
                                      const std::string& where) {
        if (!roles.block_container && style.text_align != base.text_align) {
            fault(where, "text-align on an element that is not a block-panel");
        }
        if (!roles.block_item && style.vertical_align != base.vertical_align) {
            fault(where, "vertical-align on an element that is not a block flow item");
        }
        const bool container = roles.flex_container || roles.grid_container;
        const bool item = roles.flex_item || roles.grid_item || roles.block_item;
        if (roles.flex_container && style.display != DisplayKind::flex) {
            fault(where, "a flex-panel is display: flex");
        }
        if (roles.grid_container && style.display != DisplayKind::grid) {
            fault(where, "a grid-panel is display: grid");
        }
        const bool inline_block = roles.block_item && style.display == DisplayKind::inline_block;
        if (inline_block && (roles.flex_container || roles.grid_container)) {
            fault(where, "inline-flex and inline-grid are not built by the corpus harness yet");
        }
        if (!container && style.display != DisplayKind::block && !inline_block) {
            fault(where, "only a flex-panel or a grid-panel changes display, and a block flow "
                         "item to inline-block");
        }
        if (!roles.flex_container && !(style.flex == base.flex)) {
            fault(where, "flex container values on an element that is not one");
        }
        const bool grid_values = !(style.grid == base.grid) ||
                                 style.alignment.justify_items != base.alignment.justify_items;
        if (!roles.grid_container && grid_values) {
            fault(where, "grid container values on an element that is not one");
        }
        const bool container_values = !(style.row_gap == base.row_gap) || !(style.column_gap == base.column_gap) ||
                                      style.alignment.justify_content != base.alignment.justify_content ||
                                      style.alignment.align_content != base.alignment.align_content ||
                                      style.alignment.align_items != base.alignment.align_items;
        if (!container && container_values) {
            fault(where, "container values on an element that is not a container");
        }
        const bool sized = !(style.width == base.width) || !(style.height == base.height) ||
                           !(style.min_width == base.min_width) || !(style.min_height == base.min_height) ||
                           !(style.max_width == base.max_width) || !(style.max_height == base.max_height);
        if (!item && (sized || style.alignment.align_self != base.alignment.align_self)) {
            fault(where, "item values on an element that is not an item (the root fills the window)");
        }
        if (!roles.flex_item && !(style.flex_item == base.flex_item)) {
            fault(where, "flex item values on an element that is not one");
        }
        const bool grid_item_values = !(style.grid_item == base.grid_item) ||
                                      style.alignment.justify_self != base.alignment.justify_self;
        if (!roles.grid_item && grid_item_values) {
            fault(where, "grid item values on an element that is not one");
        }
        const bool positioned = style.position != base.position || !(style.inset == base.inset);
        if (!item && positioned) fault(where, "position and insets on an element that is not an item");
        // An absolute item with both insets `auto` on an axis is placed at
        // its static position, which the containers take as the content
        // box's start (a recorded limit), not CSS's in-flow position.
        const BoxSides<AutoLengthPercentage>& inset = style.inset;
        const bool static_axis = (inset.left.automatic && inset.right.automatic) ||
                                 (inset.top.automatic && inset.bottom.automatic);
        if (style.position == PositionKind::absolute && static_axis) {
            fault(where, "an absolute item with both insets auto on an axis (its static position) is not compared");
        }
        // The containers take percentages of their content box, and an
        // absolute item's containing block whole (recorded limits): CSS
        // takes a grid item's area, and a block's height only when it is
        // definite.
        if (style.position == PositionKind::absolute && roles.grid_item && !(style.grid_item == base.grid_item)) {
            fault(where, "grid placement on an absolute item is not built");
        }
        const bool percent_inset = inset.top.value.percent != 0.0 || inset.right.value.percent != 0.0 ||
                                   inset.bottom.value.percent != 0.0 || inset.left.value.percent != 0.0;
        const bool percent_vertical = inset.top.value.percent != 0.0 || inset.bottom.value.percent != 0.0;
        if (style.position == PositionKind::relative && roles.grid_item && percent_inset) {
            fault(where, "a relative grid item's percentage inset (of its grid area in CSS) is not built");
        }
        if (style.position == PositionKind::relative && roles.block_item && percent_vertical) {
            fault(where, "a relative block flow item's vertical percentage inset is not built");
        }
        const BoxSides<AutoLengthPercentage>& margin = style.margin;
        const bool automatic_margin =
            margin.top.automatic || margin.right.automatic || margin.bottom.automatic || margin.left.automatic;
        if (!item && automatic_margin) fault(where, "an auto margin outside a flex, grid or block flow container");
    }

    static Control::Ptr make_kind(FixtureKind kind, const std::string& identity) {
        switch (kind) {
        case FixtureKind::box:
            return make_control<Panel>(StableId(identity));
        case FixtureKind::flow_layout_panel_compat:
            return make_control<FlowLayoutPanelCompat>(StableId(identity));
        case FixtureKind::table_layout_panel_compat:
            return make_control<TableLayoutPanelCompat>(StableId(identity));
        case FixtureKind::flex_panel:
            return make_control<FlexPanel>(StableId(identity));
        case FixtureKind::grid_panel:
            return make_control<GridPanel>(StableId(identity));
        case FixtureKind::block_panel:
            return make_control<BlockPanel>(StableId(identity));
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

    static bool apply_flow(FlowLayoutPanelCompat& flow, const StyleDeclaration& declaration,
                           const std::string& where) {
        const std::string& value = declaration.value;
        if (declaration.name == "display") {
            if (value != "flex") fault(where, "a flow-layout-panel-compat is display: flex");
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
            else fault(where, "FlowLayoutPanelCompat has no " + value);
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
            // FlowLayoutPanelCompat packs its lines at the start.
            if (value != "flex-start" && value != "start") {
                fault(where, "FlowLayoutPanelCompat packs lines at the start only");
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
                fault(where, "FlowLayoutPanelCompat admits flex: none or flex: <grow> 0 auto");
            }
            grow = parse_double(components[0], where);
            return true;
        }
        if (declaration.name == "flex-grow") {
            grow = parse_double(value, where);
            return true;
        }
        if (declaration.name == "flex-shrink") {
            if (value != "0") fault(where, "FlowLayoutPanelCompat never shrinks items");
            return true;
        }
        if (declaration.name == "flex-basis") {
            if (value != "auto") fault(where, "FlowLayoutPanelCompat items have basis auto");
            return true;
        }
        return false;
    }

    static bool apply_table(TableLayoutPanelCompat& table, const StyleDeclaration& declaration,
                            ContainerDefaults& defaults, std::vector<TableLayoutStyle>& columns,
                            std::vector<TableLayoutStyle>& rows, const std::string& where) {
        const std::string& value = declaration.value;
        if (declaration.name == "display") {
            if (value != "grid") fault(where, "a table-layout-panel-compat is display: grid");
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
            // TableLayoutPanelCompat packs its tracks at the start; CSS's default
            // (normal) stretches auto tracks into free space, which a
            // fixture records as a difference where it shows.
            if (value != "start" && value != "normal") {
                fault(where, "TableLayoutPanelCompat packs tracks at the start only");
            }
            return true;
        }
        if (declaration.name == "grid-auto-flow") {
            if (value == "row") table.set_grow_style(TableLayoutGrowStyle::add_rows);
            else if (value == "column") table.set_grow_style(TableLayoutGrowStyle::add_columns);
            else fault(where, "TableLayoutPanelCompat grows by rows or columns only");
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

    static void configure_table(TableLayoutPanelCompat& table, const std::vector<TableLayoutStyle>& columns,
                                const std::vector<TableLayoutStyle>& rows, const std::string& where) {
        if (columns.empty()) fault(where, "a table-layout-panel-compat needs grid-template-columns");
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

    static void place_table_child(TableLayoutPanelCompat& table, const BuiltElement& child,
                                  const ContainerDefaults& defaults, const std::string& where) {
        const ChildLayout& layout = child.layout;
        Control& control = *child.control;
        if (layout.column.start_line.has_value() != layout.row.start_line.has_value()) {
            fault(where, "TableLayoutPanelCompat places a child on both axes or neither");
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
    // Reused for every flex and grid element of the fixture.
    gui_forms::layout_style::LayoutStyleReader style_reader_;
    gui_forms::theme_language::ThemeDiagnosticList diagnostics_;
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
    std::uint64_t profiles{};
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
                 const FixtureEntry& entry, const std::optional<std::filesystem::path>& native_out,
                 FixtureTally& tally) {
    const std::string& name = entry.name;
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
        if (right_to_left(profile) && !entry.bidirectional) continue;
        ++tally.profiles;
        const std::vector<ProfileBoxes>::const_iterator reference = std::find_if(
            references.begin(), references.end(), ProfileMatches{profile});
        if (reference == references.end()) {
            std::cout << name << " " << profile_name(profile) << ": no reference profile\n";
            passed = false;
            continue;
        }
        FixtureBuilder builder(name);
        const Control::Ptr root = builder.build(fixture);
        if (right_to_left(profile)) (*root).set_layout_direction(LayoutDirection::right_to_left);
        const std::unique_ptr<Window> held_window = std::make_unique<Window>(
            root, Size{static_cast<double>(profile.width), static_cast<double>(profile.height)});
        Window& window = *held_window;
        window.set_scale(profile.scale);
        window.perform_layout();
        native << "  <profile viewport=\"" << profile.width << 'x' << profile.height
               << "\" scale=\"" << format_number(profile.scale) << '"'
               << (right_to_left(profile) ? " direction=\"rtl\"" : "") << ">\n";
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
        for (const FixtureEntry& entry : corpus.fixtures) {
            FixtureTally tally;
            const bool fixture_passed = run_fixture(corpus_root, corpus, entry, native_out, tally);
            std::cout << entry.name << ": " << tally.profiles << " profiles, " << tally.nodes
                      << " nodes, largest edge difference "
                      << tally.largest_edge << ", " << tally.over_tolerance
                      << " over tolerance, " << tally.recorded << " recorded\n";
            passed = passed && fixture_passed;
            total.profiles += tally.profiles;
            total.nodes += tally.nodes;
            total.over_tolerance += tally.over_tolerance;
            total.recorded += tally.recorded;
            total.largest_edge = std::max(total.largest_edge, tally.largest_edge);
        }
        std::cout << "corpus: " << corpus.fixtures.size() << " fixtures, "
                  << total.profiles << " fixture profiles, " << total.nodes << " nodes, "
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
