#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source locator strategy — how a source is addressed.
enum class SourceLocator : uint8_t {
    NotSpecified = 0,
    ByUri,
    ByPath,
    ByToken,
    ByStream
};

} // namespace kivo::core::contract
