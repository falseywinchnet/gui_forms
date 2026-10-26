#include "tables.hpp"

#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>

namespace stx_cng::detail {
namespace {

// Builds one context's table from its q bytes (at most maximum_table_entries).
void build_table(std::span<const std::uint8_t> q, NormalizeWorkspace& workspace, DecodeTable& table) noexcept {
    table.escape = static_cast<std::uint32_t>(q.size()) - 1;
    table.used = normalize(q, table.frequency, workspace);
    if (!table.used) {
        return;
    }
    std::uint32_t accumulated = 0;
    for (std::size_t entry = 0; entry < q.size(); ++entry) {
        std::uint16_t frequency = table.frequency[entry];
        table.start[entry] = static_cast<std::uint16_t>(accumulated);
        std::fill_n(table.slot.begin() + accumulated, frequency, static_cast<std::uint16_t>(entry));
        accumulated += frequency;
    }
}

} // namespace

bool prepare_reader(TablesReader& reader) noexcept {
    if (reader.context != nullptr) {
        return true;
    }
    if (!reserve_context(reader.context_storage, ZSTD_estimateDCtxSize())) {
        return false;
    }
    reader.context = ZSTD_initStaticDCtx(reader.context_storage.words.get(), reader.context_storage.bytes);
    bool prepared = reader.context != nullptr;
    return prepared;
}

bool single_frame(std::span<const std::uint8_t> frame) noexcept {
    if (frame.empty() || ZSTD_isSkippableFrame(frame.data(), frame.size()) != 0) {
        return false;
    }
    std::size_t compressed = ZSTD_findFrameCompressedSize(frame.data(), frame.size());
    bool single = ZSTD_isError(compressed) == 0 && compressed == frame.size();
    return single;
}

CngError read_tables(std::span<const std::uint8_t> frame, TablesReader& reader, DecodeTables& tables) noexcept {
    if (!single_frame(frame)) {
        return CngError::invalid_tables;
    }
    std::size_t count = ZSTD_decompressDCtx(reader.context, reader.body.data(), reader.body.size(), frame.data(), frame.size());
    if (ZSTD_isError(count) != 0) {
        return CngError::invalid_tables;
    }
    std::span<const std::uint8_t> body(reader.body.data(), count);
    std::size_t position = 0;
    for (std::uint32_t context = 0; context < contexts; ++context) {
        if (count - position < 2) {
            return CngError::invalid_tables;
        }
        std::size_t listed = (static_cast<std::size_t>(body[position]) << 8) | body[position + 1];
        position += 2;
        if (listed > green_symbols || count - position < listed + 1) {
            return CngError::invalid_tables;
        }
        std::span<const std::uint8_t> q = body.subspan(position, listed + 1);
        for (std::uint8_t value : q) {
            if (value > maximum_quantized) {
                return CngError::invalid_tables;
            }
        }
        build_table(q, reader.normalize, tables[context]);
        position += listed + 1;
    }
    if (position != count) {
        return CngError::invalid_tables;
    }
    return CngError::none;
}

} // namespace stx_cng::detail
