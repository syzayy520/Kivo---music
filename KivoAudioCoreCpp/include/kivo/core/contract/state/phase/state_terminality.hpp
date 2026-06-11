#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class StateTerminality : uint8_t {
    Unknown = 0,
    NonTerminal,
    Terminal
};

} // namespace kivo::core::contract