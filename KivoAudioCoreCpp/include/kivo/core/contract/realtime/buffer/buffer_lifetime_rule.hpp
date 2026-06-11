#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class RealtimeBufferLifetimeProof : uint8_t {
    NotSpecified = 0,
    MustBeProvableAcrossBoundary,
    MustBeProvableWithinBoundary,
    NoRequirement
};

} // namespace kivo::core::contract
