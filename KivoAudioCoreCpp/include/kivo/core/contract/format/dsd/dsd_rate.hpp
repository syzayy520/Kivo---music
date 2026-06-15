#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class DsdRate : uint8_t {
    Unknown = 0,
    Dsd64,
    Dsd128,
    Dsd256,
    Dsd512
};

[[nodiscard]] constexpr uint16_t dsd_rate_multiplier(DsdRate rate) noexcept {
    switch (rate) {
    case DsdRate::Dsd64:  return 64;
    case DsdRate::Dsd128: return 128;
    case DsdRate::Dsd256: return 256;
    case DsdRate::Dsd512: return 512;
    default:              return 0;
    }
}

[[nodiscard]] constexpr uint32_t dsd_bit_rate(DsdRate rate) noexcept {
    return static_cast<uint32_t>(44100u * dsd_rate_multiplier(rate));
}

[[nodiscard]] constexpr bool dsd_rate_is_known(DsdRate rate) noexcept {
    return dsd_rate_multiplier(rate) != 0;
}

} // namespace kivo::core::contract
