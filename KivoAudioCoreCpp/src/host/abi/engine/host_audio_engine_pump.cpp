#include "host/abi/engine/host_audio_engine.hpp"

namespace kivo::host::abi {

kivo_audio_result HostAudioEngine::pump(
    const kivo_audio_pump_request_v1& request,
    kivo_audio_pump_result_v1& result) noexcept {
    std::unique_lock lock{command_mutex_, std::try_to_lock};
    if (!lock.owns_lock()) {
        return KIVO_AUDIO_RESULT_BUSY;
    }
    result = {
        sizeof(kivo_audio_pump_result_v1),
        KIVO_AUDIO_ABI_STRUCT_VERSION_1
    };
    if (request.maximum_steps == 0u) {
        last_result_ = KIVO_AUDIO_RESULT_INVALID_ARGUMENT;
        return last_result_;
    }

    for (uint32_t index = 0; index < request.maximum_steps; ++index) {
        const auto production = runtime_.produce_step();
        const auto rendering = runtime_.render_step();
        ++result.steps_executed;
        result.produced_frames += production.queued_frames;
        result.rendered_frames += rendering.accepted_frames;
        if (production.disposition
                == core::playback::DecodeRenderQueueProducerDisposition::Queued
            || production.disposition
                == core::playback::DecodeRenderQueueProducerDisposition::Primed
            || rendering.made_progress()) {
            result.made_progress = 1u;
        }
        if (production.disposition
            == core::playback::DecodeRenderQueueProducerDisposition::Backpressure) {
            result.backpressured = 1u;
        }
        if (rendering.disposition
            == core::render::RenderPumpDisposition::EndOfStream) {
            result.reached_end_of_stream = 1u;
        }
        if (production.disposition
            == core::playback::DecodeRenderQueueProducerDisposition::Failed) {
            last_result_ = KIVO_AUDIO_RESULT_DECODE_FAILURE;
            return last_result_;
        }
        if (rendering.disposition
            == core::render::RenderPumpDisposition::Failed) {
            last_result_ = KIVO_AUDIO_RESULT_RENDER_FAILURE;
            return last_result_;
        }
        if (result.reached_end_of_stream != 0u
            || (production.disposition
                    == core::playback::DecodeRenderQueueProducerDisposition::Backpressure
                && !rendering.made_progress())) {
            break;
        }
    }
    last_result_ = KIVO_AUDIO_RESULT_OK;
    return last_result_;
}

} // namespace kivo::host::abi
