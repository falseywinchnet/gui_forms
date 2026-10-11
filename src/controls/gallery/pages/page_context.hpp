#pragma once

#include "gallery_pages.hpp"

#include "gallery.hpp"
#include "gallery_page_specs.hpp"

#include <algorithm>
#include <any>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// Shared by every page builder under src/controls/gallery/pages/ and by the
// Gallery shell (src/controls/gallery_pages.cpp): the retained page context
// stored in the shell root's Tag, the page palette and style, and the small
// helpers the builders compose pages from. Builder-local classes stay inside
// their builder.
namespace gui_forms::gallery::pages {

constexpr Color shell_blue = Color::rgba(39, 71, 101);
constexpr Color shell_blue_dark = Color::rgba(24, 45, 66);
constexpr Color shell_blue_light = Color::rgba(83, 127, 164);
constexpr Color canvas = Color::rgba(239, 243, 247);
constexpr Color paper = Color::rgba(255, 255, 255);
constexpr Color rule = Color::rgba(166, 179, 191);
constexpr Color ink = Color::rgba(28, 39, 50);
constexpr Color muted = Color::rgba(91, 107, 121);
constexpr Color accent = Color::rgba(39, 116, 184);
constexpr Color green = Color::rgba(49, 139, 94);
constexpr Color orange = Color::rgba(211, 121, 42);
constexpr Color violet = Color::rgba(116, 82, 153);

inline BasicControlStyle page_style() {
    BasicControlStyle style;
    style.face = Color::rgba(226, 232, 238);
    style.face_light = Color::rgba(248, 250, 252);
    style.paper = paper;
    style.highlight = Color::rgba(255, 255, 255);
    style.border = Color::rgba(144, 159, 173);
    style.dark_border = Color::rgba(67, 86, 103);
    style.text = ink;
    style.disabled_text = Color::rgba(133, 143, 152);
    style.accent = accent;
    style.accent_light = Color::rgba(203, 229, 247);
    style.link = Color::rgba(27, 88, 143);
    style.visited_link = violet;
    return style;
}

using LayoutPanel = ScaledPanel;
using LayoutGroup = ScaledGroupBox;

using Surface = ScaledPanel;

using EasingBoard = EasingPreview;

using DiagnosticsCard = MetricsView;
using DrawingEffectsBoard = DrawingSurface;

struct PageContext final {
    std::vector<Control::Ptr> pages;
    std::vector<std::shared_ptr<Button>> navigation;
    std::vector<SubscriptionToken> subscriptions;
    std::shared_ptr<Label> status;
    std::shared_ptr<EasingBoard> easing;
    std::shared_ptr<Button> motion_pause;
    std::vector<std::shared_ptr<ProgressBar>> animated_progress;
    ComponentContainer components;
    std::shared_ptr<Timer> ui_timer;
    std::shared_ptr<ToolTip> tooltips;
    std::shared_ptr<ErrorProvider> error_provider;
    std::shared_ptr<HelpProvider> help_provider;
    std::shared_ptr<BindingSource> binding_source;
    std::shared_ptr<Label> timer_status;
    std::shared_ptr<ProgressBar> timer_progress;
    std::shared_ptr<TrackBar> timer_interval;
    std::shared_ptr<ScaledPanel> timer_motion;
    std::shared_ptr<Button> timer_motion_target;
    std::shared_ptr<Button> timer_start;
    std::shared_ptr<Button> timer_stop;
    std::shared_ptr<Button> tooltip_show_disabled;
    std::shared_ptr<Button> tooltip_disabled_target;
    std::shared_ptr<TextBox> provider_error_target;
    std::shared_ptr<Button> provider_error_toggle;
    std::shared_ptr<TextBox> binding_editor;
    std::shared_ptr<CheckBox> binding_enabled;
    std::shared_ptr<TrackBar> binding_gain;
    std::shared_ptr<Button> binding_previous;
    std::shared_ptr<Button> binding_next;
    std::shared_ptr<Label> binding_status;
    std::shared_ptr<Label> dispatcher_status;
    std::shared_ptr<Button> dispatcher_invoke;
    std::vector<DispatchOperation> dispatcher_operations;
    std::thread dispatcher_worker;
    std::string dispatcher_trace;
    std::uint64_t dispatcher_batch{};
    std::shared_ptr<Label> host_services_status;
    std::shared_ptr<TextBox> clipboard_editor;
    std::uint64_t timer_ticks{};
    std::uint64_t next_host_request_id{1U};
    std::size_t selected_page{};
    // Registry-driven shell state: page ids and titles in navigation order,
    // the index of the page whose UI Timer runs while shown, and the
    // compiled-DML context so page selection is observable in GalleryModel.
    std::vector<std::string_view> page_ids;
    std::vector<std::string_view> page_titles;
    std::size_t timing_page{static_cast<std::size_t>(-1)};
    std::shared_ptr<GalleryContext> compiled;
    std::string last_dialog_acceptance{"No accepted dialog value yet"};
    bool live_motion{true};
    bool user_paused{};
    bool reduced_motion{};

