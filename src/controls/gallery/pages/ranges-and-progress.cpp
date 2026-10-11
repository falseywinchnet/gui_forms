#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_ranges(const std::shared_ptr<Surface>& page,
                const std::shared_ptr<PageContext>& context) {
    (*page).add_at(label("gallery.ranges.heading", "RANGES AND PROGRESS", 22.0, 700,
                       shell_blue_dark), {24.0, 18.0, 500.0, 34.0});
    (*page).add_at(label("gallery.ranges.lead",
                       "Live captured dragging, keyboard control, vertical orientation, and retained animation.",
                       12.0, 400, muted), {26.0, 52.0, 850.0, 24.0});

    std::shared_ptr<LayoutGroup> sliders = group("gallery.ranges.sliders", "TrackBar · classic, filled, compact · ScrollBar",
                         {930.0, 230.0});
    (*page).add_at(sliders, {24.0, 92.0, 930.0, 230.0});
    std::vector<std::shared_ptr<TrackBar>> tracks;
    for (std::size_t index = 0; index < 3U; ++index) {
        std::shared_ptr<Label> caption = label("gallery.ranges.slider.caption." + std::to_string(index),
                             index == 0U ? "Classic ticks" : index == 1U
                                 ? "Filled rail" : "Compact rail", 11.0, 600, muted);
        (*sliders).add_at(caption,
                        {24.0, 38.0 + static_cast<double>(index) * 58.0,
                         130.0, 24.0});
        std::shared_ptr<gui_forms::TrackBar> track = make_control<TrackBar>(
            StableId("gallery.ranges.slider." + std::to_string(index)));
        (*track).set_accessible_name(index == 0U ? "Classic ticks"
            : index == 1U ? "Filled rail" : "Compact rail");
        (*track).set_visual_style(static_cast<TrackBarVisualStyle>(index));
        (*track).set_value(28.0 + static_cast<double>(index) * 24.0);
        (*sliders).add_at(track,
                        {155.0, 35.0 + static_cast<double>(index) * 58.0,
                         690.0, 32.0});
        tracks.push_back(track);
    }
    std::shared_ptr<gui_forms::HScrollBar> horizontal_scroll = make_control<HScrollBar>(
        StableId("gallery.ranges.scroll.horizontal"));
    (*horizontal_scroll).set_accessible_name("Horizontal viewport scrollbar");
    (*horizontal_scroll).set_range(0.0, 100.0);
    (*horizontal_scroll).set_large_change(18.0);
    (*horizontal_scroll).set_value(36.0);
    (*sliders).add_at(horizontal_scroll, {155.0, 200.0, 690.0, 18.0});
    (*sliders).add_at(label("gallery.ranges.scroll.caption", "Viewport", 11.0, 600,
                          muted), {24.0, 197.0, 120.0, 22.0});

    std::shared_ptr<gui_forms::VScrollBar> vertical_scroll = make_control<VScrollBar>(
        StableId("gallery.ranges.scroll.vertical"));
    (*vertical_scroll).set_accessible_name("Vertical viewport scrollbar");
    (*vertical_scroll).set_range(0.0, 100.0);
    (*vertical_scroll).set_large_change(18.0);
    (*vertical_scroll).set_value(36.0);
    (*sliders).add_at(vertical_scroll, {874.0, 38.0, 18.0, 160.0});
    const std::weak_ptr<VScrollBar> weak_vertical_scroll = vertical_scroll;
    (*context).subscriptions.push_back((*horizontal_scroll).value_changed().subscribe(
        *vertical_scroll, WeakRangeValueMirror<VScrollBar>(weak_vertical_scroll)));
    const std::weak_ptr<HScrollBar> weak_horizontal_scroll = horizontal_scroll;
    (*context).subscriptions.push_back((*vertical_scroll).value_changed().subscribe(
        *horizontal_scroll,
        WeakRangeValueMirror<HScrollBar>(weak_horizontal_scroll)));

    std::shared_ptr<LayoutGroup> progress_group = group("gallery.ranges.progress",
                                "ProgressBar · static, indeterminate, luminance, marching, laser",
                                {930.0, 325.0});
    (*page).add_at(progress_group, {24.0, 340.0, 930.0, 325.0});
    for (std::size_t index = 0; index < 6U; ++index) {
        std::shared_ptr<Label> caption = label("gallery.ranges.progress.caption." + std::to_string(index),
                             index == 0U ? "Blocks" : index == 1U ? "Continuous"
                                 : index == 2U ? "Marquee"
                                 : index == 3U ? "Luminance pulse"
                                 : index == 4U ? "Marching stripes"
                                               : "Laser etch",
                             11.0, 600, muted);
        (*progress_group).add_at(caption,
                               {24.0, 36.0 + static_cast<double>(index) * 45.0,
                                122.0, 24.0});
        std::shared_ptr<gui_forms::ProgressBar> progress = make_control<ProgressBar>(
            StableId("gallery.ranges.progress." + std::to_string(index)));
        (*progress).set_accessible_name(index == 0U ? "Blocks progress"
            : index == 1U ? "Continuous progress"
            : index == 2U ? "Marquee progress"
            : index == 3U ? "Slow luminance pulse progress"
            : index == 4U ? "Classic marching stripe progress"
                          : "Laser etching progress");
        (*progress).set_visual_style(static_cast<ProgressBarVisualStyle>(index));
        if (index == 3U)
            (*progress).set_animation_period(std::chrono::milliseconds(2400));
        else if (index == 4U)
            (*progress).set_animation_period(std::chrono::milliseconds(850));
        else if (index == 5U)
            (*progress).set_animation_period(std::chrono::milliseconds(1250));
        (*progress).set_value(64.0);
        (*progress_group).add_at(progress,
                               {160.0, 38.0 + static_cast<double>(index) * 45.0,
                                695.0, 24.0});
        if (index >= 2U) {
            (*context).animated_progress.push_back(progress);
        }
        if (index < tracks.size()) {
            (*context).subscriptions.push_back((*tracks[index]).value_changed().subscribe(
                *progress, WeakProgressValueSetter(progress)));
        }
    }
}

} // namespace gui_forms::gallery::pages
