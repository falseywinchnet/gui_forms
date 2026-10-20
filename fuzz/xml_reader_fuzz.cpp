#include "gui_forms/xml.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string_view>

#if defined(GUI_FORMS_FUZZ_STANDALONE)
#include <array>
#include <string>
#endif

// Reads the input; whatever is accepted must write, and the written form
// must read back to the same tree and write the same bytes.
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    gui_forms::XmlLimits limits{};
    limits.maximum_input_bytes = 1U * 1024U * 1024U;
    const std::string_view input(reinterpret_cast<const char*>(data), size);
    const gui_forms::XmlReadResult result = gui_forms::read_xml(input, limits);
    if (!result) return 0;
    const gui_forms::XmlWriteResult written = gui_forms::write_xml(result.root, {}, limits);
    if (!written) std::abort();
    const gui_forms::XmlReadResult reread = gui_forms::read_xml(written.text, limits);
    if (!(reread.root == result.root)) std::abort();
    const gui_forms::XmlWriteResult rewritten = gui_forms::write_xml(reread.root, {}, limits);
    if (rewritten.text != written.text) std::abort();
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
    const std::array<std::string_view, 2> seeds{
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<dock_layout format=\"2\" application_version=\"1\">\n"
        "  <main_site>\n"
        "    <split id=\"1\" axis=\"horizontal\" ratio=\"0.25\">\n"
        "      <tool_stack id=\"2\" selected=\"0\"><panel id=\"tools.solution\"/></tool_stack>\n"
        "      <document_area id=\"3\"/>\n"
        "    </split>\n"
        "  </main_site>\n"
        "</dock_layout>\n",
        "<r a='&lt;&#65;&#x1F600;'>\n <!-- c -->\n <e>caf\xC3\xA9 &amp;</e><f/>\n</r>"};
    std::uint64_t state = 0x6a09e667f3bcc909ULL;
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
            input.insert(input.size() / 2U, static_cast<std::size_t>(state % 512U), '<');
        }
        LLVMFuzzerTestOneInput(reinterpret_cast<const std::uint8_t*>(input.data()), input.size());
    }
    return 0;
}
#endif
