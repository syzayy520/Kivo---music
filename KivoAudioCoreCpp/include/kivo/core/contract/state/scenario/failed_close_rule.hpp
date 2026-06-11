#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"
#include "../transition/state_transition_decision.hpp"

namespace kivo::core::contract {

struct FailedCloseRule {
    CoreState failed_state = CoreState::Failed;
    CoreState close_target = CoreState::Closed;
    StateTransitionDecision decision = StateTransitionDecision::Allow;

    [[nodiscard]] bool operator==(const FailedCloseRule&) const noexcept = default;
};

} // namespace kivo::core::contract