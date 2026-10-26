#pragma once

#include "gui_forms/anatomy/types/anatomy_types.hpp"
#include "gui_forms/engine/authored_gradient/authored_gradient.hpp"
#include "gui_forms/engine/engine_expression/engine_expression.hpp"
#include "gui_forms/engine/motion_value/motion_value.hpp"
#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"
#include "gui_forms/engine/types/engine_types.hpp"
#include "gui_forms/surface_material/types/surface_material_types.hpp"
#include "gui_forms/types/paint_types/paint_types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

// The cascade (T2 plan, phase E4): which primitive draws a part, and with
// which parameters. Theme data is rules; each rule has a selector, a cascade
// level and declarations. T3 fills the rules by parsing theme documents and
// FD fills the inline styles; until then they are C++ data (tests).
//
// The levels, lowest first. A declaration at a higher level wins over every
// declaration at a lower one; within a level, the more specific selector
// wins, and among equals the later rule. `built_in` and
// `forced_high_contrast` are not authored: the built-in recipe is what a
// part falls back to when no rule selects a primitive, and forced high
// contrast is the engine's own pass over the result.
enum class CascadeLevel : std::uint8_t {
    built_in,
    theme_sheet,
    class_rule,
    stable_id_rule,
    inline_style,
    forced_high_contrast,
    count,
};

[[nodiscard]] std::string_view cascade_level_name(CascadeLevel level) noexcept;

// Whether `name` is a valid style class name: 1 to 64 characters of
// lowercase ASCII letters, digits, '-' and '_', the first a letter.
[[nodiscard]] bool valid_style_class_name(std::string_view name) noexcept;

// What a rule applies to. Every field it sets must match; an empty field
// matches anything. No combinators: a selector names one part of one
// control.
// - `control_type`: the control's anatomy id (ControlAnatomy::control_id,
//   its id in manifests/controls.json).
// - `stable_id`: the control's stable ID.
// - `classes`: classes the control must all have (Control::classes).
// - `part`: the part's declared name. A part is known by name only when it
//   is painted with a key (PartPaint::key); an unkeyed part never matches a
//   selector that names one.
// - `role`: the part's role.
// - `states`: states the part must all be in.
struct RuleSelector final {
    std::string control_type{};
    std::string stable_id{};
    std::vector<std::string> classes{};
    std::string part{};
    std::optional<PartRole> role{};
    std::vector<AnatomyState> states{};

    friend bool operator==(const RuleSelector& left, const RuleSelector& right) noexcept {
        return left.control_type == right.control_type && left.stable_id == right.stable_id &&
               left.classes == right.classes && left.part == right.part && left.role == right.role &&
               left.states == right.states;
    }
};

// A selector's specificity: the stable ID, the classes, the control type,
// the part (its name or role, one each) and the states, compared in that
// order (T2 plan, E4).
struct RuleSpecificity final {
    std::uint32_t ids{};
    std::uint32_t classes{};
    std::uint32_t types{};
    std::uint32_t parts{};
    std::uint32_t states{};

    friend constexpr bool operator==(RuleSpecificity left, RuleSpecificity right) noexcept {
        return left.ids == right.ids && left.classes == right.classes && left.types == right.types &&
               left.parts == right.parts && left.states == right.states;
    }
    friend constexpr bool operator<(RuleSpecificity left, RuleSpecificity right) noexcept {
        if (left.ids != right.ids) return left.ids < right.ids;
        if (left.classes != right.classes) return left.classes < right.classes;
        if (left.types != right.types) return left.types < right.types;
        if (left.parts != right.parts) return left.parts < right.parts;
        return left.states < right.states;
    }
};

[[nodiscard]] RuleSpecificity rule_specificity(const RuleSelector& selector) noexcept;

// The name of the declaration that selects a part's primitive. Its value is
// a registered primitive's name, or `built_in` for the built-in recipe.
inline constexpr std::string_view primitive_property = "primitive";
inline constexpr std::string_view built_in_primitive_name = "built_in";

enum class RuleValueKind : std::uint8_t {
    // A name: the primitive property's value.
    name,
    number,
    colour,
    // A bounded expression (E5) giving a number parameter's value from the
    // part's inputs, states and the primitive's other parameters (E6).
    expression,
};

// One declaration: a property and its value. Every property but `primitive`
// is a parameter of the primitive the same rule selects, so a rule that sets
// parameters names its primitive too.
//
// An expression's program reads inputs by position (engine_input_position,
// published_value_position), states, and the primitive's number parameters
// by their declaration position. A parameter it reads has the value the
// cascade gives it before any expression, so expressions never read one
// another. `number` is the expression's fallback. Its value is clamped to
// the parameter's range.
struct RuleDeclaration final {
    std::string property{};
    RuleValueKind kind{RuleValueKind::number};
    std::string name{};
    double number{};
    Color colour{};
    std::vector<ExpressionInstruction> program{};

