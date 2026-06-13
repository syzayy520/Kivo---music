#include "decode_output_smoke_steps.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

namespace kivo::hardware_test::decode_output {

namespace {

[[nodiscard]] int produce_once(DecodeOutputSmokeContext& context) {
    const auto production = context.producer->step();
    if (production.disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Failed) {
        std::cerr << "Decode/render production failed: failure="
                  << static_cast<int>(production.decode_failure)
                  << " queue=" << static_cast<int>(production.queue_result)
                  << "\n";
        return 10;
    }
    context.reached_eos = production.disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream;
    return production.disposition
            == core::playback::DecodeRenderQueueProducerDisposition::Backpressure
        ? 1
        : 0;
}

[[nodiscard]] std::chrono::milliseconds playback_timeout(
    const DecodeOutputSmokeContext& context) {
    const auto duration_frames = context.probe.duration_known
        ? context.probe.duration_frames
        : context.producer->snapshot().decoded_frames;
    const auto rate = context.format.format.sample_rate;
    const auto duration_ms = rate == 0
        ? uint64_t{0}
        : (duration_frames / rate) * 1000u
            + (duration_frames % rate) * 1000u / rate;
    return std::chrono::milliseconds{
        static_cast<int64_t>(std::min<uint64_t>(
            duration_ms + 10000u,
            3600000u))};
}

} // namespace

int play_decode_output_smoke(DecodeOutputSmokeContext& context) {
    using namespace std::chrono_literals;

    while (context.queue->snapshot().queued_frames
               < context.render_buffer_frames
        && !context.reached_eos) {
        const auto result = produce_once(context);
        if (result == 10) {
            return result;
        }
        if (result == 1) {
            break;
        }
    }

    if (!context.renderer.start().is_success()) {
        std::cerr << "WASAPI start failed\n";
        return 7;
    }
    if (!context.worker->start()) {
        std::cerr << "Render worker start failed\n";
        static_cast<void>(context.renderer.stop());
        return 8;
    }

    while (!context.reached_eos) {
        const auto result = produce_once(context);
        if (result == 10) {
            return 11;
        }
        if (result == 1) {
            if (context.worker->snapshot().state
                == platform::windows::wasapi::WasapiRenderWorkerState::Failed) {
                return 12;
            }
            std::this_thread::yield();
        }
    }

    context.producer_snapshot = context.producer->snapshot();
    if (!context.worker->join(playback_timeout(context))) {
        std::cerr << "Render worker join timed out\n";
        context.worker->request_stop();
        static_cast<void>(context.worker->join(5s));
        return 13;
    }
    context.worker_snapshot = context.worker->snapshot();
    return 0;
}

} // namespace kivo::hardware_test::decode_output
