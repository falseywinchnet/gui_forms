#include "gui_forms/threading.hpp"

#include <atomic>
#include <iostream>
#include <thread>

namespace {
struct Work final {
    std::atomic<bool> entered{false};
    int result{0};
};

void perform(const gui_forms::CancellationFlag& cancellation, void* const address) {
    Work& work = *static_cast<Work*>(address);
    work.entered.store(true, std::memory_order_release);
    // A real producer checks between bounded jobs; this small example yields
    // until its owner cancels, then publishes its completion through join.
    while (!cancellation.requested()) std::this_thread::yield();
    work.result = 42;
}
} // namespace

int main() {
    Work work{};
    gui_forms::Worker worker(perform, &work);
    while (!work.entered.load(std::memory_order_acquire)) std::this_thread::yield();
    worker.request_cancel();
    worker.join();
    if (work.result != 42) return 1;
    std::cout << "Cooperative worker joined before its context was destroyed\n";
    return 0;
}
