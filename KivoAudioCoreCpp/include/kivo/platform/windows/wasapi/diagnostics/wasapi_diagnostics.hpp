#pragma once

#include <chrono>
#include <cstdint>

#include "kivo/core/contract/generation/identity/device_generation.hpp"

namespace kivo::platform::windows::wasapi {

enum class WasapiOutputMode : uint8_t {
    Closed = 0,
    Shared,
    Exclusive
};

struct WasapiModeCapabilities {
    bool shared_mode{true};
    bool exclusive_mode{false};
    bool event_driven_shared_mode{true};

    [[nodiscard]] constexpr bool operator==(
        const WasapiModeCapabilities&) const noexcept = default;
};

struct WasapiDiagnostics {
    WasapiOutputMode mode{WasapiOutputMode::Closed};
    WasapiModeCapabilities mode_capabilities{};
    bool event_driven{false};
    bool reopen_required{false};
    std::chrono::nanoseconds default_device_period{0};
    std::chrono::nanoseconds minimum_device_period{0};
    std::chrono::nanoseconds buffer_duration{0};
    uint64_t underrun_count{0};
    uint64_t device_invalidation_count{0};
    uint64_t power_transition_count{0};
    int64_t last_platform_code{0};
    core::contract::DeviceGeneration device_generation{};

    [[nodiscard]] constexpr bool operator==(const WasapiDiagnostics&) const noexcept = default;
};

} // namespace kivo::platform::windows::wasapi
