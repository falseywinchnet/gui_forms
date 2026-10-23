#pragma once

#include "gui_forms/types.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace gui_forms {

// A conic gradient (E9.6) sweeps its stops clockwise about `center`,
// starting `angle_degrees` from up (CSS `from`), over `period_degrees` of
// the turn: 360 for `conic-gradient`, less for `repeating-conic-gradient`
// with spread `repeat`. A radial gradient repeats or reflects its stops
// past its radii with spread `repeat` or `reflect` (E9.6).
//
// The engine's material primitive draws both exactly. The built-in path,
// paint_surface_material, draws a repeating radial gradient padded and a
// conic gradient as a solid of its first stop: a theme that needs them
// selects the material primitive for the part.
// How a fill layer joins the layers beneath it (E9.6, CSS
// `background-blend-mode`): source-over, multiply or screen, on
// premultiplied colour. A material whose fills all use `normal` draws them
// as before; one with another mode first gathers its fills into one face,
// apart from the shadows beneath, as CSS blends background layers only with
// one another. The built-in path, paint_surface_material, draws every
// layer source-over.
enum class MaterialBlendMode : std::uint8_t {
    normal,
    multiply,
    screen,
};

enum class MaterialFillKind : std::uint8_t {
    solid, linear_gradient, radial_gradient, image, conic_gradient,
};
enum class MaterialImageMode : std::uint8_t {
    stretch, tile, nine_patch,
};
enum class MaterialCoordinateSpace : std::uint8_t {
    normalized, logical,
};
enum class MaterialLinearGeometry : std::uint8_t {
    endpoints, css_angle,
};

// One renderer-neutral material layer with explicit coordinate, spread,
// image sampling, scale, opacity, and nine-patch state.
struct MaterialFillLayer final {
    MaterialFillKind kind{MaterialFillKind::solid};
    MaterialCoordinateSpace coordinate_space{MaterialCoordinateSpace::normalized};
    MaterialLinearGeometry linear_geometry{MaterialLinearGeometry::endpoints};
    double angle_degrees{180.0};
    Color color{Color::rgba(255, 255, 255)};
    Point start{};
    Point end{1.0, 0.0};
    Point center{0.5, 0.5};
    Size radii{0.5, 0.5};
    std::vector<GradientStop> stops;
    GradientSpreadMode spread{GradientSpreadMode::pad};
    ImageId image{};
    Size image_pixel_size{};
    Insets image_slice{};
    double image_scale{1.0};
    double opacity{1.0};
    MaterialImageMode image_mode{MaterialImageMode::stretch};
    double period_degrees{360.0};
    MaterialBlendMode blend{MaterialBlendMode::normal};

    [[nodiscard]] static MaterialFillLayer solid(Color color);
    [[nodiscard]] static MaterialFillLayer linear(
        Point start, Point end, std::vector<GradientStop> stops,
        MaterialCoordinateSpace space = MaterialCoordinateSpace::normalized,
        GradientSpreadMode spread = GradientSpreadMode::pad);
    [[nodiscard]] static MaterialFillLayer linear_css_angle(
        double angle_degrees, std::vector<GradientStop> stops,
        GradientSpreadMode spread = GradientSpreadMode::pad);
    [[nodiscard]] static MaterialFillLayer linear_css_angle(
        double angle_degrees, const GradientStop* stops,
        std::size_t stop_count,
        GradientSpreadMode spread = GradientSpreadMode::pad);
    [[nodiscard]] static MaterialFillLayer linear(
        Point start, Point end, const GradientStop* stops,
        std::size_t stop_count,
        MaterialCoordinateSpace space = MaterialCoordinateSpace::normalized,
        GradientSpreadMode spread = GradientSpreadMode::pad);
    [[nodiscard]] static MaterialFillLayer repeating_linear(
        Point start, Point end, std::vector<GradientStop> stops,
        MaterialCoordinateSpace space = MaterialCoordinateSpace::logical);
    [[nodiscard]] static MaterialFillLayer repeating_linear(
        Point start, Point end, const GradientStop* stops,
        std::size_t stop_count,
        MaterialCoordinateSpace space = MaterialCoordinateSpace::logical);
    [[nodiscard]] static MaterialFillLayer radial(
        Point center, Size radii, std::vector<GradientStop> stops,
        MaterialCoordinateSpace space = MaterialCoordinateSpace::normalized);
    [[nodiscard]] static MaterialFillLayer radial(
        Point center, Size radii, const GradientStop* stops,
        std::size_t stop_count,
        MaterialCoordinateSpace space = MaterialCoordinateSpace::normalized);
    [[nodiscard]] static MaterialFillLayer repeating_radial(
        Point center, Size radii, std::vector<GradientStop> stops,
        MaterialCoordinateSpace space = MaterialCoordinateSpace::normalized);
    [[nodiscard]] static MaterialFillLayer conic(
        Point center, double from_degrees, std::vector<GradientStop> stops,
        MaterialCoordinateSpace space = MaterialCoordinateSpace::normalized);
    [[nodiscard]] static MaterialFillLayer repeating_conic(
        Point center, double from_degrees, double period_degrees,
        std::vector<GradientStop> stops,
        MaterialCoordinateSpace space = MaterialCoordinateSpace::normalized);
    [[nodiscard]] static MaterialFillLayer stretched_image(
        ImageId image, Size pixel_size, double opacity = 1.0);
    [[nodiscard]] static MaterialFillLayer tiled_image(
        ImageId image, Size pixel_size,
        double source_pixels_per_logical_pixel = 1.0,
        double opacity = 1.0);
    [[nodiscard]] static MaterialFillLayer nine_patch(
        ImageId image, Size pixel_size, Insets source_slice,
        double source_pixels_per_logical_pixel = 1.0,
        double opacity = 1.0);
    friend bool operator==(const MaterialFillLayer& left,
                           const MaterialFillLayer& right) noexcept(noexcept(
        left.kind == right.kind &&
        left.coordinate_space == right.coordinate_space &&
        left.linear_geometry == right.linear_geometry &&
        left.angle_degrees == right.angle_degrees &&
        left.color == right.color && left.start == right.start &&
        left.end == right.end && left.center == right.center &&
        left.radii == right.radii && left.stops == right.stops &&
        left.spread == right.spread && left.image == right.image &&
        left.image_pixel_size == right.image_pixel_size &&
        left.image_slice == right.image_slice &&
        left.image_scale == right.image_scale &&
        left.opacity == right.opacity && left.image_mode == right.image_mode &&
        left.period_degrees == right.period_degrees &&
        left.blend == right.blend)) {
        return left.kind == right.kind &&
               left.coordinate_space == right.coordinate_space &&
               left.linear_geometry == right.linear_geometry &&
               left.angle_degrees == right.angle_degrees &&
               left.color == right.color && left.start == right.start &&
               left.end == right.end && left.center == right.center &&
               left.radii == right.radii && left.stops == right.stops &&
               left.spread == right.spread && left.image == right.image &&
               left.image_pixel_size == right.image_pixel_size &&
               left.image_slice == right.image_slice &&
               left.image_scale == right.image_scale &&
               left.opacity == right.opacity &&
               left.image_mode == right.image_mode &&
               left.period_degrees == right.period_degrees &&
               left.blend == right.blend;
    }
};

} // namespace gui_forms
