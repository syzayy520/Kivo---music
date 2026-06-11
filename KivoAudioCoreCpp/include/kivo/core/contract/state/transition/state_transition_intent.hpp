#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"

namespace kivo::core::contract {

struct StateTransitionIntent {
    CoreState from_state = CoreState::Unknown;
    CoreState to_state = CoreState::Unknown;
    uint32_t trigger_id = 0;

    [[nodiscard]] bool operator==(const StateTransitionIntent&) const noexcept = default;
};

} // namespace kivo::core::contract