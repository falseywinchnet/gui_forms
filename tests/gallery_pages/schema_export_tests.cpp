#include "support.hpp"

#include "gui_forms/schema.hpp"

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <span>
#include <string_view>

namespace gallery_pages_tests {
namespace {

// The control schema (stage T1): every registry type's parts, states, item
// systems, properties and interactions, read from a laid-out Gallery and
// from directly constructed components, written as one JSON document. The
// committed copy is manifests/control-schema.json; CMake passes its path in
// GUI_FORMS_CONTROL_SCHEMA. The test fails when the export and the
// committed copy differ, and GUI_FORMS_CONTROL_SCHEMA_WRITE=1 rewrites the
// committed copy instead.

// One registry type as the export records it. Controls carry an anatomy and
// properties; components carry interactions only.
struct TypeRecord final {
    const ControlAnatomy* anatomy{};
    std::vector<PropertyDescriptor> properties;
    std::vector<InteractionDeclaration> interactions;
    bool seen{};
};

std::string_view value_kind_name(BindingValueKind kind) {
    switch (kind) {
    case BindingValueKind::null: return "null";
    case BindingValueKind::boolean: return "boolean";
    case BindingValueKind::signed_integer: return "signed_integer";
    case BindingValueKind::unsigned_integer: return "unsigned_integer";
    case BindingValueKind::number: return "number";
    case BindingValueKind::text: return "text";
    case BindingValueKind::point: return "point";
    case BindingValueKind::size: return "size";
    case BindingValueKind::rectangle: return "rectangle";
    case BindingValueKind::insets: return "insets";
    case BindingValueKind::color: return "color";
    case BindingValueKind::font: return "font";
    case BindingValueKind::image: return "image";
    case BindingValueKind::enumeration: return "enumeration";
    case BindingValueKind::object: return "object";
    case BindingValueKind::collection: return "collection";
    }
    return "unknown";
}

std::string_view serialization_name(PropertySerializationVisibility visibility) {
    switch (visibility) {
    case PropertySerializationVisibility::hidden: return "hidden";
    case PropertySerializationVisibility::visible: return "visible";
    case PropertySerializationVisibility::content: return "content";
    }
    return "unknown";
}

// Appends `text` as a JSON string literal.
void append_json_string(std::string& out, std::string_view text) {
    out += '"';
    for (std::size_t index = 0; index < text.size(); ++index) {
        const char character = text[index];
        if (character == '"' || character == '\\') {
            out += '\\';
            out += character;
        } else if (character == '\n') {
            out += "\\n";
        } else if (static_cast<unsigned char>(character) < 0x20U) {
            constexpr std::string_view hex = "0123456789abcdef";
            const unsigned int code = static_cast<unsigned char>(character);
            out += "\\u00";
            out += hex[code >> 4U];
            out += hex[code & 0xFU];
        } else {
            out += character;
        }
    }
    out += '"';
}

void append_key(std::string& out, std::string_view indent, std::string_view key) {
    out += indent;
    append_json_string(out, key);
    out += ": ";
}

void append_bool(std::string& out, bool value) {
    out += value ? "true" : "false";
}

void append_state_list(std::string& out, std::span<const AnatomyState> states) {
    out += '[';
    for (std::size_t index = 0; index < states.size(); ++index) {
        if (index > 0U) out += ", ";
        append_json_string(out, anatomy_state_name(states[index]));
    }
    out += ']';
}

void append_name_list(std::string& out, std::span<const std::string_view> names) {
    out += '[';
    for (std::size_t index = 0; index < names.size(); ++index) {
        if (index > 0U) out += ", ";
        append_json_string(out, names[index]);
    }
    out += ']';
}

// The atom vocabulary (design language 8.5 and 12.7): every part role,
// state, admitted custom state, mark, value type, recipe family, semantic
// role, semantic action and semantic node field, in enum order. `none` is
// the absence of a mark, not a mark, and is left out. Values and node fields
// have no enum; they are listed here in the order they were admitted.
void append_vocabulary(std::string& out) {
    std::vector<std::string_view> names;
    out += "  \"vocabulary\": {\n";
    names.clear();
    for (std::size_t index = 0; index < part_role_count; ++index) {
        names.push_back(part_role_name(static_cast<PartRole>(index)));
    }
    append_key(out, "    ", "part_roles");
    append_name_list(out, names);
    out += ",\n";
    names.clear();
    for (std::size_t index = 0; index < anatomy_state_count; ++index) {
        names.push_back(anatomy_state_name(static_cast<AnatomyState>(index)));
    }
    append_key(out, "    ", "states");
    append_name_list(out, names);
    out += ",\n";
    names.clear();
    for (std::size_t index = 0; index < admitted_custom_states.size(); ++index) {
        names.push_back(admitted_custom_states[index].name);
    }
    append_key(out, "    ", "custom_states");
    append_name_list(out, names);
    out += ",\n";
    names.clear();
    for (std::size_t index = 1; index < part_mark_count; ++index) {
        names.push_back(part_mark_name(static_cast<PartMark>(index)));
    }
    append_key(out, "    ", "marks");
    append_name_list(out, names);
    out += ",\n";
    names.clear();
    names.push_back("accent_index");
    names.push_back("text_orientation");
    names.push_back("dock_region");
    append_key(out, "    ", "values");
    append_name_list(out, names);
    out += ",\n";
    names.clear();
    for (std::size_t index = 0; index < control_visual_role_count; ++index) {
        names.push_back(control_visual_role_name(static_cast<ControlVisualRole>(index)));
    }
    append_key(out, "    ", "families");
    append_name_list(out, names);
    out += ",\n";
    names.clear();
    for (std::size_t index = 0; index < semantic_role_count; ++index) {
        names.push_back(semantic_role_name(static_cast<SemanticRole>(index)));
    }
    append_key(out, "    ", "semantic_roles");
    append_name_list(out, names);
    out += ",\n";
    names.clear();
    for (std::size_t index = 0; index < semantic_action_count; ++index) {
        names.push_back(semantic_action_name(static_cast<SemanticAction>(index)));
    }
    append_key(out, "    ", "semantic_actions");
    append_name_list(out, names);
    out += ",\n";
    names.clear();
    names.push_back("position_in_set");
    names.push_back("set_size");
    names.push_back("has_popup");
    names.push_back("controlled_panel");
    names.push_back("orientation");
    append_key(out, "    ", "semantic_fields");
    append_name_list(out, names);
    out += "\n  },\n";
}

void append_custom_state_list(std::string& out, std::span<const CustomStateDeclaration> states) {
    out += '[';
    for (std::size_t index = 0; index < states.size(); ++index) {
        if (index > 0U) out += ", ";
        append_json_string(out, states[index].name);
    }
    out += ']';
}

// One part per line: name, role, the system it reuses and its own states.
void append_parts(std::string& out, std::string_view indent,
                  std::span<const PartDeclaration> parts) {
    out += "[\n";
    for (std::size_t index = 0; index < parts.size(); ++index) {
        const PartDeclaration& part = parts[index];
        out += indent;
        out += "  {\"name\": ";
        append_json_string(out, part.name);
        out += ", \"role\": ";
        append_json_string(out, part_role_name(part.role));
        out += ", \"composed_from\": ";
        if (part.composed_from.empty()) {
            out += "null";
        } else {
            append_json_string(out, part.composed_from);
        }
        out += ", \"states\": ";
        append_state_list(out, part.states);
        out += index + 1U < parts.size() ? "},\n" : "}\n";
    }
    out += indent;
    out += ']';
}

void append_item_systems(std::string& out, std::span<const ItemSystemDeclaration> systems) {
    out += "[";
    for (std::size_t index = 0; index < systems.size(); ++index) {
        const ItemSystemDeclaration& system = systems[index];
        out += index == 0U ? "\n" : ",\n";
        out += "        {\n";
        append_key(out, "          ", "name");
        append_json_string(out, system.name);
        out += ",\n";
        append_key(out, "          ", "parts");
        append_parts(out, "          ", system.parts);
        out += ",\n";
        append_key(out, "          ", "states");
        append_state_list(out, system.states);
        out += ",\n";
        append_key(out, "          ", "custom_states");
        append_custom_state_list(out, system.custom_states);
        out += "\n        }";
    }
    out += systems.empty() ? "]" : "\n      ]";
}

void append_properties(std::string& out, std::span<const PropertyDescriptor> properties) {
    out += "[";
    for (std::size_t index = 0; index < properties.size(); ++index) {
        const PropertyDescriptor& property = properties[index];
        out += index == 0U ? "\n" : ",\n";
        out += "        {\"name\": ";
        append_json_string(out, property.name);
        out += ", \"kind\": ";
        append_json_string(out, value_kind_name(property.kind));
        out += ", \"category\": ";
        append_json_string(out, property.category);
        out += ", \"enumeration\": ";
        if (property.enumeration == nullptr) {
            out += "null";
        } else {
            append_json_string(out, (*property.enumeration).type_name);
        }
        // The values a document may write for an enumeration.
        out += ", \"choices\": [";
        if (property.enumeration != nullptr) {
            const std::vector<PropertyEnumChoice>& choices = (*property.enumeration).choices;
            for (std::size_t choice = 0; choice < choices.size(); ++choice) {
                if (choice > 0U) out += ", ";
                append_json_string(out, choices[choice].name);
            }
        }
        out += "]";
        out += ", \"readable\": ";
        append_bool(out, property.readable);
        out += ", \"writable\": ";
        append_bool(out, property.writable);
        out += ", \"bindable\": ";
        append_bool(out, property.bindable);
        out += ", \"nullable\": ";
        append_bool(out, property.nullable);
        out += ", \"serialization\": ";
        append_json_string(out, serialization_name(property.serialization_visibility));
        out += "}";
    }
    out += properties.empty() ? "]" : "\n      ]";
}

void append_interactions(std::string& out, std::span<const InteractionDeclaration> interactions) {
    out += "[";
    for (std::size_t index = 0; index < interactions.size(); ++index) {
        const InteractionDeclaration& interaction = interactions[index];
        out += index == 0U ? "\n" : ",\n";
        out += "        {\"name\": ";
        append_json_string(out, interaction.name);
        out += ", \"payload\": ";
        append_json_string(out, interaction.payload);
        out += ", \"mutable_payload\": ";
        append_bool(out, interaction.mutable_payload);
        out += "}";
    }
    out += interactions.empty() ? "]" : "\n      ]";
}

// Keeps only the entries of `kept` whose names `other` also has, in
// `kept`'s order. A Gallery-local subclass reports its base type's anatomy
// id but may add properties or events; the type's own schema is what every
// instance shares.
void keep_common_properties(std::vector<PropertyDescriptor>& kept,
                            std::span<const PropertyDescriptor> other) {
    std::vector<PropertyDescriptor> common;
    common.reserve(kept.size());
    for (std::size_t index = 0; index < kept.size(); ++index) {
        bool shared = false;
        for (std::size_t position = 0; position < other.size(); ++position) {
            if (other[position].name == kept[index].name) shared = true;
        }
        if (shared) common.push_back(kept[index]);
    }
    kept.swap(common);
}

void keep_common_interactions(std::vector<InteractionDeclaration>& kept,
                              std::span<const InteractionDeclaration> other) {
    std::vector<InteractionDeclaration> common;
    common.reserve(kept.size());
    for (std::size_t index = 0; index < kept.size(); ++index) {
        if (find_interaction(other, kept[index].name) < other.size()) {
            common.push_back(kept[index]);
        }
    }
    kept.swap(common);
}

std::size_t catalog_position(std::string_view id) {
    for (std::size_t index = 0; index < gallery::control_catalog.size(); ++index) {
        if (gallery::control_catalog[index].id == id) return index;
    }
    return gallery::control_catalog.size();
}

void record_control(const Control& control, std::vector<TypeRecord>& records) {
    const ControlAnatomy& anatomy = control.anatomy();
    const std::size_t position = catalog_position(anatomy.control_id);
    // Gallery-private types publish the base anatomy ("control").
    if (position >= records.size()) return;
    TypeRecord& record = records[position];
    const std::vector<PropertyDescriptor> properties = control.property_descriptors();
    const std::vector<InteractionDeclaration> interactions = control.interactions();
    if (!record.seen) {
        record.anatomy = &anatomy;
        record.properties = properties;
        record.interactions = interactions;
        record.seen = true;
        return;
    }
    keep_common_properties(record.properties, properties);
    keep_common_interactions(record.interactions, interactions);
}

void record_tree(const Control::Ptr& root, std::vector<TypeRecord>& records) {
    std::vector<Control::Ptr> pending{root};
    while (!pending.empty()) {
        const Control::Ptr control = pending.back();
        pending.pop_back();
        record_control(*control, records);
        const std::span<const Control::Ptr> children = (*control).children();
        for (std::size_t index = 0; index < children.size(); ++index) {
            pending.push_back(children[index]);
        }
    }
}

void record_component(std::string_view id, const Component& component,
                      std::vector<TypeRecord>& records) {
    const std::size_t position = catalog_position(id);
    require(position < records.size(), "every exported component is in the registry");
    TypeRecord& record = records[position];
    record.interactions = component.interactions();
    record.seen = true;
}

std::string render_schema(std::span<const TypeRecord> records) {
    std::string out;
    out += "{\n";
    out += "  \"schema\": 2,\n";
    out += "  \"notes\": [\n";
    out += "    \"Generated by gui_forms_gallery_pages_tests from the run-time registry "
           "(stage T1): set GUI_FORMS_CONTROL_SCHEMA_WRITE=1 and run that test to rewrite it. "
           "Do not edit.\",\n";
    out += "    \"Controls list their parts, states, item systems, properties and interactions; "
           "components list their interactions. Every name is stored in snake_case; CSS-family "
           "documents write it in kebab-case and displays in PascalCase.\"\n";
    out += "  ],\n";
    append_vocabulary(out);
    out += "  \"types\": [";
    for (std::size_t index = 0; index < records.size(); ++index) {
        const TypeRecord& record = records[index];
        const gallery::CatalogEntry& entry = gallery::control_catalog[index];
        out += index == 0U ? "\n" : ",\n";
        out += "    {\n";
        append_key(out, "      ", "id");
        append_json_string(out, entry.id);
        out += ",\n";
        append_key(out, "      ", "type");
        append_json_string(out, entry.type);
        out += ",\n";
        append_key(out, "      ", "kind");
        if (record.anatomy != nullptr) {
            append_json_string(out, "control");
        } else if (record.seen) {
            append_json_string(out, "component");
        } else {
            append_json_string(out, "service");
        }
        if (record.anatomy != nullptr) {
            const ControlAnatomy& anatomy = *record.anatomy;
            out += ",\n";
            append_key(out, "      ", "parts");
            append_parts(out, "      ", anatomy.parts);
            out += ",\n";
            append_key(out, "      ", "states");
            append_state_list(out, anatomy.states);
            out += ",\n";
            append_key(out, "      ", "custom_states");
            append_custom_state_list(out, anatomy.custom_states);
            out += ",\n";
            append_key(out, "      ", "item_systems");
            append_item_systems(out, anatomy.item_systems);
            out += ",\n";
            append_key(out, "      ", "properties");
            append_properties(out, record.properties);
        }
        out += ",\n";
        append_key(out, "      ", "interactions");
        append_interactions(out, record.interactions);
        out += "\n    }";
    }
    out += "\n  ]\n}\n";
    return out;
}

// The committed file's text without carriage returns, which a Windows
// checkout may add.
std::string read_committed(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};
    const std::string raw((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string text;
    text.reserve(raw.size());
    for (std::size_t index = 0; index < raw.size(); ++index) {
        if (raw[index] != '\r') text += raw[index];
    }
    return text;
}

} // namespace

void test_schema_export() {
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    std::vector<TypeRecord> records(gallery::control_catalog.size());
    for (const gallery::GalleryPageSpec& page : gallery::gallery_pages) {
        require(gallery::select_gallery_page(window, page.id),
                "every Gallery page must be selectable");
        window.perform_layout();
        record_tree(window.root(), records);
    }
    // Registry controls the Gallery does not attach to its root tree, as in
    // the anatomy coverage test.
    const std::shared_ptr<ButtonBase> button_base =
        make_control<ButtonBase>(StableId("schema.button-base"), "Base");
    const std::shared_ptr<ScrollBar> scroll_bar =
        make_control<ScrollBar>(StableId("schema.scroll-bar"));
    const std::shared_ptr<AnchoredPopupLayer> popup_layer = make_control<AnchoredPopupLayer>(
        StableId("schema.anchored-popup-layer"), button_base, AnchoredPopupPlacement{});
    const std::shared_ptr<RangeControl> range_control =
        make_control<RangeControl>(StableId("schema.range-control"));
    const std::shared_ptr<TaskDialog> task_dialog = make_control<TaskDialog>(StableId("schema.task-dialog"));
    record_control(*button_base, records);
    record_control(*scroll_bar, records);
    record_control(*popup_layer, records);
    record_control(*range_control, records);
    // A dialog opens on demand, outside the root tree.
    const std::shared_ptr<FontDialog> font_dialog = make_control<FontDialog>(StableId("schema.font-dialog"));
    record_control(*font_dialog, records);
    record_control(*task_dialog, records);

    // Components, constructed directly.
    const std::shared_ptr<Command> command = std::make_shared<Command>();
    const std::shared_ptr<GalleryCommand> gallery_command = std::make_shared<GalleryCommand>(command);
    const std::shared_ptr<ContextMenu> context_menu =
        std::make_shared<ContextMenu>("schema.context-menu");
    const std::shared_ptr<ToolTip> tool_tip = std::make_shared<ToolTip>(window);
    const std::shared_ptr<RibbonToolTip> ribbon_tool_tip = std::make_shared<RibbonToolTip>(window);
    const std::shared_ptr<Timer> timer =
        std::make_shared<Timer>(window, std::chrono::milliseconds(1000));
    const std::shared_ptr<BackgroundWorker> background_worker = std::make_shared<BackgroundWorker>(window);
    const std::shared_ptr<ImageList> image_list = std::make_shared<ImageList>(window);
    const std::shared_ptr<NotifyIcon> notify_icon = std::make_shared<NotifyIcon>(window);
    const std::shared_ptr<ErrorProvider> error_provider = std::make_shared<ErrorProvider>(window);
    const std::shared_ptr<HelpProvider> help_provider = std::make_shared<HelpProvider>(window);
    const std::shared_ptr<SemanticFeedback> feedback = std::make_shared<SemanticFeedback>(window);
    const std::shared_ptr<BindingSource> binding_source = std::make_shared<BindingSource>(window);
    const std::shared_ptr<BindingContext> binding_context = std::make_shared<BindingContext>(window);
    const std::shared_ptr<Label> bound = make_control<Label>(StableId("schema.bound"));
    const std::shared_ptr<Binding> binding =
        (*bound).data_bindings().add("Text", binding_source, "Name");
    require(binding != nullptr, "a binding to an empty source is created");
    record_component("command", *command, records);
    record_component("gallery_command", *gallery_command, records);
    record_component("context_menu", *context_menu, records);
    record_component("tool_tip", *tool_tip, records);
    record_component("ribbon_tool_tip", *ribbon_tool_tip, records);
    record_component("timer", *timer, records);
    record_component("background_worker", *background_worker, records);
    record_component("notify_icon", *notify_icon, records);
    record_component("image_list", *image_list, records);
    record_component("error_provider", *error_provider, records);
    record_component("help_provider", *help_provider, records);
    record_component("semantic_feedback", *feedback, records);
    record_component("binding", *binding, records);
    record_component("binding_source", *binding_source, records);
    record_component("binding_context", *binding_context, records);

    std::size_t services = 0;
    for (std::size_t index = 0; index < records.size(); ++index) {
        if (!records[index].seen) ++services;
    }
    require(services == 1U, "every registry type but the LiveSurface service is exported");
    const std::string schema = render_schema(records);

    const char* committed_path = std::getenv("GUI_FORMS_CONTROL_SCHEMA");
    if (committed_path == nullptr) {
        std::cout << "control schema: " << records.size()
                  << " types exported; GUI_FORMS_CONTROL_SCHEMA unset, not compared\n";
        return;
    }
    const char* write = std::getenv("GUI_FORMS_CONTROL_SCHEMA_WRITE");
    if (write != nullptr && std::string_view(write) == "1") {
        std::ofstream file(committed_path, std::ios::binary | std::ios::trunc);
        file << schema;
        require(static_cast<bool>(file), "the control schema must be writable");
        std::cout << "control schema: wrote " << committed_path << "\n";
        return;
    }
    if (read_committed(committed_path) != schema) {
        throw std::runtime_error(
            "manifests/control-schema.json is stale; run gui_forms_gallery_pages_tests with "
            "GUI_FORMS_CONTROL_SCHEMA_WRITE=1 to rewrite it");
    }
    std::cout << "control schema: " << records.size() << " types match the committed manifest\n";
}

} // namespace gallery_pages_tests
