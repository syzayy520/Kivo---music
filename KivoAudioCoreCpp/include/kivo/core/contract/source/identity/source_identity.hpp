#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source unique identity wrapper.
struct SourceIdentity {
    uint64_t value{0};

    [[nodiscard]] constexpr bool operator==(const SourceIdentity&) const noexcept = default;
};

} // namespace kivo::core::contract
