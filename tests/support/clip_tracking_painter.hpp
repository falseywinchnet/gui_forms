#pragma once

#include "gui_forms/types/painter/painter.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace gui_forms::test_support {

// One text draw and the clip in force when it was made.
struct ClippedText final {
    std::string text;
    Point origin;
    Color color;
    Rect clip;
};

// Records each text draw with its clip (the intersection of the clips in
// force, through save and restore), for tests that check how a control
// layers its text: that no glyph is drawn twice over the same pixels.
// Translations are not followed; tests paint the control directly.
class ClipTrackingPainter final : public Painter {
public:
    void save() override { saved_.push_back(clip_); }
    void restore() override {
        if (saved_.empty()) return;
        clip_ = saved_.back();
        saved_.pop_back();
    }
    void translate(Point) override {}
    void clip_rect(Rect rect) override { clip_ = Rect::intersection(clip_, rect); }
    void fill_rect(Rect, Color) override {}
    void stroke_rect(Rect, Color, double) override {}
    void draw_line(Point, Point, Color, double) override {}
    void draw_image(ImageId, Rect, double) override {}
    void draw_text_utf8(Point origin, std::string_view text, FontSpec, Color color) override {
        texts.push_back({std::string(text), origin, color, clip_});
    }

    // Whether two draws of the same text at the same origin have clips that
    // overlap: the same glyph drawn twice where they do.
    [[nodiscard]] bool overdraws() const {
        for (std::size_t first = 0U; first < texts.size(); ++first) {
            for (std::size_t second = first + 1U; second < texts.size(); ++second) {
                const ClippedText& left = texts[first];
                const ClippedText& right = texts[second];
                if (left.text != right.text || left.origin.x != right.origin.x ||
                    left.origin.y != right.origin.y) {
                    continue;
                }
                const Rect shared = Rect::intersection(left.clip, right.clip);
                if (shared.width > 0.0 && shared.height > 0.0) return true;
            }
        }
        return false;
    }
    // How many draws of `text` there are.
    [[nodiscard]] std::size_t count(std::string_view text) const {
        std::size_t result = 0U;
        for (const ClippedText& painted : texts) {
            if (painted.text == text) ++result;
        }
        return result;
    }

    std::vector<ClippedText> texts;

private:
    Rect clip_{-1.0e9, -1.0e9, 2.0e9, 2.0e9};
    std::vector<Rect> saved_;
};

} // namespace gui_forms::test_support
