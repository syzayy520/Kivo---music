#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source file path — opaque token, not a string.
struct SourcePath {
    uint64_t value{0};

    [[nodiscard]] constexpr bool operator==(const SourcePath&) const noexcept = default;
};

} // namespace kivo::core::contract
