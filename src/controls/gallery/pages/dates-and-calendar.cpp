#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_dates_and_calendar(const std::shared_ptr<Surface>& page,
                            const std::shared_ptr<PageContext>& context) {
    class DateValueStatus final {
    public:
        DateValueStatus(std::weak_ptr<DateTimePicker> picker,
                        std::weak_ptr<Label> status) noexcept
            : picker_(std::move(picker)), status_(std::move(status)) {}

        void operator()(DateTimeValue) const
        {
            const std::shared_ptr<DateTimePicker> picker = picker_.lock();
            const std::shared_ptr<Label> status = status_.lock();
            if (picker && status) {
                (*status).set_text(
                    "Committed · " + (*picker).formatted_value() +
                    " · retained focus restored");
            }
        }

    private:
        std::weak_ptr<DateTimePicker> picker_;
        std::weak_ptr<Label> status_;
    };

    class DatePopupStatus final {
    public:
        explicit DatePopupStatus(std::weak_ptr<Label> status) noexcept
            : status_(std::move(status)) {}

        void operator()(bool expanded) const
        {
            if (const std::shared_ptr<Label> status = status_.lock()) {
                (*status).set_text(expanded
                    ? "Calendar open · arrows navigate · Enter commits · Esc cancels"
                    : "Calendar closed · popup and focus scope revoked");
            }
        }

    private:
        std::weak_ptr<Label> status_;
    };

    class OptionalDateStatus final {
    public:
        explicit OptionalDateStatus(std::weak_ptr<Label> status) noexcept
            : status_(std::move(status)) {}

        void operator()(bool checked) const
        {
            if (const std::shared_ptr<Label> status = status_.lock()) {
                (*status).set_text(checked
                    ? "Optional date enabled · value retained"
                    : "Optional date unchecked · value retained but inactive");
            }
        }

    private:
        std::weak_ptr<Label> status_;
    };

    enum class DateCommand {
        previous_day,
        next_day,
        open_calendar,
    };

    class DateCommandClick final {
    public:
        DateCommandClick(std::weak_ptr<DateTimePicker> picker,
                         DateCommand command) noexcept
            : picker_(std::move(picker)), command_(command) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<DateTimePicker> picker = picker_.lock();
            if (!picker) return;
            switch (command_) {
            case DateCommand::previous_day:
                (*picker).set_value(add_days((*picker).value(), -1));
                return;
            case DateCommand::next_day:
                (*picker).set_value(add_days((*picker).value(), 1));
                return;
            case DateCommand::open_calendar:
                (*picker).set_dropped_down(true);
                return;
            }
        }

    private:
        std::weak_ptr<DateTimePicker> picker_;
        DateCommand command_{};
    };

    (*page).add_at(label("gallery.date.heading", "DATES AND CALENDAR", 22.0,
                       700, shell_blue_dark), {24.0, 18.0, 620.0, 34.0});
    (*page).add_at(label(
        "gallery.date.lead",
        "Validated civil time, caller-owned formats, nullable and spinner modes, retained popup focus, and semantic date cells.",
        12.0, 400, muted), {26.0, 52.0, 920.0, 24.0});

    const DateTimeValue specimen{2026, 8U, 5U, 14U, 7U, 9U, 125U};
    std::shared_ptr<LayoutGroup> formats = group("gallery.date.formats",
                         "Formatting · four modes and an owned provider",
                         {930.0, 180.0});
    (*page).add_at(formats, {24.0, 92.0, 930.0, 180.0});
    const std::array<std::string_view, 4> captions{
        "Long date", "Short date", "Time", "Custom"};
    for (std::size_t index = 0U; index < captions.size(); ++index) {
        const double column = index % 2U == 0U ? 22.0 : 474.0;
        const double row = index < 2U ? 40.0 : 102.0;
        (*formats).add_at(label("gallery.date.format.caption." + std::to_string(index),
                              std::string(captions[index]), 10.0, 700,
                              shell_blue_dark),
                        {column, row, 86.0, 22.0});
        std::shared_ptr<gui_forms::DateTimePicker> picker = make_control<DateTimePicker>(
            StableId("gallery.date.format." + std::to_string(index)));
        (*picker).set_style(page_style());
        (*picker).set_value(specimen);
        (*picker).set_accessible_name(std::string(captions[index]) + " picker");
        if (index == 1U) {
            (*picker).set_format(DateTimePickerFormat::short_date);
        } else if (index == 2U) {
            (*picker).set_format(DateTimePickerFormat::time);
        } else if (index == 3U) {
            (*picker).set_custom_format("yyyy-MM-dd '·' HH:mm");
            (*picker).set_format(DateTimePickerFormat::custom);
        }
        (*formats).add_at(picker, {column + 92.0, row - 5.0, 320.0, 30.0});
    }

    std::shared_ptr<LayoutGroup> modes = group("gallery.date.modes",
                       "Modes and states · nullable, spinner, provider, disabled",
                       {930.0, 180.0});
    (*page).add_at(modes, {24.0, 290.0, 930.0, 180.0});

    std::shared_ptr<gui_forms::DateTimePicker> optional = make_control<DateTimePicker>(
        StableId("gallery.date.optional"));
    (*optional).set_style(page_style());
    (*optional).set_value(specimen);
    (*optional).set_format(DateTimePickerFormat::short_date);
    (*optional).set_show_check_box(true);
    (*optional).set_checked(false);
    (*optional).set_accessible_name("Optional unchecked date");
    (*modes).add_at(label("gallery.date.optional.caption", "Optional value", 10.0,
                        700, shell_blue_dark), {22.0, 40.0, 106.0, 22.0});
    (*modes).add_at(optional, {132.0, 35.0, 284.0, 30.0});

    std::shared_ptr<gui_forms::DateTimePicker> spinner = make_control<DateTimePicker>(
        StableId("gallery.date.spinner"));
    (*spinner).set_style(page_style());
    (*spinner).set_value(specimen);
    (*spinner).set_format(DateTimePickerFormat::short_date);
    (*spinner).set_show_up_down(true);
    (*spinner).set_accessible_name("Date spinner");
    (*modes).add_at(label("gallery.date.spinner.caption", "Up/down date", 10.0,
                        700, shell_blue_dark), {474.0, 40.0, 106.0, 22.0});
    (*modes).add_at(spinner, {584.0, 35.0, 320.0, 30.0});

    std::shared_ptr<gui_forms::DateTimePicker> provider = make_control<DateTimePicker>(
        StableId("gallery.date.provider"));
    (*provider).set_style(page_style());
    (*provider).set_value(specimen);
    (*provider).set_format_provider(french_date_provider());
    (*provider).set_accessible_name("Caller-owned French date provider");
    (*modes).add_at(label("gallery.date.provider.caption", "Owned provider", 10.0,
                        700, shell_blue_dark), {22.0, 102.0, 106.0, 22.0});
    (*modes).add_at(provider, {132.0, 97.0, 284.0, 30.0});

    std::shared_ptr<gui_forms::DateTimePicker> disabled = make_control<DateTimePicker>(
        StableId("gallery.date.disabled"));
    (*disabled).set_style(page_style());
    (*disabled).set_value(specimen);
    (*disabled).set_custom_format("MMM d, yyyy");
    (*disabled).set_format(DateTimePickerFormat::custom);
    (*disabled).set_enabled(false);
    (*disabled).set_accessible_name("Disabled date picker");
    (*modes).add_at(label("gallery.date.disabled.caption", "Disabled state", 10.0,
                        700, shell_blue_dark), {474.0, 102.0, 106.0, 22.0});
    (*modes).add_at(disabled, {584.0, 97.0, 320.0, 30.0});

    std::shared_ptr<LayoutGroup> lifecycle = group("gallery.date.lifecycle",
                           "Popup lifecycle · commit, cancel, bounds, semantics",
                           {930.0, 138.0});
    (*page).add_at(lifecycle, {24.0, 488.0, 930.0, 138.0});
    std::shared_ptr<gui_forms::DateTimePicker> primary = make_control<DateTimePicker>(
        StableId("gallery.date.primary"));
    (*primary).set_style(page_style());
    (*primary).set_value(specimen);
    (*primary).set_range({2026, 8U, 1U}, {2026, 8U, 31U, 23U, 59U, 59U, 999U});
    (*primary).set_accessible_name("Bounded appointment date");
    (*lifecycle).add_at(primary, {22.0, 38.0, 310.0, 31.0});

    std::shared_ptr<gui_forms::Button> previous = make_control<Button>(
        StableId("gallery.date.previous"), "Previous day");
    std::shared_ptr<gui_forms::Button> next = make_control<Button>(StableId("gallery.date.next"), "Next day");
    std::shared_ptr<gui_forms::Button> open = make_control<Button>(StableId("gallery.date.open"), "Open calendar");
    (*open).set_visual_style(ButtonVisualStyle::accent);
    (*lifecycle).add_at(previous, {352.0, 38.0, 138.0, 32.0});
    (*lifecycle).add_at(next, {504.0, 38.0, 124.0, 32.0});
    (*lifecycle).add_at(open, {642.0, 38.0, 164.0, 32.0});
    std::shared_ptr<Label> status = label("gallery.date.status",
                        "Ready · August 1–31, 2026 · F4 or Alt+Down opens",
                        10.0, 600, green);
    (*status).set_text_wrapping(TextWrapping::word);
    (*status).set_vertical_alignment(VerticalAlignment::near);
    (*lifecycle).add_at(status, {22.0, 86.0, 884.0, 34.0});

    const std::weak_ptr<DateTimePicker> weak_primary = primary;
    const std::weak_ptr<Label> weak_status = status;
    (*context).subscriptions.push_back((*primary).value_changed().subscribe(
        *status, DateValueStatus(weak_primary, weak_status)));
    (*context).subscriptions.push_back((*primary).drop_down_changed().subscribe(
        *status, DatePopupStatus(weak_status)));
    (*context).subscriptions.push_back((*optional).checked_changed().subscribe(
        *status, OptionalDateStatus(weak_status)));
    (*context).subscriptions.push_back((*previous).clicked().subscribe(
        *primary, DateCommandClick(weak_primary, DateCommand::previous_day)));
    (*context).subscriptions.push_back((*next).clicked().subscribe(
        *primary, DateCommandClick(weak_primary, DateCommand::next_day)));
    (*context).subscriptions.push_back((*open).clicked().subscribe(
        *primary, DateCommandClick(weak_primary, DateCommand::open_calendar)));
}

} // namespace gui_forms::gallery::pages
