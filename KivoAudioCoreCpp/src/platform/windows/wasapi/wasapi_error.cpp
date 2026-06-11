#include "wasapi_error.hpp"

#include <audioclient.h>

namespace kivo::platform::windows::wasapi::detail {

bool is_device_invalidation(HRESULT result) noexcept {
    return result == AUDCLNT_E_DEVICE_INVALIDATED
        || result == AUDCLNT_E_RESOURCES_INVALIDATED
        || result == AUDCLNT_E_SERVICE_NOT_RUNNING;
}

core::render::RenderFailure map_wasapi_error(HRESULT result) noexcept {
    if (SUCCEEDED(result)) {
        return core::render::RenderFailure::None;
    }
    if (is_device_invalidation(result)) {
        return core::render::RenderFailure::DeviceLost;
    }
    if (result == AUDCLNT_E_UNSUPPORTED_FORMAT) {
        return core::render::RenderFailure::UnsupportedFormat;
    }
    if (result == AUDCLNT_E_BUFFER_TOO_LARGE
        || result == AUDCLNT_E_BUFFER_SIZE_ERROR
        || result == AUDCLNT_E_OUT_OF_ORDER) {
        return core::render::RenderFailure::Overrun;
    }
    if (result == E_INVALIDARG || result == E_POINTER) {
        return core::render::RenderFailure::InvalidBuffer;
    }
    return core::render::RenderFailure::BoundaryFailure;
}

} // namespace kivo::platform::windows::wasapi::detail
