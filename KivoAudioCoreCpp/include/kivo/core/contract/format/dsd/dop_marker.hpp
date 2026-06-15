#pragma once

#include <cstdint>

namespace kivo::core::contract {

struct DopMarkerPair {
    uint8_t first{0x05};
    uint8_t second{0xFA};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return (first == 0x05 && second == 0xFA)
            || (first == 0xFA && second == 0x05);
    }

    [[nodiscard]] constexpr bool operator==(const DopMarkerPair&) const noexcept = default;
};

} // namespace kivo::core::contract
