#pragma once

#include "kivo/core/contract/format/negotiation/conversion_policy.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/processing/policy/dither_policy.hpp"
#include "kivo/core/processing/policy/resample_quality.hpp"

namespace kivo::core::processing {

struct AudioConversionSnapshot {
    contract::ConversionPolicy conversion{};
    ResampleQuality resample_quality{ResampleQuality::Balanced};
    DitherPolicy dither_policy{DitherPolicy::Disabled};
    bool dither_active{false};
    contract::FrameCount latency_frames{0};
    contract::FrameCount pending_tail_frames{0};

    [[nodiscard]] constexpr bool is_passthrough() const noexcept {
        return conversion.is_passthrough() && !dither_active;
    }

    [[nodiscard]] constexpr bool operator==(
        const AudioConversionSnapshot&) const noexcept = default;
};

} // namespace kivo::core::processing
