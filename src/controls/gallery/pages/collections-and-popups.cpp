#include "pages.hpp"

namespace gui_forms::gallery::pages {
namespace {

class CollectionsAndPopupsPage final : public Component {
public:
    CollectionsAndPopupsPage(const std::shared_ptr<Surface>& body,
                             const std::shared_ptr<PageContext>& context) {
        static_cast<void>(context);
        (*body).add_at(label("gallery.collections.heading", "COLLECTIONS AND POPUPS", 22.0,
                             700, shell_blue_dark), {24.0, 18.0, 620.0, 34.0});
        (*body).add_at(label("gallery.collections.lead",
                             "Single and extended selection, virtualized rows, keyboard traversal, overlay dismissal, and focus restoration.",
                             12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});
        build_lists(body);
        build_combos(body);
    }

private:
    void build_lists(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> single_group = group(
            "gallery.collections.single", "ListBox · single selection", {450.0, 360.0});
        (*body).add_at(single_group, {24.0, 92.0, 450.0, 360.0});
        single_ = make_control<ListBox>(StableId("gallery.collections.single.list"));
        (*single_).set_accessible_name("Single-selection locations");
        (*single_).set_items({"Local volumes", "Recent locations", "Pinned folders",
                              "Saved searches", "Plugin surfaces", "Diagnostics",
                              "Appearance", "Keyboard map", "Security policy",
                              "About GUI.Forms"});
        (*single_).select_index(2U);
        (*single_group).add_at(single_, {24.0, 38.0, 400.0, 280.0});
        // selection_changed is the state event; item_activated (double-click,
        // Enter) is input only, so a programmatic select never "opens" a row.
        on((*single_).selection_changed(), *this, &CollectionsAndPopupsPage::on_location_selected);
        on((*single_).item_activated(), *this, &CollectionsAndPopupsPage::on_location_activated);

        std::shared_ptr<LayoutGroup> multi_group = group(
            "gallery.collections.multi", "ListBox · extended selection", {460.0, 360.0});
        (*body).add_at(multi_group, {494.0, 92.0, 460.0, 360.0});
        multi_ = make_control<ListBox>(StableId("gallery.collections.multi.list"));
        (*multi_).set_accessible_name("Extended-selection framework features");
        (*multi_).set_selection_mode(ListSelectionMode::multiple_extended);
        (*multi_).set_items({"CPU renderer", "Retained tree", "Damage regions",
                             "Tokenized events", "Unicode store", "Focus scopes",
                             "Popup controller", "Frame deadlines", "Host protocol",
                             "Drawing recorder"});
        (*multi_).select_index(1U);
        (*multi_).select_index(4U, true, false);
        (*multi_group).add_at(multi_, {24.0, 38.0, 410.0, 280.0});
        on((*multi_).selection_changed(), *this, &CollectionsAndPopupsPage::on_features_selected);
    }

    void build_combos(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> combo_group = group(
            "gallery.collections.combos", "ComboBox · retained overlay popup", {930.0, 160.0});
        (*body).add_at(combo_group, {24.0, 474.0, 930.0, 160.0});
        profile_ = make_control<ComboBox>(StableId("gallery.collections.combo.profile"));
        (*profile_).set_accessible_name("Visual profile");
        (*profile_).set_items({"Windows 7 professional", "Windows 10 professional",
                               "System neutral", "High contrast"});
        (*profile_).set_selected_index(1U);
        (*combo_group).add_at(profile_, {24.0, 42.0, 300.0, 36.0});
        std::shared_ptr<ComboBox> density = make_control<ComboBox>(
            StableId("gallery.collections.combo.density"));
        (*density).set_accessible_name("Control density");
        (*density).set_items({"Compact", "Comfortable", "Spacious", "Touch"});
        (*density).set_placeholder_text("Choose density");
        (*combo_group).add_at(density, {348.0, 42.0, 250.0, 36.0});
        // A searchable field: typing narrows the list to the items that
        // contain the text, with the matched text highlighted.
        search_ = make_control<ComboBox>(StableId("gallery.collections.combo.search"));
        (*search_).set_accessible_name("Find a station");
        (*search_).set_items({"Amsterdam Centraal", "Antwerpen-Centraal", "Basel SBB", "Berlin Hauptbahnhof",
                              "Bruxelles-Midi", "Frankfurt (Main) Hbf", "Hamburg Hbf", "Köln Hbf",
                              "London St Pancras", "Lyon Part-Dieu", "Milano Centrale", "Paris Gare du Nord",
                              "Rotterdam Centraal", "Wien Hauptbahnhof", "Zürich HB"});
        (*search_).set_drop_down_style(ComboBoxStyle::drop_down);
        (*search_).set_drop_down_filter(ComboBoxFilter::substring);
        (*combo_group).add_at(search_, {622.0, 42.0, 284.0, 36.0});
        status_ = label("gallery.collections.status",
                        "Popup overlays are root-attached, owner-revoked, and focus-contained.",
                        11.0, 600, green);
        (*combo_group).add_at(status_, {24.0, 96.0, 780.0, 26.0});
        on((*profile_).selected_index_changed(), *this,
           &CollectionsAndPopupsPage::on_profile_committed);
        on((*density).drop_down_changed(), *this, &CollectionsAndPopupsPage::on_density_popup);
        on((*search_).drop_down_changed(), *this, &CollectionsAndPopupsPage::on_search_popup);
    }

    void on_search_popup(const bool open) {
        (*status_).set_text(open ? "Stations matching \"" + std::string((*search_).text()) + "\": " +
                                       std::to_string((*search_).drop_down_items().size())
                                 : "Station search closed · the editor kept the focus");
    }

    void on_location_selected(const ListSelectionChange& change) {
        (*status_).set_text(change.current.empty()
            ? std::string("Locations: no selection")
            : "Location " + std::to_string(change.current.front() + 1U) +
                  " selected · selection_changed is the state event");
    }

    void on_location_activated(const std::size_t index) {
        (*status_).set_text("Location " + std::to_string(index + 1U) +
                            " activated · item_activated reports input only");
    }

    void on_features_selected(const ListSelectionChange& change) {
        (*status_).set_text(std::to_string(change.current.size()) +
                            " features selected · Shift and Ctrl extend the range");
    }

    void on_profile_committed() {
        (*status_).set_text("Profile committed: " + std::string((*profile_).selected_text()) +
                            " · overlay detached · focus restored");
    }

    void on_density_popup(const bool open) {
        (*status_).set_text(open ? "Density popup: OPEN · contained ListBox focus scope"
                                 : "Density popup: CLOSED · owner focus restored");
    }

    std::shared_ptr<ListBox> single_;
    std::shared_ptr<ListBox> multi_;
    std::shared_ptr<ComboBox> profile_;
    std::shared_ptr<ComboBox> search_;
    std::shared_ptr<Label> status_;
};

} // namespace

void add_collections(const std::shared_ptr<Surface>& page,
                     const std::shared_ptr<PageContext>& context) {
    (*context).components.add(std::make_shared<CollectionsAndPopupsPage>(page, context));
}

} // namespace gui_forms::gallery::pages
