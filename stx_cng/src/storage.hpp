#pragma once

// Owner storage shared by the encoder and the decoder: growth that reports
// allocation failure instead of throwing, the static Zstandard context
// workspace, and the fork-join that runs a call's workers.
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <span>
#include <stdexcept>
#include <system_error>
#include <thread>
#include <vector>

namespace stx_cng::detail {

// Grows `storage` to at least `count` elements, reserving exactly that many
// first (resize alone may over-allocate geometrically). False when allocation
// fails, leaving it as it was; elements already present keep their values and
// new elements are value-initialized.
template<typename Element>
bool grow(std::vector<Element>& storage, std::size_t count) noexcept {
    if (storage.size() >= count) {
        return true;
    }
    try {
        storage.reserve(count);
        storage.resize(count);
    } catch (const std::bad_alloc&) {
        return false;
    } catch (const std::length_error&) {
        return false;
    }
    return true;
}

template<typename Element>
std::size_t capacity_bytes(const std::vector<Element>& storage) noexcept {
    std::size_t bytes = storage.capacity() * sizeof(Element);
    return bytes;
}

// An aligned workspace for a static Zstandard context, allocated once by its
// owner; Zstandard borrows it and never resizes or frees it.
struct ContextStorage final {
    std::unique_ptr<std::uint64_t[]> words;
    std::size_t bytes = 0;
};

inline bool reserve_context(ContextStorage& storage, std::size_t required) noexcept {
    if (storage.bytes >= required) {
        return true;
    }
    std::size_t words = required / sizeof(std::uint64_t) + 1;
    try {
        storage.words = std::make_unique_for_overwrite<std::uint64_t[]>(words);
    } catch (const std::bad_alloc&) {
        storage.bytes = 0;
        return false;
    }
    storage.bytes = words * sizeof(std::uint64_t);
    return true;
}

// Runs run(jobs[k]) for every job: job 0 on the calling thread, the others on
// threads the call starts and joins before it returns. A thread that cannot be
// started leaves its job to the calling thread, which runs it after job 0.
// Jobs must touch disjoint mutable state; `threads` holds at least as many
// entries as there are jobs and is left holding no running thread.
template<typename Job>
void run_jobs(std::span<Job> jobs, std::span<std::thread> threads, void (*run)(Job&) noexcept) noexcept {
    std::size_t count = jobs.size();
    for (std::size_t index = 1; index < count; ++index) {
        try {
            threads[index] = std::thread(run, std::ref(jobs[index]));
        } catch (const std::system_error&) {
            threads[index] = std::thread();
        } catch (const std::bad_alloc&) {
            threads[index] = std::thread();
        }
    }
    if (count != 0) {
        run(jobs[0]);
    }
    for (std::size_t index = 1; index < count; ++index) {
        if (threads[index].joinable()) {
            threads[index].join();
        } else {
            run(jobs[index]);
        }
    }
}

} // namespace stx_cng::detail
