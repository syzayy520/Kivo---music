#pragma once

#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/render/failure/render_failure.hpp"
#include "kivo/platform/windows/wasapi/wasapi_wait_result.hpp"

namespace kivo::platform::windows::wasapi {

enum class WasapiRenderWorkerState : uint8_t {
    Idle = 0,
    Running,
    Stopping,
    Stopped,
    EndOfStream,
    QueueDrained,
    Failed
};

struct WasapiRenderWorkerSnapshot {
    WasapiRenderWorkerState state{WasapiRenderWorkerState::Idle};
    core::render::RenderFailure last_failure{core::render::RenderFailure::None};
    WasapiWaitResult last_wait_result{WasapiWaitResult::Closed};
    bool mmcss_registered{false};
    bool mmcss_high_priority{false};
    core::contract::FrameCount accepted_frames{0};
    uint64_t discarded_stale_blocks{0};
    uint64_t wait_timeout_count{0};
    uint32_t last_platform_code{0};

    [[nodiscard]] constexpr bool terminal_success() const noexcept {
        return state == WasapiRenderWorkerState::Stopped
            || state == WasapiRenderWorkerState::EndOfStream
            || state == WasapiRenderWorkerState::QueueDrained;
    }

    [[nodiscard]] constexpr bool operator==(
        const WasapiRenderWorkerSnapshot&) const noexcept = default;
};

} // namespace kivo::platform::windows::wasapi
