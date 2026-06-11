#pragma once
#include <cstdint>

namespace kivo::core::contract::policy {

struct SourceIdentitySanitization {
    uint8_t strip_username{0};
    uint8_t strip_hostname{0};
    uint8_t strip_device_serial{0};

    friend bool operator==(const SourceIdentitySanitization&, const SourceIdentitySanitization&) noexcept = default;
};

} // namespace kivo::core::contract::policy