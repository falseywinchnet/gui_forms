#pragma once

#include "gui_forms/controls/panel/text_box/text_box.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

// A token's identity in its TokenBox: assigned when the token is added and
// never reused, so it stays valid while positions change. It is not an index.
struct TokenId final {
    std::uint64_t value{};

    friend constexpr bool operator==(TokenId left, TokenId right) noexcept {
        const bool result = left.value == right.value;
        return result;
    }
    friend constexpr bool operator!=(TokenId left, TokenId right) noexcept {
        const bool result = left.value != right.value;
        return result;
    }
};

// One accepted entry.
struct Token final {
    TokenId id{};
    std::string text{};
};

// A text field that turns each accepted entry into a token: a removable
// item with a stable identity, shown before the editor and wrapping onto
// further lines. An entry is accepted by Enter, by a delimiter character,
// or by accepting a completion of the editor's auto-complete. With no entry
// typed (nothing, or only spaces), Enter goes on to the containers and the
// window's accept button (owner decision W-6). A
// GUI.Forms-only control (Windows Forms has none).
class TokenBox final : public Control {
public:
    static constexpr bool initialize_tree_after_construction = true;
    explicit TokenBox(StableId stable_id);
    ~TokenBox() override;
    void initialize_control_tree();

    // The tokens, first to last.
    [[nodiscard]] std::span<const Token> tokens() const noexcept { return tokens_; }
    // Adds a token after the last and returns its identity. The text is
    // trimmed of ASCII spaces; text that is not valid UTF-8, or empty once
    // trimmed, throws std::invalid_argument and adds nothing.
    TokenId add_token(std::string text);
    // Removes a token; false, changing nothing, for an identity that is not
    // (or no longer) in the box.
    bool remove_token(TokenId id);
    void clear_tokens();
    [[nodiscard]] std::optional<std::size_t> token_index(TokenId id) const noexcept;
    // The token the keyboard or pointer selected, or none. A selected token
    // is removed by Backspace or Delete.
    [[nodiscard]] std::optional<TokenId> selected_token() const noexcept { return selected_; }
    // Throws std::invalid_argument for an identity not in the box.
    void select_token(std::optional<TokenId> id);
    // The characters that end an entry when typed or pasted, default ",;".
    // Only ASCII punctuation and the space are admitted; anything else
    // throws std::invalid_argument. Empty: only Enter ends an entry.
    [[nodiscard]] std::string_view delimiters() const noexcept { return delimiters_; }
    void set_delimiters(std::string delimiters);
    [[nodiscard]] FontSpec font() const noexcept { return font_; }
    void set_font(FontSpec font);
    [[nodiscard]] std::shared_ptr<TextBox> editor() const noexcept { return editor_; }

    // The editor's auto-complete (see TextBox); an accepted completion
    // becomes a token. list_items throws std::invalid_argument.
    [[nodiscard]] AutoCompleteMode auto_complete_mode() const noexcept;
    void set_auto_complete_mode(AutoCompleteMode mode);
    [[nodiscard]] AutoCompleteSource auto_complete_source() const noexcept;
    void set_auto_complete_source(AutoCompleteSource source);
    [[nodiscard]] const std::shared_ptr<AutoCompleteStringCollection>&
    auto_complete_custom_source() const noexcept;
    void set_auto_complete_custom_source(std::shared_ptr<AutoCompleteStringCollection> source);

    // Raised after a token is added, with the token.
    [[nodiscard]] Event<const Token&>& token_added() noexcept { return token_added_; }
    // Raised after a token is removed, with the token as it was.
    [[nodiscard]] Event<const Token&>& token_removed() noexcept { return token_removed_; }
    [[nodiscard]] Event<std::optional<TokenId>>& selected_token_changed() noexcept {
        return selected_token_changed_;
    }

