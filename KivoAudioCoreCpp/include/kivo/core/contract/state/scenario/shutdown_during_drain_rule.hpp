#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"
#include "../transition/state_transition_decision.hpp"

namespace kivo::core::contract {

struct ShutdownDuringDrainRule {
    CoreState draining_state = CoreState::Draining;
    CoreState closed_target = CoreState::Closed;
    StateTransitionDecision decision = StateTransitionDecision::Allow;

    [[nodiscard]] bool operator==(const ShutdownDuringDrainRule&) const noexcept = default;
};

} // namespace kivo::core::contract