#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class SpscSemantics : uint8_t {
    NotSpecified = 0,
    Declared,
    NotDeclared
};

} // namespace kivo::core::contract