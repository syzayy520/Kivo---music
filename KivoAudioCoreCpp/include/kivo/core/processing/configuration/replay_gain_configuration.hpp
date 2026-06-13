#pragma once

#include <cmath>
#include <optional>

namespace kivo::core::processing {

struct ReplayGainConfiguration {
    bool enabled{false};
    double gain_db{0.0};
    std::optional<double> peak{};
    bool prevent_clipping{true};

    [[nodiscard]] bool is_valid() const noexcept {
        return std::isfinite(gain_db)
            && gain_db >= -60.0
            && gain_db <= 30.0
            && (!peak.has_value()
                || (std::isfinite(*peak) && *peak > 0.0));
    }

    [[nodiscard]] bool operator==(
        const ReplayGainConfiguration&) const noexcept = default;
};

} // namespace kivo::core::processing
