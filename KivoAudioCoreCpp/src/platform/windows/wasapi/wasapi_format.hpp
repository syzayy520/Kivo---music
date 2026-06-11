#pragma once

#include <Windows.h>
#include <mmreg.h>

#include "kivo/core/contract/format/roles/render_format.hpp"

namespace kivo::platform::windows::wasapi::detail {

[[nodiscard]] bool map_wave_format(
    const WAVEFORMATEX* wave_format,
    core::contract::RenderFormat& render_format) noexcept;

} // namespace kivo::platform::windows::wasapi::detail
