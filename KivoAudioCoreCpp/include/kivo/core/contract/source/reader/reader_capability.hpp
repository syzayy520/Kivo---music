#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Reader capability — what operations the reader supports.
struct ReaderCapability {
    bool can_seek{false};
    bool can_pause{false};
    bool supports_metadata{false};

    [[nodiscard]] constexpr bool operator==(const ReaderCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
