#include "audio_worker.hpp"
#include "gui_forms/audio/audio.hpp"
#include <atomic>

namespace {
struct Decode final {
    const std::filesystem::path& path;
    std::atomic<bool> released{false};
    gui_forms::AudioClipResult result{};
};

void decode(const gui_forms::CancellationFlag& cancellation, void* const address) {
    Decode& work = *static_cast<Decode*>(address);
    // Hold this queued job so the example deterministically demonstrates a
    // cancelled load. A running decoder observes the same flag between blocks.
    work.released.wait(false, std::memory_order_acquire);
    work.result = gui_forms::AudioClip::load_ogg(work.path, cancellation);
}
} // namespace

bool cancel_ogg_load(const std::filesystem::path& path) {
    gui_forms::CancellationFlag uncancelled{};
    const gui_forms::AudioClipResult baseline = gui_forms::AudioClip::load_ogg(path, uncancelled);
    if (baseline.status != gui_forms::AudioStatus::ok || !baseline.clip) return false;
    Decode work{path};
    gui_forms::Worker worker(decode, &work);
    worker.request_cancel();
    work.released.store(true, std::memory_order_release);
    work.released.notify_one();
    worker.join();
    const bool cancelled = work.result.status == gui_forms::AudioStatus::cancelled && !work.result.clip;
    return cancelled;
}
