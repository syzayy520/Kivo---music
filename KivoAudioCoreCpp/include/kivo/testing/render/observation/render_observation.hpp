#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/render/failure/render_failure.hpp"
#include "kivo/core/render/generation/render_generation_set.hpp"
#include "kivo/core/render/lifecycle/render_lifecycle_state.hpp"

namespace kivo::testing::render {

inline constexpr size_t kMaximumObservationHistoryCapacity = 256;

enum class RenderObservationOperation : uint8_t {
    FaultScheduled = 0,
    Open,
    Start,
    Write,
    Consume,
    Flush,
    Drain,
    Reset,
    Stop,
    Close
};

struct RenderObservation {
    RenderObservationOperation operation{RenderObservationOperation::Open};
    std::chrono::nanoseconds timestamp{0};
    core::render::RenderLifecycleState state{core::render::RenderLifecycleState::Closed};
    core::render::RenderFailure failure{core::render::RenderFailure::None};
    core::contract::FrameCount submitted_frames{0};
    core::contract::FrameCount accepted_frames{0};
    uint64_t ordinal{0};
    core::render::RenderGenerationSet generations{};

    [[nodiscard]] constexpr bool operator==(const RenderObservation&) const noexcept = default;
};

} // namespace kivo::testing::render
