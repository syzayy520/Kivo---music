#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source type — how the source delivers data.
enum class SourceType : uint8_t {
    NotSpecified = 0,
    File,
    Stream,
    Network,
    Buffer
};

} // namespace kivo::core::contract
