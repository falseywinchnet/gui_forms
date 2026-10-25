#include "gui_forms/components/color_dialog/color_dialog.hpp"

#include "../file_dialog/file_dialog_support.hpp"
#include "gui_forms/components/file_dialog_types/file_dialog_types.hpp"
#include "gui_forms/window.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <variant>

namespace gui_forms {
namespace {

// The host contract's colors are 0xRRGGBBAA.
std::uint32_t packed_rgba(Color color) noexcept {
    const std::uint32_t red = static_cast<std::uint32_t>(color.red) << 24U;
    const std::uint32_t green = static_cast<std::uint32_t>(color.green) << 16U;
    const std::uint32_t blue = static_cast<std::uint32_t>(color.blue) << 8U;
    const std::uint32_t result = red | green | blue | static_cast<std::uint32_t>(color.alpha);
    return result;
}

Color unpacked_rgba(std::uint32_t rgba) noexcept {
    const std::uint8_t red = static_cast<std::uint8_t>(rgba >> 24U);
    const std::uint8_t green = static_cast<std::uint8_t>(rgba >> 16U);
    const std::uint8_t blue = static_cast<std::uint8_t>(rgba >> 8U);
    const std::uint8_t alpha = static_cast<std::uint8_t>(rgba);
    const Color result = Color::rgba(red, green, blue, alpha);
    return result;
}

} // namespace

ColorDialog::ColorDialog() = default;

ColorDialog::~ColorDialog() = default;

void ColorDialog::set_title(std::string title) {
    if (!file_dialog_text_valid(title)) {
        throw std::invalid_argument("ColorDialog title must be valid UTF-8 without NUL");
    }
    title_ = std::move(title);
}

void ColorDialog::reset() {
    title_.clear();
    color_ = Color::rgba(0U, 0U, 0U, 255U);
    allow_alpha_ = false;
    status_ = {};
}

DialogResult ColorDialog::show_dialog(Window& owner) {
    if (!is_alive()) throw std::logic_error("GUI.Forms cannot show a disposed ColorDialog");
    owner.verify_access("ColorDialog::show_dialog");
    if (showing_) throw std::logic_error("GUI.Forms cannot show a ColorDialog that is already showing");
    const std::weak_ptr<Presence> presence = presence_;
    const detail::ShowingScope showing(showing_, presence);
    HostServices* const services = owner.host_services();
    if (services == nullptr) {
        status_ = {HostServiceError::unsupported};
        return DialogResult::none;
    }
    HostColorDialogRequest payload;
    payload.title = title_;
    payload.initial_rgba = packed_rgba(color_);
    payload.allow_alpha = allow_alpha_;
    HostDialogRequest host_request;
    host_request.request_id = detail::take_dialog_request_id();
    const Control::Ptr root = owner.root();
    if (root) host_request.owner_id.assign((*root).stable_id().value());
    host_request.payload = std::move(payload);
    const HostDialogResult answer = (*services).show_dialog(host_request);
    if (presence.expired()) return DialogResult::cancel;
    status_ = answer.status;
    if (!answer.status.accepted()) return DialogResult::none;
    const HostColorDialogResult* const chosen = std::get_if<HostColorDialogResult>(&answer.payload);
    if (chosen == nullptr || (*chosen).outcome != HostDialogOutcome::accepted) return DialogResult::cancel;
    if (!is_alive()) return DialogResult::cancel;
    Color taken = unpacked_rgba((*chosen).rgba);
    if (!allow_alpha_) taken.alpha = 255U;
    color_ = taken;
    return DialogResult::ok;
}

} // namespace gui_forms
