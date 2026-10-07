# Named handlers, shared state and item-reporting controls

Development C++ source contract, 2026-10-07. Include `event.hpp`, `value.hpp`,
`commands.hpp`, and `collection_controls.hpp` as needed. All operations are
confined to the UI execution thread. None starts a timer or requests idle work.
These are GUI.Forms capabilities; no PlaySuite source changes are required here.

## Event ownership and typed values

`on(event, owner, &Owner::method, values...)` transfers the connection token to
a Component owner. Publisher destruction/disconnection immediately unlinks the
entry. No dead token, token-store allocation or capacity remains after the last
publisher disappears. `owned_subscription_count()` is a diagnostic traversal,
not a polling API. Caller-held subscriptions retain their existing contract.
Both storage forms revoke in reverse acquisition order on owner teardown.

The handler returns void. It takes zero, one or two bound values first, then a
leading prefix of the event arguments; unused trailing arguments may be omitted.
The longest compatible prefix is selected at compile time. Bound values are
immutable trivial copies totaling at most 16 bytes, stored inline in the
connection's existing slot allocation. There is no separately allocated payload.
Pass owner-data indices for strings or objects. A pointer value remains a borrow.

Registration may allocate. Emission does not allocate for member bindings.
Publisher or owner destruction during dispatch cancels later callbacks, while
the active connection remains retained until return. This does not keep the
callback's owner alive: do not access an owner after deleting it. A derived
owner whose destructor emits events must dispose before tearing down handler
state. The compiler's `gui_forms::on` assertions explain signature, count, size
and trivial-copy violations; the build runs four deliberate-failure fixtures.

## State and input notifications

State events describe all actual changes; input events describe input actions.
Setting an equal value does nothing. In particular:

| Control | All state changes | Input-only action |
|---|---|---|
| RangeControl / TrackBar | `value_changed`, `range_changed` | `scroll` |
| CheckBox | `checked_changed`, `check_state_changed` | `clicked` |
| Button | `selected_changed`, inherited `enabled_changed` | `clicked` |
| Button disclosure | `expanded_changed` | `clicked` / semantic activation |
| ChoiceGroup | `changed(int)` | `activated(int)` |
| ExpandableSections | `toggled(int,bool)` | Header `clicked` |
| ListBox | `selection_changed` | `item_activated(std::size_t)` |

Programmatic setters do not synthesize input events. No quiet subclass is needed
for TrackBar: observe `scroll` for user input, or bind a Value for shared state.
Existing BeginInit/EndInit coalescing of control state notifications is retained.

## Application-owned scalar values

`Value<T>` supports arithmetic and enum scalars. `get`, `set`, `changed` and
`validating` form its public contract. Floating values must be finite. Validation
runs before committing a change; validators may reject with an exception.
An exception from a changed observer propagates after the new value is committed,
under the existing Event exception contract. Equal writes are ignored. A different
recursive write during a notification is rejected with `logic_error`; defer a
new application transaction until the notification returns. This explicit rule
prevents mutually correcting observers from producing an unbounded loop.

`RangeControl::bind(Value<double>&)`, `CheckBox::bind(Value<bool>&)` and
`ChoiceGroup::bind(Value<int>&)` establish two-way connections. The model wins
at bind time. Existing bindings are replaced. Controls borrow the model; tokens
revoke access on model disposal or destruction, and control teardown disconnects
the subscriptions. A surviving control retains its last local state. `unbind()`
ends the binding explicitly. Binding may allocate; ordinary propagation does not.

A matching model update does not call the originating control's setter again.
Changes dirty paint/semantics through existing setters and await the next frame;
no polling, timer, idle task or implicit painting is introduced. Range and choice
constraints participate in model validation. Source setters validate before local
mutation so a second bound control can reject an inadmissible value consistently.
Configure compatible ranges/item counts before binding; unbind when replacing a
range with a disjoint domain. Selection uses `-1` for none. `selection()` exposes
a ChoiceGroup's local Value; `set_selected_index` is its concise mutation seam.

## Application-owned commands

`Command` remains the existing command authority. A default-constructed command
is anonymous and suitable for an owner member; use the existing id/text
constructor for application command registries and identifiable invocations.
`set_checked` makes checked state present, `clear_checked` removes it, and
`CommandState::checkable` distinguishes absence from false. `enabled`, existing
metadata and `invoked` retain their established meaning.

