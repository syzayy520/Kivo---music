#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Reader unique identity wrapper.
struct ReaderIdentity {
    uint64_t value{0};

    [[nodiscard]] constexpr bool operator==(const ReaderIdentity&) const noexcept = default;
};

} // namespace kivo::core::contract