    [[nodiscard]] static RuleDeclaration primitive(std::string_view primitive_name);
    [[nodiscard]] static RuleDeclaration number_value(std::string_view parameter, double value);
    [[nodiscard]] static RuleDeclaration colour_value(std::string_view parameter, Color value);
    // A choice parameter's value by the choice's name, such as a shape.
    [[nodiscard]] static RuleDeclaration choice_value(std::string_view parameter, std::string_view choice);
    [[nodiscard]] static RuleDeclaration expression_value(std::string_view parameter,
                                                          std::span<const ExpressionInstruction> program,
                                                          double fallback);

    friend bool operator==(const RuleDeclaration& left, const RuleDeclaration& right) noexcept {
        return left.property == right.property && left.kind == right.kind && left.name == right.name &&
               left.number == right.number && left.colour == right.colour && left.program == right.program;
    }
};

// The engine's presentational property (T2 GIVEN C1-1): one multiplier,
// from 0 to 1, on the part's whole composite. It is no vocabulary atom and
// no primitive parameter; only a transition sets it, and a part without one
// is opaque.
inline constexpr std::string_view opacity_property = "opacity";

// The engine's presentational scale (T2 GIVEN D5-1, E8): the part drawn at
// this multiple of its size about its centre, within the overflow its
// control grants (PartPaint::granted_overflow), its hit region and layout
// unchanged. Like opacity it is no atom and no parameter; only a
// transition sets it, and a part without one is drawn at 1. It is a
// capability: the built-in rules never use it, and a part granted no
// overflow never grows.
inline constexpr std::string_view scale_property = "scale";
inline constexpr double minimum_presentational_scale = 0.5;
inline constexpr double maximum_presentational_scale = 2.0;

// The engine's face cross-fade (E7r form a): when a primitive-drawn part's
// face changes between discrete states (its raster is replaced), the old
// face fades into the new over the transition's curve, each baked once and
// blended through a paint group (old at 1 - t, new at t, added). The
// transition names no states: it runs on every change of face. A change
// while a fade runs fades from the face shown last to the new one. The
// two faces' rasters must have the same pixel extent and display scale:
// a new face baked at another size (such as a part resting at a popped
// size) replaces the old one without a fade. The presentational scale
// does not stop a fade: while it moves, both faces are stretched over the
// same rectangle (E7r form b). Like scale, no built-in rule uses it.
inline constexpr std::string_view face_property = "face";

// The engine's presentational translation (E9.6, CSS `translate`): the
// part drawn moved by this many logical units across or down, within the
// overflow its control grants, its hit region, layout and text slot
// unchanged. The part's raster is composited at the offset, so a move
// bakes nothing. Like scale it is no atom and no parameter; only a
// transition sets it, a part without one rests at 0, and a part granted
// no overflow (beyond what its scale uses) never moves. A machine's values
// lie within ±maximum_presentational_translate; the grant clamps further.
inline constexpr std::string_view translate_x_property = "translate_x";
inline constexpr std::string_view translate_y_property = "translate_y";
inline constexpr double maximum_presentational_translate = 32.0;

// How a part joins what lies beneath it (E9.6, CSS `mix-blend-mode`).
enum class PartBlend : std::uint8_t { normal, multiply, screen };

// The engine's blend property (E9.6): a declaration of this name, whose
// value is the name `normal`, `multiply` or `screen`, draws the part
// through a paint group of that composite (PaintGroupComposite), whatever
// draws it, so that it multiplies or screens what was painted beneath it.
// Like opacity it is no atom and no primitive parameter, and a rule that
// declares it needs no primitive; the last matching rule that declares it
// wins. The group's bounds are the part's extent (its bounds grown by its
// primitive's outsets and its granted overflow): nothing the part draws
// beyond them is shown. The forced high-contrast pass draws no blend.
inline constexpr std::string_view blend_property = "blend";

// What a transition moves: a primitive parameter, or one of the engine's
// presentational properties.
enum class PresentationProperty : std::uint8_t {
    parameter,
    opacity,
    scale,
    face,
    translate_x,
    translate_y,
};

// The most states one presentational state machine has, and the most
// transitions one part runs: opacity, scale, face and both translations,
// with room for a parameter.
inline constexpr std::size_t maximum_presentation_states = 8;
inline constexpr std::size_t maximum_part_transitions = 6;

