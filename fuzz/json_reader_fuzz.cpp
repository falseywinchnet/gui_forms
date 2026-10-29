#include "../src/core/json/json_reader/json_reader.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>

#if defined(GUI_FORMS_FUZZ_STANDALONE)
#include <array>
#endif

namespace {

namespace detail = gui_forms::detail;

void write_string(std::string& out, std::string_view text) {
    constexpr std::string_view digits = "0123456789abcdef";
    out.push_back('"');
    for (const char character : text) {
        const unsigned char byte = static_cast<unsigned char>(character);
        if (character == '"' || character == '\\') {
            out.push_back('\\');
            out.push_back(character);
        } else if (byte < 0x20U) {
            out.append("\\u00");
            out.push_back(digits[byte >> 4U]);
            out.push_back(digits[byte & 0x0FU]);
        } else {
            out.push_back(character);
        }
    }
    out.push_back('"');
}

// A canonical form of a read value. Recursive, but over a value the reader
// accepted, so at most the depth limit deep.
void write_value(std::string& out, const detail::JsonValue& value) {
    switch (value.kind) {
    case detail::JsonKind::null: out.append("null"); return;
    case detail::JsonKind::boolean: out.append(value.boolean ? "true" : "false"); return;
    case detail::JsonKind::number: out.append(value.text); return;
    case detail::JsonKind::string: write_string(out, value.text); return;
    case detail::JsonKind::array:
    case detail::JsonKind::object:
    default: break;
    }
    const bool object = value.kind == detail::JsonKind::object;
    out.push_back(object ? '{' : '[');
    for (std::size_t index = 0U; index < value.items.size(); ++index) {
        if (index != 0U) out.push_back(',');
        if (object) {
            write_string(out, value.names[index]);
            out.push_back(':');
        }
        write_value(out, value.items[index]);
    }
    out.push_back(object ? '}' : ']');
}

} // namespace

// Reads the input under small limits. A refusal leaves a null root and a
// position inside the input; an accepted document has one name per object
// member, reads the same a second time, and its canonical form reads back
// to the same value.
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    detail::JsonLimits limits{};
    limits.maximum_input_bytes = 64U * 1024U;
    limits.maximum_depth = 24U;
    limits.maximum_values = 4096U;
    limits.maximum_members = 256U;
    limits.maximum_string_bytes = 4096U;
    const std::string_view input(reinterpret_cast<const char*>(data), size);
    const detail::JsonReadResult result = detail::read_json(input, limits);
    if (!result) {
        if (result.root.kind != detail::JsonKind::null || !result.root.items.empty()) std::abort();
        if (result.error != detail::JsonError::input_too_large && result.position.byte_offset > size) std::abort();
        return 0;
    }
    const detail::JsonReadResult again = detail::read_json(input, limits);
    if (!(again.root == result.root)) std::abort();
    std::string written{};
    write_value(written, result.root);
    detail::JsonLimits wide = limits;
    wide.maximum_input_bytes = written.size() + 1U;
    const detail::JsonReadResult reread = detail::read_json(written, wide);
    if (!reread || !(reread.root == result.root)) std::abort();
    return 0;
}

#if defined(GUI_FORMS_FUZZ_STANDALONE)
namespace {

// One xorshift64 step: advances `state` and returns it.
std::uint64_t next_random(std::uint64_t& state) noexcept {
    state ^= state << 13U;
    state ^= state >> 7U;
    state ^= state << 17U;
    return state;
}

} // namespace

int main() {
    const std::array<std::string_view, 3> seeds{
        "{\n  \"format\": \"gui-forms-theme/1\",\n  \"name\": \"sapphire-dusk\",\n  \"version\": \"1.0.0\",\n"
        "  \"engine\": \">=0.3\",\n  \"sheets\": [{\"path\": \"main.theme.css\", \"sha256\": "
        "\"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855\"}],\n"
        "  \"published_values\": [\"progress\"]\n}\n",
        "[1, -0.5e-7, true, false, null, \"\\u00e9\\uD83D\\uDE00\\n\\\"\", {\"a\": [[], {}]}]",
        "\"caf\xC3\xA9 \\/ \\\\\""};
    std::uint64_t state = 0x510e527fade682d1ULL;
    for (std::size_t run = 0; run < 100'000; ++run) {
        const std::uint64_t run_draw = next_random(state);
        std::string input(seeds[run % seeds.size()]);
        const std::size_t mutations = 1U + static_cast<std::size_t>(run_draw % 8U);
        // Each mutation flips one bit of one byte.
        for (std::size_t mutation = 0; mutation < mutations; ++mutation) {
            const std::uint64_t draw = next_random(state);
            if (input.empty()) continue;
            const std::size_t offset = static_cast<std::size_t>(draw % input.size());
            const char flipped_bit = static_cast<char>(1U << (draw % 8U));
            input[offset] = static_cast<char>(input[offset] ^ flipped_bit);
        }
        if (run % 5U == 0 && !input.empty()) {
            input.resize(static_cast<std::size_t>(state % input.size()));
        } else if (run % 7U == 0) {
            input.insert(input.size() / 2U, static_cast<std::size_t>(state % 64U), '[');
        }
        LLVMFuzzerTestOneInput(reinterpret_cast<const std::uint8_t*>(input.data()), input.size());
    }
    return 0;
}
#endif
