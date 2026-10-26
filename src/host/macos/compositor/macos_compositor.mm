#include "macos_compositor.hpp"

// The backdrop container: a plain view that only holds the effect view and
// the raster view (make_macos_content_view).
@interface GUIFormsBackdropContainer : NSView
@end

@implementation GUIFormsBackdropContainer
@end

namespace gui_forms::host::detail {
namespace {

// The container holding `raster`, or nil when the window was made without
// one.
GUIFormsBackdropContainer* container_of(NSView* raster) {
    NSView* const parent = raster.superview;
    if (![parent isKindOfClass:[GUIFormsBackdropContainer class]]) return nil;
    GUIFormsBackdropContainer* const container = static_cast<GUIFormsBackdropContainer*>(parent);
    return container;
}

NSVisualEffectView* effect_of(GUIFormsBackdropContainer* container) {
    if (container == nil || container.subviews.count == 0U) return nil;
    NSView* const first = container.subviews[0];
    if (![first isKindOfClass:[NSVisualEffectView class]]) return nil;
    NSVisualEffectView* const effect = static_cast<NSVisualEffectView*>(first);
    return effect;
}

// The materials the portable purposes map to (PLAN.md §2.2). The bench
// run of the C3 probe confirms them; CI's runner has reduce transparency
// on and shows only AppKit's opaque fallback.
NSVisualEffectMaterial material_for(WindowBackdrop backdrop) noexcept {
    if (backdrop == WindowBackdrop::transient_material) return NSVisualEffectMaterialPopover;
    return NSVisualEffectMaterialUnderWindowBackground;
}

} // namespace

CompositorState read_macos_compositor_state(CompositorGeneration generation) {
    CompositorState state{};
    state.generation = generation;
    state.compositing = CompositorPresence::present;
    state.support[static_cast<std::size_t>(WindowBackdrop::transparent)] = WindowBackdropSupport::supported;
    state.support[static_cast<std::size_t>(WindowBackdrop::window_material)] = WindowBackdropSupport::supported;
    state.support[static_cast<std::size_t>(WindowBackdrop::transient_material)] = WindowBackdropSupport::supported;
    // macOS has no separate transparency-effects switch.
    state.transparency_enabled = true;
    NSWorkspace* const workspace = NSWorkspace.sharedWorkspace;
    state.reduced_transparency = workspace.accessibilityDisplayShouldReduceTransparency == YES;
    state.high_contrast = false;
    return state;
}

NSView* make_macos_content_view(NSView* raster, bool non_opaque) {
    if (!non_opaque) return raster;
    const NSRect bounds = NSMakeRect(0.0, 0.0, raster.frame.size.width, raster.frame.size.height);
    GUIFormsBackdropContainer* const container = [[GUIFormsBackdropContainer alloc] initWithFrame:bounds];
    NSVisualEffectView* const effect = [[NSVisualEffectView alloc] initWithFrame:bounds];
    effect.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    effect.blendingMode = NSVisualEffectBlendingModeBehindWindow;
    effect.state = NSVisualEffectStateFollowsWindowActiveState;
    effect.material = NSVisualEffectMaterialUnderWindowBackground;
    effect.hidden = YES;
    raster.frame = bounds;
    raster.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [container addSubview:effect];
    [container addSubview:raster];
    return container;
}

HostServiceStatus apply_macos_backdrop(NSView* raster, WindowBackdrop backdrop) {
    NSWindow* const window = raster == nil ? nil : raster.window;
    if (window == nil) {
        if (backdrop == WindowBackdrop::opaque) return {};
        return {HostServiceError::backend_failure};
    }
    NSVisualEffectView* const effect = effect_of(container_of(raster));
    const bool material = backdrop == WindowBackdrop::window_material ||
                          backdrop == WindowBackdrop::transient_material;
    // Validated before anything changes.
    if (material && effect == nil) return {HostServiceError::backend_failure};
    if (backdrop == WindowBackdrop::opaque) {
        if (effect != nil) effect.hidden = YES;
        window.backgroundColor = NSColor.windowBackgroundColor;
        window.opaque = YES;
        [window invalidateShadow];
        return {};
    }
    window.opaque = NO;
    window.backgroundColor = NSColor.clearColor;
    window.ignoresMouseEvents = NO;
    if (!material) {
        if (effect != nil) effect.hidden = YES;
        [window invalidateShadow];
        return {};
    }
    effect.material = material_for(backdrop);
    effect.blendingMode = NSVisualEffectBlendingModeBehindWindow;
    effect.state = NSVisualEffectStateFollowsWindowActiveState;
    effect.hidden = NO;
    // The shadow follows the window's alpha, which has just changed.
    [window invalidateShadow];
    return {};
}

} // namespace gui_forms::host::detail
