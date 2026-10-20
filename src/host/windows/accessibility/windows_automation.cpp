#include "windows_automation.hpp"

#include <algorithm>
#include <atomic>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <new>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

// UI Automation providers (stage A).
//
// Objects: one UiaElement per element a client asks for. It names its node
// by SemanticElementId (the scope's and the node's runtime IDs; the client
// root is {0, 0}) and reads a fresh snapshot for every call, so a client
// never sees a tree older than its call. The client root is the fragment
// root; its host provider is the window's, so UI Automation merges the
// window's own properties with these.
//
// Identity (interview GF017): a control's runtime ID is never reused, so an
// element of a disposed control resolves to nothing and answers
// UIA_E_ELEMENTNOTAVAILABLE, never another control's node. RuntimeId is
// UiaAppendRuntimeId followed by the scope's and the node's runtime IDs,
// each as two 32-bit halves, unique within the window; AutomationId is the
// node's stable ID (unique among its siblings, as UI Automation asks).
//
// Ownership: an element holds a shared reference to its window's
// SemanticSource and nothing else; COM reference counting ends its life.
// Arrays returned to UI Automation (SAFEARRAY) and BSTRs belong to the
// caller.
//
// Errors: E_INVALIDARG for a null out-parameter or a value out of range;
// UIA_E_ELEMENTNOTAVAILABLE when the window has gone or the node has left
// the tree; UIA_E_ELEMENTNOTENABLED for an action on a disabled node;
// UIA_E_INVALIDOPERATION when the node does not take the action or its
// control refuses it; E_OUTOFMEMORY when an allocation fails.

