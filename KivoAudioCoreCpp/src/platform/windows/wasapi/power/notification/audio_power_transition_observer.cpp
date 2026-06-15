#include "platform/windows/wasapi/power/notification/audio_power_transition_observer.hpp"

namespace kivo::platform::windows::wasapi::detail {

AudioPowerTransitionObserver::~AudioPowerTransitionObserver() {
    unregister_callback();
}

DWORD AudioPowerTransitionObserver::register_callback() noexcept {
    if (registration_ != nullptr) {
        return ERROR_ALREADY_EXISTS;
    }
    DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS parameters{
        &AudioPowerTransitionObserver::callback,
        this
    };
    return PowerRegisterSuspendResumeNotification(
        DEVICE_NOTIFY_CALLBACK,
        reinterpret_cast<HANDLE>(&parameters),
        &registration_);
}

void AudioPowerTransitionObserver::unregister_callback() noexcept {
    if (registration_ == nullptr) {
        return;
    }
    static_cast<void>(
        PowerUnregisterSuspendResumeNotification(registration_));
    registration_ = nullptr;
    transition_pending_.store(false, std::memory_order_release);
}

bool AudioPowerTransitionObserver::consume_transition() noexcept {
    return transition_pending_.exchange(false, std::memory_order_acq_rel);
}

uint64_t AudioPowerTransitionObserver::transition_count() const noexcept {
    return transition_count_.load(std::memory_order_acquire);
}

bool AudioPowerTransitionObserver::is_registered() const noexcept {
    return registration_ != nullptr;
}

ULONG AudioPowerTransitionObserver::on_notification(ULONG type) noexcept {
    switch (type) {
    case PBT_APMSUSPEND:
    case PBT_APMRESUMECRITICAL:
    case PBT_APMRESUMESUSPEND:
    case PBT_APMRESUMESTANDBY:
    case PBT_APMRESUMEAUTOMATIC:
        publish_transition();
        break;
    default:
        break;
    }
    return ERROR_SUCCESS;
}

ULONG CALLBACK AudioPowerTransitionObserver::callback(
    void* context,
    ULONG type,
    void*) noexcept {
    return context == nullptr
        ? ERROR_INVALID_PARAMETER
        : static_cast<AudioPowerTransitionObserver*>(context)
            ->on_notification(type);
}

void AudioPowerTransitionObserver::publish_transition() noexcept {
    transition_count_.fetch_add(1, std::memory_order_relaxed);
    transition_pending_.store(true, std::memory_order_release);
}

} // namespace kivo::platform::windows::wasapi::detail
