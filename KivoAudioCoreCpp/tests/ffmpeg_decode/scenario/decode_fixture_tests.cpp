#include "../fixture/ffmpeg_decode_test_factory.hpp"
#include "../fixture/ffmpeg_decode_test_runner.hpp"

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

void decode_fixture(
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

void decodes_wave_to_requested_pcm(const FfmpegDecodeTestContext& context) {
    decode_fixture(
        context,
        {"tone_pcm_s16le.wav",
         core::decode::AudioCodec::Pcm,
         core::decode::MediaContainer::Wave});
}

void decodes_flac_to_requested_pcm(const FfmpegDecodeTestContext& context) {
    decode_fixture(
        context,
        {"tone.flac",
         core::decode::AudioCodec::Flac,
         core::decode::MediaContainer::Flac});
}

void decodes_mp3_to_requested_pcm(const FfmpegDecodeTestContext& context) {
    decode_fixture(
        context,
        {"tone.mp3",
         core::decode::AudioCodec::Mp3,
         core::decode::MediaContainer::Mp3,
         44100,
         core::contract::ChannelLayout::Stereo,
         95000,
         97000,
         false});
}

void decodes_vbr_mp3_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    decode_fixture(
        context,
        {"tone_vbr.mp3",
         core::decode::AudioCodec::Mp3,
         core::decode::MediaContainer::Mp3,
         44100,
         core::contract::ChannelLayout::Stereo,
         95000,
         97000,
         false});
}

void decodes_aac_m4a_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    decode_fixture(
        context,
        {"tone_aac.m4a",
         core::decode::AudioCodec::Aac,
         core::decode::MediaContainer::Mp4,
         44100,
         core::contract::ChannelLayout::Stereo,
         94000,
         97000});
}

void decodes_mono_22050_wave_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    decode_fixture(
        context,
        {"mono_22050.wav",
         core::decode::AudioCodec::Pcm,
         core::decode::MediaContainer::Wave,
         22050,
         core::contract::ChannelLayout::Mono});
}

void decodes_stereo_96000_flac_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    decode_fixture(
        context,
        {"stereo_96000.flac",
         core::decode::AudioCodec::Flac,
         core::decode::MediaContainer::Flac,
         96000});
}

void decodes_surround_48000_wave_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    decode_fixture(
        context,
        {"surround_48000.wav",
         core::decode::AudioCodec::Pcm,
         core::decode::MediaContainer::Wave,
         48000,
         core::contract::ChannelLayout::Surround51,
         95000,
         97000,
         false});
}

void decodes_long_wave_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    decode_fixture(
        context,
        {"long_pcm_s16le.wav",
         core::decode::AudioCodec::Pcm,
         core::decode::MediaContainer::Wave,
         44100,
         core::contract::ChannelLayout::Stereo,
         1439000,
         1441000});
}

} // namespace

void run_ffmpeg_decode_fixture_tests(FfmpegDecodeTestRunner& runner) {
    runner.run("decodes_wave_to_requested_pcm", decodes_wave_to_requested_pcm);
    runner.run("decodes_flac_to_requested_pcm", decodes_flac_to_requested_pcm);
    runner.run("decodes_mp3_to_requested_pcm", decodes_mp3_to_requested_pcm);
    runner.run(
        "decodes_vbr_mp3_to_requested_pcm",
        decodes_vbr_mp3_to_requested_pcm);
    runner.run(
        "decodes_aac_m4a_to_requested_pcm",
        decodes_aac_m4a_to_requested_pcm);
    runner.run(
        "decodes_mono_22050_wave_to_requested_pcm",
        decodes_mono_22050_wave_to_requested_pcm);
    runner.run(
        "decodes_stereo_96000_flac_to_requested_pcm",
        decodes_stereo_96000_flac_to_requested_pcm);
    runner.run(
        "decodes_surround_48000_wave_to_requested_pcm",
        decodes_surround_48000_wave_to_requested_pcm);
    runner.run(
        "decodes_long_wave_to_requested_pcm",
        decodes_long_wave_to_requested_pcm);
}
