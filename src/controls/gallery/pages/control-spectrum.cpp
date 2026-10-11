#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_control_spectrum(const std::shared_ptr<Surface>& page,
                          const std::shared_ptr<PageContext>& context) {
    class SoundCueClick final {
    public:
        SoundCueClick(std::weak_ptr<CheckBox> enabled,
                      HostSoundCue cue,
                      std::shared_ptr<PageContext> context) noexcept
            : enabled_(std::move(enabled)), cue_(cue), context_(std::move(context)) {}

        void operator()(ButtonBase& source) const
        {
            HostServices* services = source.attached_window() == nullptr
                ? nullptr : (*source.attached_window()).host_services();
            HostServiceStatus result{HostServiceError::unsupported};
            if (services != nullptr) {
                const std::chrono::steady_clock::duration now =
                    std::chrono::steady_clock::now().time_since_epoch();
                const std::chrono::nanoseconds::rep stamp =
                    std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
                const std::shared_ptr<CheckBox> enabled = enabled_.lock();
                result = (*services).play_sound_cue(
                    {cue_, enabled && (*enabled).checked() ? 0.72 : 0.0,
                     static_cast<std::uint64_t>(stamp)});
            }
            (*(*context_).status).set_text(
                std::string("Sound cue ") + host_sound_cue_name(cue_) +
                " · " + host_service_error_name(result.error) +
                " · visual meaning remains complete when muted");
        }

    private:
        std::weak_ptr<CheckBox> enabled_;
        HostSoundCue cue_{};
        std::shared_ptr<PageContext> context_;
    };

    (*page).add_at(label("gallery.controls.heading", "CONTROL SPECTRUM", 22.0, 700,
                       shell_blue_dark), {24.0, 18.0, 500.0, 34.0});
    (*page).add_at(label("gallery.controls.lead",
                       "Every public stock control, variant, state, and keyboard path lives here.",
                       12.0, 400, muted), {26.0, 52.0, 760.0, 24.0});

    std::shared_ptr<LayoutGroup> button_group = group("gallery.controls.buttons", "Buttons · four public styles",
                              {930.0, 142.0});
    (*page).add_at(button_group, {24.0, 92.0, 930.0, 142.0});
    const std::array<std::pair<ButtonVisualStyle, std::string_view>, 4> buttons{{
        {ButtonVisualStyle::standard, "&Standard"},
        {ButtonVisualStyle::flat, "&Flat"},
        {ButtonVisualStyle::accent, "&Accent"},
        {ButtonVisualStyle::command, "&Command"},
    }};
    for (std::size_t index = 0; index < buttons.size(); ++index) {
        std::shared_ptr<gui_forms::Button> button = make_control<Button>(
            StableId("gallery.controls.button." + std::to_string(index)),
            std::string(buttons[index].second));
        (*button).set_visual_style(buttons[index].first);
        (*button).set_style(page_style());
        (*button_group).add_at(button,
                             {24.0 + static_cast<double>(index) * 215.0,
                              42.0, 185.0, 42.0});
    }
    std::shared_ptr<gui_forms::Button> disabled = make_control<Button>(StableId("gallery.controls.button.disabled"),
                                         "Disabled state");
    (*disabled).set_enabled(false);
    (*button_group).add_at(disabled, {24.0, 92.0, 185.0, 30.0});
    std::shared_ptr<gui_forms::Button> default_button = make_control<Button>(
        StableId("gallery.controls.button.default"), "&Default action");
    (*button_group).add_at(default_button, {239.0, 92.0, 185.0, 30.0});
    std::shared_ptr<gui_forms::LinkLabel> link = make_control<LinkLabel>(StableId("gallery.controls.link"),
                                        "LinkLabel · visited on activation");
    (*button_group).add_at(link, {454.0, 92.0, 260.0, 30.0});

    std::shared_ptr<LayoutGroup> check_group = group("gallery.controls.checks",
                             "Check boxes · classic, modern, toggle", {450.0, 210.0});
    (*page).add_at(check_group, {24.0, 252.0, 450.0, 210.0});
    for (std::size_t index = 0; index < 3U; ++index) {
        std::shared_ptr<gui_forms::CheckBox> check = make_control<CheckBox>(
            StableId("gallery.controls.check." + std::to_string(index)),
            index == 0U ? "&Classic checkbox" : index == 1U
                ? "&Modern checkbox" : "&Toggle checkbox");
        (*check).set_indicator_style(static_cast<ChoiceIndicatorStyle>(index));
        (*check).set_checked(index != 0U);
        (*check_group).add_at(check,
                            {24.0, 42.0 + static_cast<double>(index) * 42.0,
                             300.0, 28.0});
    }
    std::shared_ptr<gui_forms::CheckBox> tri = make_control<CheckBox>(StableId("gallery.controls.check.tri"),
                                      "Three-state indeterminate");
    (*tri).set_three_state(true);
    (*tri).set_check_state(CheckState::indeterminate);
    (*check_group).add_at(tri, {24.0, 168.0, 300.0, 28.0});

    std::shared_ptr<LayoutGroup> radio_group = group("gallery.controls.radios",
                             "Radio buttons · three indicator systems", {460.0, 210.0});
    (*page).add_at(radio_group, {494.0, 252.0, 460.0, 210.0});
    for (std::size_t index = 0; index < 3U; ++index) {
        std::shared_ptr<gui_forms::RadioButton> radio = make_control<RadioButton>(
            StableId("gallery.controls.radio." + std::to_string(index)),
            index == 0U ? "Classic radio" : index == 1U
                ? "Modern radio" : "Toggle radio");
        (*radio).set_indicator_style(static_cast<ChoiceIndicatorStyle>(index));
        (*radio).set_group_name("gallery-radio-style");
        (*radio).set_checked(index == 1U);
        (*radio_group).add_at(radio,
                            {24.0, 42.0 + static_cast<double>(index) * 42.0,
                             310.0, 28.0});
    }

    std::shared_ptr<LayoutGroup> typography = group("gallery.controls.typography", "Labels and typography roles",
                            {930.0, 150.0});
    (*page).add_at(typography, {24.0, 480.0, 930.0, 150.0});
    std::shared_ptr<Label> title = label("gallery.controls.type.title", "Portsmouth Rapids control title",
                       19.0, 700, shell_blue_dark);
    (*title).set_font({FontRole::control, 19.0, 700, false});
    (*typography).add_at(title, {24.0, 38.0, 420.0, 30.0});
    std::shared_ptr<Label> content = label("gallery.controls.type.content",
        "Carlito body specimen: editable and field text stays quiet, clear, and host-independent.",
                         12.0, 400, ink);
    (*content).set_text_wrapping(TextWrapping::word);
    (*content).set_vertical_alignment(VerticalAlignment::near);
    (*typography).add_at(content, {24.0, 76.0, 430.0, 48.0});
    std::shared_ptr<Label> center = label("gallery.controls.type.center", "Centered label", 12.0, 600, accent);
    (*center).set_alignment(HorizontalAlignment::center);
    (*typography).add_at(center, {24.0, 118.0, 250.0, 20.0});

    (*typography).add_at(label("gallery.controls.sound.title",
                             "SEMANTIC SOUND CUES · OPTIONAL HOST", 11.0, 700,
                             shell_blue_dark), {500.0, 34.0, 390.0, 22.0});
    const std::array<std::pair<HostSoundCue, std::string_view>, 4> cues{{
        {HostSoundCue::notification, "Notice"},
        {HostSoundCue::success, "Success"},
        {HostSoundCue::warning, "Warning"},
        {HostSoundCue::error, "Error"},
    }};
    std::shared_ptr<gui_forms::CheckBox> sound_enabled = make_control<CheckBox>(
        StableId("gallery.controls.sound.enabled"), "Sound enabled");
    (*sound_enabled).set_checked(true);
    (*sound_enabled).set_indicator_style(ChoiceIndicatorStyle::modern);
    (*typography).add_at(sound_enabled, {500.0, 108.0, 160.0, 26.0});
    for (std::size_t index = 0U; index < cues.size(); ++index) {
        std::shared_ptr<gui_forms::Button> cue_button = make_control<Button>(
            StableId("gallery.controls.sound." + std::to_string(index)),
            std::string(cues[index].second));
        (*cue_button).set_visual_style(index == 3U ? ButtonVisualStyle::standard
                                                : ButtonVisualStyle::command);
        (*typography).add_at(cue_button,
                           {500.0 + static_cast<double>(index) * 98.0,
                            64.0, 88.0, 32.0});
        const HostSoundCue cue = cues[index].first;
        const std::weak_ptr<CheckBox> weak_enabled = sound_enabled;
        (*context).subscriptions.push_back((*cue_button).clicked().subscribe(
            *typography, SoundCueClick(weak_enabled, cue, context)));
    }
}

} // namespace gui_forms::gallery::pages
