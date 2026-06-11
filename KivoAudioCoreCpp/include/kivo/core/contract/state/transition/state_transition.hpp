#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"

namespace kivo::core::contract {

struct StateTransition {
    CoreState from_state = CoreState::Unknown;
    CoreState to_state = CoreState::Unknown;
    uint64_t timestamp_ns = 0;
    uint32_t generation_id = 0;

    [[nodiscard]] bool operator==(const StateTransition&) const noexcept = default;
};

} // namespace kivo::core::contract