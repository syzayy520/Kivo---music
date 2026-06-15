#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include "platform/windows/wasapi/format/wasapi_format.hpp"

namespace kivo::platform::windows::wasapi::detail {

namespace {

constexpr REFERENCE_TIME kSharedBufferDuration = 1'000'000;

} // namespace

HRESULT WasapiRendererState::initialize_shared_stream(
    const core::render::RenderOpenRequest& request,
    core::contract::RenderFormat& accepted_format) noexcept {
    WAVEFORMATEX* mix_format = nullptr;
    auto code = audio_client_->GetMixFormat(&mix_format);
    if (FAILED(code)) {
        return code;
    }

    const auto mapped = map_wave_format(mix_format, accepted_format);
    if (!mapped
        || (request.format_policy == core::render::FormatAcceptancePolicy::Exact
            && request.requested_format != accepted_format)) {
        CoTaskMemFree(mix_format);
        return AUDCLNT_E_UNSUPPORTED_FORMAT;
    }

    code = audio_client_->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
        kSharedBufferDuration,
        0,
        mix_format,
        nullptr);
    CoTaskMemFree(mix_format);
    return code;
}

} // namespace kivo::platform::windows::wasapi::detail
