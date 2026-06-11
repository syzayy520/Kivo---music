#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"
#include "../transition/state_transition_decision.hpp"

namespace kivo::core::contract {

struct DeviceLostDuringSeekRule {
    CoreState seeking_state = CoreState::Seeking;
    CoreState failed_target = CoreState::Failed;
    StateTransitionDecision decision = StateTransitionDecision::Allow;

    [[nodiscard]] bool operator==(const DeviceLostDuringSeekRule&) const noexcept = default;
};

} // namespace kivo::core::contract