`button.bind(command)` borrows the command. Enabled state follows the command;
checked state is projected to CheckBox checked state or Button selected state.
CheckBox changes update the command. Clicking invokes it once. Rebinding a
CheckBox to Value replaces its Command binding and vice versa. `unbind_command`
ends only the command connection. A destroyed/disposed command is never invoked
through a surviving button. This additive API does not change the ownership of
the older `CommandBinding(shared_ptr, shared_ptr)` compatibility helper.

`command.bind_shortcut(window, gesture)` returns an AcceleratorToken. Keep it
for the shortcut's lifetime. Command or Window teardown revokes it. It executes
the same command and checks the same enabled state as buttons and existing
command-backed menus. Window input routing retains editor/menu first refusal.

## Collections and named skin parts

Construct collection controls with `make_control`, preserving retained parent
ownership. One subscription on the group survives `set_items`. Item ids must be
unique; labels/glyphs/accessibility names must be valid UTF-8. Replacements reuse
controls with the same id and disconnect removed items even if another owner
retains their controls. List construction/replacement may allocate. Retained
state changes and group dispatch do not allocate after configuration.

`CommandBar` accepts CommandItem records with id, label, textual glyph,
accessible name, optional shortcut and borrowed Command. A shortcut requires a
Command and is installed while attached to a Window. The bound command runs
before the bar's `invoked(const CommandItem&)`. The notification retains the old
item revision throughout dispatch, so replacing items inside a handler does not
invalidate the reference received by later handlers. Do not retain that borrow
after dispatch. Command pointers in item descriptions are configuration borrows;
never reuse a description whose application-owned model has been destroyed.

The bar has toolbar semantics and a single roving tab stop. Arrow keys move among
available items; Tab remains with Window traversal. `item(index)` and
`part("item", index)` expose its retained Button for visual recipes, typography
and content layout. Group layout currently distributes one horizontal row evenly.

`ChoiceGroup` accepts text, glyph and/or registered ImageId items. It owns scalar
selection state and binds an application Value<int>. `changed(int)` reports
selection changes; `activated(int)` reports input activation. Arrow keys select
and focus the next choice. Accessibility exposes a radio group and checked radio
items; native adapters map that to each platform's available group/radio roles.
Shrinking the list clears an index outside the new list to -1. Same-id controls
are reused. The named `item` part exposes ordinary Button visual recipes and
image/text placement, allowing chip, segmented-row or image-tile appearance.
Use the group's selection API rather than mutating an item's selected flag.

`ExpandableSections` accepts id, heading label, owned content and an optional
borrowed Value<bool>. The named parts are `header` (retained disclosure button)
and `body` (retained content). Keyboard/semantic activation uses the standard
button route. Setting expanded state changes content visibility, accessibility
and layout, and emits `toggled(index, expanded)`. Single-open mode closes the
other section models; at list replacement the first open section wins. Models
and contents must be distinct across sections. Header height is 28 logical
units; expanded body height uses its requested height. No hidden panel polling.

The parts are explicit group anatomy over existing controls. They do not claim
that the separate complete skin engine, two new skins, common Grid, form records
or designer have shipped. Those remain required in the availability plan.

Existing ListBox supplies `item_activated(std::size_t)` with persistent group
subscription and replacement APIs. A second list implementation is unnecessary.

Replacement rejects malformed records before publishing the new list. Once
published, a throwing allocation or application notification may interrupt
presentation configuration; the new records remain retained and safe to inspect,
and a later `set_items` can retry. Recursive replacement during configuration is
rejected. Replacement from ordinary group invocation is supported. An operation
that tries to change an actively notifying Value is rejected by its recursion
rule. No strong rollback guarantee is claimed across application callbacks.

## SDK and compatibility

The standalone `examples/reference` CMake project uses only the installed SDK.
It compiles/runs `owned_events`, `bound_values`, `shared_state` and
`collection_state`; native CI runs these after installation on each platform.

Component/Revocable, Event slot, CommandState and affected control layouts
change. Rebuild GUI.Forms and every C++ consumer together. Existing source APIs
remain available, but old object files are not binary-compatible. Header-aware
compiler caches invalidate them normally; importing a cache is not ABI repair.
No frozen C ABI, host protocol or FM0 availability manifest is changed.
