#pragma once

#include "gui_forms/controls/panel/masked_text_box/masked_text_provider/masked_text_provider.hpp"
#include "gui_forms/controls/panel/text_box/text_box.hpp"
#include "gui_forms/delegate.hpp"

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

// Input the mask refused: the position the character was tested at, why,
// and the refused text (one character typed, or the whole value offered).
struct MaskInputRejection final {
    MaskPosition position{};
    MaskResultHint hint{MaskResultHint::invalid_input};
    std::string text{};
};

// One run of the validation hook (Windows Forms' TypeValidationCompleted).
// The hook reads `text` (the text in text_mask_format) and writes `valid`
// and, when it refuses, a `message`; setting `cancel` keeps the focus in the
// box when validation runs because the focus is leaving.
struct MaskValidation final {
    std::string text{};
    bool mask_completed{};
    bool valid{true};
    std::string message{};
    bool cancel{};
};

// A text box that takes input through a mask (Windows Forms' MaskedTextBox).
// The mask and the assigned characters live in a MaskedTextProvider; the
// field is a child TextBox showing the provider's display text, with a prompt
// character in each unassigned position and the mask's literals in place.
// Typing, deletion, paste and assistive set-value all go through the
// provider: input that does not fit is refused and reported by
// mask_input_rejected, and the caret passes over literals. An empty mask
// takes any text, as a plain TextBox.
class MaskedTextBox final : public Panel {
public:
    static constexpr bool initialize_tree_after_construction = true;
    explicit MaskedTextBox(StableId stable_id, std::string mask = {});
    ~MaskedTextBox() override;
    void initialize_control_tree();

    [[nodiscard]] std::string_view mask() const noexcept { return provider_.mask(); }
    // Replaces the mask and keeps as much of the text as the new mask takes
    // (the assigned characters, without prompts or literals, placed again
    // from the first position); if the new mask refuses them, it starts
    // empty. Throws std::invalid_argument for an invalid mask
    // (MaskedTextProvider).
    void set_mask(std::string mask);
    [[nodiscard]] const MaskCulture& culture() const noexcept { return provider_.culture(); }
    void set_culture(MaskCulture culture);
    // The mask and the assigned characters, for reading; edits go through
    // the box so that the field and its events follow.
    [[nodiscard]] const MaskedTextProvider& provider() const noexcept { return provider_; }

    // The text in text_mask_format.
    [[nodiscard]] std::string text() const;
    // Clears the box and places `text` from the first position, literals in
    // the text matching the mask's literals. Refused text leaves the box
    // unchanged and raises mask_input_rejected.
    MaskEditResult set_text(std::string_view text);
    void clear();
    [[nodiscard]] MaskTextFormat text_mask_format() const noexcept { return text_mask_format_; }
    void set_text_mask_format(MaskTextFormat format);
    // Every required position is assigned.
    [[nodiscard]] bool mask_completed() const noexcept { return provider_.mask_completed(); }
    // Every edit position is assigned.
    [[nodiscard]] bool mask_full() const noexcept { return provider_.mask_full(); }

    [[nodiscard]] char32_t prompt_character() const noexcept { return provider_.prompt_character(); }
    void set_prompt_character(char32_t character);
    [[nodiscard]] bool allow_prompt_as_input() const noexcept { return provider_.allow_prompt_as_input(); }
    void set_allow_prompt_as_input(bool allow);
    [[nodiscard]] bool ascii_only() const noexcept { return provider_.ascii_only(); }
    void set_ascii_only(bool ascii_only);
    [[nodiscard]] bool skip_literals() const noexcept { return provider_.skip_literals(); }
    void set_skip_literals(bool skip);
    [[nodiscard]] bool reset_on_prompt() const noexcept { return provider_.reset_on_prompt(); }
    void set_reset_on_prompt(bool reset);
    [[nodiscard]] bool reset_on_space() const noexcept { return provider_.reset_on_space(); }
    void set_reset_on_space(bool reset);
    // With it off (the default), a paste keeps the characters the mask takes
    // and reports each one it drops; with it on, one refused character
    // refuses the whole paste.
    [[nodiscard]] bool reject_input_on_first_failure() const noexcept {
        return reject_input_on_first_failure_;
    }
    void set_reject_input_on_first_failure(bool reject);
    // Typing overwrites the character at the caret instead of inserting
    // before it. The Insert key toggles it.
    [[nodiscard]] bool overwrite_mode() const noexcept { return overwrite_mode_; }
    void set_overwrite_mode(bool overwrite);
    [[nodiscard]] bool read_only() const noexcept { return read_only_; }
    void set_read_only(bool read_only);

