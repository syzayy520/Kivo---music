#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source URI — opaque token, not a string.
struct SourceUri {
    uint64_t value{0};

    [[nodiscard]] constexpr bool operator==(const SourceUri&) const noexcept = default;
};

} // namespace kivo::core::contract
