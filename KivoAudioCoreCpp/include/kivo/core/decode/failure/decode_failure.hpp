#pragma once

#include <cstdint>

namespace kivo::core::decode {

enum class DecodeFailure : uint8_t {
    None = 0,
    InvalidRequest,
    InvalidBuffer,
    UnsupportedSource,
    SourceOpenFailed,
    SourceReadFailed,
    SourceSeekFailed,
    UnsupportedContainer,
    NoAudioStream,
    UnsupportedCodec,
    DecoderOpenFailed,
    InvalidMediaData,
    ConversionRequired,
    ConversionFailed,
    SeekUnsupported,
    FlushUnsupported,
    EndOfStream,
    BoundaryFailure
};

} // namespace kivo::core::decode
