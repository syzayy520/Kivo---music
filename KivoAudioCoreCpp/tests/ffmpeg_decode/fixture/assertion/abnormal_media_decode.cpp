#include "abnormal_media_decode.hpp"

#include "../ffmpeg_decode_test_factory.hpp"

namespace ffmpeg_decode_test {

AbnormalMediaOutcome decode_abnormal_media(
    const std::filesystem::path& path,
    uint64_t source_identity) {
    adapters::ffmpeg::FfmpegAudioDecodeSession session;
    auto source = open_source(path, source_identity);
    if (!source) {
        return {false, false, core::decode::DecodeFailure::SourceOpenFailed};
    }

    const auto opened = session.open(std::move(source), open_request());
    if (!opened.is_accepted()) {
        return {false, false, opened.failure()};
    }

    for (size_t step_count = 0; step_count < 10000; ++step_count) {
        const auto step = session.decode_next();
        if (step.disposition()
            == core::decode::DecodeStepDisposition::Failed) {
            static_cast<void>(session.close());
            return {true, false, step.failure()};
        }
        if (step.disposition()
            == core::decode::DecodeStepDisposition::EndOfStream) {
            static_cast<void>(session.close());
            return {true, true, step.failure()};
        }
    }

    static_cast<void>(session.close());
    return {true, false, core::decode::DecodeFailure::BoundaryFailure};
}

} // namespace ffmpeg_decode_test