// One state of a part's presentational state machine (E7; design language
// §9.6). The machine is in the first of its states whose `when` states the
// part's published states all include (an empty `when` always matches), and
// moves its property to `target`. A part whose states match none rests at
// the property's own value: its cascade value, or 1 for opacity.
//
// When the machine enters this state from another, or the part first
// appears in it, `enter_from`, if set, stands the property at that value
// first; this is how a part fades in when it appears (the docking tiles,
// T2 GIVEN C2-1). Otherwise every move starts from the value shown, so a
// transition retargets and never restarts.
//
// `stops` (E9.6, multi-stop @keyframes), at most maximum_motion_stops,
// make the move into this state pass each stop's value at its offset of
// the duration, offsets strictly increasing within (0, 1), values within
// the property's range; they need a timed curve. They play when the move
// into the state begins: on entering it from another state, or on
// appearing in it from `enter_from`. A part that appears in the state
// without an entry value stands at its target, as before. An animated
// parameter shows each stop at its nearest keyframe level, as any moving
// value (E7r).
//
// The machine is presentational only: it reads the published states and
// writes nothing back, so it can neither create an observable state nor
// delay behaviour.
struct PresentationState final {
    std::vector<AnatomyState> when{};
    double target{};
    std::optional<double> enter_from{};
    std::vector<MotionStop> stops{};

    friend bool operator==(const PresentationState& left, const PresentationState& right) noexcept {
        return left.when == right.when && left.target == right.target && left.enter_from == right.enter_from &&
               left.stops == right.stops;
    }
};

// A transition on one resolved property: `opacity`, or a number parameter
// of the rule's primitive that the primitive declares animated (such as
// the bulge's amplitude), so that its frames never remake the part's
// preparation. It runs on the window's MotionClock with `curve`; every
// duration is clamped to the 400 ms ceiling. Under reduced motion and in an
// occluded or minimized window it jumps to its end (GIVEN D2-1, D4-1).
struct PropertyTransition final {
    std::string property{};
    std::vector<PresentationState> states{};
    MotionCurve curve{};

    friend bool operator==(const PropertyTransition& left, const PropertyTransition& right) noexcept {
        return left.property == right.property && left.states == right.states && left.curve == right.curve;
    }
};

// The ambient budget (design language §9.4, the accepted starting tokens):
// at most 30 frames per second, over a region of at most 10 % of the window
// on everyday surfaces. A surface the application marks ceremonial
// (Control::set_ceremonial, GIVEN B3-1) lifts the area cap for the parts on
// it.
inline constexpr double maximum_ambient_frames_per_second = 30.0;
inline constexpr double maximum_ambient_window_fraction = 0.10;

// An ambient effect (E7): its part repaints as time passes, at most
// `frames_per_second`, and reads time_in_state within one `period` (it
// wraps), so that the effect cycles. Its region is the part's extent. It
// runs only while a primitive draws the part and the part has a record,
// and stops in an occluded or minimized window, under reduced motion and
// while the application's motion switch is off
// (PresentationSettings::ambient_motion); the part then rests at `pose`,
// the time within the period it shows when still, such as an activity
// object's static pose.
struct AmbientEffect final {
    FrameInterval period{};
    double frames_per_second{};
    FrameInterval pose{};

    friend bool operator==(const AmbientEffect& left, const AmbientEffect& right) noexcept {
        return left.period == right.period && left.frames_per_second == right.frames_per_second &&
               left.pose == right.pose;
    }
};

// An image layer's size (T3 P6d, CSS `background-size`), applied when the
// material is assembled. The engine keeps an image's proportions, so one
// length sets the size:
// - natural: the image's own size, its layer's image_scale as written
//   (CSS `auto`);
// - width or height: `length` across or down, a fraction of the box's
//   side plus logical units, the other side in proportion;
// - cover and contain: the smallest size that covers the box and the
//   largest that fits in it;
// - stretch: the box (CSS `100% 100%`, MaterialImageMode::stretch).
enum class MaterialImageSizing : std::uint8_t { natural, width, height, cover, contain, stretch };

struct MaterialImageSize final {
    MaterialImageSizing sizing{MaterialImageSizing::natural};
    BoxLength length{};
    friend constexpr bool operator==(const MaterialImageSize& left, const MaterialImageSize& right) noexcept {
        return left.sizing == right.sizing && left.length == right.length;
    }
};

// An image layer's position (CSS `background-position`): each axis's
// fraction is the layer's image_anchor, of the room the box leaves around
// one tile, and its units its image_offset.
struct MaterialImagePosition final {
    BoxLength across{};
    BoxLength down{};
    friend constexpr bool operator==(const MaterialImagePosition& left, const MaterialImagePosition& right) noexcept {
        return left.across == right.across && left.down == right.down;
    }
};

// An image layer's repetition (CSS `background-repeat`): tiled, or one
// tile alone (MaterialImageMode::single).
enum class MaterialImageRepeat : std::uint8_t { repeat, no_repeat };

// How a drawn edge's line is made (T3 P6e, CSS `border-style`): one line
// in its MaterialBorderStyle, or, drawn as two keylines on the edge in
// place of the border (each edge's pair in CSS's order, top first, the
// outer line first, so a later edge's line covers an earlier one's in the
// corner square they share):
// - double_line: two lines each a third of the width, a third apart;
// - groove and ridge: two halves, one in the edge's colour and one in its
//   shade (each colour channel halved, rounded down): a groove's outer
//   half is the shade on the top and left edges and the colour on the
//   bottom and right, and a ridge's the reverse.
enum class MaterialEdgeLines : std::uint8_t { single, double_line, groove, ridge };

