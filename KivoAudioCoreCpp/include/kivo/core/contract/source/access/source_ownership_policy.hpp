#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source ownership policy — who owns the source resource.
enum class SourceOwnershipPolicy : uint8_t {
    NotSpecified = 0,
    Owned,
    Borrowed,
    Shared
};

} // namespace kivo::core::contract
