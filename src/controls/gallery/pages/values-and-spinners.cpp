#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_values(const std::shared_ptr<Surface>& page,
                const std::shared_ptr<PageContext>& context) {
    enum class PropertySpecimenAction {
        inspect_numeric,
        inspect_items,
        inspect_style,
        add_item,
    };

    class PropertySpecimenClick final {
    public:
        PropertySpecimenClick(std::weak_ptr<PropertyGrid> grid,
                              std::weak_ptr<NumericUpDown> numeric,
                              std::weak_ptr<ComboBox> combo,
                              std::weak_ptr<Label> status,
                              PropertySpecimenAction action) noexcept
            : grid_(std::move(grid)), numeric_(std::move(numeric)),
              combo_(std::move(combo)), status_(std::move(status)),
              action_(action) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<PropertyGrid> grid = grid_.lock();
            if (!grid) return;
            switch (action_) {
            case PropertySpecimenAction::inspect_numeric:
                inspect_numeric(*grid);
                return;
            case PropertySpecimenAction::inspect_items:
                inspect_items(*grid);
                return;
            case PropertySpecimenAction::inspect_style:
                inspect_style(*grid);
                return;
            case PropertySpecimenAction::add_item:
                add_item(*grid);
                return;
            }
        }

    private:
        void set_status(std::string text) const
        {
            if (const std::shared_ptr<Label> status = status_.lock()) {
                (*status).set_text(std::move(text));
            }
        }

        void inspect_numeric(PropertyGrid& grid) const
        {
            const std::shared_ptr<NumericUpDown> numeric = numeric_.lock();
            if (!numeric) return;
            grid.set_selected_object(numeric);
            static_cast<void>(grid.set_property_expanded("Bounds", true));
            set_status(
                "PropertyGrid · retained NumericUpDown factory and typed commit");
        }

        void inspect_items(PropertyGrid& grid) const
        {
            const std::shared_ptr<ComboBox> combo = combo_.lock();
            if (!combo) return;
            grid.set_selected_object(combo);
            static_cast<void>(grid.set_property_expanded("Items", true));
            if (Window* window = grid.attached_window()) {
                static_cast<void>((*window).request_focus(grid.editor("Items[0]")));
            }
            set_status(
                "PropertyGrid · ComboBox.Items immutable collection expanded");
        }

        void inspect_style(PropertyGrid& grid) const
        {
            const std::shared_ptr<Label> status = status_.lock();
            if (!status) return;
            grid.set_selected_object(status);
            static_cast<void>(grid.set_property_expanded("ForeColor", true));
            (*status).set_text(
                "PropertyGrid · flags popup and canonical color editor services");
        }

        void add_item(PropertyGrid& grid) const
        {
            const std::shared_ptr<ComboBox> combo = combo_.lock();
            if (!combo) return;
            if (grid.selected_object() != combo) grid.set_selected_object(combo);
            static_cast<void>(grid.set_property_expanded("Items", true));
            const bool added = grid.insert_collection_item(
                "Items", (*combo).items().size(),
                BindingValue{std::string("CW")});
            if (added) {
                if (Window* window = grid.attached_window()) {
                    static_cast<void>((*window).request_focus(grid.editor(
                        "Items[" +
                        std::to_string((*combo).items().size() - 1U) + "]")));
                }
            }
            set_status(added
                ? "PropertyGrid · inserted CW through ComboBox.Items setter"
                : "PropertyGrid · Items insertion rejected truthfully");
        }

        std::weak_ptr<PropertyGrid> grid_;
        std::weak_ptr<NumericUpDown> numeric_;
        std::weak_ptr<ComboBox> combo_;
        std::weak_ptr<Label> status_;
        PropertySpecimenAction action_{};
    };

    class NumericValueStatus final {
    public:
        NumericValueStatus(std::weak_ptr<Label> status,
                           std::size_t index) noexcept
            : status_(std::move(status)), index_(index) {}

        void operator()(double value) const
        {
            if (const std::shared_ptr<Label> status = status_.lock()) {
                (*status).set_text(
                    "Numeric " + std::to_string(index_ + 1U) +
                    " committed " + std::to_string(value) +
                    " · event emitted after value and editor synchronization");
            }
        }

    private:
        std::weak_ptr<Label> status_;
        std::size_t index_{};
    };

    class PropertyValueStatus final {
    public:
        explicit PropertyValueStatus(std::weak_ptr<Label> status) noexcept
            : status_(std::move(status)) {}

        void operator()(const PropertyGridValueChange& change) const
        {
            if (const std::shared_ptr<Label> status = status_.lock()) {
                (*status).set_text(
                    "PropertyGrid committed " + change.property_name +
                    " · origin " +
                    std::string(property_value_origin_name(change.origin)) +
                    (change.reset ? " · reset" : ""));
            }
        }

    private:
        std::weak_ptr<Label> status_;
    };

    (*page).add_at(label("gallery.values.heading", "VALUES AND SPINNERS", 22.0, 700,
                       shell_blue_dark), {24.0, 18.0, 600.0, 34.0});
    (*page).add_at(label("gallery.values.lead",
                       "Compound TextBox editing, bounded numeric commit, decimal and hexadecimal formats, keys, wheel, and spinner buttons.",
                       12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});
    std::shared_ptr<LayoutGroup> group_box = group("gallery.values.numeric", "NumericUpDown · public compound control",
                           {930.0, 360.0});
    (*page).add_at(group_box, {24.0, 92.0, 930.0, 360.0});

    const std::array<std::string_view, 4> captions{
        "Integer range", "Decimal increment", "Signed range", "Hexadecimal"};
    std::vector<std::shared_ptr<NumericUpDown>> values;
    for (std::size_t index = 0; index < captions.size(); ++index) {
        (*group_box).add_at(label("gallery.values.caption." + std::to_string(index),
                                std::string(captions[index]), 11.0, 600, muted),
                          {24.0, 44.0 + static_cast<double>(index) * 62.0, 170.0, 26.0});
        std::shared_ptr<gui_forms::NumericUpDown> numeric = make_control<NumericUpDown>(
            StableId("gallery.values.numeric." + std::to_string(index)));
        (*numeric).set_accessible_name(std::string(captions[index]));
        if (index == 0U) {
            (*numeric).set_range(0.0, 100.0);
            (*numeric).set_value(42.0);
        } else if (index == 1U) {
            (*numeric).set_range(0.0, 10.0);
            (*numeric).set_increment(0.25);
            (*numeric).set_decimal_places(2U);
            (*numeric).set_value(3.75);
        } else if (index == 2U) {
            (*numeric).set_range(-120.0, 120.0);
            (*numeric).set_increment(5.0);
            (*numeric).set_value(-15.0);
        } else {
            (*numeric).set_range(0.0, 255.0);
            (*numeric).set_hexadecimal(true);
            (*numeric).set_value(175.0);
        }
        (*group_box).add_at(numeric,
                          {194.0, 38.0 + static_cast<double>(index) * 62.0,
                           260.0, 36.0});
        values.push_back(numeric);
    }
    std::shared_ptr<gui_forms::PropertyGrid> inspector = make_control<PropertyGrid>(
        StableId("gallery.values.property-grid"));
    (*inspector).set_accessible_name("Decimal control property grid");
    (*inspector).set_selected_object(values[1]);
    static_cast<void>((*inspector).set_property_expanded("Bounds", true));
    std::shared_ptr<gui_forms::ComboBox> items_target = make_control<ComboBox>(
        StableId("gallery.values.items-target"));
    (*items_target).set_accessible_name("Inspectable mode collection");
    (*items_target).set_items({"AM", "FM", "WFM"});
    (*items_target).set_selected_index(1U);
    (*group_box).add_at(items_target, {486.0, 34.0, 174.0, 30.0});
    std::shared_ptr<gui_forms::Button> inspect_numeric = make_control<Button>(
        StableId("gallery.values.inspect-numeric"), "Numeric");
    std::shared_ptr<gui_forms::Button> inspect_items = make_control<Button>(
        StableId("gallery.values.inspect-items"), "Items");
    std::shared_ptr<gui_forms::Button> inspect_style = make_control<Button>(
        StableId("gallery.values.inspect-style"), "Style");
    std::shared_ptr<gui_forms::Button> add_item = make_control<Button>(
        StableId("gallery.values.add-item"), "Add CW");
    (*inspect_numeric).set_visual_style(ButtonVisualStyle::command);
    (*inspect_items).set_visual_style(ButtonVisualStyle::command);
    (*inspect_style).set_visual_style(ButtonVisualStyle::command);
    (*add_item).set_visual_style(ButtonVisualStyle::command);
    (*group_box).add_at(inspect_numeric, {668.0, 34.0, 58.0, 30.0});
    (*group_box).add_at(inspect_items, {732.0, 34.0, 50.0, 30.0});
    (*group_box).add_at(inspect_style, {788.0, 34.0, 54.0, 30.0});
    (*group_box).add_at(add_item, {848.0, 34.0, 50.0, 30.0});
    (*group_box).add_at(inspector, {486.0, 72.0, 420.0, 222.0});
    std::shared_ptr<Label> status = label("gallery.values.status",
                        "PropertyGrid uses instance-owned converter/editor services.",
                        10.5, 600, green);
    (*group_box).add_at(status, {494.0, 306.0, 404.0, 34.0});
    (*status).set_text_wrapping(TextWrapping::word);
    (*status).set_vertical_alignment(VerticalAlignment::near);
    const std::weak_ptr<PropertyGrid> weak_inspector = inspector;
    const std::weak_ptr<NumericUpDown> weak_numeric = values[1];
    const std::weak_ptr<ComboBox> weak_items_target = items_target;
    const std::weak_ptr<Label> weak_inspection_status = status;
    (*context).subscriptions.push_back((*inspect_numeric).clicked().subscribe(
        *status, PropertySpecimenClick(
            weak_inspector, weak_numeric, weak_items_target,
            weak_inspection_status, PropertySpecimenAction::inspect_numeric)));
    (*context).subscriptions.push_back((*inspect_items).clicked().subscribe(
        *status, PropertySpecimenClick(
            weak_inspector, weak_numeric, weak_items_target,
            weak_inspection_status, PropertySpecimenAction::inspect_items)));
    (*context).subscriptions.push_back((*inspect_style).clicked().subscribe(
        *status, PropertySpecimenClick(
            weak_inspector, weak_numeric, weak_items_target,
            weak_inspection_status, PropertySpecimenAction::inspect_style)));
    (*context).subscriptions.push_back((*add_item).clicked().subscribe(
        *status, PropertySpecimenClick(
            weak_inspector, weak_numeric, weak_items_target,
            weak_inspection_status, PropertySpecimenAction::add_item)));
    for (std::size_t index = 0; index < values.size(); ++index) {
        const std::weak_ptr<Label> weak_status = status;
        (*context).subscriptions.push_back((*values[index]).value_changed().subscribe(
            *status, NumericValueStatus(weak_status, index)));
    }
    const std::weak_ptr<Label> weak_property_status = status;
    (*context).subscriptions.push_back(
        (*inspector).property_value_changed().subscribe(
            *status, PropertyValueStatus(weak_property_status)));

    std::shared_ptr<LayoutGroup> binding = group("gallery.values.binding",
                         "BindingSource · currency, edit, conversion, two-way controls",
                         {930.0, 164.0});
    (*page).add_at(binding, {24.0, 474.0, 930.0, 164.0});
    (*context).binding_editor = make_control<TextBox>(
        StableId("gallery.values.binding.name"));
    (*(*context).binding_editor).set_accessible_name("Bound profile name");
    (*context).binding_enabled = make_control<CheckBox>(
        StableId("gallery.values.binding.enabled"), "Enabled");
    (*context).binding_gain = make_control<TrackBar>(
        StableId("gallery.values.binding.gain"));
    (*(*context).binding_gain).set_visual_style(TrackBarVisualStyle::filled);
    (*context).binding_previous = make_control<Button>(
        StableId("gallery.values.binding.previous"), "‹ Previous");
    (*context).binding_next = make_control<Button>(
        StableId("gallery.values.binding.next"), "Next ›");
    (*(*context).binding_previous).set_visual_style(ButtonVisualStyle::command);
    (*(*context).binding_next).set_visual_style(ButtonVisualStyle::command);
    (*context).binding_status = label(
        "gallery.values.binding.status",
        "Runtime binding initializes after the retained Window attaches",
        10.0, 600, green);
    (*binding).add_at((*context).binding_editor, {20.0, 42.0, 190.0, 32.0});
    (*binding).add_at((*context).binding_enabled, {228.0, 44.0, 120.0, 28.0});
    (*binding).add_at((*context).binding_gain, {360.0, 42.0, 250.0, 32.0});
    (*binding).add_at((*context).binding_previous, {630.0, 42.0, 128.0, 32.0});
    (*binding).add_at((*context).binding_next, {772.0, 42.0, 128.0, 32.0});
    (*binding).add_at((*context).binding_status, {20.0, 94.0, 880.0, 26.0});
}

} // namespace gui_forms::gallery::pages
