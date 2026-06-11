#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class PreallocationRequirement : uint8_t {
    NotSpecified = 0,
    Required,
    Recommended,
    NotRequired
};

} // namespace kivo::core::contract