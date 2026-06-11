#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class StateTransitionValidity : uint8_t {
    Unknown = 0,
    Valid,
    InvalidFromState,
    InvalidToState,
    InvalidTrigger,
    InvalidGeneration
};

} // namespace kivo::core::contract