#pragma once

#include <cstddef>
#include <span>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"

namespace kivo::core::processing::detail {

struct GainApplicationResult {
    bool succeeded{false};
    uint64_t clipped_samples{0};
};

[[nodiscard]] GainApplicationResult apply_gain(
    std::span<std::byte> bytes,
    contract::RenderFormat format,
    contract::FrameCount frame_count,
    double gain,
    bool dither,
    uint64_t& dither_state) noexcept;

} // namespace kivo::core::processing::detail
