#pragma once

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/decode/generation/decode_generation.hpp"
#include "kivo/core/processing/policy/dither_policy.hpp"
#include "kivo/core/processing/policy/resample_quality.hpp"

namespace kivo::core::decode {

struct DecodeOpenRequest {
    contract::RenderFormat target_format{};
    DecodeGeneration decode_generation{};
    bool allow_conversion{false};
    processing::ResampleQuality resample_quality{
        processing::ResampleQuality::Balanced};
    processing::DitherPolicy conversion_dither{
        processing::DitherPolicy::Disabled};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return target_format.is_valid();
    }

    [[nodiscard]] constexpr bool operator==(
        const DecodeOpenRequest&) const noexcept = default;
};

} // namespace kivo::core::decode
