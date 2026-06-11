#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"
#include "../transition/state_transition_decision.hpp"

namespace kivo::core::contract {

struct DrainingPauseRule {
    CoreState draining_state = CoreState::Draining;
    CoreState pause_target = CoreState::Paused;
    StateTransitionDecision decision = StateTransitionDecision::Defer;

    [[nodiscard]] bool operator==(const DrainingPauseRule&) const noexcept = default;
};

} // namespace kivo::core::contract