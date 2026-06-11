#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Container failure mode.
enum class ContainerFailure : uint8_t {
    NotSpecified = 0,
    CorruptedHeader,
    UnsupportedFormat,
    TruncatedFile,
    InvalidIndex,
    NoStreams
};

} // namespace kivo::core::contract
