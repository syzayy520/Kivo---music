#include "../fixture/ffmpeg_decode_test_factory.hpp"
#include "../fixture/ffmpeg_decode_test_runner.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

[[nodiscard]] std::filesystem::path make_truncated_copy(
    const std::filesystem::path& source_path) {
    const auto destination =
        source_path.parent_path() / ("truncated_" + source_path.filename().string());
    std::ifstream source(source_path, std::ios::binary);
    std::ofstream output(destination, std::ios::binary | std::ios::trunc);
    FFMPEG_ASSERT(source.is_open());
    FFMPEG_ASSERT(output.is_open());

    auto remaining = std::filesystem::file_size(source_path) / 3u;
    std::array<char, 65536> buffer{};
    while (remaining != 0) {
        const auto count = static_cast<std::streamsize>(
            std::min<uintmax_t>(remaining, buffer.size()));
        source.read(buffer.data(), count);
        const auto read = source.gcount();
        FFMPEG_ASSERT(read > 0);
        output.write(buffer.data(), read);
        FFMPEG_ASSERT(output.good());
        remaining -= static_cast<uintmax_t>(read);
    }
    return destination;
}

void require_truncated_media_failure(
    const FfmpegDecodeTestContext& context,
    const char* fixture_name,
    uint64_t source_identity) {
    const auto path = make_truncated_copy(
        context.fixture_directory / fixture_name);
    adapters::ffmpeg::FfmpegAudioDecodeSession session;
    auto source = open_source(path, source_identity);
    FFMPEG_ASSERT(source != nullptr);

    const auto opened = session.open(std::move(source), open_request());
    if (!opened.is_accepted()) {
        FFMPEG_ASSERT(
            opened.failure() == core::decode::DecodeFailure::InvalidMediaData
            || opened.failure()
                == core::decode::DecodeFailure::UnsupportedContainer);
        std::filesystem::remove(path);
        return;
    }

    size_t step_count = 0;
    for (;;) {
        const auto step = session.decode_next();
        ++step_count;
        FFMPEG_ASSERT(step_count < 10000);
        if (step.disposition()
            == core::decode::DecodeStepDisposition::Failed) {
            FFMPEG_ASSERT(
                step.failure()
                == core::decode::DecodeFailure::InvalidMediaData);
            break;
        }
        FFMPEG_ASSERT(
            step.disposition()
            != core::decode::DecodeStepDisposition::EndOfStream);
    }
    FFMPEG_ASSERT(session.close().is_success());
    std::filesystem::remove(path);
}

void truncated_wave_is_not_eos(
    const FfmpegDecodeTestContext& context) {
    require_truncated_media_failure(
        context,
        "tone_pcm_s16le.wav",
        301);
}

void truncated_flac_is_not_eos(
    const FfmpegDecodeTestContext& context) {
    require_truncated_media_failure(context, "tone.flac", 302);
}

void truncated_vbr_mp3_is_not_eos(
    const FfmpegDecodeTestContext& context) {
    require_truncated_media_failure(context, "tone_vbr.mp3", 303);
}

void truncated_aac_m4a_is_not_eos(
    const FfmpegDecodeTestContext& context) {
    require_truncated_media_failure(context, "tone_aac.m4a", 304);
}

} // namespace

void run_ffmpeg_truncated_media_tests(FfmpegDecodeTestRunner& runner) {
    runner.run("truncated_wave_is_not_eos", truncated_wave_is_not_eos);
    runner.run("truncated_flac_is_not_eos", truncated_flac_is_not_eos);
    runner.run("truncated_vbr_mp3_is_not_eos", truncated_vbr_mp3_is_not_eos);
    runner.run("truncated_aac_m4a_is_not_eos", truncated_aac_m4a_is_not_eos);
}
