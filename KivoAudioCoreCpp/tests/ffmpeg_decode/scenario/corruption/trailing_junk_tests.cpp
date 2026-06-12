#include "../../fixture/ffmpeg_decode_test_factory.hpp"
#include "../../fixture/ffmpeg_decode_test_runner.hpp"
#include "../../fixture/mutation/media_mutation_file.hpp"

#include <array>

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

void appended_junk_does_not_destabilize_valid_audio(
    const FfmpegDecodeTestContext& context) {
    constexpr std::array<const char*, 5> fixtures{
        "tone_pcm_s16le.wav",
        "tone.flac",
        "tone.mp3",
        "tone_vbr.mp3",
        "tone_aac.m4a"};
    uint64_t identity = 451;
    for (const auto* fixture : fixtures) {
        const auto mutation = MediaMutationFile::appended_junk(
            context.fixture_directory / fixture,
            8192,
            "trailing_junk");
        adapters::ffmpeg::FfmpegAudioDecodeSession session;
        auto source = open_source(mutation.path(), identity++);
        FFMPEG_ASSERT(source);
        require_opened(session.open(std::move(source), open_request()));

        size_t step_count = 0;
        for (;;) {
            const auto step = session.decode_next();
            ++step_count;
            FFMPEG_ASSERT(step_count < 10000);
            if (step.disposition()
                == core::decode::DecodeStepDisposition::EndOfStream) {
                break;
            }
            FFMPEG_ASSERT(
                step.disposition()
                == core::decode::DecodeStepDisposition::Produced);
        }
        FFMPEG_ASSERT(session.close().is_success());
    }
}

} // namespace

void run_ffmpeg_trailing_junk_tests(FfmpegDecodeTestRunner& runner) {
    runner.run(
        "appended_junk_does_not_destabilize_valid_audio",
        appended_junk_does_not_destabilize_valid_audio);
}
