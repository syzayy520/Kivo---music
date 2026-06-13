#include "../fixture/session_test_assert.hpp"
#include "../runner/playback_session_test_cases.hpp"

#include "kivo/core/playback/recovery/classification/decode_failure_domain.hpp"

namespace playback_session_test {

void decode_failures_map_to_stable_error_domains() {
    using namespace kivo::core;
    using contract::ErrorDomain;
    using decode::DecodeFailure;
    using playback::classify_decode_failure;

    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::SourceOpenFailed)
        == ErrorDomain::SourceUnavailable);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::SourceReadFailed)
        == ErrorDomain::SourceUnavailable);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::SourceSeekFailed)
        == ErrorDomain::SourceUnavailable);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::UnsupportedCodec)
        == ErrorDomain::CodecError);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::DecoderOpenFailed)
        == ErrorDomain::CodecError);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::InvalidMediaData)
        == ErrorDomain::MediaError);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::UnsupportedSource)
        == ErrorDomain::UnsupportedFormat);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::UnsupportedContainer)
        == ErrorDomain::UnsupportedFormat);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::NoAudioStream)
        == ErrorDomain::UnsupportedFormat);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::ConversionRequired)
        == ErrorDomain::UnsupportedFormat);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::SeekUnsupported)
        == ErrorDomain::UnsupportedFormat);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::FlushUnsupported)
        == ErrorDomain::UnsupportedFormat);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::ConversionFailed)
        == ErrorDomain::FormatError);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::InvalidRequest)
        == ErrorDomain::InternalBug);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::InvalidBuffer)
        == ErrorDomain::InternalBug);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::StaleSourceGeneration)
        == ErrorDomain::InternalBug);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::StaleDecodeGeneration)
        == ErrorDomain::InternalBug);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::EndOfStream)
        == ErrorDomain::InternalBug);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::BoundaryFailure)
        == ErrorDomain::InternalBug);
    SESSION_ASSERT(
        classify_decode_failure(DecodeFailure::None)
        == ErrorDomain::Unknown);
}

} // namespace playback_session_test
