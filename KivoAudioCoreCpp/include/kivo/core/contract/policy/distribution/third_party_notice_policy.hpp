#pragma once
#include <cstdint>

namespace kivo::core::contract::policy {

struct ThirdPartyNoticePolicy {
    uint8_t require_license_notice{0};
    uint8_t require_attribution{0};
    uint8_t require_source_offer{0};

    [[nodiscard]] constexpr bool operator==(const ThirdPartyNoticePolicy&) const noexcept = default;
};

} // namespace kivo::core::contract::policy
