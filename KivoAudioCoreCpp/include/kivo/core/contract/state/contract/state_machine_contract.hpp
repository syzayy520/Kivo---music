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
    ReentrantTransitionPolicy reentrant_policy = ReentrantTransitionPolicy::Unknown;
    TransitionPreemptionPolicy preemption_policy = TransitionPreemptionPolicy::Unknown;
    TerminalStateRule terminal_rule{};

    // Scenario fields (7)
    SeekingReentryRule seeking_reentry_rule{};
    RecoveringSeekRule recovering_seek_rule{};
    DrainingPauseRule draining_pause_rule{};
    FailedCloseRule failed_close_rule{};
    ClosedMutationRule closed_mutation_rule{};
    DeviceLostDuringSeekRule device_lost_during_seek_rule{};
    ShutdownDuringDrainRule shutdown_during_drain_rule{};

    [[nodiscard]] bool operator==(const StateMachineContract&) const noexcept = default;
};

} // namespace kivo::core::contract