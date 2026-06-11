#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class FailedCloseRule : uint8_t {
    Unknown = 0,
    CloseMustSucceed,
    CloseIsIdempotent,
    CleanupThenClose
};

} // namespace kivo::core::contract