    ~PageContext() {
        if (dispatcher_worker.joinable()) dispatcher_worker.join();
    }

    [[nodiscard]] MotionPolicy motion_policy() const noexcept {
        return {live_motion, user_paused, reduced_motion};
    }

    void update_motion_status() {
        if (!status) return;
        const MotionPolicy policy = motion_policy();
        if (!policy.enabled) {
            (*status).set_text("Motion disabled · zero animation wakeups");
        } else if (policy.paused && policy.reduced) {
            (*status).set_text(
                "Reduced motion + paused · phase retained · zero animation wakeups");
        } else if (policy.paused) {
            (*status).set_text("Motion paused · phase retained · zero animation wakeups");
        } else if (policy.reduced) {
            (*status).set_text(
                "Reduced motion · low cadence · limited excursion · animation active");
        } else {
            (*status).set_text("Full motion · continuous retained animation active");
        }
    }

    void apply_motion_policy() {
        const MotionPolicy policy = motion_policy();
        if (easing) (*easing).set_motion_policy(policy);
        for (const std::shared_ptr<gui_forms::ProgressBar>& progress : animated_progress) {
            (*progress).set_motion_policy(policy);
        }
        if (motion_pause) {
            (*motion_pause).set_enabled(live_motion);
            (*motion_pause).set_text(!live_motion ? "Motion disabled"
                                                   : user_paused ? "Resume motion"
                                                                 : "Pause motion");
        }
        update_motion_status();
    }

    void select_page(std::size_t index) {
        if (index >= pages.size()) {
            return;
        }
        selected_page = index;
        for (std::size_t page = 0; page < pages.size(); ++page) {
            (*pages[page]).set_visible(page == selected_page);
            BasicControlStyle style = page_style();
            if (page == selected_page) {
                style.face = Color::rgba(198, 222, 241);
                style.face_light = Color::rgba(235, 245, 252);
            }
            (*navigation[page]).set_style(style);
        }
        if (ui_timer) {
            if (selected_page == timing_page) (*ui_timer).start();
            else if ((*ui_timer).enabled()) (*ui_timer).stop();
        }
        (*status).set_text(std::string(page_titles[index]) +
                         "  ·  public GUI.Forms behavior  ·  CPU retained renderer");
        if (compiled) {
            static_cast<void>((*compiled).model.select_page(page_ids[index]));
            (*compiled).synchronize(std::string(navigation_id_prefix) +
                                    std::string(page_ids[index]));
        }
    }
};

inline std::shared_ptr<Label> label(std::string id, std::string text,
                             double size = 12.0, int weight = 400,
                             Color color = ink) {
    std::shared_ptr<gui_forms::Label> control = make_control<Label>(StableId(std::move(id)), std::move(text));
    (*control).set_font({FontRole::content, size,
                       static_cast<std::uint16_t>(weight), false});
    (*control).set_foreground(color);
    return control;
}

inline std::shared_ptr<LayoutGroup> group(std::string id, std::string title,
                                   Size design_size) {
    std::shared_ptr<gui_forms::ScaledGroupBox> control = make_control<LayoutGroup>(StableId(std::move(id)),
                                             std::move(title), design_size);
    (*control).set_background(paper);
    (*control).set_style(page_style());
    (*control).set_font({FontRole::control, 12.0, 700, false, 0.24});
    return control;
}

inline DateTimeFormatProvider french_date_provider() {
    DateTimeFormatProvider provider =
        DateTimeFormatProvider::english_united_states();
    provider.month_names = {
        "janvier", "février", "mars", "avril", "mai", "juin",
        "juillet", "août", "septembre", "octobre", "novembre", "décembre"};
    provider.abbreviated_month_names = {
        "janv.", "févr.", "mars", "avr.", "mai", "juin",
        "juil.", "août", "sept.", "oct.", "nov.", "déc."};
    provider.day_names = {
        "dimanche", "lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi"};
    provider.abbreviated_day_names = {
        "dim.", "lun.", "mar.", "mer.", "jeu.", "ven.", "sam."};
    provider.long_date_pattern = "dddd d MMMM yyyy";
    provider.short_date_pattern = "dd/MM/yyyy";
    provider.time_pattern = "HH:mm";
    provider.am_designator.clear();
    provider.pm_designator.clear();
    return provider;
}

template <typename RangeControlType>
class WeakRangeValueMirror final {
public:
    explicit WeakRangeValueMirror(
        std::weak_ptr<RangeControlType> target) noexcept
        : target_(std::move(target)) {}

