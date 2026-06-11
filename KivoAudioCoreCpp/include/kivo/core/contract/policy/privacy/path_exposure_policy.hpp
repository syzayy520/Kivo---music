#pragma once
#include <cstdint>

namespace kivo::core::contract::policy {

struct PathExposurePolicy {
    uint8_t allow_absolute_paths{0};
    uint8_t allow_home_relative{0};
    uint8_t sanitize_separators{0};

    friend bool operator==(const PathExposurePolicy&, const PathExposurePolicy&) noexcept = default;
};

} // namespace kivo::core::contract::policy