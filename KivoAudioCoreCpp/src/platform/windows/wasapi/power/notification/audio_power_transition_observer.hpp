#pragma once

#include <Windows.h>
#include <powrprof.h>

#include <atomic>
#include <cstdint>

namespace kivo::platform::windows::wasapi::detail {

class AudioPowerTransitionObserver final {
public:
    ~AudioPowerTransitionObserver();

    AudioPowerTransitionObserver() noexcept = default;
    AudioPowerTransitionObserver(const AudioPowerTransitionObserver&) = delete;
    AudioPowerTransitionObserver& operator=(
        const AudioPowerTransitionObserver&) = delete;

    [[nodiscard]] DWORD register_callback() noexcept;
    void unregister_callback() noexcept;
    [[nodiscard]] bool consume_transition() noexcept;
    [[nodiscard]] uint64_t transition_count() const noexcept;
    [[nodiscard]] bool is_registered() const noexcept;
    [[nodiscard]] ULONG on_notification(ULONG type) noexcept;

private:
    static ULONG CALLBACK callback(
        void* context,
        ULONG type,
        void* setting) noexcept;
    void publish_transition() noexcept;

    HPOWERNOTIFY registration_{nullptr};
    std::atomic<bool> transition_pending_{false};
    std::atomic<uint64_t> transition_count_{0};
};

} // namespace kivo::platform::windows::wasapi::detail
