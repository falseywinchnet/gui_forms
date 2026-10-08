#include "gui_forms/gui_forms.hpp"
#include <atomic>
#include <iostream>
#include <stdexcept>

namespace {
void require(const bool condition, const char* const message) {
    if (!condition) throw std::runtime_error(message);
}

struct CountWake final {
    std::atomic<unsigned int>& count;
    void operator()() const { count.fetch_add(1, std::memory_order_relaxed); }
};

void publish(gui_forms::LiveSurface& surface) {
    gui_forms::LiveSurfaceWriteLease write = surface.try_acquire_write();
    require(static_cast<bool>(write), "write unavailable");
    require(write.publish() != 0, "publication rejected");
}

struct Publication final {
    gui_forms::LiveSurface& surface;
};

void publish_worker(const gui_forms::CancellationFlag&, void* const address) {
    Publication& publication = *static_cast<Publication*>(address);
    publish(publication.surface);
}
} // namespace

int main() {
    try {
        std::atomic<unsigned int> wakes{0};
        const std::shared_ptr<gui_forms::Control> root =
            gui_forms::make_control<gui_forms::Control>(gui_forms::StableId("live"));
        const std::shared_ptr<gui_forms::LiveSurface> surface =
            gui_forms::LiveSurface::create({.width = 8, .height = 8});
        gui_forms::Window window(root, {8, 8});
        window.perform_layout();
        require(window.queue_live_surface_presentation(root, surface), "registration failed");
        window.set_live_surface_idle_wake_handler(CountWake{wakes});
        publish(*surface);
        require(wakes.load() == 0, "active publication posted UI work");
        // The producer won the race between empty drain and idle arming.
        window.set_live_surface_idle_waiting(true);
        require(wakes.load() == 1, "arm lost an already published generation");
        publish(*surface);
        require(wakes.load() == 1, "wake was not coalesced");
        static_cast<void>(window.take_live_surface_presentations());
        window.set_live_surface_idle_waiting(true);
        require(wakes.load() == 1, "unchanged generation woke idle host");
        publish(*surface);
        require(wakes.load() == 2, "idle publication did not wake host");
        window.set_live_surface_idle_waiting(false);
        publish(*surface);
        require(wakes.load() == 2, "disarmed host received producer wake");
        // Race a real pthread producer against arming. Either side must observe
        // the transition, and their common atomic gate must release it once.
        Publication publication{*surface};
        for (unsigned int index = 0; index < 100; ++index) {
            static_cast<void>(window.take_live_surface_presentations());
            gui_forms::Worker worker(publish_worker, &publication);
            window.set_live_surface_idle_waiting(true);
            worker.join();
            require(wakes.load() == index + 3, "racing publication lost or duplicated wake");
        }
        (*root).set_visible(false);
        publish(*surface);
        static_cast<void>(window.take_live_surface_presentations());
        window.set_live_surface_idle_waiting(true);
        require(wakes.load() == 102, "hidden generation continuously rearmed idle");
        window.set_live_surface_idle_waiting(false);
        window.set_live_surface_idle_wake_handler({});
        window.set_live_surface_idle_waiting(true);
        publish(*surface);
        require(wakes.load() == 102, "removed handler received a wake");
        std::cout << "Idle arming, coalescing, removal and publication races passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
