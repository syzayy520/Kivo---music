#pragma once

#include <cstdint>

#include "../rule/reentrant_transition_policy.hpp"
#include "../rule/transition_preemption_policy.hpp"
#include "../rule/terminal_state_rule.hpp"
#include "../scenario/seeking_reentry_rule.hpp"
#include "../scenario/recovering_seek_rule.hpp"
#include "../scenario/draining_pause_rule.hpp"
#include "../scenario/failed_close_rule.hpp"
#include "../scenario/closed_mutation_rule.hpp"
#include "../scenario/device_lost_during_seek_rule.hpp"
#include "../scenario/shutdown_during_drain_rule.hpp"

namespace kivo::core::contract {

struct StateMachineContract {
    // Rule fields (3)
    ReentrantTransitionPolicy reentrant_transition{ReentrantTransitionPolicy::Unknown};
    TransitionPreemptionPolicy transition_preemption{TransitionPreemptionPolicy::Unknown};
    TerminalStateRule terminal_state{TerminalStateRule::Unknown};

    // Scenario fields (7)
    SeekingReentryRule seeking_reentry{SeekingReentryRule::Unknown};
    RecoveringSeekRule recovering_seek{RecoveringSeekRule::Unknown};
    DrainingPauseRule draining_pause{DrainingPauseRule::Unknown};
    FailedCloseRule failed_close{FailedCloseRule::Unknown};
    ClosedMutationRule closed_mutation{ClosedMutationRule::Unknown};
    DeviceLostDuringSeekRule device_lost_during_seek{DeviceLostDuringSeekRule::Unknown};
    ShutdownDuringDrainRule shutdown_during_drain{ShutdownDuringDrainRule::Unknown};

    [[nodiscard]] constexpr bool operator==(const StateMachineContract&) const noexcept = default;
};

} // namespace kivo::core::contract
