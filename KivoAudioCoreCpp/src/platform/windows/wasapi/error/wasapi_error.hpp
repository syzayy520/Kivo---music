#pragma once

#include <Windows.h>

#include "kivo/core/render/failure/render_failure.hpp"

namespace kivo::platform::windows::wasapi::detail {

[[nodiscard]] core::render::RenderFailure map_wasapi_error(HRESULT result) noexcept;
[[nodiscard]] bool is_device_invalidation(HRESULT result) noexcept;

} // namespace kivo::platform::windows::wasapi::detail
