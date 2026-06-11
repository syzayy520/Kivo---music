#pragma once
#include <cstdint>

namespace kivo::core::contract::policy {

struct CodecDistributionPolicy {
    uint8_t allow_static_linking{0};
    uint8_t allow_dynamic_linking{0};
    uint8_t allow_redistribution{0};

    [[nodiscard]] bool operator==(const CodecDistributionPolicy&) const noexcept = default;
};

} // namespace kivo::core::contract::policy
