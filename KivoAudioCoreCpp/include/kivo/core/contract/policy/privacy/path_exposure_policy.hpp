#pragma once
#include <cstdint>

namespace kivo::core::contract::policy {

struct PathExposurePolicy {
    uint8_t allow_absolute_paths{0};
    uint8_t allow_home_relative{0};
    uint8_t sanitize_separators{0};

    [[nodiscard]] constexpr bool operator==(const PathExposurePolicy&) const noexcept = default;
};

} // namespace kivo::core::contract::policy
