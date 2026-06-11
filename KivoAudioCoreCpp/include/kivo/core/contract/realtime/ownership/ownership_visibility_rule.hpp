#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class OwnershipVisibility : uint8_t {
    NotSpecified = 0,
    MustBeProvable,
    BestEffort,
    NoRequirement
};

} // namespace kivo::core::contract