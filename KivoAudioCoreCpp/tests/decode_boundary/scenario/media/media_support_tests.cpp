#include "../../fixture/decode_boundary_test_runner.hpp"

#include "kivo/core/decode/media/support/media_support_classification.hpp"

namespace {

using namespace kivo::core::decode;

void pcm_codec_and_container_are_supported() {
    const auto support = classify_media_support(AudioCodec::Flac,
                                                MediaContainer::Flac);

    DECODE_ASSERT(support.is_supported());
    DECODE_ASSERT(support.support == MediaPlaybackSupport::SupportedPcm);
    DECODE_ASSERT(support.failure == DecodeFailure::None);
}

void native_dsd_is_explicitly_unsupported() {
    const auto dsf = classify_media_support(AudioCodec::Dsd,
                                            MediaContainer::Dsf);
    const auto dsdiff = classify_media_support(AudioCodec::Dsd,
                                               MediaContainer::Dsdiff);

    DECODE_ASSERT(!dsf.is_supported());
    DECODE_ASSERT(!dsdiff.is_supported());
    DECODE_ASSERT(dsf.support == MediaPlaybackSupport::UnsupportedNativeDsd);
    DECODE_ASSERT(dsdiff.failure == DecodeFailure::UnsupportedCodec);
}

void dop_carrier_is_explicitly_unsupported() {
    const auto support = classify_media_support(AudioCodec::Pcm,
                                                MediaContainer::Wave,
                                                true);

    DECODE_ASSERT(!support.is_supported());
    DECODE_ASSERT(support.dop_carrier);
    DECODE_ASSERT(support.support == MediaPlaybackSupport::UnsupportedDopCarrier);
    DECODE_ASSERT(support.failure == DecodeFailure::UnsupportedCodec);
}

void unknown_container_and_codec_are_separate_failures() {
    const auto bad_container = classify_media_support(AudioCodec::Flac,
                                                     MediaContainer::Unknown);
    const auto bad_codec = classify_media_support(AudioCodec::Unknown,
                                                  MediaContainer::Flac);

    DECODE_ASSERT(bad_container.failure == DecodeFailure::UnsupportedContainer);
    DECODE_ASSERT(bad_codec.failure == DecodeFailure::UnsupportedCodec);
}

} // namespace

void run_media_support_tests(DecodeBoundaryTestRunner& runner) {
    runner.run("pcm_codec_and_container_are_supported",
               pcm_codec_and_container_are_supported);
    runner.run("native_dsd_is_explicitly_unsupported",
               native_dsd_is_explicitly_unsupported);
    runner.run("dop_carrier_is_explicitly_unsupported",
               dop_carrier_is_explicitly_unsupported);
    runner.run("unknown_container_and_codec_are_separate_failures",
               unknown_container_and_codec_are_separate_failures);
}
