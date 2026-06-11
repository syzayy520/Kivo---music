#pragma once
#include <cstdint>

namespace kivo::core::contract::policy {

struct CommercialClaimPolicy {
    uint8_t allow_commercial_use{0};
    uint8_t require_brand_approval{0};
    uint8_t restrict_commercial_distribution{0};

    friend bool operator==(const CommercialClaimPolicy&, const CommercialClaimPolicy&) noexcept = default;
};

} // namespace kivo::core::contract::policy