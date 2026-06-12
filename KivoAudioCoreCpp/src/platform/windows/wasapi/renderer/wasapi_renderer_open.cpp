#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include "platform/windows/wasapi/renderer/wasapi_stream_open_result.hpp"

namespace kivo::platform::windows::wasapi::detail {

core::render::RenderOpenResult WasapiRendererState::open(
    const core::render::RenderOpenRequest& request) noexcept {
    using core::render::RenderFailure;
    using core::render::RenderLifecycleState;
    using core::render::RenderOutputMode;

    if (snapshot_.state != RenderLifecycleState::Closed) {
        return core::render::RenderOpenResult::Rejected(
            RenderFailure::InvalidState);
    }
    if (!request.is_valid()) {
        return core::render::RenderOpenResult::Rejected(
            RenderFailure::InvalidFormat);
    }
    if (request.output_mode == RenderOutputMode::Exclusive
        && request.format_policy != core::render::FormatAcceptancePolicy::Exact) {
        return core::render::RenderOpenResult::Rejected(
            RenderFailure::UnsupportedFormat);
    }

    uint64_t endpoint_identity = 0;
    REFERENCE_TIME default_device_period = 0;
    REFERENCE_TIME minimum_device_period = 0;
    auto code = prepare_open_environment(
        endpoint_identity,
        default_device_period,
        minimum_device_period);
    if (FAILED(code)) {
        return fail_open(code);
    }

    WasapiStreamOpenResult stream{};
    stream.mode = request.output_mode == RenderOutputMode::Exclusive
        ? WasapiOutputMode::Exclusive
        : WasapiOutputMode::Shared;
    code = request.output_mode == RenderOutputMode::Exclusive
        ? initialize_exclusive_stream(
            request,
            minimum_device_period,
            stream.accepted_format)
        : initialize_shared_stream(request, stream.accepted_format);
    if (code == AUDCLNT_E_UNSUPPORTED_FORMAT) {
        diagnostics_.last_platform_code = code;
        close_stream();
        return core::render::RenderOpenResult::Rejected(
            RenderFailure::UnsupportedFormat);
    }
    if (FAILED(code)) {
        return fail_open(code);
    }

    code = acquire_render_service(stream.buffer_frames);
    if (FAILED(code)) {
        return fail_open(code);
    }
    stream.code = S_OK;
    return complete_open(
        request,
        stream,
        endpoint_identity,
        default_device_period,
        minimum_device_period);
}

} // namespace kivo::platform::windows::wasapi::detail
