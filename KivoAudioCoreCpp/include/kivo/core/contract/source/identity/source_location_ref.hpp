#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source location reference — opaque index into location table.
struct SourceLocationRef {
    uint64_t value{0};

    [[nodiscard]] constexpr bool operator==(const SourceLocationRef&) const noexcept = default;
};

} // namespace kivo::core::contract
