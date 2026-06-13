#pragma once

#include <cstdint>

#include "dsd_rate.hpp"

namespace kivo::core::contract {

struct NativeDsdFormat {
    DsdRate rate{DsdRate::Unknown};
    uint8_t channels{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return dsd_rate_is_known(rate) && channels != 0 && channels <= 8;
    }

    [[nodiscard]] constexpr uint32_t bit_rate() const noexcept {
        return dsd_bit_rate(rate);
    }

    [[nodiscard]] constexpr bool operator==(const NativeDsdFormat&) const noexcept = default;
};

} // namespace kivo::core::contract
