#include "wasapi_tone_cycle.hpp"

#include "../fixture/tone_generator.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

namespace kivo::hardware_test::wasapi {

using namespace std::chrono_literals;

int run_wasapi_tone_cycle(
    platform::windows::wasapi::WasapiRenderer& renderer,
    platform::windows::wasapi::WasapiRenderWorker& worker,
    core::render::SpscAudioBlockQueue& queue,
    std::span<std::byte> pcm,
    const core::contract::RenderFormat& format,
    core::contract::FrameCount chunk_frames,
    core::contract::FrameCount total_frames,
    const ToneOptions& options,
    uint32_t cycle_index,
    ToneCycleTotals& totals) {
    using core::contract::FrameCount;

    queue.reset();
    const auto start_result = renderer.start();
    if (!start_result.is_success()) {
        std::cerr << "WASAPI start failed at cycle " << (cycle_index + 1) << "\n";
        return 4;
    }
    const auto generations = renderer.snapshot().generations;
    if (!worker.start()) {
        std::cerr << "WASAPI render worker start failed at cycle "
                  << (cycle_index + 1) << "\n";
        static_cast<void>(renderer.stop());
        return 5;
    }

    FrameCount submitted_frames = 0;
    while (submitted_frames < total_frames) {
        const auto frames = std::min(chunk_frames, total_frames - submitted_frames);
        const auto active_bytes =
            static_cast<size_t>(frames) * format.format.bytes_per_frame();
        auto bytes = pcm.first(active_bytes);
        if (!fill_tone(
                bytes,
                format,
                totals.submitted_frames,
                frames,
                options.frequency_hz,
                options.amplitude)) {
            std::cerr << "Accepted device format is not supported by the tone generator\n";
            worker.request_stop();
            static_cast<void>(worker.join(2s));
            return 6;
        }

        const core::render::AudioBlockView block{
            std::span<const std::byte>{bytes},
            format,
            frames,
            submitted_frames,
            totals.next_buffer_id,
            {},
            generations,
            submitted_frames + frames == total_frames
        };
        while (true) {
            const auto push_result = queue.try_push(block);
            if (push_result == core::render::SpscQueuePushResult::Pushed) {
                break;
            }
            if (push_result == core::render::SpscQueuePushResult::Full) {
                if (worker.snapshot().state
                    == platform::windows::wasapi::WasapiRenderWorkerState::Failed) {
                    std::cerr << "WASAPI render worker failed while producing\n";
                    worker.request_stop();
                    static_cast<void>(worker.join(2s));
                    return 7;
                }
                std::this_thread::yield();
                continue;
            }
            std::cerr << "Render queue rejected block: "
                      << static_cast<int>(push_result) << "\n";
            worker.request_stop();
            static_cast<void>(worker.join(2s));
            return 8;
        }
        submitted_frames += frames;
        totals.submitted_frames += frames;
        totals.next_buffer_id.value += 1;
    }

    queue.close_producer();
    const auto timeout = std::chrono::milliseconds{
        static_cast<int64_t>(options.duration_seconds * 1000.0) + 5000};
    if (!worker.join(timeout)) {
        std::cerr << "WASAPI render worker join timed out at cycle "
                  << (cycle_index + 1) << "\n";
        worker.request_stop();
        static_cast<void>(worker.join(2s));
        return 9;
    }

    const auto snapshot = worker.snapshot();
    if (snapshot.state
            != platform::windows::wasapi::WasapiRenderWorkerState::EndOfStream
        || snapshot.accepted_frames != submitted_frames) {
        std::cerr << "WASAPI render worker completion mismatch: state="
                  << static_cast<int>(snapshot.state)
                  << " accepted=" << snapshot.accepted_frames
                  << " queued=" << submitted_frames
                  << " failure=" << static_cast<int>(snapshot.last_failure)
                  << " platform_code=" << snapshot.last_platform_code << "\n";
        return 10;
    }
    totals.wait_timeouts += snapshot.wait_timeout_count;
    totals.mmcss_registered =
        totals.mmcss_registered && snapshot.mmcss_registered;
    totals.mmcss_high_priority =
        totals.mmcss_high_priority && snapshot.mmcss_high_priority;

    const auto drain_result = renderer.drain({2s});
    if (!drain_result.is_success()) {
        std::cerr << "WASAPI drain failed at cycle " << (cycle_index + 1)
                  << ": " << static_cast<int>(drain_result.failure()) << "\n";
        return 11;
    }

    core::render::RenderResetRequest reset{};
    reset.generations = renderer.snapshot().generations;
    if (!renderer.reset(reset).is_success()) {
        std::cerr << "WASAPI reset failed at cycle " << (cycle_index + 1) << "\n";
        return 12;
    }
    std::cout << "  cycle_" << (cycle_index + 1)
              << "_submitted_frames=" << submitted_frames << "\n"
              << "  cycle_" << (cycle_index + 1)
              << "_worker_accepted_frames=" << snapshot.accepted_frames << "\n"
              << "  cycle_" << (cycle_index + 1)
              << "_mmcss_registered=" << snapshot.mmcss_registered << "\n"
              << "  cycle_" << (cycle_index + 1)
              << "_mmcss_high_priority=" << snapshot.mmcss_high_priority << "\n";
    return 0;
}

} // namespace kivo::hardware_test::wasapi
