# Painting and damage

GUI.Forms paints on the CPU, only when something changed, and only where it
changed. There is no GPU path and no redraw loop. This page explains how a
mutation becomes a damaged rectangle, how a frame is painted and presented
coherently, and how to read the counters that prove it.

## From mutation to damage

A setter marks `Dirty::paint` on its control. When the outermost update scope
closes, the flush converts each paint mark into a damage rectangle in window
coordinates: the control's arranged bounds plus its `visual_outsets` (focus
rings and shadows that extend beyond layout without affecting it), plus the
outsets it had when last painted, clipped to each ancestor's client area.
Rectangles accumulate in a `DamageRegion` per paint plane. The region
compacts overlapping rectangles and collapses to a bounding box when its
fixed capacity is exceeded; `damage_region_compactions` and
`damage_region_collapses` count both events, and `maximum_damage_rectangles`
records the peak count held.

Paint planes let a control choose where it paints: the ordinary content
plane, or an overlay plane for popups, tooltips and drag images, so that a
tooltip appearing does not damage the controls beneath it. A host that
presents planes separately asks for `take_damage(plane)`.

## Display chunks

Each control's paint output is a retained display chunk: a recorded list of
`Painter` commands with their resolved geometry, text runs and rasterized
glyphs. Painting a frame replays chunks; it does not call `on_paint` on clean
controls. Only controls inside the damage have their chunks rebuilt.
`display_chunks_rebuilt`, `display_chunks_reused` and
`display_commands_replayed` show the split; a scroll that rebuilds every chunk
is invalidating text or style when it only moved geometry.

Glyph bitmaps live with the chunk that drew them and are released with it.
There is no global glyph cache and no eviction policy; memory follows the
retained tree.

## The Painter

Controls draw through the renderer-neutral `Painter` vocabulary: fills,
strokes, paths, rounded rectangles, images by `ImageId`, text runs by font
role, clips and transforms. The `Painter` is implemented by the private
Skia-based CPU adapter on macOS and Linux and by the DIB compositor on
Windows. No Skia type is visible to a control, and the renderer can be
replaced without touching one.

Images are PNG only. The resource registry validates and decodes a PNG once;
controls refer to it by ID.

## Coherent frames

A paint pass holds one exclusive UI-thread paint lease. Within the pass,
owner callbacks rebuild chunks into a transaction recorder; no command
reaches the host painter until every callback has succeeded. If a callback
throws, the pass restores the previous chunks, abandons the candidate frame
and republishes the damage so a later pass can retry. A mutation made during
paint increments `content_revision` and survives as one later pass rather
than reentering the current one.

`Window::paint(painter, requested_damage)` returns a `PaintReceipt` only after
the backend has replayed every command and the window has re-validated the
surface epoch. The host presents that exact receipt and then calls
`notify_presented(receipt, duration)`. Duplicate, backward, forged-future and
stale-epoch receipts are rejected and counted. A resize or scale change
increments the surface epoch, so a frame rendered for the old size can never
be presented into the new surface.

`PaintLeaseSnapshot` exposes `content_revision`, `rendered_revision`,
`presented_revision`, `surface_epoch`, the lease state (`dirty_queued`,
`rendering`, `rendering_dirty`, `ready`) and the number of requests queued or
merged. Intermediate revisions are never stored as render jobs; a burst of
mutations produces one frame.

## Input during paint

Pointer, key, text, drag and semantic events that arrive while a paint lease
is held are queued, not dispatched. Consecutive moves from one pointer
compact to the latest; downs, ups, wheels, keys and text keep causal order.
The queue holds 1,024 entries; overflow is rejected and reported. One drain
runs after the outermost lease releases. `DeferredInputSnapshot` reports
pending, delivered, compacted and rejected counts.

## Waking the host

Model-originated damage does not paint by itself; it arms one wake through
`set_paint_wake_handler`. The host consumes all damage, presents, and the wake
is rearmed for the next independent mutation. Occlusion suppresses the wake
without discarding dirtiness, and exposure requests one wake. This is why a
second window controlled by the first can mutate it and have it present
without any input passing through the changed window.

Animations and live surfaces request frames explicitly through
`schedule_paint` and `FrameRequestToken`; deadlines coalesce, callback faults
are isolated per request, and nothing repaints while the window is idle.
`active_surface_count` tells you how many surfaces currently keep the
scheduler awake, and it should be zero in a resting window.

## Live surfaces

A live surface (`LiveSurfaceDescription`, `LiveSurfaceFrame`) lets a producer (an audio visualizer, a video decoder, a
game board) write pixels directly into a pooled frame and have the window
composite it. Frames are premultiplied BGRA or RGBA; the surface can promise
`opaque`, which lets the presenter copy instead of blend and skip painting
what lies beneath. On macOS a live-only redraw can omit retained painting
entirely when the existing raster is valid and opaque clips cover every
damaged device pixel. `native_live_surface_pixel_format()` reports the
host's native order so a producer can avoid a conversion.

## Reading the counters

`MetricsSnapshot` carries `paint_passes`, `controls_painted`,
`paint_invalidations_consumed`, `requested_damage_area`,
`painted_damage_area`, `full_window_paints`, `partial_paints`,
`frames_presented`, `present_duration_nanoseconds`,
`worst_present_duration_nanoseconds`, `renderer_name` and `cpu_only`. A
healthy interaction shows `partial_paints` climbing while
`full_window_paints` stays at the resize count, and `painted_damage_area`
close to `requested_damage_area`. `cpu_only` is always true; the field exists
so that a snapshot states it.
