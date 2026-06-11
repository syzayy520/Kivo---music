#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"
#include "state_transition_intent.hpp"

namespace kivo::core::contract {

struct StateTransition {
    CoreState from{CoreState::Unknown};
    CoreState to{CoreState::Unknown};
    StateTransitionIntent intent{StateTransitionIntent::Unknown};

    [[nodiscard]] bool operator==(const StateTransition&) const noexcept = default;
};

} // namespace kivo::core::contract