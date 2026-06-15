#pragma once

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/render/failure/render_failure.hpp"
#include "kivo/core/render/generation/render_generation_set.hpp"
#include "kivo/core/render/lifecycle/render_lifecycle_state.hpp"

namespace kivo::core::render {

struct RenderSnapshot {
    RenderLifecycleState state{RenderLifecycleState::Closed};
    contract::RenderFormat accepted_format{};
    contract::FrameCount buffer_capacity_frames{0};
    contract::FrameCount buffered_frames{0};
    contract::FrameCount total_accepted_frames{0};
    RenderGenerationSet generations{};
    RenderFailure last_failure{RenderFailure::None};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        if (buffered_frames > buffer_capacity_frames) {
            return false;
        }
        if (state == RenderLifecycleState::Closed) {
            return buffer_capacity_frames == 0
                && buffered_frames == 0
                && !accepted_format.is_valid();
        }
        if (state == RenderLifecycleState::Failed && !accepted_format.is_valid()) {
            return buffer_capacity_frames == 0
                && buffered_frames == 0
                && last_failure != RenderFailure::None;
        }
        return accepted_format.is_valid() && buffer_capacity_frames != 0;
    }

    [[nodiscard]] constexpr bool operator==(const RenderSnapshot&) const noexcept = default;
};

} // namespace kivo::core::render
