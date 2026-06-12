#include "../fixture/ffmpeg_decode_test_factory.hpp"
#include "../fixture/ffmpeg_decode_test_runner.hpp"

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

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

void run_ffmpeg_seek_flush_tests(FfmpegDecodeTestRunner& runner) {
    runner.run(
        "seek_and_flush_advance_generation",
        seek_and_flush_advance_generation);
}
