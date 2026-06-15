#include "abnormal_media_decode.hpp"

#include "../decode/ffmpeg_decode_test_factory.hpp"

namespace ffmpeg_decode_test {

AbnormalMediaOutcome decode_abnormal_media(
    const std::filesystem::path& path,
    uint64_t source_identity) {
    kivo::adapters::ffmpeg::FfmpegAudioDecodeSession session;
    auto source = open_source(path, source_identity);
    if (!source) {
        return {
            false,
            false,
            0,
            kivo::core::decode::DecodeFailure::SourceOpenFailed};
    }

    const auto opened = session.open(std::move(source), open_request());
    if (!opened.is_accepted()) {
        return {false, false, 0, opened.failure()};
    }

    uint64_t produced_frames = 0;
    for (size_t step_count = 0; step_count < 10000; ++step_count) {
        const auto step = session.decode_next();
        if (step.disposition()
            == kivo::core::decode::DecodeStepDisposition::Produced) {
            produced_frames += step.block().frame_count;
            continue;
        }
        if (step.disposition()
            == kivo::core::decode::DecodeStepDisposition::Failed) {
            static_cast<void>(session.close());
            return {true, false, produced_frames, step.failure()};
        }
        if (step.disposition()
            == kivo::core::decode::DecodeStepDisposition::EndOfStream) {
            static_cast<void>(session.close());
            return {true, true, produced_frames, step.failure()};
        }
    }

    static_cast<void>(session.close());
    return {
        true,
        false,
        produced_frames,
        kivo::core::decode::DecodeFailure::BoundaryFailure};
}

} // namespace ffmpeg_decode_test
