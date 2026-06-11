#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CallbackConstraint : uint8_t {
    NotSpecified = 0,
    Forbidden,
    Allowed,
    Conditional
};

} // namespace kivo::core::contract