#include "fixture_decode_assertion.hpp"

#include "../harness/ffmpeg_decode_test_runner.hpp"

namespace ffmpeg_decode_test {

using namespace kivo;

void assert_fixture_decodes(
    const FfmpegDecodeTestContext& context,
    const FixtureExpectation& expectation) {
    adapters::ffmpeg::FfmpegAudioDecodeSession session;
    auto source = open_source(context.fixture_directory / expectation.file_name);
    FFMPEG_ASSERT(source != nullptr);

    const auto opened = session.open(std::move(source), open_request());
    require_opened(opened);
    const auto& probe = opened.probe();
    FFMPEG_ASSERT(probe.codec == expectation.codec);
    FFMPEG_ASSERT(probe.container == expectation.container);
    FFMPEG_ASSERT(
        probe.native_format.format.sample_rate
        == expectation.native_sample_rate);
    FFMPEG_ASSERT(
        probe.native_format.format.channel_layout
        == expectation.native_channel_layout);
    FFMPEG_ASSERT(probe.output_format == target_format());
    FFMPEG_ASSERT(probe.source_identity.value == 101);
    FFMPEG_ASSERT(probe.source_generation.value() == 7);
    FFMPEG_ASSERT(probe.decode_generation.value() == 11);
    FFMPEG_ASSERT(probe.duration_known);
    FFMPEG_ASSERT(
        probe.duration_frames >= expectation.minimum_output_frames);
    FFMPEG_ASSERT(
        probe.duration_frames <= expectation.maximum_output_frames);
    FFMPEG_ASSERT(probe.seekable);

    core::contract::FrameCount decoded_frames = 0;
    core::contract::SamplePosition expected_offset = 0;
    bool saw_terminal_block = false;
    size_t block_count = 0;
    for (;;) {
        const auto step = session.decode_next();
        if (step.disposition()
            == core::decode::DecodeStepDisposition::EndOfStream) {
            break;
        }
        FFMPEG_ASSERT(
            step.disposition()
            == core::decode::DecodeStepDisposition::Produced);
        const auto& block = step.block();
        FFMPEG_ASSERT(block.is_valid());
        FFMPEG_ASSERT(block.format == target_format());
        FFMPEG_ASSERT(block.source_generation.value() == 7);
        FFMPEG_ASSERT(block.decode_generation.value() == 11);
        FFMPEG_ASSERT(block.frame_offset == expected_offset);
        FFMPEG_ASSERT(
            block.bytes.size()
            == static_cast<size_t>(block.frame_count)
                * target_format().format.bytes_per_frame());
        expected_offset += block.frame_count;
        decoded_frames += block.frame_count;
        saw_terminal_block = saw_terminal_block || block.end_of_stream;
        ++block_count;
        FFMPEG_ASSERT(block_count < 10000);
    }
    FFMPEG_ASSERT(block_count > 0);
    FFMPEG_ASSERT(decoded_frames >= expectation.minimum_output_frames);
    FFMPEG_ASSERT(decoded_frames <= expectation.maximum_output_frames);
    FFMPEG_ASSERT(
        saw_terminal_block
        || !expectation.terminal_block_required);
    FFMPEG_ASSERT(session.close().is_success());
}

} // namespace ffmpeg_decode_test
