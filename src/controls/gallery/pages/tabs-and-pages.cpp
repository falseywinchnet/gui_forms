#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_tabs_and_pages(const std::shared_ptr<Surface>& page,
                        const std::shared_ptr<PageContext>& context) {
    class PrimaryTabSelectionChanged final {
    public:
        PrimaryTabSelectionChanged(std::weak_ptr<Label> status,
                                   std::weak_ptr<TabControl> tabs) noexcept
            : status_(std::move(status)), tabs_(std::move(tabs)) {}

        void operator()(const TabSelectionChange&) const
        {
            const std::shared_ptr<Label> status = status_.lock();
            const std::shared_ptr<TabControl> tabs = tabs_.lock();
            if (status && tabs && (*tabs).selected_tab()) {
                (*status).set_text(
                    (*(*tabs).selected_tab()).text() +
                    " selected · focus and semantics reconciled");
            }
        }

    private:
        std::weak_ptr<Label> status_;
        std::weak_ptr<TabControl> tabs_;
    };

    (*page).add_at(label("gallery.tabs.heading", "TABS AND RETAINED PAGES", 22.0,
                       700, shell_blue_dark), {24.0, 18.0, 680.0, 34.0});
    (*page).add_at(label("gallery.tabs.lead",
                       "Owned page identity, selection events, focus restoration, keyboard traversal, four alignments, and three appearances.",
                       12.0, 400, muted), {26.0, 52.0, 920.0, 24.0});

    std::shared_ptr<LayoutGroup> primary_group = group("gallery.tabs.primary.group",
                               "TabControl · interactive retained pages",
                               {930.0, 280.0});
    (*page).add_at(primary_group, {24.0, 92.0, 930.0, 280.0});
    std::shared_ptr<gui_forms::TabControl> primary = make_control<TabControl>(StableId("gallery.tabs.primary"));
    (*primary).set_style(page_style());
    (*primary).set_accessible_name("Primary workspace tabs");
    (*primary).set_item_size({150.0, 30.0});
    (*primary_group).add_at(primary, {20.0, 36.0, 890.0, 220.0});
    constexpr std::array<std::string_view, 4> primary_names{
        "Overview", "Activity", "Security", "Diagnostics"};
    for (std::size_t index = 0U; index < primary_names.size(); ++index) {
        std::shared_ptr<gui_forms::TabPage> tab_page = make_control<TabPage>(
            StableId("gallery.tabs.primary." +
                     std::string(index == 0U ? "overview" : index == 1U
                         ? "activity" : index == 2U ? "security" : "diagnostics")),
            std::string(primary_names[index]));
        std::shared_ptr<Label> title = label("gallery.tabs.primary.page.title." +
                               std::to_string(index),
                           std::string(primary_names[index]) + " workspace",
                           16.0, 700, shell_blue_dark);
        (*title).set_requested_bounds({22.0, 22.0, 420.0, 30.0});
        (*tab_page).add_child(title);
        std::shared_ptr<Label> copy = label("gallery.tabs.primary.page.copy." +
                              std::to_string(index),
                          index == 0U
                              ? "A retained page owns ordinary controls and remains stable across selection cycles."
                              : index == 1U
                              ? "Ctrl+Tab and Ctrl+Shift+Tab traverse pages from focused child controls."
                              : index == 2U
                              ? "Hidden pages leave paint, hit testing, focus traversal, and accessibility."
                              : "Removal and direct disposal reconcile the surviving selected page.",
                          11.0, 400, ink);
        (*copy).set_requested_bounds({22.0, 58.0, 660.0, 26.0});
        (*tab_page).add_child(copy);
        if (index == 0U) {
            std::shared_ptr<gui_forms::Button> action = make_control<Button>(
                StableId("gallery.tabs.primary.action"), "Run page action");
            (*action).set_visual_style(ButtonVisualStyle::accent);
            (*action).set_requested_bounds({22.0, 104.0, 170.0, 36.0});
            (*tab_page).add_child(action);
        } else if (index == 1U) {
            std::shared_ptr<gui_forms::CheckBox> toggle = make_control<CheckBox>(
                StableId("gallery.tabs.primary.activity.toggle"),
                "Keep activity visible");
            (*toggle).set_indicator_style(ChoiceIndicatorStyle::toggle);
            (*toggle).set_checked(true);
            (*toggle).set_requested_bounds({22.0, 104.0, 220.0, 30.0});
            (*tab_page).add_child(toggle);
        } else if (index == 2U) {
            std::shared_ptr<gui_forms::RadioButton> radio = make_control<RadioButton>(
                StableId("gallery.tabs.primary.security.radio"),
                "Professional policy");
            (*radio).set_checked(true);
            (*radio).set_requested_bounds({22.0, 104.0, 220.0, 30.0});
            (*tab_page).add_child(radio);
        } else {
            std::shared_ptr<gui_forms::ProgressBar> value = make_control<ProgressBar>(
                StableId("gallery.tabs.primary.diagnostics.progress"));
            (*value).set_range(0.0, 100.0);
            (*value).set_value(78.0);
            (*value).set_accessible_name("Diagnostics completion");
            (*value).set_requested_bounds({22.0, 108.0, 360.0, 24.0});
            (*tab_page).add_child(value);
        }
        (*primary).add_page(tab_page);
    }
    (*primary).set_selected_index(1U);

    std::shared_ptr<Label> status = label("gallery.tabs.status",
                        "Activity selected · hidden pages are fully quiescent",
                        11.0, 600, green);
    (*primary_group).add_at(status, {620.0, 7.0, 286.0, 24.0});
    const std::weak_ptr<Label> weak_status = status;
    const std::weak_ptr<TabControl> weak_primary = primary;
    (*context).subscriptions.push_back((*primary).selected_index_changed().subscribe(
        *status, PrimaryTabSelectionChanged(weak_status, weak_primary)));

    std::shared_ptr<LayoutGroup> variants = group("gallery.tabs.variants",
                          "Alignment and appearance matrix", {930.0, 230.0});
    (*page).add_at(variants, {24.0, 394.0, 930.0, 230.0});
    constexpr std::array<std::tuple<TabAlignment, TabAppearance,
                                    std::string_view>, 3> variants_spec{{
        {TabAlignment::bottom, TabAppearance::buttons, "Bottom · buttons"},
        {TabAlignment::left, TabAppearance::flat_buttons, "Left · flat"},
        {TabAlignment::right, TabAppearance::normal, "Right · normal"},
    }};
    for (std::size_t index = 0U; index < variants_spec.size(); ++index) {
        const double x = 20.0 + static_cast<double>(index) * 303.0;
        std::shared_ptr<Label> caption = label("gallery.tabs.variant.caption." +
                                 std::to_string(index),
                             std::string(std::get<2>(variants_spec[index])),
                             10.0, 700, shell_blue_dark);
        (*variants).add_at(caption, {x, 34.0, 280.0, 22.0});
        std::shared_ptr<gui_forms::TabControl> tabs = make_control<TabControl>(
            StableId("gallery.tabs.variant." + std::to_string(index)));
        (*tabs).set_accessible_name(std::string(std::get<2>(variants_spec[index])) +
                                  " tabs");
        (*tabs).set_style(page_style());
        (*tabs).set_alignment(std::get<0>(variants_spec[index]));
        (*tabs).set_appearance(std::get<1>(variants_spec[index]));
        (*tabs).set_item_size(index == 0U ? Size{86.0, 28.0} : Size{82.0, 28.0});
        (*variants).add_at(tabs, {x, 60.0, 280.0, 148.0});
        for (std::size_t page_index = 0U; page_index < 3U; ++page_index) {
            std::shared_ptr<gui_forms::TabPage> tab_page = make_control<TabPage>(
                StableId("gallery.tabs.variant." + std::to_string(index) +
                         ".page." + std::to_string(page_index)),
                page_index == 0U ? "One" : page_index == 1U ? "Two" : "Three");
            std::shared_ptr<Label> content = label("gallery.tabs.variant.content." +
                                     std::to_string(index) + "." +
                                     std::to_string(page_index),
                                 "Retained page " + std::to_string(page_index + 1U),
                                 10.0, 600, muted);
            (*content).set_requested_bounds({12.0, 14.0, 150.0, 24.0});
            (*tab_page).add_child(content);
            (*tabs).add_page(tab_page);
        }
        (*tabs).set_selected_index(index % 3U);
    }
}

} // namespace gui_forms::gallery::pages
