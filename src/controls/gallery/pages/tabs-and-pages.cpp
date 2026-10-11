#include "pages.hpp"

namespace gui_forms::gallery::pages {

// Owner of the Tabs and Retained Pages page: one interactive TabControl whose
// selection state event drives the status line, plus a matrix of alignment
// and appearance variants. The Activity page's toggle binds a Value<bool>
// that the owner observes instead of the control.
class TabsPage final : public gui_forms::Component {
public:
    TabsPage(const std::shared_ptr<Surface>& page,
             const std::shared_ptr<PageContext>& context) {
        static_cast<void>(context);
        (*page).add_at(label("gallery.tabs.heading", "TABS AND RETAINED PAGES", 22.0,
                             700, shell_blue_dark),
                       {24.0, 18.0, 680.0, 34.0});
        (*page).add_at(label("gallery.tabs.lead",
                             "Owned page identity, selection events, focus restoration, keyboard traversal, four alignments, and three appearances.",
                             12.0, 400, muted),
                       {26.0, 52.0, 920.0, 24.0});
        build_primary(page);
        build_variants(page);
        on((*primary_).selected_index_changed(), *this, &TabsPage::on_primary_selected);
        on(activity_visible_.changed(), *this, &TabsPage::on_activity_visible_changed);
    }

private:
    static constexpr std::array<std::string_view, 4> primary_names{
        "Overview", "Activity", "Security", "Diagnostics"};
    static constexpr std::array<std::string_view, 4> primary_ids{
        "overview", "activity", "security", "diagnostics"};
    static constexpr std::array<std::string_view, 4> primary_copy{
        "A retained page owns ordinary controls and remains stable across selection cycles.",
        "Ctrl+Tab and Ctrl+Shift+Tab traverse pages from focused child controls.",
        "Hidden pages leave paint, hit testing, focus traversal, and accessibility.",
        "Removal and direct disposal reconcile the surviving selected page."};

    void build_primary(const std::shared_ptr<Surface>& page) {
        std::shared_ptr<LayoutGroup> primary_group = group(
            "gallery.tabs.primary.group", "TabControl · interactive retained pages",
            {930.0, 280.0});
        (*page).add_at(primary_group, {24.0, 92.0, 930.0, 280.0});
        primary_ = make_control<TabControl>(StableId("gallery.tabs.primary"));
        (*primary_).set_style(page_style());
        (*primary_).set_accessible_name("Primary workspace tabs");
        (*primary_).set_item_size({150.0, 30.0});
        (*primary_group).add_at(primary_, {20.0, 36.0, 890.0, 220.0});
        for (std::size_t index = 0U; index < primary_names.size(); ++index) {
            std::shared_ptr<TabPage> tab_page = make_control<TabPage>(
                StableId("gallery.tabs.primary." + std::string(primary_ids[index])),
                std::string(primary_names[index]));
            std::shared_ptr<Label> title = label(
                "gallery.tabs.primary.page.title." + std::to_string(index),
                std::string(primary_names[index]) + " workspace", 16.0, 700,
                shell_blue_dark);
            (*title).set_requested_bounds({22.0, 22.0, 420.0, 30.0});
            (*tab_page).add_child(title);
            std::shared_ptr<Label> copy = label(
                "gallery.tabs.primary.page.copy." + std::to_string(index),
                std::string(primary_copy[index]), 11.0, 400, ink);
            (*copy).set_requested_bounds({22.0, 58.0, 660.0, 26.0});
            (*tab_page).add_child(copy);
            (*tab_page).add_child(page_content(index));
            (*primary_).add_page(tab_page);
        }
        (*primary_).set_selected_index(1U);
        status_ = label("gallery.tabs.status",
                        "Activity selected · hidden pages are fully quiescent",
                        11.0, 600, green);
        (*primary_group).add_at(status_, {620.0, 7.0, 286.0, 24.0});
    }

