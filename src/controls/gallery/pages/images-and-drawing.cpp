#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_images_and_drawing(const std::shared_ptr<Surface>& page,
                            const std::shared_ptr<PageContext>& context) {
    (*page).add_at(label("gallery.images.heading", "IMAGES AND DRAWING", 22.0, 700,
                       shell_blue_dark), {24.0, 18.0, 620.0, 34.0});
    (*page).add_at(label("gallery.images.lead",
                       "Window-owned image resources, every PictureBox sizing policy, opacity, clipping, and retained painter primitives.",
                       12.0, 400, muted), {26.0, 52.0, 920.0, 24.0});

    std::shared_ptr<LayoutGroup> modes = group("gallery.images.modes",
                       "PictureBox · five canonical sizing modes", {930.0, 290.0});
    (*page).add_at(modes, {24.0, 92.0, 930.0, 290.0});
    constexpr std::array<std::pair<PictureBoxSizeMode, std::string_view>, 5> specs{{
        {PictureBoxSizeMode::normal, "Normal"},
        {PictureBoxSizeMode::stretch_image, "StretchImage"},
        {PictureBoxSizeMode::auto_size, "AutoSize"},
        {PictureBoxSizeMode::center_image, "CenterImage"},
        {PictureBoxSizeMode::zoom, "Zoom"},
    }};
    for (std::size_t index = 0; index < specs.size(); ++index) {
        const double x = 22.0 + static_cast<double>(index) * 180.0;
        std::shared_ptr<gui_forms::PictureBox> picture = make_control<PictureBox>(
            StableId("gallery.images.picture." + std::to_string(index)));
        (*picture).set_size_mode(specs[index].first);
        (*picture).set_border_style(BorderStyle::sunken);
        (*picture).set_background(index % 2U == 0U ? Color::rgba(244, 247, 249)
                                                  : Color::rgba(226, 234, 240));
        (*picture).set_accessible_name(std::string(specs[index].second) +
                                     " PictureBox test card");
        (*modes).add_at(picture, {x, 40.0, 158.0, 166.0});
        std::shared_ptr<Label> caption = label("gallery.images.caption." + std::to_string(index),
                             std::string(specs[index].second), 11.0, 700,
                             shell_blue_dark);
        (*caption).set_alignment(HorizontalAlignment::center);
        (*modes).add_at(caption, {x, 216.0, 158.0, 24.0});
        std::shared_ptr<Label> behavior = label("gallery.images.behavior." + std::to_string(index),
                              index == 0U ? "Native pixels" : index == 1U
                                  ? "Fill bounds" : index == 2U
                                  ? "Intrinsic measure" : index == 3U
                                  ? "Native centered" : "Aspect fit",
                              10.0, 400, muted);
        (*behavior).set_alignment(HorizontalAlignment::center);
        (*modes).add_at(behavior, {x, 242.0, 158.0, 22.0});
    }

    std::shared_ptr<LayoutGroup> opacity = group("gallery.images.opacity",
                         "Image opacity · renderer-neutral", {450.0, 220.0});
    (*page).add_at(opacity, {24.0, 404.0, 450.0, 220.0});
    constexpr std::array<double, 3> opacities{1.0, 0.60, 0.25};
    for (std::size_t index = 0; index < opacities.size(); ++index) {
        std::shared_ptr<gui_forms::PictureBox> picture = make_control<PictureBox>(
            StableId("gallery.images.opacity." + std::to_string(index)));
        (*picture).set_size_mode(PictureBoxSizeMode::zoom);
        (*picture).set_image_opacity(opacities[index]);
        (*picture).set_border_style(BorderStyle::line);
        (*picture).set_background(Color::rgba(230, 236, 241));
        (*picture).set_accessible_name(
            "Test card at " + std::to_string(static_cast<int>(opacities[index] * 100.0)) +
            " percent opacity");
        (*opacity).add_at(picture,
                        {22.0 + static_cast<double>(index) * 136.0, 42.0,
                         120.0, 112.0});
        std::shared_ptr<Label> caption = label("gallery.images.opacity.caption." +
                                 std::to_string(index),
                             std::to_string(static_cast<int>(opacities[index] * 100.0)) +
                                 "%",
                             10.0, 700, shell_blue_dark);
        (*caption).set_alignment(HorizontalAlignment::center);
        (*opacity).add_at(caption,
                        {22.0 + static_cast<double>(index) * 136.0, 164.0,
                         120.0, 22.0});
    }

    std::shared_ptr<LayoutGroup> primitives = group("gallery.images.primitives",
                             "Painter · image materials", {460.0, 220.0});
    (*page).add_at(primitives, {494.0, 404.0, 460.0, 220.0});
    std::shared_ptr<gui_forms::DrawingSurface> board = make_control<DrawingEffectsBoard>(
        StableId("gallery.images.drawing.board"));
    (*board).set_accessible_name("Renderer-neutral drawing primitive composition");
    (*board).set_accessible_description(
        "Nested clipping, translation, fills, strokes, lines, and text");
    (*board).set_paint_callback(DrawingEffectsPaint());
    (*primitives).add_at(board, {20.0, 38.0, 194.0, 158.0});

    std::shared_ptr<gui_drawing::Bitmap> raster_bitmap = std::make_shared<gui_drawing::Bitmap>(12U, 12U);
    for (std::uint32_t index = 0U; index < 12U; ++index) {
        (*raster_bitmap).set_pixel(
            index, index,
            gui_drawing::Color::from_rgb(39U, 116U, 184U));
        (*raster_bitmap).set_pixel(
            11U - index, index,
            gui_drawing::Color::from_rgb(211U, 121U, 42U));
    }
    std::shared_ptr<gui_forms::RasterCanvas> raster = make_control<RasterCanvas>(
        StableId("gallery.images.raster.canvas"));
    (*raster).set_accessible_name("Raster canvas transparency and sampling");
    (*raster).set_bitmap(std::move(raster_bitmap));
    (*raster).set_zoom(6.0);
    (*raster).set_transparency_cell_size(8.0);
    (*primitives).add_at(raster, {224.0, 54.0, 96.0, 132.0});
    std::shared_ptr<Label> raster_caption = label("gallery.images.raster.caption",
                                "RASTER", 9.0, 700, shell_blue_dark);
    (*raster_caption).set_alignment(HorizontalAlignment::center);
    (*primitives).add_at(raster_caption, {224.0, 34.0, 96.0, 16.0});

    std::shared_ptr<gui_forms::MaterialPanel> nine_patch = make_control<MaterialPanel>(
        StableId("gallery.images.material.nine-patch"));
    (*nine_patch).set_accessible_name("Density-aware nine-patch material");
    (*nine_patch).set_accessible_description(
        "Corners remain fixed while edge and center source regions stretch");
    (*primitives).add_at(nine_patch, {334.0, 52.0, 106.0, 54.0});
    std::shared_ptr<Label> nine_caption = label("gallery.images.material.nine-patch.caption",
                              "NINE-PATCH", 9.0, 700, shell_blue_dark);
    (*primitives).add_at(nine_caption, {334.0, 34.0, 106.0, 16.0});
    std::shared_ptr<gui_forms::MaterialPanel> tiled = make_control<MaterialPanel>(
        StableId("gallery.images.material.tile"));
    (*tiled).set_accessible_name("Tiled source-image material");
    (*tiled).set_accessible_description(
        "Exact logical period with cropped partial edge tiles");
    (*primitives).add_at(tiled, {334.0, 132.0, 106.0, 54.0});
    std::shared_ptr<Label> tile_caption = label("gallery.images.material.tile.caption",
                              "TEXTURE TILE", 9.0, 700, shell_blue_dark);
    (*primitives).add_at(tile_caption, {334.0, 114.0, 106.0, 16.0});
    static_cast<void>(context);
}

} // namespace gui_forms::gallery::pages