    void operator()(double value) const
    {
        if (const std::shared_ptr<RangeControlType> target = target_.lock();
            target && (*target).value() != value) {
            (*target).set_value(value);
        }
    }

private:
    std::weak_ptr<RangeControlType> target_;
};

class WeakProgressValueSetter final {
public:
    explicit WeakProgressValueSetter(
        std::weak_ptr<ProgressBar> progress) noexcept
        : progress_(std::move(progress)) {}

    void operator()(double value) const
    {
        if (const std::shared_ptr<ProgressBar> progress = progress_.lock()) {
            (*progress).set_value(value);
        }
    }

private:
    std::weak_ptr<ProgressBar> progress_;
};

class DrawingEffectsPaint final {
public:
    void operator()(Painter& painter, Rect bounds, Rect) const
    {
        const double board_width = bounds.width;
        const double board_height = bounds.height;
        painter.fill_rect(bounds, Color::rgba(26, 39, 52));
        painter.fill_rect({0.0, 0.0, board_width, 4.0}, accent);
        painter.draw_text_utf8(
            {18.0, 27.0}, "RETAINED PAINTER COMPOSITION",
            {FontRole::control, 12.0, 700, false},
            Color::rgba(230, 240, 247));
        painter.draw_text_utf8(
            {18.0, 48.0},
            "save · clip · translate · fill · stroke · line · UTF-8",
            {FontRole::content, 10.0, 400, false},
            Color::rgba(164, 192, 211));
        painter.save();
        painter.clip_rect(
            {18.0, 62.0, board_width - 36.0, board_height - 78.0});
        painter.translate({18.0, 62.0});
        const double width = board_width - 36.0;
        for (std::size_t index = 0; index < 12U; ++index) {
            const double x = static_cast<double>(index) * width / 11.0;
            const Color color = index % 3U == 0U ? accent
                : index % 3U == 1U ? green : orange;
            painter.draw_line({0.0, 66.0}, {x, 0.0}, color, 1.5);
        }
        painter.fill_rect(
            {8.0, 20.0, width * 0.30, 38.0}, Color::rgba(43, 82, 112));
        painter.stroke_rect(
            {8.5, 20.5, width * 0.30 - 1.0, 37.0},
            Color::rgba(211, 228, 239), 1.0);
        painter.fill_rect(
            {width * 0.37, 12.0, width * 0.25, 54.0}, violet);
        painter.stroke_rect(
            {width * 0.37 + 0.5, 12.5, width * 0.25 - 1.0, 53.0},
            Color::rgba(234, 220, 244), 1.0);
        painter.fill_rect(
            {width * 0.68, 28.0, width * 0.29, 22.0}, green);
        painter.restore();
    }
};

} // namespace gui_forms::gallery::pages