    // One ordinary control per retained page, proving hidden pages keep
    // their children while leaving paint, hit testing and semantics.
    [[nodiscard]] Control::Ptr page_content(const std::size_t index) {
        if (index == 0U) {
            std::shared_ptr<Button> action = make_control<Button>(
                StableId("gallery.tabs.primary.action"), "Run page action");
            (*action).set_visual_style(ButtonVisualStyle::accent);
            (*action).set_requested_bounds({22.0, 104.0, 170.0, 36.0});
            return action;
        }
        if (index == 1U) {
            std::shared_ptr<CheckBox> toggle = make_control<CheckBox>(
                StableId("gallery.tabs.primary.activity.toggle"),
                "Keep activity visible");
            (*toggle).set_indicator_style(ChoiceIndicatorStyle::toggle);
            (*toggle).set_requested_bounds({22.0, 104.0, 220.0, 30.0});
            (*toggle).bind(activity_visible_);
            return toggle;
        }
        if (index == 2U) {
            std::shared_ptr<RadioButton> radio = make_control<RadioButton>(
                StableId("gallery.tabs.primary.security.radio"),
                "Professional policy");
            (*radio).set_checked(true);
            (*radio).set_requested_bounds({22.0, 104.0, 220.0, 30.0});
            return radio;
        }
        std::shared_ptr<ProgressBar> value = make_control<ProgressBar>(
            StableId("gallery.tabs.primary.diagnostics.progress"));
        (*value).set_range(0.0, 100.0);
        (*value).set_value(78.0);
        (*value).set_accessible_name("Diagnostics completion");
        (*value).set_requested_bounds({22.0, 108.0, 360.0, 24.0});
        return value;
    }

    void build_variants(const std::shared_ptr<Surface>& page) {
        std::shared_ptr<LayoutGroup> variants = group(
            "gallery.tabs.variants", "Alignment and appearance matrix", {930.0, 230.0});
        (*page).add_at(variants, {24.0, 394.0, 930.0, 230.0});
        constexpr std::array<std::tuple<TabAlignment, TabAppearance, std::string_view>, 3>
            variants_spec{{
                {TabAlignment::bottom, TabAppearance::buttons, "Bottom · buttons"},
                {TabAlignment::left, TabAppearance::flat_buttons, "Left · flat"},
                {TabAlignment::right, TabAppearance::normal, "Right · normal"},
            }};
        for (std::size_t index = 0U; index < variants_spec.size(); ++index) {
            const double x = 20.0 + static_cast<double>(index) * 303.0;
            std::shared_ptr<Label> caption = label(
                "gallery.tabs.variant.caption." + std::to_string(index),
                std::string(std::get<2>(variants_spec[index])), 10.0, 700,
                shell_blue_dark);
            (*variants).add_at(caption, {x, 34.0, 280.0, 22.0});
            std::shared_ptr<TabControl> tabs = make_control<TabControl>(
                StableId("gallery.tabs.variant." + std::to_string(index)));
            (*tabs).set_accessible_name(
                std::string(std::get<2>(variants_spec[index])) + " tabs");
            (*tabs).set_style(page_style());
            (*tabs).set_alignment(std::get<0>(variants_spec[index]));
            (*tabs).set_appearance(std::get<1>(variants_spec[index]));
            (*tabs).set_item_size(index == 0U ? Size{86.0, 28.0} : Size{82.0, 28.0});
            (*variants).add_at(tabs, {x, 60.0, 280.0, 148.0});
            for (std::size_t page_index = 0U; page_index < 3U; ++page_index) {
                std::shared_ptr<TabPage> tab_page = make_control<TabPage>(
                    StableId("gallery.tabs.variant." + std::to_string(index) +
                             ".page." + std::to_string(page_index)),
                    page_index == 0U ? "One" : page_index == 1U ? "Two" : "Three");
                std::shared_ptr<Label> content = label(
                    "gallery.tabs.variant.content." + std::to_string(index) + "." +
                        std::to_string(page_index),
                    "Retained page " + std::to_string(page_index + 1U), 10.0, 600,
                    muted);
                (*content).set_requested_bounds({12.0, 14.0, 150.0, 24.0});
                (*tab_page).add_child(content);
                (*tabs).add_page(tab_page);
            }
            (*tabs).set_selected_index(index % 3U);
        }
    }

    // State event: fires for pointer, keyboard, semantic and programmatic
    // selection alike, so the status line never needs a second route.
    void on_primary_selected(const TabSelectionChange&) {
        const std::shared_ptr<TabPage> selected = (*primary_).selected_tab();
        if (!selected) return;
        (*status_).set_text((*selected).text() +
                            " selected · focus and semantics reconciled");
    }

    void on_activity_visible_changed(const bool visible) {
        (*status_).set_text(visible
            ? "Activity · keep visible on · bound Value<bool> changed"
            : "Activity · keep visible off · bound Value<bool> changed");
    }

    std::shared_ptr<TabControl> primary_;
    std::shared_ptr<Label> status_;
    Value<bool> activity_visible_{true};
};

void add_tabs_and_pages(const std::shared_ptr<Surface>& page,
                        const std::shared_ptr<PageContext>& context) {
    (*context).components.add(std::make_shared<TabsPage>(page, context));
}

} // namespace gui_forms::gallery::pages
