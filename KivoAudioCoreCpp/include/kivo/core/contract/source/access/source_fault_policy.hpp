#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source fault policy — how to handle source errors.
enum class SourceFaultPolicy : uint8_t {
    NotSpecified = 0,
    FailImmediate,
    RetryOnce,
    RetryWithBackoff,
    SkipAndContinue,
    ReportAndContinue
};

} // namespace kivo::core::contract
