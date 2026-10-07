#pragma once

#include "gui_forms/value.hpp"

namespace gui_forms::detail {

// Connection lifetime is independent of either borrowed endpoint. The model
// token is also the authority to dereference model_: publisher teardown makes
// it disconnected before the model storage goes away.
template <typename Owner, typename T>
class ScalarBinding final {
public:
    using Getter = T (Owner::*)() const noexcept;
    using Setter = void (Owner::*)(T);
    using Validator = void (Owner::*)(T) const;

    ScalarBinding(Owner& owner, Value<T>& model, Event<T>& local_changes,
                  const Getter getter, const Setter setter,
                  const Validator validator = nullptr)
        : owner_(owner), model_(model), getter_(getter), setter_(setter), validator_(validator) {
        if (!model.is_alive()) throw std::invalid_argument("cannot bind a disposed Value");
        validate(model.get());
        if (validator != nullptr) {
            const Delegate<T> validation = Delegate<T>::template bind<
                ScalarBinding, &ScalarBinding::validate>(*this);
            validation_ = model.validating().subscribe(owner, validation);
        }
        const Delegate<T> model_handler = Delegate<T>::template bind<
            ScalarBinding, &ScalarBinding::apply>(*this);
        model_changed_ = model.changed().subscribe(owner, model_handler);
        const Delegate<T> local_handler = Delegate<T>::template bind<
            ScalarBinding, &ScalarBinding::publish>(*this);
        local_changed_ = local_changes.subscribe(owner, local_handler);
    }

    void synchronize() { apply(model_.get()); }
    void validate_update(const T value) const {
        if (!model_changed_.connected()) return;
        model_.validate_candidate(value);
    }

private:
    void validate(const T value) const {
        if (validator_ != nullptr) (owner_.*validator_)(value);
    }
    void apply(const T value) {
        const T current = (owner_.*getter_)();
        if (current == value) return;
        // Last operation: a notification may destroy the binding or control.
        (owner_.*setter_)(value);
    }
    void publish(const T value) {
        if (!model_changed_.connected()) return;
        model_.set(value);
    }
    Owner& owner_;
    Value<T>& model_;
    Getter getter_;
    Setter setter_;
    Validator validator_;
    SubscriptionToken validation_{};
    SubscriptionToken model_changed_{};
    SubscriptionToken local_changed_{};
};

} // namespace gui_forms::detail
