#include "wasapi_tone_playback.hpp"

#include "wasapi_tone_cycle.hpp"
#include "../fixture/tone_generator.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

namespace kivo::hardware_test::wasapi {

int run_wasapi_tone(const ToneOptions& options) {
    using core::contract::FrameCount;

    platform::windows::wasapi::WasapiRenderer renderer;
    core::render::RenderOpenRequest request{};
    request.requested_format = preferred_tone_format();
    request.requested_buffer_frames = 4800;
    request.format_policy = core::render::FormatAcceptancePolicy::AllowCompatible;

    const auto open_result = renderer.open(request);
    if (!open_result.is_accepted()) {
        const auto diagnostics = renderer.diagnostics();
        std::cerr << "WASAPI open failed: failure="
                  << static_cast<int>(open_result.failure())
                  << " platform_code=" << diagnostics.last_platform_code << "\n";
        return 2;
    }

    const auto format = open_result.accepted_format();
    const auto chunk_frames = std::max<FrameCount>(
        1,
        std::min<FrameCount>(
            open_result.capabilities().maximum_write_frames,
            format.format.sample_rate / 100u));
    const auto chunk_bytes =
        static_cast<size_t>(chunk_frames) * format.format.bytes_per_frame();
    std::vector<std::byte> pcm(chunk_bytes);
    auto queue = core::render::SpscAudioBlockQueue::create({32, chunk_bytes});
    if (!queue) {
        std::cerr << "Render queue allocation failed\n";
        return 3;
    }
    platform::windows::wasapi::WasapiRenderWorker worker(renderer, *queue);

    std::cout << "WASAPI shared event-driven output\n"
              << "  sample_rate=" << format.format.sample_rate << "\n"
              << "  channels=" << static_cast<int>(format.format.channel_count()) << "\n"
              << "  sample_format="
              << static_cast<int>(format.format.sample_format) << "\n"
              << "  buffer_frames=" << open_result.buffer_frames() << "\n"
              << "  tone_hz=" << options.frequency_hz << "\n"
              << "  amplitude=" << options.amplitude << "\n"
              << "  duration_seconds=" << options.duration_seconds << "\n"
              << "  cycles=" << options.cycles << "\n";

    const auto total_frames = static_cast<FrameCount>(
        options.duration_seconds * format.format.sample_rate);
    ToneCycleTotals totals{};
    for (uint32_t cycle = 0; cycle < options.cycles; ++cycle) {
        const auto result = run_wasapi_tone_cycle(
            renderer,
            worker,
            *queue,
            pcm,
            format,
            chunk_frames,
            total_frames,
            options,
            cycle,
            totals);
        if (result != 0) {
            static_cast<void>(renderer.close());
            return result;
        }
    }

    const auto diagnostics = renderer.diagnostics();
    std::cout << "  total_submitted_frames=" << totals.submitted_frames << "\n"
              << "  actual_mode=" << static_cast<int>(diagnostics.mode) << "\n"
              << "  shared_supported=" << diagnostics.mode_capabilities.shared_mode << "\n"
              << "  exclusive_supported="
              << diagnostics.mode_capabilities.exclusive_mode << "\n"
              << "  default_device_period_ns="
              << diagnostics.default_device_period.count() << "\n"
              << "  minimum_device_period_ns="
              << diagnostics.minimum_device_period.count() << "\n"
              << "  buffer_duration_ns=" << diagnostics.buffer_duration.count() << "\n"
              << "  worker_mmcss_registered_all_cycles="
              << totals.mmcss_registered << "\n"
              << "  worker_mmcss_high_priority_all_cycles="
              << totals.mmcss_high_priority << "\n"
              << "  worker_wait_timeouts=" << totals.wait_timeouts << "\n"
              << "  underrun_count=" << diagnostics.underrun_count << "\n"
              << "  device_invalidations=" << diagnostics.device_invalidation_count << "\n"
              << "  device_generation=" << diagnostics.device_generation.value() << "\n";
    return renderer.close().is_success() ? 0 : 13;
}

} // namespace kivo::hardware_test::wasapi