namespace gui_forms::host {

namespace {

const HRESULT element_not_available = static_cast<HRESULT>(UIA_E_ELEMENTNOTAVAILABLE);
const HRESULT element_not_enabled = static_cast<HRESULT>(UIA_E_ELEMENTNOTENABLED);
const HRESULT invalid_operation = static_cast<HRESULT>(UIA_E_INVALIDOPERATION);

// The framework name clients see in FrameworkId.
constexpr const wchar_t* framework_id = L"GUI.Forms";

// The longest text a client may set through IValueProvider::SetValue, in
// UTF-16 code units.
constexpr std::size_t maximum_value_length = 1024U * 1024U;

// -- what a node offers ----------------------------------------------------

bool toggle_pattern(const SemanticNode& node) noexcept {
    if (checkable_role(node.role)) return true;
    if (node.role == SemanticRole::radio_button) return false;
    // A toggle button or a checked menu item publishes checked (or mixed)
    // while it is on; the tree has no flag for one that is off (stage A gap
    // list), so the pattern is offered while the state is published.
    const bool on = has_semantic_state(node.states, SemanticState::checked) ||
                    has_semantic_state(node.states, SemanticState::mixed);
    const bool toggles = on && semantic_has_action(node, SemanticAction::press);
    return toggles;
}

bool invoke_pattern(const SemanticNode& node) noexcept {
    const bool invokes = semantic_has_action(node, SemanticAction::press) &&
                         !toggle_pattern(node) && node.role != SemanticRole::radio_button;
    return invokes;
}

bool selection_item_pattern(const SemanticNode& node) noexcept {
    const bool item = node.role == SemanticRole::radio_button ||
                      semantic_has_action(node, SemanticAction::select);
    return item;
}

bool selection_pattern(const SemanticNode& node) noexcept {
    for (const SemanticNode& child : node.children) {
        if (selection_item_pattern(child)) return true;
    }
    return false;
}

bool range_value_pattern(const SemanticNode& node) noexcept {
    const bool range = node.numeric_value.has_value();
    return range;
}

bool value_pattern(const SemanticNode& node) noexcept {
    if (node.numeric_value.has_value()) return false;
    const bool value = semantic_has_action(node, SemanticAction::set_value) ||
                       node.role == SemanticRole::text_box;
    return value;
}

bool expand_collapse_pattern(const SemanticNode& node) noexcept {
    const bool expands = semantic_has_action(node, SemanticAction::expand) ||
                         semantic_has_action(node, SemanticAction::collapse) ||
                         semantic_has_action(node, SemanticAction::show_menu);
    return expands;
}

bool supports_pattern(const SemanticNode& node, PATTERNID pattern) noexcept {
    switch (pattern) {
    case UIA_InvokePatternId: return invoke_pattern(node);
    case UIA_TogglePatternId: return toggle_pattern(node);
    case UIA_SelectionItemPatternId: return selection_item_pattern(node);
    case UIA_SelectionPatternId: return selection_pattern(node);
    case UIA_RangeValuePatternId: return range_value_pattern(node);
    case UIA_ValuePatternId: return value_pattern(node);
    case UIA_ExpandCollapsePatternId: return expand_collapse_pattern(node);
    default: return false;
    }
}

bool read_only_value(const SemanticNode& node) noexcept {
    const bool read_only = has_semantic_state(node.states, SemanticState::read_only) ||
                           !semantic_has_action(node, SemanticAction::set_value);
    return read_only;
}

bool selected_item(const SemanticNode& node) noexcept {
    if (node.role == SemanticRole::radio_button) {
        return has_semantic_state(node.states, SemanticState::checked);
    }
    return has_semantic_state(node.states, SemanticState::selected);
}

ToggleState toggle_state(const SemanticNode& node) noexcept {
    if (has_semantic_state(node.states, SemanticState::mixed)) return ToggleState_Indeterminate;
    if (has_semantic_state(node.states, SemanticState::checked)) return ToggleState_On;
    return ToggleState_Off;
}

ExpandCollapseState expand_collapse_state(const SemanticNode& node) noexcept {
    if (has_semantic_state(node.states, SemanticState::expanded)) {
        return ExpandCollapseState_Expanded;
    }
    return ExpandCollapseState_Collapsed;
}

CONTROLTYPEID control_type(SemanticRole role) noexcept {
    switch (role) {
    case SemanticRole::generic: return UIA_GroupControlTypeId;
    case SemanticRole::group: return UIA_GroupControlTypeId;
    case SemanticRole::static_text: return UIA_TextControlTypeId;
    case SemanticRole::button: return UIA_ButtonControlTypeId;
    case SemanticRole::check_box: return UIA_CheckBoxControlTypeId;
    case SemanticRole::radio_button: return UIA_RadioButtonControlTypeId;
    case SemanticRole::link: return UIA_HyperlinkControlTypeId;
    case SemanticRole::text_box: return UIA_EditControlTypeId;
    case SemanticRole::list: return UIA_ListControlTypeId;
    case SemanticRole::list_item: return UIA_ListItemControlTypeId;
    // A checked list's item is a list item with the toggle pattern.
    case SemanticRole::check_list_item: return UIA_ListItemControlTypeId;
    case SemanticRole::combo_box: return UIA_ComboBoxControlTypeId;
    case SemanticRole::slider: return UIA_SliderControlTypeId;
    case SemanticRole::scroll_bar: return UIA_ScrollBarControlTypeId;
    case SemanticRole::progress_bar: return UIA_ProgressBarControlTypeId;
    case SemanticRole::image: return UIA_ImageControlTypeId;
    case SemanticRole::tab_group: return UIA_TabControlTypeId;
    case SemanticRole::tab: return UIA_TabItemControlTypeId;
    case SemanticRole::split_pane: return UIA_PaneControlTypeId;
    case SemanticRole::numeric_field: return UIA_SpinnerControlTypeId;
    case SemanticRole::tool_tip: return UIA_ToolTipControlTypeId;
    // A date field with a drop-down calendar, as MSAA's drop list is.
    case SemanticRole::date_picker: return UIA_ComboBoxControlTypeId;
    case SemanticRole::calendar: return UIA_CalendarControlTypeId;
    case SemanticRole::date_cell: return UIA_DataItemControlTypeId;
    case SemanticRole::property_grid: return UIA_TableControlTypeId;
    case SemanticRole::property_group: return UIA_GroupControlTypeId;
    case SemanticRole::property_row: return UIA_DataItemControlTypeId;
    case SemanticRole::menu_bar: return UIA_MenuBarControlTypeId;
    case SemanticRole::menu_bar_item: return UIA_MenuItemControlTypeId;
    case SemanticRole::menu: return UIA_MenuControlTypeId;
    case SemanticRole::menu_item: return UIA_MenuItemControlTypeId;
    case SemanticRole::separator: return UIA_SeparatorControlTypeId;
    case SemanticRole::toolbar: return UIA_ToolBarControlTypeId;
    case SemanticRole::radio_group: return UIA_GroupControlTypeId;
    case SemanticRole::split_button: return UIA_SplitButtonControlTypeId;
    // A button that opens a menu: a button with the expand-collapse pattern.
    case SemanticRole::menu_button: return UIA_ButtonControlTypeId;
    case SemanticRole::pane: return UIA_PaneControlTypeId;
    // The content a tab shows is a pane, as a tab item's content is.
    case SemanticRole::tab_panel: return UIA_PaneControlTypeId;
    case SemanticRole::count: return UIA_GroupControlTypeId;
    }
    return UIA_GroupControlTypeId;
}

// -- VARIANT and SAFEARRAY helpers -----------------------------------------

void set_bool(VARIANT* out, bool value) noexcept {
    (*out).vt = VT_BOOL;
    (*out).boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
}

void set_int(VARIANT* out, int value) noexcept {
    (*out).vt = VT_I4;
    (*out).lVal = value;
}

// Leaves `out` VT_EMPTY for empty text, so UI Automation supplies its
// default.
HRESULT set_text(VARIANT* out, std::string_view text) noexcept {
    BSTR value = nullptr;
    const HRESULT status = utf8_to_bstr(text, &value);
    if (FAILED(status)) return status;
    if (value == nullptr) return S_OK;
    (*out).vt = VT_BSTR;
    (*out).bstrVal = value;
    return S_OK;
}

HRESULT set_wide_text(VARIANT* out, const wchar_t* text) noexcept {
    BSTR value = SysAllocString(text);
    if (value == nullptr) return E_OUTOFMEMORY;
    (*out).vt = VT_BSTR;
    (*out).bstrVal = value;
    return S_OK;
}

// The four 32-bit runtime ID parts after UiaAppendRuntimeId: the scope's
// and the node's runtime IDs, high half first.
constexpr LONG runtime_id_length = 5;

int high_half(std::uint64_t value) noexcept {
    const std::uint32_t half = static_cast<std::uint32_t>(value >> 32U);
    return static_cast<int>(half);
}

int low_half(std::uint64_t value) noexcept {
    const std::uint32_t half = static_cast<std::uint32_t>(value & 0xFFFFFFFFULL);
    return static_cast<int>(half);
}

// Puts `element`'s provider at `index` of a VT_UNKNOWN array, which takes
// its own reference; `element`'s is released.
HRESULT put_element(SAFEARRAY* array, LONG index, IRawElementProviderSimple* element) noexcept {
    IUnknown* unknown = static_cast<IUnknown*>(element);
    const HRESULT status = SafeArrayPutElement(array, &index, unknown);
    (*element).Release();
    return status;
}

// -- the provider ----------------------------------------------------------

class UiaElement final : public IRawElementProviderSimple,
                         public IRawElementProviderFragment,
                         public IRawElementProviderFragmentRoot,
                         public IInvokeProvider,
                         public IToggleProvider,
                         public IValueProvider,
                         public IRangeValueProvider,
                         public IExpandCollapseProvider,
                         public ISelectionItemProvider,
                         public ISelectionProvider {
public:
    UiaElement(std::shared_ptr<SemanticSource> source, SemanticElementId id) noexcept
        : source_(std::move(source)), id_(id) {}
    UiaElement(const UiaElement&) = delete;
    UiaElement& operator=(const UiaElement&) = delete;

    // IUnknown.
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override;
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;

    // IRawElementProviderSimple.
    HRESULT STDMETHODCALLTYPE get_ProviderOptions(ProviderOptions* out) override;
    HRESULT STDMETHODCALLTYPE GetPatternProvider(PATTERNID pattern, IUnknown** out) override;
    HRESULT STDMETHODCALLTYPE GetPropertyValue(PROPERTYID property, VARIANT* out) override;
    HRESULT STDMETHODCALLTYPE get_HostRawElementProvider(IRawElementProviderSimple** out) override;

    // IRawElementProviderFragment.
    HRESULT STDMETHODCALLTYPE Navigate(NavigateDirection direction,
                                       IRawElementProviderFragment** out) override;
    HRESULT STDMETHODCALLTYPE GetRuntimeId(SAFEARRAY** out) override;
    HRESULT STDMETHODCALLTYPE get_BoundingRectangle(UiaRect* out) override;
    HRESULT STDMETHODCALLTYPE GetEmbeddedFragmentRoots(SAFEARRAY** out) override;
    HRESULT STDMETHODCALLTYPE SetFocus() override;
    HRESULT STDMETHODCALLTYPE get_FragmentRoot(IRawElementProviderFragmentRoot** out) override;

    // IRawElementProviderFragmentRoot (answered by every element; UI
    // Automation asks the root).
    HRESULT STDMETHODCALLTYPE ElementProviderFromPoint(double x, double y,
                                                       IRawElementProviderFragment** out) override;
    HRESULT STDMETHODCALLTYPE GetFocus(IRawElementProviderFragment** out) override;

    // IInvokeProvider.
    HRESULT STDMETHODCALLTYPE Invoke() override;

    // IToggleProvider.
    HRESULT STDMETHODCALLTYPE Toggle() override;
    HRESULT STDMETHODCALLTYPE get_ToggleState(ToggleState* out) override;

    // IValueProvider and IRangeValueProvider; get_IsReadOnly serves both
    // (read-only when the node is or when it takes no value).
    HRESULT STDMETHODCALLTYPE SetValue(LPCWSTR value) override;
    HRESULT STDMETHODCALLTYPE get_Value(BSTR* out) override;
    HRESULT STDMETHODCALLTYPE get_IsReadOnly(BOOL* out) override;
    HRESULT STDMETHODCALLTYPE SetValue(double value) override;
    HRESULT STDMETHODCALLTYPE get_Value(double* out) override;
    HRESULT STDMETHODCALLTYPE get_Maximum(double* out) override;
    HRESULT STDMETHODCALLTYPE get_Minimum(double* out) override;
    HRESULT STDMETHODCALLTYPE get_LargeChange(double* out) override;
    HRESULT STDMETHODCALLTYPE get_SmallChange(double* out) override;

    // IExpandCollapseProvider.
    HRESULT STDMETHODCALLTYPE Expand() override;
    HRESULT STDMETHODCALLTYPE Collapse() override;
    HRESULT STDMETHODCALLTYPE get_ExpandCollapseState(ExpandCollapseState* out) override;

    // ISelectionItemProvider.
    HRESULT STDMETHODCALLTYPE Select() override;
    HRESULT STDMETHODCALLTYPE AddToSelection() override;
    HRESULT STDMETHODCALLTYPE RemoveFromSelection() override;
    HRESULT STDMETHODCALLTYPE get_IsSelected(BOOL* out) override;
    HRESULT STDMETHODCALLTYPE get_SelectionContainer(IRawElementProviderSimple** out) override;

    // ISelectionProvider.
    HRESULT STDMETHODCALLTYPE GetSelection(SAFEARRAY** out) override;
    HRESULT STDMETHODCALLTYPE get_CanSelectMultiple(BOOL* out) override;
    HRESULT STDMETHODCALLTYPE get_IsSelectionRequired(BOOL* out) override;

private:
    ~UiaElement() = default;

    // Takes a snapshot into `client` and finds this element's node.
    HRESULT resolve(SemanticNode& client, SemanticLocation& self) noexcept;
    // As resolve, and refuses a node that does not offer `pattern`.
    HRESULT resolve_pattern(PATTERNID pattern, SemanticNode& client,
                            SemanticLocation& self) noexcept;
    // Performs `action` on this element's node, which must offer `pattern`.
    HRESULT act(PATTERNID pattern, SemanticAction action, std::string_view value) noexcept;
    // A new element for the located node, as IRawElementProviderFragment.
    HRESULT fragment_for(const SemanticLocation& location,
                         IRawElementProviderFragment** out) noexcept;
    HRESULT property_value(const SemanticNode& client, const SemanticLocation& self,
                           PROPERTYID property, VARIANT* out) noexcept;
    HRESULT controller_for(const SemanticNode& client, const SemanticNode& tab,
                           VARIANT* out) noexcept;

    std::atomic<ULONG> references_{1U};
    std::shared_ptr<SemanticSource> source_;
    SemanticElementId id_;
};

// Performs a press posted by IInvokeProvider::Invoke on the window's next
// dispatcher turn: UI Automation requires Invoke to return without waiting
// for the action, which may open a modal dialog. It finds the element
// again first, so an element whose control went in between does nothing.
struct PostedInvoke final {
    std::shared_ptr<SemanticSource> source;
    SemanticElementId id;

