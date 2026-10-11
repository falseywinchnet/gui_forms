# Layout

Layout in GUI.Forms is a two-phase retained pass, measure then arrange, that
runs for dirty controls when layout is flushed, including when an update scope
closes, a read barrier demands it, or a paint needs current geometry. There is
no constraint solver and no CSS engine; there are a small number of
deterministic layout containers and a base
`Control` policy they all share.

## Coordinates and units

All layout runs in logical pixels. The window has a `scale` (1.0, 2.0,
fractional values on Windows and Linux) that converts logical to device
pixels at paint time. Controls never see device pixels except through the
`Painter`. Bounds are `Rect{x, y, width, height}` in the parent's client
space; `requested_bounds` is what you set, `arranged_bounds` is what the last
layout produced, and `absolute_bounds` is in window client coordinates.
Reading arranged or absolute geometry may trigger the layout read barrier.

## Base policy on every control

Every `Control` carries the inputs the containers consume:

| Input | Meaning |
|---|---|
| `requested_bounds`, `set_bounds(rect, BoundsSpecified)` | Authored position and size; partial masks update only x, y, width or height atomically |
| `minimum_size`, `maximum_size` | Hard clamps applied after measure |
| `margin`, `padding` | Outside and inside spacing in logical pixels, finite and nonnegative |
| `auto_size`, `auto_size_mode` | Grow-only or grow-and-shrink to the preferred size |
| `dock`, `anchor` | Edge-relative placement inside the parent |
| `visible` | Hidden children take no space in flow and table layout |

`get_preferred_size(available)` returns the size a control wants. The base
implementation sums visible child bounds, trailing margins and padding, then
clamps to the min/max constraints. Controls with intrinsic content (labels,
buttons, text boxes) override `measure` to shape their text and report that.

A parent assigns a child a private layout slot; it never overwrites the
child's authored `requested_bounds`. That is why repeated layout and resize
are stable: the authored input is still there to measure from.

## Containers

`Panel`, `GroupBox`
: Absolute placement by `requested_bounds`, plus Dock and Anchor. Dock
  consumes children in reverse z-order (backmost first) so that the topmost
  child receives the remaining fill area, while paint stays back-to-front
  and hit testing front-to-back. Both inherit retained scrolling from
  `ScrollableControl`.

`FlowLayoutPanel`
: Orders visible children left-to-right, right-to-left, top-down or
  bottom-up, honouring child margins, container padding, optional wrapping
  and per-child flow breaks (`set_flow_break`). Hidden children leave no gap. It is a
  deterministic flow, not flexbox.

`TableLayoutPanel`
: Bounded row and column tracks with absolute, auto-size and weighted
  percent sizing; explicit or row-major automatic cell placement; row and
  column spans; fixed, add-rows or add-columns growth; cell borders;
  resolved-track inspection and overflow reporting. Dock fills or aligns a
  child within its cell; with `DockStyle::none`, Anchor determines edge
  alignment, centering and stretching inside the cell after margins.

`SplitContainer`, `MasterDetailView`
: Two panes with a draggable seam and stable panel/seam IDs. The seam is a
  real control and participates in focus and keyboard resizing.

`ScaledPanel`, `ScaledGroupBox`
: Children are placed in a caller-declared design coordinate space and each
  slot is scaled proportionally into the arranged bounds. Useful for fixed
  compositions that must survive resizing without per-child anchors.

`ResponsiveTrackPanel`
: Nested fixed, content-measured and weighted tracks with priority-authored
  collapse, for toolbars and shells that must degrade deterministically at
  small sizes.

## Scrolling

`ScrollableControl` is the shared base for `Panel` and `ContainerControl`.
`auto_scroll`, `auto_scroll_margin`, `auto_scroll_min_size` and
`auto_scroll_position` determine a two-axis content extent; child requested bounds stay in content
coordinates and never drift as the view scrolls. Scrollbars are an overlay
outside the child viewport, so descendants cannot paint or receive hits
through them. Notifying scroll operations report type, orientation and
old/new values; programmatic `scroll_to` and `scroll_by` default to
`notify = false`. `scroll_control_into_view` honours each
target's `auto_scroll_offset`. Multiline `TextBox` reports its document extent
through the same base, so a text box gets real scrollbars rather than private
offsets.

## Custom layout

A custom control overrides two hooks:

```cpp
gf::Size measure(gf::Size available) override;        // report preferred size
void arrange(gf::Rect final_bounds) override;         // place children
```

Keep `measure` free of retained mutations: read children's preferred sizes
and your own properties to compute a size. Mutations can request another pass;
a layout that keeps invalidating itself eventually hits the bounded pass
limit and reports `bounded_pass_limit_hits`.

In `arrange`, call `arrange_self(final_bounds)` to commit your own geometry,
then `set_child_layout(child, rect)` to assign each child's layout slot.
Calling the child's `set_bounds` changes its authored `requested_bounds`
even during arrangement; it is not a layout-slot assignment.

Declare the layout effects of your properties with `invalidate_declared`
(see [Update scopes and dirtiness](update-scopes-and-dirtiness)) so that
changing a property re-measures only when it has to.

## Instrumentation

`MetricsSnapshot` reports `measure_passes`, `arrange_passes`,
`controls_measured`, `controls_arranged`, `measure_nodes_visited` and
`arrange_nodes_visited`. A layout change that visits more nodes than it
measures is traversing clean subtrees; a change whose arranged count equals
the control count is re-laying-out everything and probably invalidating too
widely.

## Not provided

There is no constraint-based or grid-template layout, no CSS, no per-monitor
DPI policy inside the core (scale is a host fact the window is told), and no
designer serialization. Flow layout uses its packing policy rather than the
base Dock/Anchor placement rules; table cells support Dock and Anchor.
