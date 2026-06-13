#pragma once

#include <cstddef>
#include <span>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"

namespace kivo::hardware_test::wasapi {

[[nodiscard]] core::contract::RenderFormat preferred_tone_format() noexcept;

[[nodiscard]] bool fill_tone(
    std::span<std::byte> bytes,
    const core::contract::RenderFormat& format,
    core::contract::FrameCount first_frame,
    core::contract::FrameCount frame_count,
    double frequency_hz,
    double amplitude) noexcept;

} // namespace kivo::hardware_test::wasapi
