#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

namespace gui_forms::theme_language {

// The theme language's source text (stage T3, planning/stages/
// T3-theme-language/PLAN.md, phase P1): a sheet's bytes as written, and the
// positions diagnostics report. Internal to the library until stage V (T3
// decision E3).
//
// A sheet is at most maximum_sheet_bytes of UTF-8 (T3 decision E2), so every
// byte position and length fits 32 bits. Lines and columns count from 1. A
// column counts Unicode scalar values from the start of its line, not bytes:
// each byte that is not a UTF-8 continuation byte (10xxxxxx) starts one. LF,
// CR LF and CR each end one line; a form feed is whitespace and ends none.

inline constexpr std::size_t maximum_sheet_bytes = 1024U * 1024U;
// The most bytes between a string's quotes, as written (E2).
inline constexpr std::size_t maximum_string_bytes = 4U * 1024U;

// A byte position in a sheet, from its first byte (a byte order mark
// included). Distinct from a column and from a token's index.
struct ByteOffset final {
    std::uint32_t value{};

    friend constexpr bool operator==(ByteOffset left, ByteOffset right) noexcept {
        return left.value == right.value;
    }
    friend constexpr bool operator<(ByteOffset left, ByteOffset right) noexcept {
        return left.value < right.value;
    }
};

// A count of bytes.
struct ByteLength final {
    std::uint32_t value{};

    friend constexpr bool operator==(ByteLength left, ByteLength right) noexcept {
        return left.value == right.value;
    }
};

// A token's position in its sheet's token buffer.
struct TokenIndex final {
    std::uint32_t value{};

    friend constexpr bool operator==(TokenIndex left, TokenIndex right) noexcept {
        return left.value == right.value;
    }
};

// A line, counting from 1.
struct LineNumber final {
    std::uint32_t value{1};

    friend constexpr bool operator==(LineNumber left, LineNumber right) noexcept {
        return left.value == right.value;
    }
};

// A column, counting Unicode scalar values from 1.
struct ColumnNumber final {
    std::uint32_t value{1};

    friend constexpr bool operator==(ColumnNumber left, ColumnNumber right) noexcept {
        return left.value == right.value;
    }
};

struct SourcePosition final {
    ByteOffset offset{};
    LineNumber line{};
    ColumnNumber column{};

    friend constexpr bool operator==(SourcePosition left, SourcePosition right) noexcept {
        return left.offset == right.offset && left.line == right.line && left.column == right.column;
    }
};

// One sheet to read: its name for diagnostics (the sheet's name inside its
// package, or `inline:<control>` for an inline style) and its bytes. Both are
// borrowed for the call they are passed to.
struct ThemeSource final {
    std::string_view file{};
    std::string_view text{};
};

// The byte offset at which each line starts, in order; line n starts at
// starts[n - 1]. Caller-owned and reused: reset() replaces the contents and
// keeps the capacity.
class LineStartTable final {
public:
    // Replaces the table with the lines of `text`, which must be at most
    // maximum_sheet_bytes (an internal precondition: callers refuse larger
    // sheets first). An empty text has one line.
    void reset(std::string_view text);
    // Ends every line's lifetime; the capacity remains.
    void clear() noexcept;

    [[nodiscard]] std::size_t line_count() const noexcept { return starts_.size(); }

    // The line and column of `offset` in `text`, the same text the table was
    // last reset from. An offset at text.size() is the end of the last line.
    [[nodiscard]] SourcePosition position(std::string_view text, ByteOffset offset) const noexcept;

    // The line that holds `offset`, counting from 0.
    [[nodiscard]] std::size_t line_index(ByteOffset offset) const noexcept;

private:
    std::vector<ByteOffset> starts_{};
};

// Positions for offsets asked in increasing order, as a reader reports its
// diagnostics: each continues counting columns from the previous position
// when both lie on one line, so a run of positions over a sheet costs one
// pass over its bytes, however long its lines. An offset before the previous
// one is counted from its line's start. The table and text are borrowed and
// must outlive the cursor; declare the cursor after them.
class SourcePositionCursor final {
public:
    SourcePositionCursor(const LineStartTable& lines, std::string_view text) noexcept;

    [[nodiscard]] SourcePosition locate(ByteOffset offset) noexcept;

private:
    const LineStartTable& lines_;
    std::string_view text_{};
    bool started_{};
    std::size_t previous_line_{};
    SourcePosition previous_{};
};

} // namespace gui_forms::theme_language
