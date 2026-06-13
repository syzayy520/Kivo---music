#include "../../fixture/decode/ffmpeg_decode_test_factory.hpp"
#include "../../fixture/harness/ffmpeg_decode_test_runner.hpp"

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

void conversion_policy_is_enforced(
    const FfmpegDecodeTestContext& context) {
    adapters::ffmpeg::FfmpegAudioDecodeSession session;
    auto source = open_source(
        context.fixture_directory / "tone_pcm_s16le.wav");
    FFMPEG_ASSERT(source != nullptr);
    const auto opened = session.open(
        std::move(source),
        open_request(false));
    FFMPEG_ASSERT(!opened.is_accepted());
    FFMPEG_ASSERT(
        opened.failure() == core::decode::DecodeFailure::ConversionRequired);
}

void int24_target_is_not_silently_reinterpreted(
    const FfmpegDecodeTestContext& context) {
    auto int24_format = target_format();
    int24_format.format.sample_format = core::contract::SampleFormat::Int24;
    int24_format.format.bits_per_sample = 24;
    int24_format.format.container_bits_per_sample = 32;
    FFMPEG_ASSERT(int24_format.is_valid());

    adapters::ffmpeg::FfmpegAudioDecodeSession session;
    auto source = open_source(
        context.fixture_directory / "tone_pcm_s16le.wav");
    FFMPEG_ASSERT(source != nullptr);
    const core::decode::DecodeOpenRequest request{
        int24_format,
        {generation(11)},
        true
    };
    const auto opened = session.open(std::move(source), request);
    FFMPEG_ASSERT(!opened.is_accepted());
    FFMPEG_ASSERT(
        opened.failure() == core::decode::DecodeFailure::ConversionRequired);
}

} // namespace

void run_ffmpeg_policy_and_corruption_tests(FfmpegDecodeTestRunner& runner) {
    runner.run("conversion_policy_is_enforced", conversion_policy_is_enforced);
    runner.run(
        "int24_target_is_not_silently_reinterpreted",
        int24_target_is_not_silently_reinterpreted);
}
