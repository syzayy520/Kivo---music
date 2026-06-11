#include "ffmpeg_decode_tests_main.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string_view>

#include "kivo/adapters/ffmpeg/ffmpeg_audio_decode_session.hpp"
#include "kivo/source/local/local_file_byte_source.hpp"

namespace {

using namespace kivo;

[[nodiscard]] constexpr core::contract::RenderFormat target_format() noexcept {
    using namespace core::contract;
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

struct FixtureExpectation {
    const char* file_name;
    core::decode::AudioCodec codec;
    core::decode::MediaContainer container;
};

[[nodiscard]] constexpr core::contract::GenerationId generation(
    uint64_t value) noexcept {
    auto result = core::contract::GenerationId::initial();
    for (uint64_t index = 0; index < value; ++index) {
        result = core::contract::GenerationId::next(result);
    }
    return result;
}

[[nodiscard]] std::unique_ptr<core::decode::ByteSourceBoundary> open_source(
    const std::filesystem::path& path,
    uint64_t identity = 101) {
    return source::local::LocalFileByteSource::open(
        path,
        {identity},
        {generation(7)});
}

[[nodiscard]] core::decode::DecodeOpenRequest open_request(
    bool allow_conversion = true) noexcept {
    return {
        target_format(),
        {generation(11)},
        allow_conversion
    };
}

void require_opened(const core::decode::DecodeOpenResult& result) {
    if (!result.is_accepted()) {
        std::ostringstream message;
        message << "open failed: disposition="
                << static_cast<int>(result.disposition())
                << " failure=" << static_cast<int>(result.failure());
        throw std::runtime_error(message.str());
    }
}

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
    FFMPEG_ASSERT(probe.output_format == target_format());
    FFMPEG_ASSERT(probe.source_identity.value == 101);
    FFMPEG_ASSERT(probe.source_generation.value() == 7);
    FFMPEG_ASSERT(probe.decode_generation.value() == 11);
    FFMPEG_ASSERT(probe.duration_known);
    FFMPEG_ASSERT(probe.duration_frames >= 95000);
    FFMPEG_ASSERT(probe.duration_frames <= 97000);
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
    FFMPEG_ASSERT(decoded_frames >= 95000);
    FFMPEG_ASSERT(decoded_frames <= 97000);
    FFMPEG_ASSERT(
        saw_terminal_block
        || expectation.codec != core::decode::AudioCodec::Mp3);
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
         core::decode::MediaContainer::Mp3});
}

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

void seek_and_flush_advance_generation(
    const FfmpegDecodeTestContext& context) {
    for (const auto* name : {
            "tone_pcm_s16le.wav",
            "tone.flac",
            "tone.mp3"}) {
        adapters::ffmpeg::FfmpegAudioDecodeSession session;
        auto source = open_source(context.fixture_directory / name);
        FFMPEG_ASSERT(source != nullptr);
        require_opened(session.open(std::move(source), open_request()));

        const auto seek = session.seek(48000, {generation(12)});
        FFMPEG_ASSERT(seek.is_success());
        FFMPEG_ASSERT(session.probe().decode_generation.value() == 12);
        const auto first = session.decode_next();
        FFMPEG_ASSERT(
            first.disposition()
            == core::decode::DecodeStepDisposition::Produced);
        FFMPEG_ASSERT(first.block().frame_offset == 48000);
        FFMPEG_ASSERT(first.block().decode_generation.value() == 12);

        const auto stale_seek = session.seek(24000, {generation(12)});
        FFMPEG_ASSERT(
            stale_seek.disposition
            == core::decode::DecodeControlDisposition::Rejected);

        const auto flush = session.flush({generation(13)});
        FFMPEG_ASSERT(flush.is_success());
        FFMPEG_ASSERT(session.probe().decode_generation.value() == 13);
    }
}

} // namespace

void run_ffmpeg_session_tests(FfmpegDecodeTestRunner& runner) {
    runner.run("decodes_wave_to_requested_pcm", decodes_wave_to_requested_pcm);
    runner.run("decodes_flac_to_requested_pcm", decodes_flac_to_requested_pcm);
    runner.run("decodes_mp3_to_requested_pcm", decodes_mp3_to_requested_pcm);
    runner.run("conversion_policy_is_enforced", conversion_policy_is_enforced);
    runner.run(
        "int24_target_is_not_silently_reinterpreted",
        int24_target_is_not_silently_reinterpreted);
    runner.run(
        "corrupt_input_is_not_reported_as_eos",
        corrupt_input_is_not_reported_as_eos);
    runner.run(
        "seek_and_flush_advance_generation",
        seek_and_flush_advance_generation);
}
