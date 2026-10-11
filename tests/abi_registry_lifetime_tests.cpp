// C ABI teardown releases what its subscriptions retained. A native event
// connection's forwarder holds its SubscriptionRecord (and, for key preview,
// the sender's ControlRecord), while the record holds the connection, so a
// connection left in place after disconnect or disposal kept the record, its
// event slot and, for key preview, the whole form alive. LeakSanitizer found
// it on 2026-10-11; this test observes the same lifetimes without a sanitizer.
#include "../src/abi/c_api.cpp"

#include <cstdio>
#include <cstdlib>
#include <memory>

namespace gui_forms::abi::detail {

struct RegistryLifetimeFixture final {
    [[nodiscard]] static std::weak_ptr<gui_forms::Control> control(Registry& registry, const gf_handle handle) {
        std::shared_ptr<ControlRecord> record;
        if (registry.get_control(handle, record) != GF_OK || !record) {
            std::fprintf(stderr, "control handle is not live\n");
            std::exit(1);
        }
        const std::weak_ptr<gui_forms::Control> result = (*record).control;
        return result;
    }

    [[nodiscard]] static std::weak_ptr<SubscriptionRecord> subscription(Registry& registry, const gf_event_token token) {
        std::shared_ptr<SubscriptionRecord> record;
        {
            std::scoped_lock lock(registry.mutex_);
            if (registry.subscription_locked(token, record) != GF_OK || !record) {
                std::fprintf(stderr, "event token is not live\n");
                std::exit(1);
            }
        }
        const std::weak_ptr<SubscriptionRecord> result = record;
        return result;
    }
};

} // namespace gui_forms::abi::detail

namespace {

using gui_forms::abi::detail::Registry;
using gui_forms::abi::detail::RegistryLifetimeFixture;
using gui_forms::abi::detail::SubscriptionRecord;

void require(const bool condition, const char* const message) {
    if (condition) return;
    std::fprintf(stderr, "%s\n", message);
    std::exit(1);
}

std::uint32_t ignore_key(gf_handle, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, void*) {
    return 0U;
}

std::uint32_t ignore_event(gf_handle, std::uint32_t, void*) {
    return 0U;
}

enum class Teardown : std::uint8_t { dispose, disconnect_then_dispose, release, disconnect_then_release };

const char* teardown_name(const Teardown teardown) {
    switch (teardown) {
    case Teardown::dispose: return "dispose";
    case Teardown::disconnect_then_dispose: return "disconnect then dispose";
    case Teardown::release: return "release";
    case Teardown::disconnect_then_release: return "disconnect then release";
    }
    return "unknown";
}

[[nodiscard]] gf_handle create(Registry& registry, const std::uint32_t kind, const char* const stable_id) {
    gf_handle handle{};
    const gf_string_view id{stable_id, std::strlen(stable_id)};
    require(registry.create(kind, id, &handle) == GF_OK, "control created");
    return handle;
}

void tear_down(Registry& registry, const gf_handle control, const gf_event_token token, const bool subscribed,
               const Teardown teardown) {
    const bool disconnect_first = teardown == Teardown::disconnect_then_dispose ||
                                  teardown == Teardown::disconnect_then_release;
    if (subscribed && disconnect_first) require(registry.disconnect(token) == GF_OK, "token disconnects");
    if (teardown == Teardown::dispose || teardown == Teardown::disconnect_then_dispose) {
        require(registry.dispose(control) == GF_OK, "control disposes");
    } else {
        require(registry.release(control) == GF_OK, "control releases");
    }
}

// A form, alone or with a key-preview subscription, must be freed by every
// teardown, and so must the subscription's record.
void check_key_preview(const Teardown teardown, const bool subscribed) {
    Registry registry;
    const gf_handle form = create(registry, GF_CONTROL_FORM, "lifetime.form");
    const std::weak_ptr<gui_forms::Control> control = RegistryLifetimeFixture::control(registry, form);
    gf_event_token token{};
    std::weak_ptr<SubscriptionRecord> record;
    if (subscribed) {
        require(registry.subscribe_key_preview(form, ignore_key, nullptr, &token) == GF_OK, "key preview subscribes");
        record = RegistryLifetimeFixture::subscription(registry, token);
    }
    tear_down(registry, form, token, subscribed, teardown);
    std::printf("key preview %s subscribed=%d form-freed=%d record-freed=%d\n", teardown_name(teardown),
                subscribed ? 1 : 0, control.expired() ? 1 : 0, record.expired() ? 1 : 0);
    require(control.expired(), "form is freed after teardown");
    require(record.expired(), "key preview record is freed after teardown");
}

// A click subscription's forwarder holds only its record; the record must
// still be freed with its connection.
void check_click(const Teardown teardown) {
    Registry registry;
    const gf_handle button = create(registry, GF_CONTROL_BUTTON, "lifetime.button");
    const std::weak_ptr<gui_forms::Control> control = RegistryLifetimeFixture::control(registry, button);
    gf_event_token token{};
    require(registry.subscribe_v2(button, GF_EVENT_CLICKED, ignore_event, nullptr, &token) == GF_OK, "click subscribes");
    const std::weak_ptr<SubscriptionRecord> record = RegistryLifetimeFixture::subscription(registry, token);
    tear_down(registry, button, token, true, teardown);
    std::printf("click %s button-freed=%d record-freed=%d\n", teardown_name(teardown), control.expired() ? 1 : 0,
                record.expired() ? 1 : 0);
    require(control.expired(), "button is freed after teardown");
    require(record.expired(), "click record is freed after teardown");
}

} // namespace

int main() {
    const Teardown teardowns[] = {Teardown::dispose, Teardown::disconnect_then_dispose, Teardown::release,
                                  Teardown::disconnect_then_release};
    for (const Teardown teardown : teardowns) {
        check_key_preview(teardown, false);
        check_key_preview(teardown, true);
        check_click(teardown);
    }
    return 0;
}