    [[nodiscard]] Size measure(Size available) override;
    void arrange(Rect final_bounds) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_pointer(PointerEvent& event) override;
    void on_key_preview(KeyEvent& event) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] std::vector<SemanticNode> semantic_virtual_children() const override;
    bool on_semantic_action(SemanticAction action, std::string_view value) override;
    bool on_semantic_child_action(std::string_view stable_id, SemanticAction action,
                                  std::string_view value) override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    [[nodiscard]] AnatomyStateSet anatomy_states() const override;
    [[nodiscard]] std::size_t realized_item_count() const override;
    [[nodiscard]] std::string realized_item_stable_id(std::size_t item) const override;
    [[nodiscard]] AnatomyStateSet realized_item_anatomy_states(std::size_t item) const override;

    // Stage T1 anatomy, as dot members. Tokens are an item system.
    struct Parts final {
        PartGeometry region;
        PartGeometry well;
        PartGeometry field;
        PartGeometry focus;
    };
    struct States final {
        bool normal{};
        bool enabled{};
        bool disabled{};
        bool focused{};
        bool focus_visible{};
    };
    struct ItemParts final {
        PartGeometry token_face;
        PartGeometry token_label;
        PartGeometry token_close;
        PartGeometry token_focus;
    };
    struct ItemStates final {
        bool selected{};
        bool hot{};
        bool focus_visible{};
    };
    struct Item final {
        TokenId id{};
        std::size_t index{};
        std::string stable_id;
        ItemParts parts;
        ItemStates states;
    };
    [[nodiscard]] Parts parts() const;
    [[nodiscard]] States states() const;
    // The tokens laid out inside the box, first to last.
    [[nodiscard]] std::vector<Item> realized_items() const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;
    void publish_realized_item_geometry(std::size_t item,
                                        std::span<PartGeometry> geometry) const override;
    void on_detached_from_window() noexcept override;

private:
    // Where one token's parts sit, in the box's coordinates.
    struct TokenPlacement final {
        std::size_t index{};
        Rect face{};
        Rect label{};
        Rect close{};
    };
    // One layout pass: the tokens that fit the box's height, and the
    // editor's field after the last of them.
    struct Layout final {
        std::vector<TokenPlacement> tokens;
        Rect field{};
        double height{};
    };
    // The one geometry function: arrange, paint, hit testing, the published
    // parts and the semantic nodes all read it.
    [[nodiscard]] Layout layout_for(double width, double height) const;
    [[nodiscard]] Layout layout() const;
    [[nodiscard]] std::string token_stable_id(TokenId id) const;
    [[nodiscard]] AnatomyStateSet token_states(std::size_t index) const;
    [[nodiscard]] bool focus_visible() const noexcept;
    // Where a point in window coordinates falls: a token, and whether on its
    // close glyph.
    struct TokenHit final {
        std::optional<std::size_t> index{};
        bool close{};
    };
    [[nodiscard]] TokenHit hit_token(Point window_point) const;
    // Takes the editor's text, or `entry`, as a token and clears the editor.
    void accept_entry(std::string_view entry);
    // Splits text holding a delimiter into tokens, leaving the rest typed.
    void split_delimited(const std::string& text);
    void remove_at(std::size_t index);
    void set_selected(std::optional<TokenId> id);
    void editor_committed(const std::string& text);
    void editor_text_changed(const std::string& text);
    void editor_completion_accepted(const std::string& text);
    void editor_focus_changed(bool focused);

    std::shared_ptr<TextBox> editor_;
    SubscriptionToken editor_commit_;
    SubscriptionToken editor_text_;
    SubscriptionToken editor_completion_;
    SubscriptionToken editor_focus_;
    std::vector<Token> tokens_;
    std::uint64_t next_id_{1U};
    std::optional<TokenId> selected_{};
    std::optional<TokenId> hot_{};
    bool hot_close_{};
    std::optional<TokenId> pressed_close_{};
    std::string delimiters_{",;"};
    FontSpec font_{FontRole::content, 12.0, 400, false};
    bool editor_focused_{};
    bool synchronizing_{};
    Event<const Token&> token_added_;
    Event<const Token&> token_removed_;
    Event<std::optional<TokenId>> selected_token_changed_;
};

} // namespace gui_forms