    // The validation hook, run by validate_text and when the focus leaves
    // the box. The delegate does not own its target; clear it (an empty
    // Delegate) before the target goes away. Without a hook validation
    // reports valid and raises nothing.
    void set_validator(Delegate<MaskValidation&> validator) noexcept { validator_ = validator; }
    // Runs the hook and raises type_validation_completed; returns whether
    // the text is valid.
    bool validate_text();

    [[nodiscard]] std::shared_ptr<TextBox> editor() const noexcept { return editor_; }

    // Raised for each refused input, before the edit returns.
    [[nodiscard]] Event<const MaskInputRejection&>& mask_input_rejected() noexcept {
        return mask_input_rejected_;
    }
    // Raised after the text (in text_mask_format) changes.
    [[nodiscard]] Event<const std::string&>& text_changed() noexcept { return text_changed_; }
    [[nodiscard]] Event<const std::string&>& mask_changed() noexcept { return mask_changed_; }
    [[nodiscard]] Event<bool>& overwrite_mode_changed() noexcept { return overwrite_mode_changed_; }
    [[nodiscard]] Event<const MaskValidation&>& type_validation_completed() noexcept {
        return type_validation_completed_;
    }

    void arrange(Rect final_bounds) override;
    void on_key_preview(KeyEvent& event) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    bool on_semantic_action(SemanticAction action, std::string_view value) override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    [[nodiscard]] AnatomyStateSet anatomy_states() const override;
    // Stage T1 anatomy, as dot members.
    struct Parts final {
        ScrollableControl::Parts scrollable;
        PartGeometry face;
        PartGeometry edge;
        PartGeometry field;
    };
    struct States final {
        bool normal{};
        bool pressed{};
        bool captured{};
        bool enabled{};
        bool disabled{};
        bool focused{};
        bool focus_visible{};
        bool read_only{};
        bool invalid{};
        bool pending{};
    };
    [[nodiscard]] Parts parts() const;
    [[nodiscard]] States states() const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    struct EditorChangeCallback final {
        std::weak_ptr<MaskedTextBox> target;
        void operator()(const std::string& text) const;
    };

    // The editor's edit filter: every user edit becomes a provider edit.
    void filter_edit(TextEdit& edit);
    void on_validating(ControlValidationEvent& event);
    void filter_typing(TextEdit& edit, MaskPosition start, MaskPosition end);
    void filter_deletion(TextEdit& edit, MaskPosition start, MaskPosition end);
    void filter_paste(TextEdit& edit, MaskPosition start, MaskPosition end);
    void filter_value(TextEdit& edit);
    [[nodiscard]] MaskValidation run_validation();
    void reject(MaskEditResult result, std::string_view text);
    // Writes the candidate's display text and caret into `edit` as a rewrite
    // and keeps the candidate until the editor reports the new text.
    void propose(TextEdit& edit, MaskedTextProvider candidate, MaskPosition caret);
    // Where the caret goes after input that ended at `last`: past it, and on
    // past literals to the next edit position.
    [[nodiscard]] MaskPosition caret_after(const MaskedTextProvider& candidate, MaskPosition last) const;
    // Shows the provider's display text in the editor (program changes).
    void show_provider();
    // Puts the editor's caret on the first unassigned edit position, or at
    // the end when every position is assigned.
    void place_caret();
    void editor_text_changed(const std::string& display);
    // Rebuilds the provider from the editor's display text (after undo or
    // redo, which the filter does not see).
    void adopt_display_text(std::string_view display);
    void publish_text_if_changed();
    // A provider with the given mask and culture and this box's settings.
    [[nodiscard]] MaskedTextProvider configured_provider(std::string mask, MaskCulture culture) const;
    void replace_provider(MaskedTextProvider next);

    MaskedTextProvider provider_;
    // A filter's rewrite waiting for the editor to apply it.
    std::optional<MaskedTextProvider> pending_;
    std::shared_ptr<TextBox> editor_;
    SubscriptionToken editor_change_;
    SubscriptionToken validating_;
    Delegate<MaskValidation&> validator_{};
    MaskTextFormat text_mask_format_{MaskTextFormat::include_literals};
    bool reject_input_on_first_failure_{};
    bool overwrite_mode_{};
    bool read_only_{};
    bool synchronizing_{};
    // The text in text_mask_format as last published by text_changed.
    std::string published_text_;
    Event<const MaskInputRejection&> mask_input_rejected_;
    Event<const std::string&> text_changed_;
    Event<const std::string&> mask_changed_;
    Event<bool> overwrite_mode_changed_;
    Event<const MaskValidation&> type_validation_completed_;
};

} // namespace gui_forms
