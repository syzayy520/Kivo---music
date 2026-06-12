#include "../fixture/ffmpeg_decode_test_factory.hpp"
#include "../fixture/ffmpeg_decode_test_runner.hpp"

#include <array>
#include <cstddef>
#include <fstream>

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

void corrupt_input_is_not_reported_as_eos(
    const FfmpegDecodeTestContext& context) {
    const auto path = context.fixture_directory / "corrupt.bin";
    {
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        const std::array<std::byte, 64> bytes{
            std::byte{0x4b}, std::byte{0x49}, std::byte{0x56}, std::byte{0x4f}
        };
        output.write(
            reinterpret_cast<const char*>(bytes.data()),
            static_cast<std::streamsize>(bytes.size()));
    }

    adapters::ffmpeg::FfmpegAudioDecodeSession session;
    auto source = open_source(path, 202);
    FFMPEG_ASSERT(source != nullptr);
    const auto opened = session.open(std::move(source), open_request());
    FFMPEG_ASSERT(!opened.is_accepted());
    FFMPEG_ASSERT(
        opened.failure() != core::decode::DecodeFailure::EndOfStream);
}

} // namespace

void run_ffmpeg_policy_and_corruption_tests(FfmpegDecodeTestRunner& runner) {
    runner.run("conversion_policy_is_enforced", conversion_policy_is_enforced);
    runner.run(
        "int24_target_is_not_silently_reinterpreted",
        int24_target_is_not_silently_reinterpreted);
    runner.run(
        "corrupt_input_is_not_reported_as_eos",
        corrupt_input_is_not_reported_as_eos);
}