// The material fields a rule may set (T3 engine change EC1, owner R2-02).
// Each cascades on its own, as a parameter does: a part's material is the
// recipe material its primitive draws (the cascade's floor), with each
// field the winning matching rule sets in its place. The fields are
// SurfaceMaterial's, split where CSS's longhands are, so that each
// cascades as CSS cascades it:
// - the fills are CSS's background-color, background-image,
//   background-blend-mode, background-position, background-size and
//   background-repeat (owner C6, 2026-10-26: they combine across rules,
//   the colour under the image layers);
// - the border is its twelve longhands, each edge's style, width and
//   colour, in that order, the edges in CSS's order (`border` sets all
//   twelve);
// - the radii are the four corners, in CSS's order (`border-radius` sets
//   all four).
enum class MaterialField : std::uint8_t {
    background_colour,
    fills,
    fill_blend_modes,
    fill_positions,
    fill_sizes,
    fill_repeats,
    shadows,
    glows,
    glow_placements,
    border_top_style,
    border_top_width,
    border_top_colour,
    border_right_style,
    border_right_width,
    border_right_colour,
    border_bottom_style,
    border_bottom_width,
    border_bottom_colour,
    border_left_style,
    border_left_width,
    border_left_colour,
    border_image,
    keylines,
    corner_top_left,
    corner_top_right,
    corner_bottom_right,
    corner_bottom_left,
    colour_filter,
    drop_shadow,
    filter_blur,
    finish,
    mask,
    count,
};

inline constexpr std::size_t material_field_count = static_cast<std::size_t>(MaterialField::count);

[[nodiscard]] std::string_view material_field_name(MaterialField field) noexcept;

// One material field a rule sets and its value. Only the members `field`
// names are read; the others stay empty:
// - background_colour: `colour`, a solid layer under the image layers;
// - fills: the image layers (gradients, images, patterns), bottom first,
//   at most SurfaceMaterial::maximum_fill_layers, empty for none; and
//   `gradients`, empty, or one per layer: a gradient as a sheet wrote it
//   (T3 P6b), resolved for the part's box when the material is assembled,
//   in place of its layer; an entry of kind `solid` keeps its layer as
//   written;
// - fill_blend_modes: `blend_modes`, in CSS's order (the top image
//   layer's first), repeated over the image layers as CSS repeats a short
//   list; empty for normal;
// - fill_positions, fill_sizes, fill_repeats: `image_positions`,
//   `image_sizes`, `image_repeats`, in the same order and repeated the
//   same way, read by the image fills (not a nine-patch) among the image
//   layers: a gradient layer covers the box;
// - shadows, glows, keylines: the lists;
// - glow_placements: `glow_placements`, in the glows' order, repeated
//   over them as CSS repeats a short list (`glow-placement`);
// - an edge's style: `border_style`, empty for none (CSS's `none`), and
//   `edge_lines`; its width: `number`, 0 for none; its colour: `colour`;
// - border_image: `border_image`, a nine-patch image fill, empty for none;
// - a corner: `number`, its radius in logical units, or, with `relative`,
//   a fraction of the box's shorter side (CSS's percentage, R2-11),
//   resolved when the material is assembled for a part;
// - colour_filter: `colour_filter`; drop_shadow: `drop_shadow`, empty for
//   none; mask: `mask`, empty for none, and `gradients`, empty or one
//   entry, the mask's authored gradient as for a fill layer;
// - filter_blur and finish: `number`.
// `initial`, set, is CSS's `initial`: the field keeps the floor's value,
// and the declaration still wins over those beneath it; its value members
// are not read.
struct MaterialDeclaration final {
    MaterialField field{MaterialField::fills};
    bool initial{};
    std::vector<MaterialFillLayer> fills{};
    std::vector<AuthoredGradient> gradients{};
    std::vector<MaterialBlendMode> blend_modes{};
    std::vector<MaterialImagePosition> image_positions{};
    std::vector<MaterialImageSize> image_sizes{};
    std::vector<MaterialImageRepeat> image_repeats{};
    std::vector<MaterialShadow> shadows{};
    std::vector<MaterialGlow> glows{};
    std::vector<MaterialGlowPlacement> glow_placements{};
    std::optional<MaterialBorderStyle> border_style{};
    MaterialEdgeLines edge_lines{MaterialEdgeLines::single};
    std::optional<MaterialFillLayer> border_image{};
    Color colour{};
    std::vector<MaterialKeyline> keylines{};
    double number{};
    bool relative{};
    MaterialColourFilter colour_filter{};
    std::optional<MaterialDropShadow> drop_shadow{};
    std::optional<MaterialFillLayer> mask{};

