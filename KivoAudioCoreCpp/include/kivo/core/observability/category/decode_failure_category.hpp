#pragma once

#include <cstddef>
#include <cstdint>

#include "kivo/core/decode/failure/decode_failure.hpp"

namespace kivo::core::observability {

enum class DecodeFailureCategory : uint8_t {
    Source = 0,
    Container,
    Codec,
    MediaData,
    Conversion,
    Processing,
    Generation,
    Boundary,
    Count
};

inline constexpr size_t kDecodeFailureCategoryCount =
    static_cast<size_t>(DecodeFailureCategory::Count);

[[nodiscard]] constexpr DecodeFailureCategory classify_decode_failure(
    decode::DecodeFailure failure) noexcept {
    using decode::DecodeFailure;
    switch (failure) {
    case DecodeFailure::SourceOpenFailed:
    case DecodeFailure::SourceReadFailed:
    case DecodeFailure::SourceSeekFailed:
        return DecodeFailureCategory::Source;
    case DecodeFailure::UnsupportedSource:
    case DecodeFailure::UnsupportedContainer:
    case DecodeFailure::NoAudioStream:
        return DecodeFailureCategory::Container;
    case DecodeFailure::UnsupportedCodec:
    case DecodeFailure::DecoderOpenFailed:
        return DecodeFailureCategory::Codec;
    case DecodeFailure::InvalidMediaData:
        return DecodeFailureCategory::MediaData;
    case DecodeFailure::ConversionRequired:
    case DecodeFailure::ConversionFailed:
        return DecodeFailureCategory::Conversion;
    case DecodeFailure::ProcessingFailed:
        return DecodeFailureCategory::Processing;
    case DecodeFailure::StaleSourceGeneration:
    case DecodeFailure::StaleDecodeGeneration:
        return DecodeFailureCategory::Generation;
    case DecodeFailure::InvalidRequest:
    case DecodeFailure::InvalidBuffer:
    case DecodeFailure::SeekUnsupported:
    case DecodeFailure::FlushUnsupported:
    case DecodeFailure::EndOfStream:
    case DecodeFailure::BoundaryFailure:
    case DecodeFailure::None:
        return DecodeFailureCategory::Boundary;
    }
    return DecodeFailureCategory::Boundary;
}

[[nodiscard]] constexpr size_t decode_failure_category_index(
    DecodeFailureCategory category) noexcept {
    return static_cast<size_t>(category);
}

} // namespace kivo::core::observability
