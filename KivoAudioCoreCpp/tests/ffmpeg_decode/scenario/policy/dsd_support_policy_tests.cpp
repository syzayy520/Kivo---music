#include "../../fixture/harness/ffmpeg_decode_test_runner.hpp"

#include "kivo/core/decode/media/support/media_support_classification.hpp"

namespace {

void dsd_policy_rejects_native_payload_before_pcm_decode_path(
    const FfmpegDecodeTestContext&) {
    using namespace kivo::core::decode;

    const auto support =
        classify_media_support(AudioCodec::Dsd, MediaContainer::Dsf);

    FFMPEG_ASSERT(!support.is_supported());
    FFMPEG_ASSERT(
        support.support == MediaPlaybackSupport::UnsupportedNativeDsd);
    FFMPEG_ASSERT(support.failure == DecodeFailure::UnsupportedCodec);
}

} // namespace

void run_ffmpeg_dsd_support_policy_tests(FfmpegDecodeTestRunner& runner) {
    runner.run("dsd_policy_rejects_native_payload_before_pcm_decode_path",
               dsd_policy_rejects_native_payload_before_pcm_decode_path);
}
