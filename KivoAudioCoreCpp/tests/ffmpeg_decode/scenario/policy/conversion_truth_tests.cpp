#include "../../fixture/decode/ffmpeg_decode_test_factory.hpp"
#include "../../fixture/harness/ffmpeg_decode_test_runner.hpp"

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

void resample_quality_and_latency_are_reported(
    const FfmpegDecodeTestContext& context) {
    adapters::ffmpeg::FfmpegAudioDecodeSession session;
    auto source = open_source(
        context.fixture_directory / "mono_22050.wav");
    FFMPEG_ASSERT(source != nullptr);
    auto request = open_request();
    request.resample_quality = core::processing::ResampleQuality::High;
    const auto opened = session.open(std::move(source), request);
    require_opened(opened);

    const auto& probe = opened.probe();
    FFMPEG_ASSERT(probe.conversion.sample_rate_convert);
    FFMPEG_ASSERT(probe.conversion.channel_remix);
    FFMPEG_ASSERT(
        probe.conversion_snapshot.resample_quality
        == core::processing::ResampleQuality::High);
    FFMPEG_ASSERT(
        probe.conversion_snapshot.conversion == probe.conversion);
    FFMPEG_ASSERT(probe.conversion_snapshot.latency_frames != 0);
    FFMPEG_ASSERT(
        probe.conversion_snapshot.pending_tail_frames
        <= probe.conversion_snapshot.latency_frames);
}

void triangular_dither_requires_precision_reduction(
    const FfmpegDecodeTestContext& context) {
    auto int16_format = target_format();
    int16_format.format.sample_format =
        core::contract::SampleFormat::Int16;
    int16_format.format.bits_per_sample = 16;
    int16_format.format.container_bits_per_sample = 16;
    FFMPEG_ASSERT(int16_format.is_valid());

    adapters::ffmpeg::FfmpegAudioDecodeSession session;
    auto source = open_source(
        context.fixture_directory / "tone.mp3");
    FFMPEG_ASSERT(source != nullptr);
    auto request = open_request();
    request.target_format = int16_format;
    request.conversion_dither =
        core::processing::DitherPolicy::TriangularPdf;
    const auto opened = session.open(std::move(source), request);
    require_opened(opened);
    FFMPEG_ASSERT(opened.probe().conversion_snapshot.dither_active);
}

} // namespace

void run_ffmpeg_conversion_truth_tests(FfmpegDecodeTestRunner& runner) {
    runner.run(
        "resample_quality_and_latency_are_reported",
        resample_quality_and_latency_are_reported);
    runner.run(
        "triangular_dither_requires_precision_reduction",
        triangular_dither_requires_precision_reduction);
}