    friend bool operator==(const MaterialDeclaration& left, const MaterialDeclaration& right) noexcept {
        return left.field == right.field && left.initial == right.initial && left.fills == right.fills &&
               left.gradients == right.gradients && left.blend_modes == right.blend_modes &&
               left.image_positions == right.image_positions && left.image_sizes == right.image_sizes &&
               left.image_repeats == right.image_repeats && left.shadows == right.shadows &&
               left.glows == right.glows && left.glow_placements == right.glow_placements &&
               left.border_style == right.border_style && left.edge_lines == right.edge_lines &&
               left.border_image == right.border_image &&
               left.colour == right.colour && left.keylines == right.keylines &&
               left.number == right.number && left.relative == right.relative &&
               left.colour_filter == right.colour_filter && left.drop_shadow == right.drop_shadow &&
               left.mask == right.mask;
    }
};

// Replaces `material` with `floor` and the declarations of `fields` (the
// winning declaration of each field, by the field's position, null where
// none) in place: a part's assembled material (EC1); an `initial`
// declaration keeps the floor's value. The fills are composed when any
// fill field is declared: the colour layer (the declared colour, else the
// floor's bottom layer when it is solid) under the image layers (the
// declared ones, each authored gradient resolved for `box`, else the
// floor's others), which take the declared blend modes, and whose image
// fills take the declared sizes (resolved for `box`), then repetitions,
// then positions; a colour of alpha 0 adds no layer, and with no layer at
// all the fill is one transparent solid. More than
// SurfaceMaterial::maximum_fill_layers is no material. The corners are
// composed when any is declared: each is its declaration's, else the
// floor's (its corner_radii's, else its corner_radius); four equal corners
// are corner_radius alone, and others corner_radii, with corner_radius the
// smallest (the built-in path rounds every corner by it). The glows take
// the declared placements; a mask's authored gradient is resolved for
// `box`. The border's
// edges are assembled
// together when any of their fields is declared: each starts as the
// floor's (its single edge, else its uniform border; an edge the floor
// lacks has the default MaterialBorder's width and colour and is not
// drawn), takes its declared style, width and colour, and is drawn unless
// its style is none or its width 0. Four equal edges are drawn as one
// uniform border, which keeps a dash and rounded corners, and unequal ones
// as single edges, which a material admits only with square corners
// (valid_surface_material); an edge drawn as two lines (MaterialEdgeLines)
// is not a border edge but its two keylines, after the declared ones. A
// border image is the top fill layer, and the border (its edges and their
// lines) is not drawn, as CSS draws a border image in place of the
// border's style. `box` is the part's box in logical units: relative
// corners are fractions of its shorter side.
void assemble_material(const SurfaceMaterial& floor,
                       const std::array<const MaterialDeclaration*, material_field_count>& fields, Size box,
                       SurfaceMaterial& material);

// Whether assembling `fields` reads the part's box: a relative corner, an
// authored gradient, of a fill or the mask, other than a fixed one
// (authored_gradient_fixed), or an image size of the box (a fraction of
// its side, cover or contain).
[[nodiscard]] bool material_fields_read_box(
    const std::array<const MaterialDeclaration*, material_field_count>& fields) noexcept;

// One rule. `label` names it in diagnostics (T3: its source position).
// `requires_engine`, when set, is the engine version the rule needs, in
// place of its sheet's. Its transitions and its ambient effect cascade as
// declarations do: for each property, the transition of the last matching
// rule that declares one (for a parameter, among the rules selecting the
// part's primitive), and the ambient effect of the last matching rule that
// declares one. Its material fields (EC1) cascade the same way, each on
// its own, from every matching rule, whatever primitive it selects: they
// change a part only where its primitive draws a theme material.
//
// `specificity`, when set, is the rule's specificity in place of the one
// its selector gives (T3 engine change EC2): a rule expanded from CSS's
// `:is()` takes its most specific argument's. A class or stable-ID rule
// with a given specificity then needs a class or an ID in it rather than
// in its selector; an inline style still needs its selector's stable ID.
struct ThemeRule final {
    std::string label{};
    CascadeLevel level{CascadeLevel::theme_sheet};
    RuleSelector selector{};
    std::optional<RuleSpecificity> specificity{};
    std::vector<RuleDeclaration> declarations{};
    std::optional<EngineVersion> requires_engine{};
    std::vector<PropertyTransition> transitions{};
    std::optional<AmbientEffect> ambient{};
    std::vector<MaterialDeclaration> materials{};

    friend bool operator==(const ThemeRule& left, const ThemeRule& right) noexcept {
        return left.label == right.label && left.level == right.level && left.selector == right.selector &&
               left.specificity == right.specificity && left.declarations == right.declarations &&
               left.requires_engine == right.requires_engine && left.transitions == right.transitions &&
               left.ambient == right.ambient && left.materials == right.materials;
    }
};

