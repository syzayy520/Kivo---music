#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"
#include "../transition/state_transition_decision.hpp"

namespace kivo::core::contract {

struct ClosedMutationRule {
    CoreState closed_state = CoreState::Closed;
    StateTransitionDecision decision = StateTransitionDecision::Reject;

    [[nodiscard]] bool operator==(const ClosedMutationRule&) const noexcept = default;
};

} // namespace kivo::core::contract