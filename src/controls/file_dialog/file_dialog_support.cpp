#include "file_dialog_support.hpp"

#include "gui_forms/components/file_dialog_types/file_dialog_types.hpp"

#include <atomic>
#include <string>
#include <utility>

namespace gui_forms::detail {
namespace {

// The last identity take_dialog_request_id gave; zero before the first.
std::atomic<std::uint64_t> last_dialog_request_id{0U};

// Characters an extension may not hold: wildcards, path separators, and the
// characters Windows forbids in a file name.
bool is_extension_character(char character) noexcept {
    switch (character) {
    case '*':
    case '?':
    case '/':
    case '\\':
    case ':':
    case '<':
    case '>':
    case '"':
    case '|':
        return false;
    default:
        return true;
    }
}

} // namespace

std::uint64_t take_dialog_request_id() noexcept {
    const std::uint64_t previous = last_dialog_request_id.fetch_add(1U, std::memory_order_relaxed);
    const std::uint64_t result = previous + 1U;
    return result;
}

bool window_alive(const std::weak_ptr<WindowLifetime>& lifetime) {
    const std::shared_ptr<WindowLifetime> locked = lifetime.lock();
    const bool result = locked != nullptr && (*locked).window != nullptr;
    return result;
}

bool file_dialog_extension_valid(std::string_view extension) noexcept {
    if (extension.empty()) return false;
    for (const char character : extension) {
        if (!is_extension_character(character)) return false;
    }
    return true;
}

bool file_dialog_has_extension(std::string_view path) {
    const std::string name = file_dialog_name_part(path);
    const std::size_t dot = name.rfind('.');
    const bool result = dot != std::string::npos && dot + 1U < name.size();
    return result;
}

ShowingScope::ShowingScope(bool& showing, std::weak_ptr<void> presence) noexcept
    : showing_(showing), presence_(std::move(presence)) {
    showing_ = true;
}

ShowingScope::~ShowingScope() {
    if (!presence_.expired()) showing_ = false;
}

} // namespace gui_forms::detail
