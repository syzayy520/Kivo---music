#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

namespace kivo::platform::windows::wasapi::detail {

HRESULT WasapiRendererState::acquire_render_service(
    core::contract::FrameCount& buffer_frames) noexcept {
    auto code = audio_client_->SetEventHandle(event_handle_);
    if (FAILED(code)) {
        return code;
    }

    UINT32 platform_buffer_frames = 0;
    code = audio_client_->GetBufferSize(&platform_buffer_frames);
    if (FAILED(code) || platform_buffer_frames == 0) {
        return FAILED(code) ? code : E_FAIL;
    }
    code = audio_client_->GetService(
        __uuidof(IAudioRenderClient),
        reinterpret_cast<void**>(render_client_.GetAddressOf()));
    if (SUCCEEDED(code)) {
        buffer_frames = platform_buffer_frames;
    }
    return code;
}

} // namespace kivo::platform::windows::wasapi::detail
