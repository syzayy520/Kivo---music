#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class BoundedTransport : uint8_t {
    NotSpecified = 0,
    Required,
    Recommended,
    NotRequired
};

} // namespace kivo::core::contract