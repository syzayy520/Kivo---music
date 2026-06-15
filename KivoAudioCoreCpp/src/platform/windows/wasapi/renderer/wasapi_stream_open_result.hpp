#pragma once

#include <Windows.h>
#include <audioclient.h>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/platform/windows/wasapi/diagnostics/wasapi_diagnostics.hpp"

namespace kivo::platform::windows::wasapi::detail {

struct WasapiStreamOpenResult {
    HRESULT code{E_FAIL};
    core::contract::RenderFormat accepted_format{};
    core::contract::FrameCount buffer_frames{0};
    WasapiOutputMode mode{WasapiOutputMode::Closed};

    [[nodiscard]] bool succeeded() const noexcept {
        return SUCCEEDED(code)
            && accepted_format.is_valid()
            && buffer_frames != 0
            && mode != WasapiOutputMode::Closed;
    }
};

} // namespace kivo::platform::windows::wasapi::detail
