#pragma once

#include <cstdint>

namespace kivo::core::contract {

struct QueueStalenessThreshold {
    uint64_t max_age_millis{0};

    [[nodiscard]] constexpr bool operator==(const QueueStalenessThreshold&) const noexcept = default;
};

} // namespace kivo::core::contract