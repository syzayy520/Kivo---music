#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"
#include "../transition/state_transition_decision.hpp"

namespace kivo::core::contract {

struct IllegalTransitionRule {
    CoreState from_state = CoreState::Unknown;
    CoreState to_state = CoreState::Unknown;
    StateTransitionDecision decision = StateTransitionDecision::Reject;

    [[nodiscard]] bool operator==(const IllegalTransitionRule&) const noexcept = default;
};

} // namespace kivo::core::contract