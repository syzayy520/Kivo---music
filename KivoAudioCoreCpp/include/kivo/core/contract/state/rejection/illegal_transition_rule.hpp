#pragma once

#include <cstdint>

#include "../transition/state_transition.hpp"
#include "../transition/state_transition_validity.hpp"
#include "state_transition_rejection_reason.hpp"

namespace kivo::core::contract {

struct IllegalTransitionRule {
    StateTransition transition{};
    StateTransitionValidity validity = StateTransitionValidity::Unknown;
    StateTransitionRejectionReason rejection_reason = StateTransitionRejectionReason::Unknown;

    [[nodiscard]] bool operator==(const IllegalTransitionRule&) const noexcept = default;
};

} // namespace kivo::core::contract