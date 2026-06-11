#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Container unique identity wrapper.
struct ContainerIdentity {
    uint64_t value{0};

    [[nodiscard]] constexpr bool operator==(const ContainerIdentity&) const noexcept = default;
};

} // namespace kivo::core::contract
