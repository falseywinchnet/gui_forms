#include "gui_forms/xml/types/xml_types.hpp"

namespace gui_forms {

std::string_view xml_error_name(XmlError error) noexcept {
    switch (error) {
    case XmlError::none: return "none";
    case XmlError::invalid_limits: return "invalid_limits";
    case XmlError::input_too_large: return "input_too_large";
    case XmlError::unsupported_encoding: return "unsupported_encoding";
    case XmlError::invalid_utf8: return "invalid_utf8";
    case XmlError::invalid_character: return "invalid_character";
    case XmlError::unexpected_end: return "unexpected_end";
    case XmlError::syntax_error: return "syntax_error";
    case XmlError::invalid_name: return "invalid_name";
    case XmlError::name_too_long: return "name_too_long";
    case XmlError::value_too_long: return "value_too_long";
    case XmlError::depth_exceeded: return "depth_exceeded";
    case XmlError::element_limit_exceeded: return "element_limit_exceeded";
    case XmlError::attribute_limit_exceeded: return "attribute_limit_exceeded";
    case XmlError::duplicate_attribute: return "duplicate_attribute";
    case XmlError::mismatched_end_tag: return "mismatched_end_tag";
    case XmlError::unknown_entity: return "unknown_entity";
    case XmlError::invalid_character_reference: return "invalid_character_reference";
    case XmlError::doctype_refused: return "doctype_refused";
    case XmlError::processing_instruction_refused: return "processing_instruction_refused";
    case XmlError::cdata_refused: return "cdata_refused";
    case XmlError::invalid_declaration: return "invalid_declaration";
    case XmlError::invalid_comment: return "invalid_comment";
    case XmlError::missing_root_element: return "missing_root_element";
    case XmlError::content_after_root: return "content_after_root";
    case XmlError::mixed_content: return "mixed_content";
    }
    return "unknown";
}

const XmlAttribute* XmlElement::find_attribute(std::string_view attribute_name) const& noexcept {
    for (const XmlAttribute& attribute : attributes) {
        if (attribute.name == attribute_name) {
            const XmlAttribute* found = &attribute;
            return found;
        }
    }
    return nullptr;
}

} // namespace gui_forms
