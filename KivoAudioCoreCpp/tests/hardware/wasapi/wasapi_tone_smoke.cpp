#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <numbers>
#include <span>
#include <string_view>
#include <thread>
#include <vector>

#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"
#include "kivo/platform/windows/wasapi/renderer/wasapi_renderer.hpp"
#include "kivo/platform/windows/wasapi/worker/wasapi_render_worker.hpp"

namespace {

using namespace std::chrono_literals;
using kivo::core::contract::FrameCount;
using kivo::core::contract::RenderFormat;
using kivo::core::contract::SampleFormat;

struct Options {
    double duration_seconds{3.0};
    double frequency_hz{440.0};
    double amplitude{0.18};
    uint32_t cycles{1};
};

[[nodiscard]] Options parse_options(int argc, char** argv) {
    Options options{};
    for (int index = 1; index + 1 < argc; index += 2) {
        const std::string_view name{argv[index]};
        if (name == "--duration") {
            options.duration_seconds = std::strtod(argv[index + 1], nullptr);
        } else if (name == "--frequency") {
            options.frequency_hz = std::strtod(argv[index + 1], nullptr);
        } else if (name == "--amplitude") {
            options.amplitude = std::strtod(argv[index + 1], nullptr);
        } else if (name == "--cycles") {
            options.cycles = static_cast<uint32_t>(
                std::strtoul(argv[index + 1], nullptr, 10));
        }
    }
    options.duration_seconds = std::clamp(options.duration_seconds, 0.1, 3600.0);
    options.frequency_hz = std::clamp(options.frequency_hz, 20.0, 20000.0);
    options.amplitude = std::clamp(options.amplitude, 0.0, 0.5);
    options.cycles = std::clamp<uint32_t>(options.cycles, 1, 20);
    return options;
}

[[nodiscard]] RenderFormat preferred_format() {
    using namespace kivo::core::contract;
    return {{
        SampleFormat::Float32,
        ChannelLayout::Stereo,
        FrameLayout::Interleaved,
        {},
        48000,
        32,
        32
    }};
}

template <typename Sample>
void store_sample(std::byte* destination, Sample sample) noexcept {
    std::memcpy(destination, &sample, sizeof(Sample));
}

[[nodiscard]] bool fill_tone(
    std::span<std::byte> bytes,
    const RenderFormat& format,
    FrameCount first_frame,
    FrameCount frame_count,
    double frequency_hz,
    double amplitude) noexcept {
    const auto channels = format.format.channel_count();
    const auto bytes_per_sample = format.format.container_bits() / 8u;
    const auto bytes_per_frame = format.format.bytes_per_frame();
    if (channels == 0
        || bytes_per_sample == 0
        || bytes.size() != static_cast<size_t>(frame_count) * bytes_per_frame) {
        return false;
    }

    const auto sample_rate = static_cast<double>(format.format.sample_rate);
    for (FrameCount frame = 0; frame < frame_count; ++frame) {
        const auto phase = 2.0 * std::numbers::pi
            * frequency_hz
            * static_cast<double>(first_frame + frame)
            / sample_rate;
        const auto value = amplitude * std::sin(phase);
        for (uint8_t channel = 0; channel < channels; ++channel) {
            auto* destination = bytes.data()
                + static_cast<size_t>(frame) * bytes_per_frame
                + static_cast<size_t>(channel) * bytes_per_sample;
            switch (format.format.sample_format) {
            case SampleFormat::Float32:
                store_sample(destination, static_cast<float>(value));
                break;
            case SampleFormat::Float64:
                store_sample(destination, value);
                break;
            case SampleFormat::Int16:
                store_sample(destination, static_cast<int16_t>(value * 32767.0));
                break;
            case SampleFormat::Int24: {
                const auto sample =
                    static_cast<int32_t>(value * 8388607.0) * 256;
                store_sample(destination, sample);
                break;
            }
            case SampleFormat::Int32:
                store_sample(destination, static_cast<int32_t>(value * 2147483647.0));
                break;
            default:
                return false;
            }
        }
    }
    return true;
}

[[nodiscard]] int run_tone(const Options& options) {
    using namespace kivo;
    platform::windows::wasapi::WasapiRenderer renderer;

    core::render::RenderOpenRequest request{};
    request.requested_format = preferred_format();
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
    const auto channels = format.format.channel_count();
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
        static_cast<void>(renderer.close());
        return 3;
    }
    platform::windows::wasapi::WasapiRenderWorker worker(renderer, *queue);

    std::cout << "WASAPI shared event-driven output\n"
              << "  sample_rate=" << format.format.sample_rate << "\n"
              << "  channels=" << static_cast<int>(channels) << "\n"
              << "  sample_format=" << static_cast<int>(format.format.sample_format) << "\n"
              << "  buffer_frames=" << open_result.buffer_frames() << "\n"
              << "  tone_hz=" << options.frequency_hz << "\n"
              << "  amplitude=" << options.amplitude << "\n"
              << "  duration_seconds=" << options.duration_seconds << "\n"
              << "  cycles=" << options.cycles << "\n";

    const auto total_frames = static_cast<FrameCount>(
        options.duration_seconds * format.format.sample_rate);
    FrameCount total_submitted_frames = 0;
    uint64_t total_wait_timeouts = 0;
    bool all_cycles_mmcss_registered = true;
    bool all_cycles_mmcss_high_priority = true;
    core::contract::BufferId buffer_id{1};

