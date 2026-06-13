#include "decode_output_smoke_steps.hpp"

#include <chrono>
#include <iostream>

namespace kivo::hardware_test::decode_output {

int complete_decode_output_smoke(DecodeOutputSmokeContext& context) {
    using namespace std::chrono_literals;

    const auto decoded_frames = context.producer_snapshot.decoded_frames;
    const auto queued_frames = context.producer_snapshot.queued_frames;
    if (context.worker_snapshot.state
            != platform::windows::wasapi::WasapiRenderWorkerState::EndOfStream
        || decoded_frames != queued_frames
        || context.worker_snapshot.accepted_frames != queued_frames) {
        std::cerr << "Frame/state mismatch: state="
                  << static_cast<int>(context.worker_snapshot.state)
                  << " decoded=" << decoded_frames
                  << " queued=" << queued_frames
                  << " accepted=" << context.worker_snapshot.accepted_frames
                  << " render_failure="
                  << static_cast<int>(context.worker_snapshot.last_failure)
                  << " platform_code="
                  << context.worker_snapshot.last_platform_code << "\n";
        return 14;
    }

    const auto drain_result = context.renderer.drain({5s});
    if (!drain_result.is_success()) {
        std::cerr << "WASAPI drain failed: failure="
                  << static_cast<int>(drain_result.failure()) << "\n";
        return 15;
    }

    const auto diagnostics = context.renderer.diagnostics();
    std::cout << "FFmpeg -> SPSC -> MMCSS -> WASAPI playback complete\n"
              << "  path=" << context.path.string() << "\n"
              << "  codec=" << static_cast<int>(context.probe.codec) << "\n"
              << "  container=" << static_cast<int>(context.probe.container) << "\n"
              << "  native_rate="
              << context.probe.native_format.format.sample_rate << "\n"
              << "  output_rate=" << context.probe.output_format.format.sample_rate << "\n"
              << "  conversion_required="
              << context.probe.conversion.any_conversion_needed() << "\n"
              << "  decoded_frames=" << decoded_frames << "\n"
              << "  queued_frames=" << queued_frames << "\n"
              << "  accepted_frames=" << context.worker_snapshot.accepted_frames << "\n"
              << "  mmcss_registered=" << context.worker_snapshot.mmcss_registered << "\n"
              << "  mmcss_high_priority="
              << context.worker_snapshot.mmcss_high_priority << "\n"
              << "  wait_timeouts=" << context.worker_snapshot.wait_timeout_count << "\n"
              << "  underruns=" << diagnostics.underrun_count << "\n"
              << "  device_invalidations="
              << diagnostics.device_invalidation_count << "\n";

    const auto decode_close = context.decoder.close();
    const auto render_close = context.renderer.close();
    return decode_close.is_success() && render_close.is_success() ? 0 : 16;
}

} // namespace kivo::hardware_test::decode_output
