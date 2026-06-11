#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class StaleGenerationClassification : uint8_t {
    NotSpecified = 0,
    Current,
    Stale,
    Future,
    Unknown
};

} // namespace kivo::core::contract