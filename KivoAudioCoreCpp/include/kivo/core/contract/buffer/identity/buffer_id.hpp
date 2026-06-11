#pragma once

#include <cstdint>

namespace kivo::core::contract {

struct BufferId {
    uint64_t value{0};

    [[nodiscard]] constexpr bool operator==(const BufferId&) const noexcept = default;
};

} // namespace kivo::core::contract