#include "decode_output_smoke_steps.hpp"

#include "../fixture/decode_output_smoke_values.hpp"

#include <algorithm>
#include <iostream>

#include "kivo/source/local/local_file_byte_source.hpp"

namespace kivo::hardware_test::decode_output {

int open_decode_output_smoke(DecodeOutputSmokeContext& context) {
    using core::contract::FrameCount;

    if (context.path.empty()) {
        std::cerr << "Missing local audio file path\n";
        return 2;
    }

    core::render::RenderOpenRequest render_request{};
    render_request.requested_format = preferred_format();
    render_request.requested_buffer_frames = 4800;
    render_request.format_policy =
        core::render::FormatAcceptancePolicy::AllowCompatible;
    render_request.generations.stream.id = generation(7);
    const auto render_open = context.renderer.open(render_request);
    if (!render_open.is_accepted()) {
        const auto diagnostics = context.renderer.diagnostics();
        std::cerr << "WASAPI open failed: failure="
                  << static_cast<int>(render_open.failure())
                  << " platform_code=" << diagnostics.last_platform_code << "\n";
        return 3;
    }

    const core::contract::SourceIdentity source_identity{501};
    const core::contract::SourceGeneration source_generation{generation(7)};
    const core::decode::DecodeGeneration decode_generation{generation(11)};
    auto source = source::local::LocalFileByteSource::open(
        context.path,
        source_identity,
        source_generation);
    if (!source) {
        std::cerr << "Local source open failed\n";
        return 4;
    }

    const core::decode::DecodeOpenRequest decode_request{
        render_open.accepted_format(),
        decode_generation,
        true
    };
    const auto decode_open =
        context.decoder.open(std::move(source), decode_request);
    if (!decode_open.is_accepted()) {
        std::cerr << "Decode open failed: failure="
                  << static_cast<int>(decode_open.failure()) << "\n";
        return 5;
    }

    context.format = render_open.accepted_format();
    context.probe = decode_open.probe();
    context.render_buffer_frames = render_open.buffer_frames();
    const auto maximum_chunk_frames = std::max<FrameCount>(
        1,
        std::min<FrameCount>(
            render_open.capabilities().maximum_write_frames,
            context.format.format.sample_rate / 50u));
    const auto maximum_chunk_bytes =
        static_cast<size_t>(maximum_chunk_frames)
        * context.format.format.bytes_per_frame();
    context.queue =
        core::render::SpscAudioBlockQueue::create({64, maximum_chunk_bytes});
    if (!context.queue) {
        std::cerr << "Render queue allocation failed\n";
        return 6;
    }

    context.worker =
        std::make_unique<platform::windows::wasapi::WasapiRenderWorker>(
            context.renderer,
            *context.queue);
    const auto maximum_decode_block_bytes =
        static_cast<size_t>(context.format.format.sample_rate)
        * context.format.format.bytes_per_frame();
    context.producer = core::playback::DecodeRenderQueueProducer::create(
        context.decoder,
        *context.queue,
        context.format,
        context.renderer.snapshot().generations,
        decode_generation,
        {maximum_decode_block_bytes, maximum_chunk_frames, {1}});
    if (!context.producer) {
        std::cerr << "Decode/render producer allocation failed\n";
        return 9;
    }
    return 0;
}

} // namespace kivo::hardware_test::decode_output
