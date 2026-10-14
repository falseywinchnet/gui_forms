#pragma once

#include "gui_forms/controls/panel/panel.hpp"
#include "gui_forms/controls/tool_strip_panel/tool_strip_panel.hpp"

#include <memory>
#include <span>

namespace gui_forms {

// Four ToolStripPanels around a content panel (Windows Forms'
// ToolStripContainer). The top and bottom panels span the container's width
// and are as tall as their strips; the left and right panels fill the height
// between them and are as wide as their strips; the content panel takes the
// rest. Put strips in a panel with ToolStripPanel::join and the window's
// controls in content_panel(). Dragging strips between panels is not
// supported yet.
class ToolStripContainer final : public Control {
public:
    static constexpr bool initialize_tree_after_construction = true;
    explicit ToolStripContainer(StableId stable_id);
    void initialize_control_tree();

    // The panels are child controls named <stable_id>.top, .bottom, .left,
    // .right and .content.
    [[nodiscard]] const std::shared_ptr<ToolStripPanel>& top_panel() const noexcept { return top_; }
    [[nodiscard]] const std::shared_ptr<ToolStripPanel>& bottom_panel() const noexcept { return bottom_; }
    [[nodiscard]] const std::shared_ptr<ToolStripPanel>& left_panel() const noexcept { return left_; }
    [[nodiscard]] const std::shared_ptr<ToolStripPanel>& right_panel() const noexcept { return right_; }
    [[nodiscard]] const std::shared_ptr<Panel>& content_panel() const noexcept { return content_; }

    // A hidden panel takes no room and shows none of its strips.
    [[nodiscard]] bool top_panel_visible() const noexcept { return top_visible_; }
    void set_top_panel_visible(bool visible);
    [[nodiscard]] bool bottom_panel_visible() const noexcept { return bottom_visible_; }
    void set_bottom_panel_visible(bool visible);
    [[nodiscard]] bool left_panel_visible() const noexcept { return left_visible_; }
    void set_left_panel_visible(bool visible);
    [[nodiscard]] bool right_panel_visible() const noexcept { return right_visible_; }
    void set_right_panel_visible(bool visible);

    void arrange(Rect final_bounds) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    // Stage T1 anatomy, as dot members; its states are Control's.
    struct Parts final {
        PartGeometry region;
        PartGeometry top_panel;
        PartGeometry bottom_panel;
        PartGeometry left_panel;
        PartGeometry right_panel;
        PartGeometry content_panel;
    };
    [[nodiscard]] Parts parts() const;

protected:
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    // Where the panels sit for one client size.
    struct Layout final {
        Rect top{};
        Rect bottom{};
        Rect left{};
        Rect right{};
        Rect content{};
    };
    [[nodiscard]] Layout layout_for(Size client);
    void set_panel_visible(bool& flag, bool visible);

    std::shared_ptr<ToolStripPanel> top_{};
    std::shared_ptr<ToolStripPanel> bottom_{};
    std::shared_ptr<ToolStripPanel> left_{};
    std::shared_ptr<ToolStripPanel> right_{};
    std::shared_ptr<Panel> content_{};
    bool top_visible_{true};
    bool bottom_visible_{true};
    bool left_visible_{true};
    bool right_visible_{true};
};

} // namespace gui_forms
