#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source recovery policy — how to recover from source failures.
enum class SourceRecoveryPolicy : uint8_t {
    NotSpecified = 0,
    NoRecovery,
    RetrySameSource,
    RetryAlternateSource,
    SkipToNext,
    StopSource
};

} // namespace kivo::core::contract