    for (uint32_t cycle = 0; cycle < options.cycles; ++cycle) {
        queue->reset();
        const auto start_result = renderer.start();
        if (!start_result.is_success()) {
            std::cerr << "WASAPI start failed at cycle " << (cycle + 1) << "\n";
            static_cast<void>(renderer.close());
            return 4;
        }
        const auto generations = renderer.snapshot().generations;
        if (!worker.start()) {
            std::cerr << "WASAPI render worker start failed at cycle "
                      << (cycle + 1) << "\n";
            static_cast<void>(renderer.stop());
            static_cast<void>(renderer.close());
            return 5;
        }

        FrameCount submitted_frames = 0;
        while (submitted_frames < total_frames) {
            const auto frames = std::min(chunk_frames, total_frames - submitted_frames);
            const auto active_bytes = static_cast<size_t>(frames)
                * format.format.bytes_per_frame();
            auto bytes = std::span<std::byte>{pcm}.first(active_bytes);
            if (!fill_tone(
                    bytes,
                    format,
                    total_submitted_frames,
                    frames,
                    options.frequency_hz,
                    options.amplitude)) {
                std::cerr << "Accepted device format is not supported by the tone generator\n";
                worker.request_stop();
                static_cast<void>(worker.join(2s));
                static_cast<void>(renderer.close());
                return 6;
            }

            const core::render::AudioBlockView block{
                std::span<const std::byte>{bytes},
                format,
                frames,
                submitted_frames,
                buffer_id,
                {},
                generations,
                submitted_frames + frames == total_frames
            };

            while (true) {
                const auto push_result = queue->try_push(block);
                if (push_result == core::render::SpscQueuePushResult::Pushed) {
                    break;
                }
                if (push_result == core::render::SpscQueuePushResult::Full) {
                    if (worker.snapshot().state
                        == platform::windows::wasapi::WasapiRenderWorkerState::Failed) {
                        std::cerr << "WASAPI render worker failed while producing\n";
                        worker.request_stop();
                        static_cast<void>(worker.join(2s));
                        static_cast<void>(renderer.close());
                        return 7;
                    }
                    std::this_thread::yield();
                    continue;
                }
                std::cerr << "Render queue rejected block: "
                          << static_cast<int>(push_result) << "\n";
                worker.request_stop();
                static_cast<void>(worker.join(2s));
                static_cast<void>(renderer.close());
                return 8;
            }
            submitted_frames += frames;
            total_submitted_frames += frames;
            buffer_id.value += 1;
        }

        queue->close_producer();
        const auto worker_timeout = std::chrono::milliseconds{
            static_cast<int64_t>(options.duration_seconds * 1000.0) + 5000};
        if (!worker.join(worker_timeout)) {
            std::cerr << "WASAPI render worker join timed out at cycle "
                      << (cycle + 1) << "\n";
            worker.request_stop();
            static_cast<void>(worker.join(2s));
            static_cast<void>(renderer.close());
            return 9;
        }
        const auto worker_snapshot = worker.snapshot();
        if (worker_snapshot.state
                != platform::windows::wasapi::WasapiRenderWorkerState::EndOfStream
            || worker_snapshot.accepted_frames != submitted_frames) {
            std::cerr << "WASAPI render worker completion mismatch: state="
                      << static_cast<int>(worker_snapshot.state)
                      << " accepted=" << worker_snapshot.accepted_frames
                      << " queued=" << submitted_frames
                      << " failure=" << static_cast<int>(worker_snapshot.last_failure)
                      << " platform_code=" << worker_snapshot.last_platform_code
                      << "\n";
            static_cast<void>(renderer.close());
            return 10;
        }
        total_wait_timeouts += worker_snapshot.wait_timeout_count;
        all_cycles_mmcss_registered =
            all_cycles_mmcss_registered && worker_snapshot.mmcss_registered;
        all_cycles_mmcss_high_priority =
            all_cycles_mmcss_high_priority && worker_snapshot.mmcss_high_priority;

        const auto drain_result = renderer.drain({2s});
        if (!drain_result.is_success()) {
            std::cerr << "WASAPI drain failed at cycle " << (cycle + 1)
                      << ": " << static_cast<int>(drain_result.failure()) << "\n";
            static_cast<void>(renderer.close());
            return 11;
        }

        core::render::RenderResetRequest reset{};
        reset.generations = renderer.snapshot().generations;
        const auto reset_result = renderer.reset(reset);
        if (!reset_result.is_success()) {
            std::cerr << "WASAPI reset failed at cycle " << (cycle + 1) << "\n";
            static_cast<void>(renderer.close());
            return 12;
        }
        std::cout << "  cycle_" << (cycle + 1)
                  << "_submitted_frames=" << submitted_frames << "\n"
                  << "  cycle_" << (cycle + 1)
                  << "_worker_accepted_frames=" << worker_snapshot.accepted_frames << "\n"
                  << "  cycle_" << (cycle + 1)
                  << "_mmcss_registered=" << worker_snapshot.mmcss_registered << "\n"
                  << "  cycle_" << (cycle + 1)
                  << "_mmcss_high_priority=" << worker_snapshot.mmcss_high_priority << "\n";
    }

    const auto diagnostics = renderer.diagnostics();
    std::cout << "  total_submitted_frames=" << total_submitted_frames << "\n"
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
              << all_cycles_mmcss_registered << "\n"
              << "  worker_mmcss_high_priority_all_cycles="
              << all_cycles_mmcss_high_priority << "\n"
              << "  worker_wait_timeouts=" << total_wait_timeouts << "\n"
              << "  underrun_count=" << diagnostics.underrun_count << "\n"
              << "  device_invalidations=" << diagnostics.device_invalidation_count << "\n"
              << "  device_generation=" << diagnostics.device_generation.value() << "\n";

    const auto close_result = renderer.close();
    return close_result.is_success() ? 0 : 13;
}

} // namespace

int main(int argc, char** argv) {
    return run_tone(parse_options(argc, argv));
}
