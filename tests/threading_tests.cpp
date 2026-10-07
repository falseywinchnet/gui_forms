#include "gui_forms/threading.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

namespace {
struct Context final {
    std::atomic<bool> entered{false};
    int completed{0}; // Read by owner only after join.
};

void require(const bool condition, const char* const message) {
    if (!condition) throw std::runtime_error(message);
}

void cancellable(const gui_forms::CancellationFlag& cancellation, void* const address) {
    Context& context = *static_cast<Context*>(address);
    context.entered.store(true, std::memory_order_release);
    while (!cancellation.requested()) std::this_thread::yield();
    context.completed = 42;
}

void finish(const gui_forms::CancellationFlag&, void* const address) {
    Context& context = *static_cast<Context*>(address);
    context.completed = 7;
}

void fail(const gui_forms::CancellationFlag&, void*) {
    throw std::runtime_error("entry failure");
}

void increment(void* const address) noexcept {
    unsigned int& value = *static_cast<unsigned int*>(address);
    ++value;
}

void pool_batches() {
    gui_forms::AtomicThreadPool pool(4);
    require(pool.thread_count() == 4, "pool lost worker count");
    std::vector<unsigned int> values(10000, 0);
    std::vector<gui_forms::AtomicTask> tasks{};
    tasks.reserve(values.size());
    for (std::size_t index = 0; index < values.size(); ++index)
        tasks.push_back({increment, &values[index]});
    for (unsigned int round = 1; round <= 100; ++round) {
        pool.run(tasks);
        for (const unsigned int value : values)
            require(value == round, "atomic pool lost or duplicated a task");
    }
    pool.run({});
    pool.run(std::span<const gui_forms::AtomicTask>(tasks.data(), 3));
    require(values[0] == 101 && values[3] == 100, "small batch changed extent");
    tasks[1].entry = nullptr;
    bool rejected = false;
    try { pool.run(tasks); } catch (const std::invalid_argument&) { rejected = true; }
    require(rejected && values[0] == 101, "invalid batch partially executed");
}

void wait_entered(const Context& context) {
    const std::chrono::steady_clock::time_point deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!context.entered.load(std::memory_order_acquire)) {
        require(std::chrono::steady_clock::now() < deadline, "worker did not enter");
        std::this_thread::yield();
    }
}
} // namespace

int main() {
    try {
        pool_batches();
        gui_forms::CancellationFlag flag{};
        require(!flag.requested(), "fresh flag cancelled");
        flag.request();
        flag.request();
        require(flag.requested(), "request was lost");
        Context context{};
        {
            gui_forms::Worker worker(cancellable, &context);
            wait_entered(context);
            worker.request_cancel();
            worker.join();
            worker.join();
            require(context.completed == 42, "join did not publish result");
        }
        context.completed = 0;
        context.entered.store(false, std::memory_order_relaxed);
        {
            gui_forms::Worker worker(cancellable, &context);
            wait_entered(context);
        }
        require(context.completed == 42, "destructor did not cancel and join");
        {
            gui_forms::Worker worker(finish, &context);
            worker.join();
            require(context.completed == 7, "normal completion failed");
        }
        bool caught = false;
        {
            gui_forms::Worker worker(fail, nullptr);
            try { worker.join(); } catch (const std::runtime_error&) { caught = true; }
            worker.join();
        }
        require(caught, "entry exception lost");
        { gui_forms::Worker worker(fail, nullptr); } // Safe cleanup without join.
        caught = false;
        try { gui_forms::Worker worker(nullptr, nullptr); }
        catch (const std::invalid_argument&) { caught = true; }
        require(caught, "null entry accepted");
        std::cout << "Cancellation, completion, cleanup and failure propagation passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
