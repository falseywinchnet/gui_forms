#include "gui_forms/surface_material/presets/material_presets.hpp"

#include <cmath>
#include <iterator>
#include <stdexcept>

namespace gui_forms {

SurfaceMaterial gloss_material(const GlossColours& colours, double corner_radius) {
    if (!std::isfinite(corner_radius) || corner_radius < 0.0 || corner_radius > 4096.0) {
        throw std::invalid_argument("a gloss material's corner radius lies in [0, 4096]");
    }
    const GradientStop stops[] = {
        {0.0, colours.top},
        {gloss_break_start, colours.above_break},
        {gloss_break_end, colours.below_break},
        {1.0, colours.bottom},
    };
    SurfaceMaterial material{};
    material.fills.clear();
    material.fills.push_back(MaterialFillLayer::linear(Point{0.0, 0.0}, Point{0.0, 1.0}, stops, std::size(stops)));
    const double border_width = 1.0;
    material.border = MaterialBorder{colours.border, border_width};
    MaterialKeyline shine{};
    shine.edge = MaterialEdge::top;
    shine.color = colours.shine;
    shine.width = 1.0;
    shine.inset = border_width;
    material.keylines.push_back(shine);
    material.corner_radius = corner_radius;
    return material;
}

} // namespace gui_forms
