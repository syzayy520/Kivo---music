#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"
#include "../transition/state_transition_decision.hpp"

namespace kivo::core::contract {

struct RecoveringSeekRule {
    CoreState recovering_state = CoreState::Recovering;
    CoreState seek_target = CoreState::Seeking;
    StateTransitionDecision decision = StateTransitionDecision::Reject;

    [[nodiscard]] bool operator==(const RecoveringSeekRule&) const noexcept = default;
};

} // namespace kivo::core::contract