// A theme's rules, as authored, with the engine version the theme declares
// it needs and the values an application publishes that its expressions
// read (E6; Window::publish_value). The name at position n of the list is
// read at published_value_position(n). Each is 1 to 64 characters of lowercase
// ASCII letters, digits, '-' and '_', the first a letter, and is listed
// once; at most maximum_published_values.
struct ThemeRuleSheet final {
    EngineVersion requires_engine{current_engine_version};
    std::vector<ThemeRule> rules{};
    std::vector<std::string> published_values{};

    friend bool operator==(const ThemeRuleSheet& left, const ThemeRuleSheet& right) noexcept {
        return left.requires_engine == right.requires_engine && left.rules == right.rules &&
               left.published_values == right.published_values;
    }
};

// Budgets (GIVEN F1-1): a part's primitive costs at most 512 declared
// operations per device pixel, and an animated part (one whose primitive has
// an animated parameter) at most 4 million per frame. A part beyond either
// keeps the built-in recipe; a primitive declaring no per-pixel cost cannot
// be bounded and is refused too.
inline constexpr std::uint32_t maximum_operations_per_device_pixel = 512U;
inline constexpr std::uint64_t maximum_animated_operations_per_frame = 4'000'000U;

// Keyframes (E7r, owner 2026-10-22: 8 to 12). While an animated parameter's
// transition runs, the part shows the nearest of this many levels evenly spaced
// over the parameter's declared range, each baked once and shared; settled,
// it shows its exact value. When a transition starts, the levels it will
// pass are baked at once: at most this many bakes, each within the animated
// budget above, so that none is baked while it runs.
inline constexpr std::size_t animated_keyframe_levels = 10U;
// The most frames in an ambient effect's loop (E7r): its time in the period
// steps through at most this many, or its frame rate's count for one
// period if fewer, each baked once, the first time it is shown, and shared.
inline constexpr std::size_t maximum_ambient_loop_frames = 24U;

enum class RuleDiagnosticKind : std::uint8_t {
    // The rule is set aside: it needs a newer engine than this one. Its
    // parts fall back to lower rules or the built-in recipe. Not an error.
    needs_newer_engine,
    // The rule's level cannot be authored (built_in, forced_high_contrast).
    level_not_authored,
    // A class rule names no class, or a stable-ID rule or inline style no
    // stable ID; the rule is dropped.
    selector_missing_level_field,
    // A selector's class or state is invalid; the rule is dropped.
    selector_invalid,
    // The declarations below are dropped one at a time; the rest of the
    // rule stays.
    unknown_primitive,
    primitive_role_mismatch,
    primitive_over_budget,
    primitive_cost_unbounded,
    primitive_value_not_a_name,
    parameter_without_primitive,
    unknown_parameter,
    parameter_wrong_kind,
    parameter_not_finite,
    parameter_out_of_range,
    duplicate_declaration,
    // An expression that does not compile (RuleDiagnostic::expression names
    // the defect), or that reads a colour parameter; the declaration is
    // dropped.
    expression_invalid,
    expression_reads_colour,
    // A rule left with no declarations after the drops above is dropped.
    rule_empty,
    // A transition (E7) is dropped: its property is neither opacity nor a
    // parameter of the rule's primitive; the parameter is not animated or
    // not a number; or it has no states or more than
    // maximum_presentation_states, a state outside the vocabulary, or a
    // target or entry value that is not finite or outside the property's
    // range; or a state's stops are too many, out of order, outside the
    // property's range, or on a curve that is not timed. A rule with more than maximum_part_transitions keeps the first.
    transition_property_unknown,
    transition_property_not_animated,
    transition_invalid,
    // The ambient effect is dropped: more than 30 frames per second (or
    // none), or a period or pose that is not a positive period with the
    // pose inside it.
    ambient_over_frame_cap,
    ambient_invalid,
    // A blend declaration (blend_property) whose value is not one of its
    // names is dropped.
    blend_invalid,
    // A published value's name is invalid, repeats an earlier one or is
    // past maximum_published_values. The diagnostic, of the sheet rather
    // than a rule, names it in `property`; its position stays, and reads as
    // not published.
    published_value_invalid,
    // A material field whose value no material admits
    // (valid_surface_material), such as a ninth fill or a negative radius.
    material_field_invalid,
};

[[nodiscard]] std::string_view rule_diagnostic_name(RuleDiagnosticKind kind) noexcept;

// The rule position of a diagnostic about the sheet itself rather than one
// of its rules.
inline constexpr std::size_t sheet_diagnostic = static_cast<std::size_t>(-1);

struct RuleDiagnostic final {
    RuleDiagnosticKind kind{RuleDiagnosticKind::rule_empty};
    // The rule's position in the sheet and its label (sheet_diagnostic and
    // empty for the sheet).
    std::size_t rule{};
    std::string label{};
    // The declaration's property, empty for a whole-rule diagnostic.
    std::string property{};
    // For expression_invalid, why the expression does not compile.
    ExpressionDefect expression{ExpressionDefect::none};
};

