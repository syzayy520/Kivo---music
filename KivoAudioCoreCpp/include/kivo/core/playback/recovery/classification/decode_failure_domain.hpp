#pragma once

#include "kivo/core/contract/error/domain/error_domain.hpp"
#include "kivo/core/decode/failure/decode_failure.hpp"

namespace kivo::core::playback {

[[nodiscard]] constexpr contract::ErrorDomain classify_decode_failure(
    decode::DecodeFailure failure) noexcept {
    using contract::ErrorDomain;
    using decode::DecodeFailure;
    switch (failure) {
    case DecodeFailure::SourceOpenFailed:
    case DecodeFailure::SourceReadFailed:
    case DecodeFailure::SourceSeekFailed:
        return ErrorDomain::SourceUnavailable;
    case DecodeFailure::UnsupportedCodec:
    case DecodeFailure::DecoderOpenFailed:
        return ErrorDomain::CodecError;
    case DecodeFailure::InvalidMediaData:
        return ErrorDomain::MediaError;
    case DecodeFailure::UnsupportedSource:
    case DecodeFailure::UnsupportedContainer:
    case DecodeFailure::NoAudioStream:
    case DecodeFailure::ConversionRequired:
    case DecodeFailure::SeekUnsupported:
    case DecodeFailure::FlushUnsupported:
        return ErrorDomain::UnsupportedFormat;
    case DecodeFailure::ConversionFailed:
        return ErrorDomain::FormatError;
    case DecodeFailure::InvalidRequest:
    case DecodeFailure::InvalidBuffer:
    case DecodeFailure::StaleSourceGeneration:
    case DecodeFailure::StaleDecodeGeneration:
    case DecodeFailure::EndOfStream:
    case DecodeFailure::BoundaryFailure:
        return ErrorDomain::InternalBug;
    case DecodeFailure::None:
        return ErrorDomain::Unknown;
    }
    return ErrorDomain::InternalBug;
}

} // namespace kivo::core::playback
