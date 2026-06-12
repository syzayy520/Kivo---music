#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <thread>

#include "kivo/adapters/ffmpeg/session/ffmpeg_audio_decode_session.hpp"
#include "kivo/core/playback/pipeline/producer/decode_render_queue_producer.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"
#include "kivo/platform/windows/wasapi/renderer/wasapi_renderer.hpp"
#include "kivo/platform/windows/wasapi/worker/wasapi_render_worker.hpp"
#include "kivo/source/local/local_file_byte_source.hpp"

namespace {

using namespace std::chrono_literals;
using kivo::core::contract::FrameCount;

[[nodiscard]] constexpr kivo::core::contract::GenerationId generation(
    uint64_t value) noexcept {
    auto result = kivo::core::contract::GenerationId::initial();
    for (uint64_t index = 0; index < value; ++index) {
        result = kivo::core::contract::GenerationId::next(result);
    }
    return result;
}

[[nodiscard]] kivo::core::contract::RenderFormat preferred_format() noexcept {
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

[[nodiscard]] int run_playback(const std::filesystem::path& path) {
    using namespace kivo;
    if (path.empty()) {
        std::cerr << "Missing local audio file path\n";
        return 2;
    }

    const core::contract::SourceIdentity source_identity{501};
    const core::contract::SourceGeneration source_generation{generation(7)};
    const core::decode::DecodeGeneration decode_generation{generation(11)};

    platform::windows::wasapi::WasapiRenderer renderer;
    core::render::RenderOpenRequest render_request{};
    render_request.requested_format = preferred_format();
    render_request.requested_buffer_frames = 4800;
    render_request.format_policy =
        core::render::FormatAcceptancePolicy::AllowCompatible;
    render_request.generations.stream.id = source_generation.id;
    const auto render_open = renderer.open(render_request);
    if (!render_open.is_accepted()) {
        const auto diagnostics = renderer.diagnostics();
        std::cerr << "WASAPI open failed: failure="
                  << static_cast<int>(render_open.failure())
                  << " platform_code=" << diagnostics.last_platform_code << "\n";
        return 3;
    }

    auto source = source::local::LocalFileByteSource::open(
        path,
        source_identity,
        source_generation);
    if (!source) {
        std::cerr << "Local source open failed\n";
        static_cast<void>(renderer.close());
        return 4;
    }

    adapters::ffmpeg::FfmpegAudioDecodeSession decoder;
    const core::decode::DecodeOpenRequest decode_request{
        render_open.accepted_format(),
        decode_generation,
        true
    };
    const auto decode_open = decoder.open(std::move(source), decode_request);
    if (!decode_open.is_accepted()) {
        std::cerr << "Decode open failed: failure="
                  << static_cast<int>(decode_open.failure()) << "\n";
        static_cast<void>(renderer.close());
        return 5;
    }

    const auto format = render_open.accepted_format();
    const auto maximum_chunk_frames = std::max<FrameCount>(
        1,
        std::min<FrameCount>(
            render_open.capabilities().maximum_write_frames,
            format.format.sample_rate / 50u));
    const auto maximum_chunk_bytes =
        static_cast<size_t>(maximum_chunk_frames)
        * format.format.bytes_per_frame();
    auto queue = core::render::SpscAudioBlockQueue::create({
        64,
        maximum_chunk_bytes
    });
    if (!queue) {
        std::cerr << "Render queue allocation failed\n";
        static_cast<void>(decoder.close());
        static_cast<void>(renderer.close());
        return 6;
    }

    const auto renderer_generations = renderer.snapshot().generations;
    platform::windows::wasapi::WasapiRenderWorker worker(renderer, *queue);
    const auto maximum_decode_block_bytes =
        static_cast<size_t>(format.format.sample_rate)
        * format.format.bytes_per_frame();
    auto producer = core::playback::DecodeRenderQueueProducer::create(
        decoder,
        *queue,
        format,
        renderer_generations,
        decode_generation,
        {maximum_decode_block_bytes, maximum_chunk_frames, {1}});
    if (!producer) {
        std::cerr << "Decode/render producer allocation failed\n";
        static_cast<void>(decoder.close());
        static_cast<void>(renderer.close());
        return 9;
    }

    bool reached_eos = false;
    while (queue->snapshot().queued_frames < render_open.buffer_frames()
        && !reached_eos) {
        const auto production = producer->step();
        if (production.disposition
            == core::playback::DecodeRenderQueueProducerDisposition::Failed) {
            std::cerr << "Decode/render production failed: failure="
                      << static_cast<int>(production.decode_failure)
                      << " queue=" << static_cast<int>(production.queue_result)
                      << "\n";
            static_cast<void>(decoder.close());
            static_cast<void>(renderer.close());
            return 10;
        }
        if (production.disposition
            == core::playback::DecodeRenderQueueProducerDisposition::Backpressure) {
            break;
        }
        reached_eos = production.disposition
            == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream;
    }

    const auto start_result = renderer.start();
    if (!start_result.is_success()) {
        std::cerr << "WASAPI start failed\n";
        static_cast<void>(decoder.close());
        static_cast<void>(renderer.close());
        return 7;
    }
    if (!worker.start()) {
        std::cerr << "Render worker start failed\n";
        static_cast<void>(renderer.stop());
        static_cast<void>(decoder.close());
        static_cast<void>(renderer.close());
        return 8;
    }

    while (!reached_eos) {
        const auto production = producer->step();
        if (production.disposition
            == core::playback::DecodeRenderQueueProducerDisposition::Failed) {
            worker.request_stop();
            static_cast<void>(worker.join(5s));
            static_cast<void>(decoder.close());
            static_cast<void>(renderer.close());
            return 11;
        }
        if (production.disposition
            == core::playback::DecodeRenderQueueProducerDisposition::Backpressure) {
            if (worker.snapshot().state
                == platform::windows::wasapi::WasapiRenderWorkerState::Failed) {
                worker.request_stop();
                static_cast<void>(worker.join(5s));
                static_cast<void>(decoder.close());
                static_cast<void>(renderer.close());
                return 12;
            }
            std::this_thread::yield();
            continue;
        }
        reached_eos = production.disposition
            == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream;
    }

    const auto producer_snapshot = producer->snapshot();
    const auto decoded_frames = producer_snapshot.decoded_frames;
    const auto queued_frames = producer_snapshot.queued_frames;
    const auto duration_frames = decode_open.probe().duration_known
        ? decode_open.probe().duration_frames
        : decoded_frames;
    const auto duration_ms = format.format.sample_rate == 0
        ? uint64_t{0}
        : (duration_frames / format.format.sample_rate) * 1000u
            + (duration_frames % format.format.sample_rate) * 1000u
                / format.format.sample_rate;
    const auto timeout = std::chrono::milliseconds{
        static_cast<int64_t>(std::min<uint64_t>(duration_ms + 10000u, 3600000u))};
    if (!worker.join(timeout)) {
        std::cerr << "Render worker join timed out\n";
        worker.request_stop();
        static_cast<void>(worker.join(5s));
        static_cast<void>(decoder.close());
        static_cast<void>(renderer.close());
        return 13;
    }

    const auto worker_snapshot = worker.snapshot();
    if (worker_snapshot.state
            != platform::windows::wasapi::WasapiRenderWorkerState::EndOfStream
        || decoded_frames != queued_frames
        || worker_snapshot.accepted_frames != queued_frames) {
        std::cerr << "Frame/state mismatch: state="
                  << static_cast<int>(worker_snapshot.state)
                  << " decoded=" << decoded_frames
                  << " queued=" << queued_frames
                  << " accepted=" << worker_snapshot.accepted_frames
                  << " render_failure="
                  << static_cast<int>(worker_snapshot.last_failure)
                  << " platform_code=" << worker_snapshot.last_platform_code
                  << "\n";
        static_cast<void>(decoder.close());
        static_cast<void>(renderer.close());
        return 14;
    }

    const auto drain_result = renderer.drain({5s});
    if (!drain_result.is_success()) {
        std::cerr << "WASAPI drain failed: failure="
                  << static_cast<int>(drain_result.failure()) << "\n";
        static_cast<void>(decoder.close());
        static_cast<void>(renderer.close());
        return 15;
    }

    const auto diagnostics = renderer.diagnostics();
    const auto probe = decode_open.probe();
    std::cout << "FFmpeg -> SPSC -> MMCSS -> WASAPI playback complete\n"
              << "  path=" << path.string() << "\n"
              << "  codec=" << static_cast<int>(probe.codec) << "\n"
              << "  container=" << static_cast<int>(probe.container) << "\n"
              << "  native_rate=" << probe.native_format.format.sample_rate << "\n"
              << "  output_rate=" << probe.output_format.format.sample_rate << "\n"
              << "  conversion_required="
              << probe.conversion.any_conversion_needed() << "\n"
              << "  decoded_frames=" << decoded_frames << "\n"
              << "  queued_frames=" << queued_frames << "\n"
              << "  accepted_frames=" << worker_snapshot.accepted_frames << "\n"
              << "  mmcss_registered=" << worker_snapshot.mmcss_registered << "\n"
              << "  mmcss_high_priority=" << worker_snapshot.mmcss_high_priority << "\n"
              << "  wait_timeouts=" << worker_snapshot.wait_timeout_count << "\n"
              << "  underruns=" << diagnostics.underrun_count << "\n"
              << "  device_invalidations="
              << diagnostics.device_invalidation_count << "\n";

    const auto decode_close = decoder.close();
    const auto render_close = renderer.close();
    return decode_close.is_success() && render_close.is_success() ? 0 : 16;
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: kivo_ffmpeg_wasapi_playback_smoke <audio-file>\n";
        return 1;
    }
    return run_playback(argv[1]);
}