// A sheet compiled against a registry: the rules that survived, each
// declaration checked, in the order the cascade visits them. Immutable once
// made; a ThemeEngine owns one. Compiling never fails as a whole: an invalid
// declaration or rule is dropped with a diagnostic and the rest stands, so a
// broken rule cannot take the theme's other rules down with it.
class CompiledThemeRules final {
public:
    CompiledThemeRules();
    CompiledThemeRules(const CompiledThemeRules& other);
    CompiledThemeRules& operator=(const CompiledThemeRules& other);
    CompiledThemeRules(CompiledThemeRules&& other) noexcept = default;
    CompiledThemeRules& operator=(CompiledThemeRules&& other) noexcept = default;
    ~CompiledThemeRules() = default;

    // Compiles `sheet` against `registry`, which must outlive the result.
    [[nodiscard]] static CompiledThemeRules compile(const ThemeRuleSheet& sheet, const PrimitiveRegistry& registry);
    // Compiles the engine's built-in rules (built_in_theme_rules, at
    // CascadeLevel::built_in, below every authored rule) and then `sheet`.
    // The theme engine's rules are made this way. Diagnostics name the
    // sheet's rules by their positions in `sheet`.
    [[nodiscard]] static CompiledThemeRules compile_with_built_in(const ThemeRuleSheet& sheet,
                                                                  const PrimitiveRegistry& registry);
    // How many of the rules are the engine's built-in rules.
    [[nodiscard]] std::size_t built_in_rule_count() const noexcept { return built_in_rules_; }

    [[nodiscard]] bool empty() const noexcept { return rules_.empty(); }
    [[nodiscard]] std::size_t rule_count() const noexcept { return rules_.size(); }
    [[nodiscard]] const std::vector<RuleDiagnostic>& diagnostics() const noexcept { return diagnostics_; }

    // The published values the sheet's expressions read, in position order
    // (an invalid name is kept empty, so later positions stay).
    [[nodiscard]] const std::vector<std::string>& published_values() const noexcept { return published_values_; }
    // The position of the published value `name`, or none when the sheet
    // does not read it.
    [[nodiscard]] std::optional<ExpressionInputIndex> published_value_input(std::string_view name) const noexcept;
    // How many inputs the sheet's expressions are evaluated with: the
    // engine inputs and the sheet's published values.
    [[nodiscard]] std::size_t input_count() const noexcept {
        const std::size_t count = engine_input_count + published_values_.size();
        return count;
    }

    // What a part is: the facts its selectors are matched against. The
    // views are borrowed for the call.
    struct PartFacts final {
        std::string_view control_type{};
        std::string_view stable_id{};
        std::span<const std::string> classes{};
        // Empty for an unkeyed part.
        std::string_view part{};
        PartRole role{PartRole::region};
        AnatomyStateSet states{};
    };

    // Why a part resolved as it did.
    enum class Outcome : std::uint8_t {
        // A rule selected a primitive; `primitive` and `parameters` are set.
        primitive,
        // No rule selects a primitive for the part.
        no_rule,
        // The winning rule selects `built_in`.
        built_in_selected,
        // The selected primitive does not paint the part's role.
        role_mismatch,
    };

    // One state of a compiled machine (see PresentationState).
    struct CompiledPresentationState final {
        AnatomyStateSet when{};
        double target{};
        std::optional<double> enter_from{};
        std::vector<MotionStop> stops{};
    };

    // A checked transition: an engine property, or the parameter at
    // `parameter`.
    struct CompiledTransition final {
        PresentationProperty property{PresentationProperty::parameter};
        std::size_t parameter{};
        std::vector<CompiledPresentationState> states{};
        MotionCurve curve{};
    };

    struct Resolution final {
        Outcome outcome{Outcome::no_rule};
        const EnginePrimitive* primitive{};
        PrimitiveParameterBlock parameters{};
        // The level of the declaration that chose the primitive (or
        // built_in); built_in when no rule did.
        CascadeLevel level{CascadeLevel::built_in};
        // The expression whose value a parameter takes, by the parameter's
        // position, where the winning declaration of the parameter is one;
        // null elsewhere. The expressions belong to the compiled rules.
        std::array<const EngineExpression*, maximum_primitive_parameters> expressions{};
        // The inputs the part reads (E6): the primitive's declared inputs
        // and every input its expressions read, as expression input bits.
        std::uint64_t inputs_read{};
        // The part's transitions (E7): its opacity's and its scale's,
        // whatever draws it, and its primitive's parameters', when a
        // primitive does. They
        // belong to the compiled rules. `transition_count` are live.
        std::array<const CompiledTransition*, maximum_part_transitions> transitions{};
        std::size_t transition_count{};
        // The part's ambient effect, or null; it belongs to the rules.
        const AmbientEffect* ambient{};
        // The part's blend (blend_property): the last matching rule's.
        PartBlend blend{PartBlend::normal};
        // The winning declaration of each material field (EC1), by the
        // field's position, or null where no matching rule sets it; they
        // belong to the compiled rules. `materials_authored` is whether any
        // is set.
        std::array<const MaterialDeclaration*, material_field_count> materials{};
        bool materials_authored{};
    };

