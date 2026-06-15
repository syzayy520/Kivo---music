#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include <algorithm>
#include <limits>

#include "platform/windows/wasapi/format/wasapi_format.hpp"

namespace kivo::platform::windows::wasapi::detail {

namespace {

[[nodiscard]] REFERENCE_TIME duration_for_frames(
    core::contract::FrameCount frames,
    uint32_t sample_rate) noexcept {
    constexpr uint64_t reference_units_per_second = 10'000'000ull;
    const auto numerator =
        static_cast<uint64_t>(frames) * reference_units_per_second;
    const auto duration = numerator / sample_rate
        + (numerator % sample_rate == 0 ? 0 : 1);
    return duration > static_cast<uint64_t>(
        std::numeric_limits<REFERENCE_TIME>::max())
        ? std::numeric_limits<REFERENCE_TIME>::max()
        : static_cast<REFERENCE_TIME>(duration);
}

} // namespace

HRESULT WasapiRendererState::initialize_exclusive_stream(
    const core::render::RenderOpenRequest& request,
    REFERENCE_TIME minimum_device_period,
    core::contract::RenderFormat& accepted_format) noexcept {
    WAVEFORMATEXTENSIBLE wave_format{};
    if (!make_wave_format(request.requested_format, wave_format)) {
        return AUDCLNT_E_UNSUPPORTED_FORMAT;
    }

    auto code = audio_client_->IsFormatSupported(
        AUDCLNT_SHAREMODE_EXCLUSIVE,
        &wave_format.Format,
        nullptr);
    if (code != S_OK) {
        return code == S_FALSE ? AUDCLNT_E_UNSUPPORTED_FORMAT : code;
    }

    auto buffer_duration = std::max(
        minimum_device_period,
        duration_for_frames(
            request.requested_buffer_frames,
            request.requested_format.format.sample_rate));
    code = audio_client_->Initialize(
        AUDCLNT_SHAREMODE_EXCLUSIVE,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        buffer_duration,
        buffer_duration,
        &wave_format.Format,
        nullptr);
    if (code == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED) {
        UINT32 aligned_frames = 0;
        code = audio_client_->GetBufferSize(&aligned_frames);
        if (FAILED(code) || aligned_frames == 0) {
            return FAILED(code) ? code : E_FAIL;
        }

        audio_client_.Reset();
        code = device_->Activate(
            __uuidof(IAudioClient),
            CLSCTX_INPROC_SERVER,
            nullptr,
            reinterpret_cast<void**>(audio_client_.GetAddressOf()));
        if (FAILED(code)) {
            return code;
        }
        buffer_duration = duration_for_frames(
            aligned_frames,
            request.requested_format.format.sample_rate);
        code = audio_client_->Initialize(
            AUDCLNT_SHAREMODE_EXCLUSIVE,
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
            buffer_duration,
            buffer_duration,
            &wave_format.Format,
            nullptr);
    }
    if (SUCCEEDED(code)) {
        accepted_format = request.requested_format;
    }
    return code;
}

} // namespace kivo::platform::windows::wasapi::detail
