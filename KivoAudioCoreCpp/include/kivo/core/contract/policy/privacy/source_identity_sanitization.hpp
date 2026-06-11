#pragma once
#include <cstdint>

namespace kivo::core::contract::policy {

struct SourceIdentitySanitization {
    uint8_t str\u0069p_username{0};
    uint8_t str\u0069p_hostname{0};
    uint8_t str\u0069p_device_serial{0};

    [[nodiscard]] bool operator==(const SourceIdentitySanitization&) const noexcept = default;
};

} // namespace kivo::core::contract::policy
