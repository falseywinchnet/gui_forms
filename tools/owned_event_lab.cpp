#include "gui_forms/event.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>

namespace {

class Counter final : public gui_forms::Component {
public:
    void add(const std::uint64_t value) noexcept { total_ += value; }
    void increment() noexcept { ++total_; }
    void add_pair(const std::uint64_t first, const std::uint64_t second) noexcept {
        total_ += first + second;
    }
    [[nodiscard]] std::uint64_t total() const noexcept { return total_; }

private:
    std::uint64_t total_{};
};

struct DispatchSample final {
    std::int64_t nanoseconds{};
    std::uint64_t callbacks{};
};

[[nodiscard]] DispatchSample measure(gui_forms::Event<std::uint64_t>& event,
                                     const Counter& counter,
                                     const std::size_t iterations) {
    const std::uint64_t before = counter.total();
    const std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();
    for (std::size_t index = 0U; index < iterations; ++index) {
        event.emit(1U);
    }
    const std::chrono::steady_clock::time_point finish =
        std::chrono::steady_clock::now();
    const std::chrono::nanoseconds elapsed =
        std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start);
    const DispatchSample sample{elapsed.count(), counter.total() - before};
    return sample;
}

void print_sample(const char* kind, const std::size_t trial,
                  const std::size_t iterations, const DispatchSample sample) {
    const double per_emit = static_cast<double>(sample.nanoseconds) /
                            static_cast<double>(iterations);
    std::cout << kind << ',' << trial << ',' << iterations << ','
              << sample.nanoseconds << ',' << per_emit << ','
              << sample.callbacks << '\n';
}

} // namespace

int main() {
    constexpr std::size_t iterations = 1000000U;
    constexpr std::size_t trials = 7U;
    Counter delegate_owner{};
    gui_forms::Event<std::uint64_t> delegate_event{};
    const gui_forms::Delegate<std::uint64_t> handler =
        gui_forms::Delegate<std::uint64_t>::bind<Counter, &Counter::add>(
            delegate_owner);
    const gui_forms::SubscriptionToken token =
        delegate_event.subscribe(delegate_owner, handler);
    static_cast<void>(measure(delegate_event, delegate_owner, 10000000U));

    // The same source can measure the preceding revision's existing dispatch
    // without requiring that revision to declare the new helper.
#ifndef GUI_FORMS_EVENT_LAB_BASELINE
    Counter owned_owner{};
    gui_forms::Event<std::uint64_t> owned_event{};
    gui_forms::on(owned_event, owned_owner, &Counter::add);
    static_cast<void>(measure(owned_event, owned_owner, 10000000U));
#ifndef GUI_FORMS_EVENT_LAB_UNBOUND_ONLY
    Counter omitted_owner{};
    Counter bound_owner{};
    Counter pair_owner{};
    gui_forms::Event<std::uint64_t> omitted_event{};
    gui_forms::Event<std::uint64_t> bound_event{};
    gui_forms::Event<std::uint64_t> pair_event{};
    gui_forms::on(omitted_event, omitted_owner, &Counter::increment);
    gui_forms::on(bound_event, bound_owner, &Counter::add, std::uint64_t{1U});
    gui_forms::on(pair_event, pair_owner, &Counter::add_pair,
                  std::uint64_t{1U}, std::uint64_t{0U});
    static_cast<void>(measure(omitted_event, omitted_owner, 10000000U));
    static_cast<void>(measure(bound_event, bound_owner, 10000000U));
    static_cast<void>(measure(pair_event, pair_owner, 10000000U));
#endif
#endif

    std::cout << "component_bytes," << sizeof(gui_forms::Component) << '\n';
    std::cout << "kind,trial,iterations,total_ns,ns_per_emit,callbacks\n";
    for (std::size_t trial = 0U; trial < trials; ++trial) {
        const DispatchSample delegate_sample =
            measure(delegate_event, delegate_owner, iterations);
        print_sample("caller_owned_delegate", trial, iterations, delegate_sample);
        if (delegate_sample.callbacks != iterations) {
            return 1;
        }
#ifndef GUI_FORMS_EVENT_LAB_BASELINE
        const DispatchSample owned_sample =
            measure(owned_event, owned_owner, iterations);
        print_sample("owner_held_member", trial, iterations, owned_sample);
        if (owned_sample.callbacks != iterations) {
            return 1;
        }
#ifndef GUI_FORMS_EVENT_LAB_UNBOUND_ONLY
        const DispatchSample omitted_sample = measure(omitted_event, omitted_owner, iterations);
        const DispatchSample bound_sample = measure(bound_event, bound_owner, iterations);
        const DispatchSample pair_sample = measure(pair_event, pair_owner, iterations);
        print_sample("omitted_arguments", trial, iterations, omitted_sample);
        print_sample("one_bound_value", trial, iterations, bound_sample);
        print_sample("two_bound_values", trial, iterations, pair_sample);
        if (omitted_sample.callbacks != iterations || bound_sample.callbacks != iterations ||
            pair_sample.callbacks != iterations) return 1;
#endif
#endif
    }
    if (!token.connected()) {
        return 1;
    }
    return 0;
}
