#pragma once

#include <cmath>

namespace kivo::core::processing {

struct SoftwareVolumeConfiguration {
    bool enabled{false};
    double linear_gain{1.0};

    [[nodiscard]] bool is_valid() const noexcept {
        return std::isfinite(linear_gain)
            && linear_gain >= 0.0
            && linear_gain <= 1.0;
    }

    [[nodiscard]] bool operator==(
        const SoftwareVolumeConfiguration&) const noexcept = default;
};

} // namespace kivo::core::processing
