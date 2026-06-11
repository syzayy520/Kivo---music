#pragma once

#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"

namespace kivo::testing::render {

enum class RenderFaultKind : uint8_t {
    None = 0,
    PartialWrite,
    ForcedUnderrun,
    DeviceLost,
    DrainTimeout
};

enum class RenderFaultTrigger : uint8_t {
    NextWrite = 0,
    WriteCount,
    FramePositionAtOrAfter,
    StreamGeneration,
    NextConsume,
    NextDrain
};

struct ScheduledRenderFault {
    RenderFaultKind kind{RenderFaultKind::None};
    RenderFaultTrigger trigger{RenderFaultTrigger::NextWrite};
    uint64_t trigger_value{0};
    core::contract::FrameCount partial_write_frames{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        if (kind == RenderFaultKind::None) {
            return false;
        }
        if (kind == RenderFaultKind::PartialWrite && partial_write_frames == 0) {
            return false;
        }
        if (kind == RenderFaultKind::ForcedUnderrun) {
            return trigger == RenderFaultTrigger::NextConsume;
        }
        if (kind == RenderFaultKind::DrainTimeout) {
            return trigger == RenderFaultTrigger::NextDrain;
        }
        return trigger != RenderFaultTrigger::NextConsume
            && trigger != RenderFaultTrigger::NextDrain;
    }

    [[nodiscard]] constexpr bool operator==(const ScheduledRenderFault&) const noexcept = default;
};

} // namespace kivo::testing::render