    void operator()() const;
};

void PostedInvoke::operator()() const {
    SemanticNode client;
    if (!(*source).snapshot(client)) return;
    SemanticLocation location;
    if (!locate_semantic_element(client, id, location)) return;
    if (!(*source).perform(location, SemanticAction::press, {})) return;
    if (!UiaClientsAreListening()) return;
    IRawElementProviderSimple* element = make_automation_element(source, id);
    if (element == nullptr) return;
    UiaRaiseAutomationEvent(element, UIA_Invoke_InvokedEventId);
    (*element).Release();
}

HRESULT UiaElement::resolve(SemanticNode& client, SemanticLocation& self) noexcept {
    if (!(*source_).snapshot(client)) return element_not_available;
    if (!locate_semantic_element(client, id_, self)) return element_not_available;
    return S_OK;
}

HRESULT UiaElement::resolve_pattern(PATTERNID pattern, SemanticNode& client,
                                    SemanticLocation& self) noexcept {
    const HRESULT status = resolve(client, self);
    if (FAILED(status)) return status;
    if (self.parent == nullptr || !supports_pattern(*self.node, pattern)) return invalid_operation;
    return S_OK;
}

HRESULT UiaElement::act(PATTERNID pattern, SemanticAction action,
                        std::string_view value) noexcept {
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(pattern, client, self);
    if (FAILED(status)) return status;
    if (!has_semantic_state((*self.node).states, SemanticState::enabled)) {
        return element_not_enabled;
    }
    if (!(*source_).perform(self, action, value)) return invalid_operation;
    return S_OK;
}

HRESULT UiaElement::fragment_for(const SemanticLocation& location,
                                 IRawElementProviderFragment** out) noexcept {
    UiaElement* element = new (std::nothrow) UiaElement(source_, location.id);
    if (element == nullptr) return E_OUTOFMEMORY;
    *out = static_cast<IRawElementProviderFragment*>(element);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::QueryInterface(REFIID iid, void** out) {
    if (out == nullptr) return E_POINTER;
    *out = nullptr;
    if (iid == IID_IUnknown || iid == IID_IRawElementProviderSimple) {
        *out = static_cast<IRawElementProviderSimple*>(this);
    } else if (iid == IID_IRawElementProviderFragment) {
        *out = static_cast<IRawElementProviderFragment*>(this);
    } else if (iid == IID_IRawElementProviderFragmentRoot) {
        *out = static_cast<IRawElementProviderFragmentRoot*>(this);
    } else if (iid == IID_IInvokeProvider) {
        *out = static_cast<IInvokeProvider*>(this);
    } else if (iid == IID_IToggleProvider) {
        *out = static_cast<IToggleProvider*>(this);
    } else if (iid == IID_IValueProvider) {
        *out = static_cast<IValueProvider*>(this);
    } else if (iid == IID_IRangeValueProvider) {
        *out = static_cast<IRangeValueProvider*>(this);
    } else if (iid == IID_IExpandCollapseProvider) {
        *out = static_cast<IExpandCollapseProvider*>(this);
    } else if (iid == IID_ISelectionItemProvider) {
        *out = static_cast<ISelectionItemProvider*>(this);
    } else if (iid == IID_ISelectionProvider) {
        *out = static_cast<ISelectionProvider*>(this);
    } else {
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}

ULONG STDMETHODCALLTYPE UiaElement::AddRef() {
    const ULONG count = ++references_;
    return count;
}

ULONG STDMETHODCALLTYPE UiaElement::Release() {
    const ULONG count = --references_;
    if (count == 0U) delete this;
    return count;
}

HRESULT STDMETHODCALLTYPE UiaElement::get_ProviderOptions(ProviderOptions* out) {
    if (out == nullptr) return E_INVALIDARG;
    // See windows_automation.hpp: COM threading keeps every call on the UI
    // thread.
    const int options = ProviderOptions_ServerSideProvider | ProviderOptions_UseComThreading;
    *out = static_cast<ProviderOptions>(options);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::GetPatternProvider(PATTERNID pattern, IUnknown** out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve(client, self);
    if (FAILED(status)) return status;
    if (self.parent == nullptr || !supports_pattern(*self.node, pattern)) return S_OK;
    *out = static_cast<IUnknown*>(static_cast<IRawElementProviderSimple*>(this));
    AddRef();
    return S_OK;
}

HRESULT UiaElement::controller_for(const SemanticNode& client, const SemanticNode& tab,
                                   VARIANT* out) noexcept {
    SemanticLocation panel;
    const SemanticLocation client_at = client_location(client);
    if (!find_control_location(client_at, tab.controlled_panel, panel)) return S_OK;
    SAFEARRAY* array = SafeArrayCreateVector(VT_UNKNOWN, 0, 1U);
    if (array == nullptr) return E_OUTOFMEMORY;
    IRawElementProviderSimple* element = make_automation_element(source_, panel.id);
    if (element == nullptr) {
        SafeArrayDestroy(array);
        return E_OUTOFMEMORY;
    }
    const HRESULT status = put_element(array, 0, element);
    if (FAILED(status)) {
        SafeArrayDestroy(array);
        return status;
    }
    (*out).vt = VT_UNKNOWN | VT_ARRAY;
    (*out).parray = array;
    return S_OK;
}

HRESULT UiaElement::property_value(const SemanticNode& client, const SemanticLocation& self,
                                   PROPERTYID property, VARIANT* out) noexcept {
    const SemanticNode& node = *self.node;
    const SemanticState states = node.states;
    // The fragment root is the window's client area. Apart from its name
    // and framework, the window's host provider supplies its properties
    // (control type Window, enabled, focusable, and so on).
    const bool root = self.parent == nullptr;
    if (root && property != UIA_NamePropertyId && property != UIA_FrameworkIdPropertyId) {
        return S_OK;
    }
    switch (property) {
    case UIA_ControlTypePropertyId:
        set_int(out, control_type(node.role));
        return S_OK;
    case UIA_NamePropertyId: return set_text(out, node.name);
    case UIA_AutomationIdPropertyId: return set_text(out, node.stable_id);
    case UIA_HelpTextPropertyId: return set_text(out, node.description);
    case UIA_FullDescriptionPropertyId: return set_text(out, node.description);
    case UIA_FrameworkIdPropertyId: return set_wide_text(out, framework_id);
    case UIA_IsEnabledPropertyId:
        set_bool(out, has_semantic_state(states, SemanticState::enabled));
        return S_OK;
    case UIA_IsKeyboardFocusablePropertyId:
        set_bool(out, has_semantic_state(states, SemanticState::focusable));
        return S_OK;
    case UIA_HasKeyboardFocusPropertyId:
        set_bool(out, has_semantic_state(states, SemanticState::focused));
        return S_OK;
    case UIA_IsOffscreenPropertyId:
        set_bool(out, !has_semantic_state(states, SemanticState::visible));
        return S_OK;
    case UIA_IsPasswordPropertyId:
        set_bool(out, has_semantic_state(states, SemanticState::protected_content));
        return S_OK;
    case UIA_IsDataValidForFormPropertyId:
        set_bool(out, !has_semantic_state(states, SemanticState::invalid));
        return S_OK;
    case UIA_IsControlElementPropertyId:
    case UIA_IsContentElementPropertyId:
        set_bool(out, true);
        return S_OK;
    case UIA_PositionInSetPropertyId:
        // A position is published with its set size or not at all.
        if (!node.position_in_set || !node.set_size) return S_OK;
        set_int(out, static_cast<int>(*node.position_in_set));
        return S_OK;
    case UIA_SizeOfSetPropertyId:
        if (!node.position_in_set || !node.set_size) return S_OK;
        set_int(out, static_cast<int>(*node.set_size));
        return S_OK;
    case UIA_OrientationPropertyId:
        if (!node.orientation) return S_OK;
        if (*node.orientation == SemanticOrientation::vertical) {
            set_int(out, OrientationType_Vertical);
        } else {
            set_int(out, OrientationType_Horizontal);
        }
        return S_OK;
    case UIA_ControllerForPropertyId:
        if (node.controlled_panel.empty()) return S_OK;
        return controller_for(client, node, out);
    default: return S_OK;
    }
}

HRESULT STDMETHODCALLTYPE UiaElement::GetPropertyValue(PROPERTYID property, VARIANT* out) {
    if (out == nullptr) return E_INVALIDARG;
    VariantInit(out);
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve(client, self);
    if (FAILED(status)) return status;
    return property_value(client, self, property, out);
}

HRESULT STDMETHODCALLTYPE UiaElement::get_HostRawElementProvider(IRawElementProviderSimple** out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    // Only the fragment root is hosted by the window; fragments are not.
    if (id_ != SemanticElementId{}) return S_OK;
    if ((*source_).hwnd == nullptr) return element_not_available;
    return UiaHostProviderFromHwnd((*source_).hwnd, out);
}

HRESULT STDMETHODCALLTYPE UiaElement::Navigate(NavigateDirection direction,
                                               IRawElementProviderFragment** out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve(client, self);
    if (FAILED(status)) return status;
    const std::size_t count = (*self.node).children.size();
    if (direction == NavigateDirection_FirstChild || direction == NavigateDirection_LastChild) {
        if (count == 0U) return S_OK;
        std::size_t index = 0U;
        if (direction == NavigateDirection_LastChild) index = count - 1U;
        return fragment_for(child_location(self, index), out);
    }
    // The fragment root's parent and siblings are the window's, which UI
    // Automation reaches through the host provider.
    SemanticLocation parent;
    if (!parent_semantic_location(client, self, parent)) return S_OK;
    if (direction == NavigateDirection_Parent) return fragment_for(parent, out);
    const std::size_t siblings = (*parent.node).children.size();
    if (direction == NavigateDirection_NextSibling && self.index + 1U < siblings) {
        return fragment_for(child_location(parent, self.index + 1U), out);
    }
    if (direction == NavigateDirection_PreviousSibling && self.index > 0U) {
        return fragment_for(child_location(parent, self.index - 1U), out);
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::GetRuntimeId(SAFEARRAY** out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve(client, self);
    if (FAILED(status)) return status;
    // The fragment root takes the window's runtime ID.
    if (self.parent == nullptr) return S_OK;
    SAFEARRAY* array = SafeArrayCreateVector(VT_I4, 0, static_cast<ULONG>(runtime_id_length));
    if (array == nullptr) return E_OUTOFMEMORY;
    const int parts[runtime_id_length] = {
        UiaAppendRuntimeId, high_half(id_.scope), low_half(id_.scope), high_half(id_.node),
        low_half(id_.node)};
    for (LONG index = 0; index < runtime_id_length; ++index) {
        int part = parts[index];
        const HRESULT put = SafeArrayPutElement(array, &index, &part);
        if (FAILED(put)) {
            SafeArrayDestroy(array);
            return put;
        }
    }
    *out = array;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::get_BoundingRectangle(UiaRect* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = UiaRect{0.0, 0.0, 0.0, 0.0};
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve(client, self);
    if (FAILED(status)) return status;
    // A fragment root hosted in a window answers an empty rectangle; UI
    // Automation takes the window's.
    if (self.parent == nullptr) return S_OK;
    const Rect bounds = (*self.node).bounds;
    const double scale = (*source_).scale;
    // A node with no area answers the empty rectangle, as UI Automation
    // reports one to its clients.
    const double width = std::round(bounds.width * scale);
    const double height = std::round(bounds.height * scale);
    if (width <= 0.0 || height <= 0.0) return S_OK;
    POINT origin{};
    ClientToScreen((*source_).hwnd, &origin);
    (*out).left = origin.x + std::round(bounds.x * scale);
    (*out).top = origin.y + std::round(bounds.y * scale);
    (*out).width = width;
    (*out).height = height;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::GetEmbeddedFragmentRoots(SAFEARRAY** out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::SetFocus() {
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve(client, self);
    if (FAILED(status)) return status;
    if (self.parent == nullptr) {
        ::SetFocus((*source_).hwnd);
        return S_OK;
    }
    if (!has_semantic_state((*self.node).states, SemanticState::focusable)) {
        return invalid_operation;
    }
    if (!(*source_).perform(self, SemanticAction::focus, {})) return invalid_operation;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::get_FragmentRoot(IRawElementProviderFragmentRoot** out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    if ((*source_).model == nullptr) return element_not_available;
    UiaElement* root = new (std::nothrow) UiaElement(source_, SemanticElementId{});
    if (root == nullptr) return E_OUTOFMEMORY;
    *out = static_cast<IRawElementProviderFragmentRoot*>(root);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::ElementProviderFromPoint(double x, double y,
                                                               IRawElementProviderFragment** out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve(client, self);
    if (FAILED(status)) return status;
    POINT origin{};
    ClientToScreen((*source_).hwnd, &origin);
    const double scale = (*source_).scale;
    const double logical_x = (x - origin.x) / scale;
    const double logical_y = (y - origin.y) / scale;
    SemanticLocation hit;
    if (!hit_semantic_location(self, logical_x, logical_y, hit)) return S_OK;
    return fragment_for(hit, out);
}

HRESULT STDMETHODCALLTYPE UiaElement::GetFocus(IRawElementProviderFragment** out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve(client, self);
    if (FAILED(status)) return status;
    SemanticLocation focused;
    if (!find_state_location(self, SemanticState::focused, focused)) return S_OK;
    if (focused.id == id_) return S_OK;
    return fragment_for(focused, out);
}

HRESULT STDMETHODCALLTYPE UiaElement::Invoke() {
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_InvokePatternId, client, self);
    if (FAILED(status)) return status;
    if (!has_semantic_state((*self.node).states, SemanticState::enabled)) {
        return element_not_enabled;
    }
    Window* model = (*source_).model;
    try {
        const DispatchOperation posted =
            (*model).begin_invoke(std::function<void()>(PostedInvoke{source_, id_}));
        if (posted.state() == DispatchOperationState::invalid) return invalid_operation;
    } catch (...) {
        return invalid_operation;
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::Toggle() {
    return act(UIA_TogglePatternId, SemanticAction::press, {});
}

HRESULT STDMETHODCALLTYPE UiaElement::get_ToggleState(ToggleState* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = ToggleState_Off;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_TogglePatternId, client, self);
    if (FAILED(status)) return status;
    *out = toggle_state(*self.node);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::SetValue(LPCWSTR value) {
    if (value == nullptr) return E_INVALIDARG;
    std::string text;
    try {
        const std::wstring_view wide(value);
        if (wide.size() > maximum_value_length) return E_INVALIDARG;
        if (!utf8_from_wide(wide, text)) return E_INVALIDARG;
    } catch (...) {
        return E_OUTOFMEMORY;
    }
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_ValuePatternId, client, self);
    if (FAILED(status)) return status;
    if (read_only_value(*self.node)) return invalid_operation;
    return act(UIA_ValuePatternId, SemanticAction::set_value, text);
}

HRESULT STDMETHODCALLTYPE UiaElement::get_Value(BSTR* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_ValuePatternId, client, self);
    if (FAILED(status)) return status;
    const HRESULT converted = utf8_to_bstr((*self.node).value, out);
    if (FAILED(converted)) return converted;
    // An empty value is an empty string, not a missing one.
    if (*out == nullptr) *out = SysAllocString(L"");
    if (*out == nullptr) return E_OUTOFMEMORY;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::get_IsReadOnly(BOOL* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = TRUE;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve(client, self);
    if (FAILED(status)) return status;
    const bool offered = supports_pattern(*self.node, UIA_ValuePatternId) ||
                         supports_pattern(*self.node, UIA_RangeValuePatternId);
    if (self.parent == nullptr || !offered) return invalid_operation;
    *out = read_only_value(*self.node) ? TRUE : FALSE;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::SetValue(double value) {
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_RangeValuePatternId, client, self);
    if (FAILED(status)) return status;
    const SemanticNode& node = *self.node;
    if (read_only_value(node)) return invalid_operation;
    if (!std::isfinite(value)) return E_INVALIDARG;
    if (node.minimum_value && value < *node.minimum_value) return E_INVALIDARG;
    if (node.maximum_value && value > *node.maximum_value) return E_INVALIDARG;
    // The shortest decimal text that reads back as `value`.
    char text[64]{};
    const std::to_chars_result written = std::to_chars(text, text + sizeof(text), value);
    if (written.ec != std::errc()) return E_INVALIDARG;
    const std::string_view number(text, static_cast<std::size_t>(written.ptr - text));
    return act(UIA_RangeValuePatternId, SemanticAction::set_value, number);
}

HRESULT STDMETHODCALLTYPE UiaElement::get_Value(double* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = 0.0;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_RangeValuePatternId, client, self);
    if (FAILED(status)) return status;
    *out = *(*self.node).numeric_value;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::get_Maximum(double* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = 0.0;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_RangeValuePatternId, client, self);
    if (FAILED(status)) return status;
    const SemanticNode& node = *self.node;
    *out = node.maximum_value ? *node.maximum_value : *node.numeric_value;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::get_Minimum(double* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = 0.0;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_RangeValuePatternId, client, self);
    if (FAILED(status)) return status;
    const SemanticNode& node = *self.node;
    *out = node.minimum_value ? *node.minimum_value : *node.numeric_value;
    return S_OK;
}

// The tree carries no step sizes (stage A gap list); zero tells a client
// none is known.
HRESULT STDMETHODCALLTYPE UiaElement::get_LargeChange(double* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = 0.0;
    SemanticNode client;
    SemanticLocation self;
    return resolve_pattern(UIA_RangeValuePatternId, client, self);
}

HRESULT STDMETHODCALLTYPE UiaElement::get_SmallChange(double* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = 0.0;
    SemanticNode client;
    SemanticLocation self;
    return resolve_pattern(UIA_RangeValuePatternId, client, self);
}

HRESULT STDMETHODCALLTYPE UiaElement::Expand() {
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_ExpandCollapsePatternId, client, self);
    if (FAILED(status)) return status;
    SemanticAction action = SemanticAction::show_menu;
    if (semantic_has_action(*self.node, SemanticAction::expand)) action = SemanticAction::expand;
    if (has_semantic_state((*self.node).states, SemanticState::expanded)) return S_OK;
    return act(UIA_ExpandCollapsePatternId, action, {});
}

HRESULT STDMETHODCALLTYPE UiaElement::Collapse() {
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_ExpandCollapsePatternId, client, self);
    if (FAILED(status)) return status;
    if (!has_semantic_state((*self.node).states, SemanticState::expanded)) return S_OK;
    return act(UIA_ExpandCollapsePatternId, SemanticAction::collapse, {});
}

HRESULT STDMETHODCALLTYPE UiaElement::get_ExpandCollapseState(ExpandCollapseState* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = ExpandCollapseState_Collapsed;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_ExpandCollapsePatternId, client, self);
    if (FAILED(status)) return status;
    *out = expand_collapse_state(*self.node);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::Select() {
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_SelectionItemPatternId, client, self);
    if (FAILED(status)) return status;
    // A radio button is selected by pressing it.
    SemanticAction action = SemanticAction::select;
    if ((*self.node).role == SemanticRole::radio_button) action = SemanticAction::press;
    return act(UIA_SelectionItemPatternId, action, {});
}

HRESULT STDMETHODCALLTYPE UiaElement::AddToSelection() {
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_SelectionItemPatternId, client, self);
    if (FAILED(status)) return status;
    // The tree has single selection only: adding is allowed only for the
    // item already selected.
    if (selected_item(*self.node)) return S_OK;
    return invalid_operation;
}

HRESULT STDMETHODCALLTYPE UiaElement::RemoveFromSelection() {
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_SelectionItemPatternId, client, self);
    if (FAILED(status)) return status;
    if (!selected_item(*self.node)) return S_OK;
    return invalid_operation;
}

HRESULT STDMETHODCALLTYPE UiaElement::get_IsSelected(BOOL* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = FALSE;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_SelectionItemPatternId, client, self);
    if (FAILED(status)) return status;
    *out = selected_item(*self.node) ? TRUE : FALSE;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::get_SelectionContainer(IRawElementProviderSimple** out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_SelectionItemPatternId, client, self);
    if (FAILED(status)) return status;
    SemanticLocation parent;
    if (!parent_semantic_location(client, self, parent)) return S_OK;
    if (!selection_pattern(*parent.node)) return S_OK;
    *out = make_automation_element(source_, parent.id);
    if (*out == nullptr) return E_OUTOFMEMORY;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::GetSelection(SAFEARRAY** out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = nullptr;
    SemanticNode client;
    SemanticLocation self;
    const HRESULT status = resolve_pattern(UIA_SelectionPatternId, client, self);
    if (FAILED(status)) return status;
    const std::vector<SemanticNode>& children = (*self.node).children;
    ULONG count = 0U;
    for (const SemanticNode& child : children) {
        if (selection_item_pattern(child) && selected_item(child)) ++count;
    }
    SAFEARRAY* array = SafeArrayCreateVector(VT_UNKNOWN, 0, count);
    if (array == nullptr) return E_OUTOFMEMORY;
    LONG position = 0;
    for (std::size_t index = 0; index < children.size(); ++index) {
        const SemanticNode& child = children[index];
        if (!selection_item_pattern(child) || !selected_item(child)) continue;
        const SemanticLocation child_at = child_location(self, index);
        IRawElementProviderSimple* element = make_automation_element(source_, child_at.id);
        if (element == nullptr) {
            SafeArrayDestroy(array);
            return E_OUTOFMEMORY;
        }
        const HRESULT put = put_element(array, position, element);
        if (FAILED(put)) {
            SafeArrayDestroy(array);
            return put;
        }
        ++position;
    }
    *out = array;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE UiaElement::get_CanSelectMultiple(BOOL* out) {
    if (out == nullptr) return E_INVALIDARG;
    // The tree has no multiple-selection flag (stage A gap list).
    *out = FALSE;
    SemanticNode client;
    SemanticLocation self;
    return resolve_pattern(UIA_SelectionPatternId, client, self);
}

HRESULT STDMETHODCALLTYPE UiaElement::get_IsSelectionRequired(BOOL* out) {
    if (out == nullptr) return E_INVALIDARG;
    *out = FALSE;
    SemanticNode client;
    SemanticLocation self;
    return resolve_pattern(UIA_SelectionPatternId, client, self);
}

// -- events ----------------------------------------------------------------

// One node of a published tree, by identity, for comparing two trees.
struct PublishedNode final {
    SemanticElementId id;
    const SemanticNode* node;
};

struct PublishedNodeBefore final {
    bool operator()(const PublishedNode& left, const PublishedNode& right) const noexcept {
        if (left.id.scope != right.id.scope) return left.id.scope < right.id.scope;
        return left.id.node < right.id.node;
    }
};

void collect_published(const SemanticLocation& at, std::vector<PublishedNode>& nodes) {
    nodes.push_back(PublishedNode{at.id, at.node});
    const std::size_t count = (*at.node).children.size();
    for (std::size_t index = 0; index < count; ++index) {
        collect_published(child_location(at, index), nodes);
    }
}

// Raises one property-changed event for `id` when `before` and `after`
// differ. `before` and `after` are VARIANTs the call clears.
void raise_property_change(const std::shared_ptr<SemanticSource>& source, SemanticElementId id,
                           PROPERTYID property, VARIANT& before, VARIANT& after) noexcept {
    IRawElementProviderSimple* element = make_automation_element(source, id);
    if (element != nullptr) {
        UiaRaiseAutomationPropertyChangedEvent(element, property, before, after);
        (*element).Release();
    }
    VariantClear(&before);
    VariantClear(&after);
}

void raise_node_changes(const std::shared_ptr<SemanticSource>& source, SemanticElementId id,
                        const SemanticNode& before, const SemanticNode& after) noexcept {
    VARIANT old_value;
    VARIANT new_value;
    VariantInit(&old_value);
    VariantInit(&new_value);
    if (toggle_pattern(after) && toggle_state(before) != toggle_state(after)) {
        set_int(&old_value, toggle_state(before));
        set_int(&new_value, toggle_state(after));
        raise_property_change(source, id, UIA_ToggleToggleStatePropertyId, old_value, new_value);
    }
    if (expand_collapse_pattern(after) &&
        expand_collapse_state(before) != expand_collapse_state(after)) {
        set_int(&old_value, expand_collapse_state(before));
        set_int(&new_value, expand_collapse_state(after));
        raise_property_change(source, id, UIA_ExpandCollapseExpandCollapseStatePropertyId,
                              old_value, new_value);
    }
    if (selection_item_pattern(after) && selected_item(before) != selected_item(after)) {
        set_bool(&old_value, selected_item(before));
        set_bool(&new_value, selected_item(after));
        raise_property_change(source, id, UIA_SelectionItemIsSelectedPropertyId, old_value,
                              new_value);
        IRawElementProviderSimple* element = make_automation_element(source, id);
        if (element != nullptr && selected_item(after)) {
            UiaRaiseAutomationEvent(element, UIA_SelectionItem_ElementSelectedEventId);
        }
        if (element != nullptr) (*element).Release();
    }
    if (value_pattern(after) && before.value != after.value) {
        const HRESULT old_text = set_text(&old_value, before.value);
        const HRESULT new_text = set_text(&new_value, after.value);
        if (SUCCEEDED(old_text) && SUCCEEDED(new_text)) {
            raise_property_change(source, id, UIA_ValueValuePropertyId, old_value, new_value);
        } else {
            VariantClear(&old_value);
            VariantClear(&new_value);
        }
    }
    if (range_value_pattern(after) && before.numeric_value && before.numeric_value != after.numeric_value) {
        old_value.vt = VT_R8;
        old_value.dblVal = *before.numeric_value;
        new_value.vt = VT_R8;
        new_value.dblVal = *after.numeric_value;
        raise_property_change(source, id, UIA_RangeValueValuePropertyId, old_value, new_value);
    }
    if (before.name != after.name) {
        const HRESULT old_text = set_text(&old_value, before.name);
        const HRESULT new_text = set_text(&new_value, after.name);
        if (SUCCEEDED(old_text) && SUCCEEDED(new_text)) {
            raise_property_change(source, id, UIA_NamePropertyId, old_value, new_value);
        } else {
            VariantClear(&old_value);
            VariantClear(&new_value);
        }
    }
    const bool enabled_before = has_semantic_state(before.states, SemanticState::enabled);
    const bool enabled_after = has_semantic_state(after.states, SemanticState::enabled);
    if (enabled_before != enabled_after) {
        set_bool(&old_value, enabled_before);
        set_bool(&new_value, enabled_after);
        raise_property_change(source, id, UIA_IsEnabledPropertyId, old_value, new_value);
    }
}

} // namespace

IRawElementProviderSimple* make_automation_element(const std::shared_ptr<SemanticSource>& source,
                                                   SemanticElementId id) noexcept {
    UiaElement* element = new (std::nothrow) UiaElement(source, id);
    if (element == nullptr) return nullptr;
    IRawElementProviderSimple* simple = static_cast<IRawElementProviderSimple*>(element);
    return simple;
}

void raise_automation_events(const std::shared_ptr<SemanticSource>& source,
                             const SemanticNode& previous, const SemanticNode& current) noexcept {
    try {
        std::vector<PublishedNode> before;
        std::vector<PublishedNode> after;
        collect_published(client_location(previous), before);
        collect_published(client_location(current), after);
        std::sort(before.begin(), before.end(), PublishedNodeBefore{});
        // Children changed: one event on the root says the whole tree may
        // have; clients read it again.
        IRawElementProviderSimple* root = make_automation_element(source, SemanticElementId{});
        if (root != nullptr) {
            int no_runtime_id = 0;
            UiaRaiseStructureChangedEvent(root, StructureChangeType_ChildrenInvalidated,
                                          &no_runtime_id, 0);
            (*root).Release();
        }
        SemanticElementId focus_before{};
        bool had_focus = false;
        for (const PublishedNode& entry : before) {
            if (has_semantic_state((*entry.node).states, SemanticState::focused)) {
                focus_before = entry.id;
                had_focus = true;
                break;
            }
        }
        for (const PublishedNode& entry : after) {
            const std::vector<PublishedNode>::const_iterator found = std::lower_bound(
                before.begin(), before.end(), entry, PublishedNodeBefore{});
            const bool both = found != before.end() && (*found).id == entry.id;
            if (both && entry.id != SemanticElementId{}) {
                raise_node_changes(source, entry.id, *(*found).node, *entry.node);
            }
            const bool focused = has_semantic_state((*entry.node).states, SemanticState::focused);
            if (focused && (!had_focus || focus_before != entry.id)) {
                IRawElementProviderSimple* element = make_automation_element(source, entry.id);
                if (element != nullptr) {
                    UiaRaiseAutomationEvent(element, UIA_AutomationFocusChangedEventId);
                    (*element).Release();
                }
            }
        }
    } catch (...) {
    }
}

} // namespace gui_forms::host
