#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_collections(const std::shared_ptr<Surface>& page,
                     const std::shared_ptr<PageContext>& context) {
    class ProfileSelectionChanged final {
    public:
        ProfileSelectionChanged(std::weak_ptr<Label> status,
                                std::weak_ptr<ComboBox> profile) noexcept
            : status_(std::move(status)), profile_(std::move(profile)) {}

        void operator()(std::optional<std::size_t>) const
        {
            const std::shared_ptr<Label> status = status_.lock();
            const std::shared_ptr<ComboBox> profile = profile_.lock();
            if (!status || !profile) return;
            (*status).set_text(
                "Profile committed: " + std::string((*profile).selected_text()) +
                " · overlay detached · focus restored");
        }

    private:
        std::weak_ptr<Label> status_;
        std::weak_ptr<ComboBox> profile_;
    };

    class DensityPopupChanged final {
    public:
        explicit DensityPopupChanged(std::weak_ptr<Label> status) noexcept
            : status_(std::move(status)) {}

        void operator()(bool open) const
        {
            if (const std::shared_ptr<Label> status = status_.lock()) {
                (*status).set_text(open
                    ? "Density popup: OPEN · contained ListBox focus scope"
                    : "Density popup: CLOSED · owner focus restored");
            }
        }

    private:
        std::weak_ptr<Label> status_;
    };

    (*page).add_at(label("gallery.collections.heading", "COLLECTIONS AND POPUPS", 22.0,
                       700, shell_blue_dark), {24.0, 18.0, 620.0, 34.0});
    (*page).add_at(label("gallery.collections.lead",
                       "Single and extended selection, virtualized rows, keyboard traversal, overlay dismissal, and focus restoration.",
                       12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});

    std::shared_ptr<LayoutGroup> single_group = group("gallery.collections.single", "ListBox · single selection",
                              {450.0, 360.0});
    (*page).add_at(single_group, {24.0, 92.0, 450.0, 360.0});
    std::shared_ptr<gui_forms::ListBox> single = make_control<ListBox>(StableId("gallery.collections.single.list"));
    (*single).set_accessible_name("Single-selection locations");
    (*single).set_items({"Local volumes", "Recent locations", "Pinned folders",
                       "Saved searches", "Plugin surfaces", "Diagnostics",
                       "Appearance", "Keyboard map", "Security policy",
                       "About GUI.Forms"});
    (*single).select_index(2U);
    (*single_group).add_at(single, {24.0, 38.0, 400.0, 280.0});

    std::shared_ptr<LayoutGroup> multi_group = group("gallery.collections.multi",
                             "ListBox · extended selection", {460.0, 360.0});
    (*page).add_at(multi_group, {494.0, 92.0, 460.0, 360.0});
    std::shared_ptr<gui_forms::ListBox> multi = make_control<ListBox>(StableId("gallery.collections.multi.list"));
    (*multi).set_accessible_name("Extended-selection framework features");
    (*multi).set_selection_mode(ListSelectionMode::multiple_extended);
    (*multi).set_items({"CPU renderer", "Retained tree", "Damage regions",
                      "Tokenized events", "Unicode store", "Focus scopes",
                      "Popup controller", "Frame deadlines", "Host protocol",
                      "Drawing recorder"});
    (*multi).select_index(1U);
    (*multi).select_index(4U, true, false);
    (*multi_group).add_at(multi, {24.0, 38.0, 410.0, 280.0});

    std::shared_ptr<LayoutGroup> combo_group = group("gallery.collections.combos",
                             "ComboBox · retained overlay popup", {930.0, 160.0});
    (*page).add_at(combo_group, {24.0, 474.0, 930.0, 160.0});
    std::shared_ptr<gui_forms::ComboBox> profile = make_control<ComboBox>(StableId("gallery.collections.combo.profile"));
    (*profile).set_accessible_name("Visual profile");
    (*profile).set_items({"Windows 7 professional", "Windows 10 professional",
                        "System neutral", "High contrast"});
    (*profile).set_selected_index(1U);
    (*combo_group).add_at(profile, {24.0, 42.0, 300.0, 36.0});
    std::shared_ptr<gui_forms::ComboBox> density = make_control<ComboBox>(StableId("gallery.collections.combo.density"));
    (*density).set_accessible_name("Control density");
    (*density).set_items({"Compact", "Comfortable", "Spacious", "Touch"});
    (*density).set_placeholder_text("Choose density");
    (*combo_group).add_at(density, {348.0, 42.0, 250.0, 36.0});
    std::shared_ptr<Label> status = label("gallery.collections.status",
                        "Popup overlays are root-attached, owner-revoked, and focus-contained.",
                        11.0, 600, green);
    (*combo_group).add_at(status, {24.0, 96.0, 780.0, 26.0});
    const std::weak_ptr<Label> weak_status = status;
    const std::weak_ptr<ComboBox> weak_profile = profile;
    (*context).subscriptions.push_back((*profile).selected_index_changed().subscribe(
        *status, ProfileSelectionChanged(weak_status, weak_profile)));
    (*context).subscriptions.push_back((*density).drop_down_changed().subscribe(
        *status, DensityPopupChanged(weak_status)));
}

} // namespace gui_forms::gallery::pages
