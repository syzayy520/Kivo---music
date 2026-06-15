#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace kivo::core::processing::detail {

template <typename Sample>
[[nodiscard]] Sample load_sample(const std::byte* source) noexcept {
    Sample value{};
    std::memcpy(&value, source, sizeof(value));
    return value;
}

template <typename Sample>
void store_sample(std::byte* destination, Sample value) noexcept {
    std::memcpy(destination, &value, sizeof(value));
}

[[nodiscard]] inline double triangular_dither(
    uint64_t& state) noexcept {
    const auto next = [&state]() noexcept {
        state ^= state << 13u;
        state ^= state >> 7u;
        state ^= state << 17u;
        return static_cast<uint32_t>(state >> 32u);
    };
    constexpr double scale =
        1.0 / static_cast<double>(std::numeric_limits<uint32_t>::max());
    const auto first = next();
    const auto second = next();
    return (static_cast<double>(first) - static_cast<double>(second)) * scale;
}

template <typename Sample>
[[nodiscard]] Sample quantize_integer(
    double value,
    double minimum,
    double maximum,
    uint64_t& clipped_samples) noexcept {
    if (!std::isfinite(value)) {
        ++clipped_samples;
        value = 0.0;
    }
    if (value < minimum) {
        ++clipped_samples;
        value = minimum;
    } else if (value > maximum) {
        ++clipped_samples;
        value = maximum;
    }
    return static_cast<Sample>(std::round(value));
}

} // namespace kivo::core::processing::detail
