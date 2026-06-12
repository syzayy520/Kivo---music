#pragma once

#include <cstdint>

namespace kivo::core::processing {

enum class AudioProcessingFailure : uint8_t {
    None = 0,
    InvalidConfiguration,
    InvalidBuffer,
    UnsupportedFormat,
    BitPerfectConflict
};

} // namespace kivo::core::processing