    // The cascade for one part: the winning primitive declaration, then
    // each of that primitive's parameters from the winning declaration of
    // it among the matching rules that select the same primitive, else its
    // declared default; and each material field from its winning
    // declaration among all the matching rules. Linear in the number of
    // rules; tables are small, and a theme with no rules returns at once.
    [[nodiscard]] Resolution resolve(const PartFacts& facts) const noexcept;

    // This compilation's serial number, unique among the compilations of
    // the process: a cache keyed by the addresses of the rules'
    // declarations (an assembled material, EC1) compares it first. A copy
    // takes a new one, since its declarations lie elsewhere; a move keeps
    // it with the declarations.
    [[nodiscard]] std::uint64_t serial() const noexcept { return serial_; }

private:
    // A checked declaration: the primitive a rule selects (null for
    // built_in) or one parameter's value, by its index in the primitive's
    // declarations.
    struct CompiledDeclaration final {
        bool selects_primitive{};
        const EnginePrimitive* primitive{};
        std::size_t parameter{};
        PrimitiveParameterValue value{};
        // An expression declaration: `program` gives the value.
        bool expression{};
        EngineExpression program{};
    };

    struct CompiledRule final {
        CascadeLevel level{CascadeLevel::theme_sheet};
        RuleSpecificity specificity{};
        // Position in the authored sheet; later wins among equals.
        std::size_t order{};
        RuleSelector selector{};
        AnatomyStateSet states{};
        // The primitive the rule selects, null when it selects built_in or
        // none; `selects` tells the two apart.
        bool selects{};
        const EnginePrimitive* primitive{};
        std::vector<CompiledDeclaration> parameters{};
        std::vector<CompiledTransition> transitions{};
        std::optional<AmbientEffect> ambient{};
        std::optional<PartBlend> blend{};
        // Checked material fields, at most one per field.
        std::vector<MaterialDeclaration> materials{};
    };

    // Compiles `sheet`'s rules into `result` at their authored levels, or,
    // for the engine's own table, at CascadeLevel::built_in.
    static void add_rules(CompiledThemeRules& result, const ThemeRuleSheet& sheet, const PrimitiveRegistry& registry,
                          bool engine_table);
    // The winning rule's primitive and parameters into `result`.
    void resolve_primitive(const PartFacts& facts, const CompiledRule& chosen, Resolution& result) const noexcept;
    // The resolved transitions, ambient effect and blend of the matching
    // rules into `result`.
    void resolve_transitions(const PartFacts& facts, Resolution& result) const noexcept;
    // A rule's transitions and ambient effect, checked; a defect drops the
    // one transition or the effect with a diagnostic.
    static void add_transitions(std::vector<RuleDiagnostic>& diagnostics, std::size_t index, const ThemeRule& rule,
                                CompiledRule& compiled);
    static void add_ambient(std::vector<RuleDiagnostic>& diagnostics, std::size_t index, const ThemeRule& rule,
                            CompiledRule& compiled);
    // A rule's blend declaration, checked; a defect drops it with a
    // diagnostic.
    static void add_blend(std::vector<RuleDiagnostic>& diagnostics, std::size_t index, const ThemeRule& rule,
                          const RuleDeclaration& declaration, CompiledRule& compiled);
    // A rule's material fields, checked; a defect or a second declaration
    // of one field drops it with a diagnostic.
    static void add_materials(std::vector<RuleDiagnostic>& diagnostics, std::size_t index, const ThemeRule& rule,
                              CompiledRule& compiled);
    // The winning material fields of the matching rules into `result`.
    void resolve_materials(const PartFacts& facts, Resolution& result) const noexcept;

    [[nodiscard]] static bool matches(const CompiledRule& rule, const PartFacts& facts) noexcept;

    // Sorted by precedence, lowest first: level, then specificity, then
    // order. The cascade's winner for a property is the last match.
    std::vector<CompiledRule> rules_{};
    std::vector<RuleDiagnostic> diagnostics_{};
    std::vector<std::string> published_values_{};
    std::size_t built_in_rules_{};
    std::uint64_t serial_{};
};

// The engine's built-in rules (E7), compiled below every theme's: the
// docking drop tiles' fade (T2 GIVEN C2-1) and the ribbon's dithered rise
// (D3-1, G4-1). Static storage, made once on first use.
[[nodiscard]] const ThemeRuleSheet& built_in_theme_rules();

} // namespace gui_forms
