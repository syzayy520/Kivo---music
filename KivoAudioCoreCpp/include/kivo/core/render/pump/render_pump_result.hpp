#pragma once

#include <chrono>
#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/render/failure/render_failure.hpp"

namespace kivo::core::render {

enum class RenderPumpDisposition : uint8_t {
    Idle = 0,
    QueueDrained,
    Progress,
    EndOfStream,
    Delayed,
    DiscardedStale,
    Rejected,
    Failed
};

struct RenderPumpResult {
    RenderPumpDisposition disposition{RenderPumpDisposition::Idle};
    RenderFailure failure{RenderFailure::None};
    contract::FrameCount submitted_frames{0};
    contract::FrameCount accepted_frames{0};
    std::chrono::nanoseconds retry_after{0};

    [[nodiscard]] constexpr bool made_progress() const noexcept {
        return disposition == RenderPumpDisposition::Progress
            || disposition == RenderPumpDisposition::EndOfStream
            || disposition == RenderPumpDisposition::DiscardedStale;
    }

    [[nodiscard]] constexpr bool operator==(
        const RenderPumpResult&) const noexcept = default;
};

} // namespace kivo::core::render
