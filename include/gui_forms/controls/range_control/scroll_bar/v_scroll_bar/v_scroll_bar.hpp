#pragma once

#include "gui_forms/controls/range_control/scroll_bar/scroll_bar.hpp"

namespace gui_forms {

class VScrollBar final : public ScrollBar {
public:
    explicit VScrollBar(StableId stable_id);

    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
};

} // namespace gui_forms
