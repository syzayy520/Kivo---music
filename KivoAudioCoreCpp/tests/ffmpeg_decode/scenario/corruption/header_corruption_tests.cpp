#include "../../fixture/assertion/abnormal_media_decode.hpp"
#include "../../fixture/ffmpeg_decode_test_runner.hpp"
#include "../../fixture/mutation/media_mutation_file.hpp"

#include <array>

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

void destroyed_container_headers_are_classified(
    const FfmpegDecodeTestContext& context) {
    constexpr std::array<const char*, 5> fixtures{
        "tone_pcm_s16le.wav",
        "tone.flac",
        "tone.mp3",
        "tone_vbr.mp3",
        "tone_aac.m4a"};
    uint64_t identity = 401;
    for (const auto* fixture : fixtures) {
        const auto mutation = MediaMutationFile::zeroed_prefix(
            context.fixture_directory / fixture,
            4096,
            "zeroed_header");
        const auto outcome =
            decode_abnormal_media(mutation.path(), identity++);
        FFMPEG_ASSERT(!outcome.reached_end_of_stream);
        FFMPEG_ASSERT(
            outcome.failure == core::decode::DecodeFailure::InvalidMediaData
            || outcome.failure
                == core::decode::DecodeFailure::UnsupportedContainer
            || outcome.failure
                == core::decode::DecodeFailure::NoAudioStream);
    }
}

} // namespace

void run_ffmpeg_header_corruption_tests(FfmpegDecodeTestRunner& runner) {
    runner.run(
        "destroyed_container_headers_are_classified",
        destroyed_container_headers_are_classified);
}
