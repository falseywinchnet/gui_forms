#include "pages.hpp"

namespace gui_forms::gallery::pages {

// Page under construction: ChoiceGroup, ExpandableSections, GroupBox, Card, ReviewCard.
void add_choices_and_sections(const std::shared_ptr<Surface>& page,
                              const std::shared_ptr<PageContext>& context) {
    static_cast<void>(context);
    (*page).add_at(label("gallery.choices-and-sections.heading", "CHOICES AND SECTIONS", 22.0, 700, shell_blue_dark),
                   {24.0, 18.0, 620.0, 34.0});
    (*page).add_at(label("gallery.choices-and-sections.pending", "ChoiceGroup, ExpandableSections, GroupBox, Card and ReviewCard bound to shared values. This page is being written.",
                         12.0, 400, muted),
                   {26.0, 52.0, 920.0, 24.0});
}

}  // namespace gui_forms::gallery::pages
