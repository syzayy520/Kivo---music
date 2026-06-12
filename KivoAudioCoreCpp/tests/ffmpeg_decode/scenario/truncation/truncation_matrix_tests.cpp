#include <array>
#include <string>

#include "../../fixture/assertion/abnormal_media_decode.hpp"
#include "../../fixture/ffmpeg_decode_test_runner.hpp"
#include "../../fixture/mutation/media_mutation_file.hpp"

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

void require_truncation_failure(
    const FfmpegDecodeTestContext& context,
    const char* fixture_name,
    uintmax_t numerator,
    uintmax_t denominator,
    uint64_t source_identity) {
    const auto tag = std::string{"truncated_"}
        + std::to_string(numerator)
        + "_of_"
        + std::to_string(denominator);
    const auto mutation = MediaMutationFile::truncated(
        context.fixture_directory / fixture_name,
        numerator,
        denominator,
        tag.c_str());
    const auto outcome =
        decode_abnormal_media(mutation.path(), source_identity);
    FFMPEG_ASSERT(!outcome.reached_end_of_stream);
    FFMPEG_ASSERT(
        outcome.failure == core::decode::DecodeFailure::InvalidMediaData
        || outcome.failure
            == core::decode::DecodeFailure::UnsupportedContainer);
}

void truncation_depth_matrix_is_classified(
    const FfmpegDecodeTestContext& context) {
    constexpr std::array<const char*, 5> fixtures{
        "tone_pcm_s16le.wav",
        "tone.flac",
        "tone.mp3",
        "tone_vbr.mp3",
        "tone_aac.m4a"};
    uint64_t identity = 301;
    for (const auto* fixture : fixtures) {
        require_truncation_failure(context, fixture, 1, 3, identity++);
        require_truncation_failure(context, fixture, 2, 3, identity++);
    }
}

} // namespace

void run_ffmpeg_truncated_media_tests(FfmpegDecodeTestRunner& runner) {
    runner.run(
        "truncation_depth_matrix_is_classified",
        truncation_depth_matrix_is_classified);
}
