#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"
#include "../transition/state_transition_decision.hpp"

namespace kivo::core::contract {

struct TerminalStateRule {
    CoreState terminal_state = CoreState::Unknown;
    StateTransitionDecision decision = StateTransitionDecision::Reject;

    [[nodiscard]] bool operator==(const TerminalStateRule&) const noexcept = default;
};

} // namespace kivo::core::contract