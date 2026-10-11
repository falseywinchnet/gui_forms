#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_states(const std::shared_ptr<Surface>& page,
                const std::shared_ptr<PageContext>& context) {
    class AppendDispatcherCharacter final {
    public:
        AppendDispatcherCharacter(
            std::weak_ptr<PageContext> context,
            char value) noexcept
            : context_(std::move(context)), value_(value) {}

        void operator()() const
        {
            if (const std::shared_ptr<PageContext> context = context_.lock()) {
                (*context).dispatcher_trace.push_back(value_);
            }
        }

    private:
        std::weak_ptr<PageContext> context_;
        char value_{};
    };

    class CompleteDispatcherTurnTwo final {
    public:
        explicit CompleteDispatcherTurnTwo(
            std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()() const
        {
            const std::shared_ptr<PageContext> state = context_.lock();
            if (!state) return;
            (*state).dispatcher_trace.push_back('D');
            const DispatcherSnapshot snapshot =
                (*(*(*state).dispatcher_status).attached_window()).dispatcher_snapshot();
            (*(*state).dispatcher_status).set_text(
                "Turn 2 committed · order " + (*state).dispatcher_trace +
                " · invoked " + std::to_string(snapshot.invoked) +
                " · faults " + std::to_string(snapshot.faulted));
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    class PostNestedDispatcherTurn final {
    public:
        explicit PostNestedDispatcherTurn(
            std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()() const
        {
            const std::shared_ptr<PageContext> context = context_.lock();
            if (!context) return;
            (*context).dispatcher_trace.push_back('B');
            (*context).dispatcher_operations.push_back(
                (*(*context).dispatcher_status).begin_invoke(
                    CompleteDispatcherTurnTwo(context_)));
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    class CompleteDispatcherTurnOne final {
    public:
        explicit CompleteDispatcherTurnOne(
            std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()() const
        {
            if (const std::shared_ptr<PageContext> context = context_.lock()) {
                (*context).dispatcher_trace.push_back('C');
                (*(*context).dispatcher_status).set_text(
                    "Turn 1 committed · order " + (*context).dispatcher_trace +
                    " · nested D remains posted");
            }
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    class PostDispatcherBatchClick final {
    public:
        explicit PostDispatcherBatchClick(
            std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()(ButtonBase& source) const
        {
            const std::shared_ptr<PageContext> context = context_.lock();
            if (!context) return;
            ++(*context).dispatcher_batch;
            (*context).dispatcher_trace.clear();
            (*context).dispatcher_operations.clear();
            (*(*context).dispatcher_status).set_text(
                "Click returned · batch " +
                std::to_string((*context).dispatcher_batch) + " is pending");
            (*context).dispatcher_operations.push_back(source.begin_invoke(
                AppendDispatcherCharacter(context_, 'A')));
            (*context).dispatcher_operations.push_back(source.begin_invoke(
                PostNestedDispatcherTurn(context_)));
            (*context).dispatcher_operations.push_back(source.begin_invoke(
                CompleteDispatcherTurnOne(context_)));
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    class UnexpectedCancelledDispatch final {
    public:
        explicit UnexpectedCancelledDispatch(
            std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()() const
        {
            if (const std::shared_ptr<PageContext> state = context_.lock()) {
                (*(*state).dispatcher_status).set_text(
                    "ERROR · cancelled callback executed");
            }
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    class CancelDispatcherClick final {
    public:
        explicit CancelDispatcherClick(
            std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()(ButtonBase& source) const
        {
            const std::shared_ptr<PageContext> context = context_.lock();
            if (!context) return;
            (*context).dispatcher_operations.clear();
            DispatchOperation operation = source.begin_invoke(
                UnexpectedCancelledDispatch(context_));
            const std::uint64_t sequence = operation.sequence();
            const bool cancelled = operation.cancel();
            (*context).dispatcher_operations.push_back(std::move(operation));
            (*(*context).dispatcher_status).set_text(
                "Cancelled posted operation #" + std::to_string(sequence) +
                (cancelled ? " before dispatch" : " too late"));
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    class CompleteWorkerInvocation final {
    public:
        explicit CompleteWorkerInvocation(
            std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()() const
        {
            if (const std::shared_ptr<PageContext> state = context_.lock()) {
                const DispatcherSnapshot snapshot =
                    (*(*(*state).dispatcher_status).attached_window())
                        .dispatcher_snapshot();
                (*(*state).dispatcher_status).set_text(
                    "Worker released after UI callback · marshalled " +
                    std::to_string(snapshot.marshalled_invocations));
                (*(*state).dispatcher_invoke).set_enabled(true);
            }
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    class DispatcherWorker final {
    public:
        explicit DispatcherWorker(std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()() const
        {
            const std::shared_ptr<PageContext> context = context_.lock();
            if (!context) return;
            try {
                (*(*context).dispatcher_status).invoke(
                    CompleteWorkerInvocation(context_));
            } catch (const DispatchCancelledError&) {
                // Window shutdown is the terminal owner of this worker.
            }
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    class InvokeDispatcherFromWorkerClick final {
    public:
        explicit InvokeDispatcherFromWorkerClick(
            std::weak_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<PageContext> context = context_.lock();
            if (!context || !(*(*context).dispatcher_invoke).enabled()) return;
            (*(*context).dispatcher_invoke).set_enabled(false);
            (*(*context).dispatcher_status).set_text(
                "Worker blocked · no nested pump · waiting for UI turn");
            if ((*context).dispatcher_worker.joinable()) {
                (*context).dispatcher_worker.join();
            }
            (*context).dispatcher_worker = std::thread(DispatcherWorker(context_));
        }

    private:
        std::weak_ptr<PageContext> context_;
    };

    (*page).add_at(label("gallery.states.heading", "STATES AND DIAGNOSTICS", 22.0, 700,
                       shell_blue_dark), {24.0, 18.0, 560.0, 34.0});
    (*page).add_at(label("gallery.states.lead",
                       "Focus, disabled, checked, indeterminate, visited, border, active-surface, and runtime accounting.",
                       12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});

    const std::array<std::pair<BorderStyle, std::string_view>, 4> borders{{
        {BorderStyle::none, "No border"}, {BorderStyle::line, "Line"},
        {BorderStyle::sunken, "Sunken"}, {BorderStyle::raised, "Raised"},
    }};
    for (std::size_t index = 0; index < borders.size(); ++index) {
        std::shared_ptr<gui_forms::Panel> panel = make_control<Panel>(
            StableId("gallery.states.border." + std::to_string(index)));
        (*panel).set_background(index % 2U == 0U ? paper : Color::rgba(230, 238, 245));
        (*panel).set_border_style(borders[index].first);
        (*page).add_at(panel, {24.0 + static_cast<double>(index) * 232.0,
                             98.0, 210.0, 104.0});
        std::shared_ptr<Label> caption = label("gallery.states.border.caption." + std::to_string(index),
                             std::string(borders[index].second), 12.0, 600, shell_blue_dark);
        (*caption).set_requested_bounds({16.0, 18.0, 170.0, 24.0});
        (*panel).add_child(caption);
        std::shared_ptr<Label> state = label("gallery.states.border.state." + std::to_string(index),
                           "Retained panel", 11.0, 400, muted);
        (*state).set_requested_bounds({16.0, 52.0, 170.0, 24.0});
        (*panel).add_child(state);
    }

    std::shared_ptr<LayoutGroup> lifecycle = group("gallery.states.lifecycle", "Lifecycle and interaction states",
                           {450.0, 240.0});
    (*page).add_at(lifecycle, {24.0, 226.0, 450.0, 240.0});
    std::shared_ptr<gui_forms::CheckBox> checked = make_control<CheckBox>(StableId("gallery.states.checked"), "Checked");
    (*checked).set_checked(true);
    (*lifecycle).add_at(checked, {24.0, 42.0, 180.0, 28.0});
    std::shared_ptr<gui_forms::CheckBox> indeterminate = make_control<CheckBox>(StableId("gallery.states.indeterminate"),
                                                "Indeterminate");
    (*indeterminate).set_three_state(true);
    (*indeterminate).set_check_state(CheckState::indeterminate);
    (*lifecycle).add_at(indeterminate, {24.0, 82.0, 180.0, 28.0});
    std::shared_ptr<gui_forms::Button> unavailable = make_control<Button>(StableId("gallery.states.unavailable"),
                                            "Unavailable action");
    (*unavailable).set_enabled(false);
    (*lifecycle).add_at(unavailable, {230.0, 42.0, 180.0, 34.0});
    std::shared_ptr<gui_forms::LinkLabel> visited = make_control<LinkLabel>(StableId("gallery.states.visited"),
                                           "Visited link state");
    (*visited).set_visited(true);
    (*lifecycle).add_at(visited, {230.0, 88.0, 180.0, 28.0});
    std::shared_ptr<Label> cursor_note = label("gallery.states.cursor",
                             "Cursor roles: arrow · text · hand · resize · wait · forbidden",
                             11.0, 400, muted);
    (*lifecycle).add_at(cursor_note, {24.0, 142.0, 390.0, 42.0});

    std::shared_ptr<gui_forms::MetricsView> diagnostics = make_control<DiagnosticsCard>(StableId("gallery.states.diagnostics"));
    (*diagnostics).set_title("RETAINED RUNTIME");
    BasicControlStyle diagnostics_style = page_style();
    diagnostics_style.text = Color::rgba(28, 39, 50);
    diagnostics_style.face = Color::rgba(175, 203, 221);
    diagnostics_style.face_light = Color::rgba(226, 238, 246);
    diagnostics_style.accent = green;
    (*diagnostics).set_style(diagnostics_style);
    (*page).add_at(diagnostics, {494.0, 226.0, 460.0, 240.0});
    std::shared_ptr<LayoutGroup> dispatcher = group(
        "gallery.states.dispatcher", "UI dispatcher · posted, bounded, cancellable",
        {930.0, 134.0});
    (*page).add_at(dispatcher, {24.0, 488.0, 930.0, 134.0});
    std::shared_ptr<gui_forms::Button> post = make_control<Button>(StableId("gallery.dispatcher.post"),
                                     "Post A · B · C → D");
    (*post).set_visual_style(ButtonVisualStyle::accent);
    std::shared_ptr<gui_forms::Button> cancel = make_control<Button>(StableId("gallery.dispatcher.cancel"),
                                       "Post then cancel");
    std::shared_ptr<gui_forms::Button> invoke = make_control<Button>(StableId("gallery.dispatcher.invoke"),
                                       "Worker Invoke");
    (*context).dispatcher_invoke = invoke;
    (*dispatcher).add_at(post, {22.0, 40.0, 174.0, 36.0});
    (*dispatcher).add_at(cancel, {208.0, 40.0, 154.0, 36.0});
    (*dispatcher).add_at(invoke, {374.0, 40.0, 170.0, 36.0});
    (*context).dispatcher_status = label(
        "gallery.dispatcher.status",
        "Ready · BeginInvoke never executes inside the click callback",
        11.0, 600, green);
    (*(*context).dispatcher_status).set_text_wrapping(TextWrapping::word);
    (*(*context).dispatcher_status).set_vertical_alignment(VerticalAlignment::near);
    (*dispatcher).add_at((*context).dispatcher_status, {566.0, 36.0, 328.0, 58.0});

    const std::weak_ptr<PageContext> weak_context = context;
    (*context).subscriptions.push_back((*post).clicked().subscribe(
        *post, PostDispatcherBatchClick(weak_context)));
    (*context).subscriptions.push_back((*cancel).clicked().subscribe(
        *cancel, CancelDispatcherClick(weak_context)));
    (*context).subscriptions.push_back((*invoke).clicked().subscribe(
        *invoke, InvokeDispatcherFromWorkerClick(weak_context)));
}

} // namespace gui_forms::gallery::pages
