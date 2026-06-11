#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class StateTransitionRejectionReason : uint8_t {
    Unknown = 0,
    InvalidSourceState,
    InvalidTargetState,
    TerminalState,
    AlreadyInRequestedState,
    ConflictingTransition,
    RequiresDrainCompletion,
    RequiresRecoveryCompletion,
    ClosedStateRejectsMutation,
    FailedStateAllowsCloseOnly
};

} // namespace kivo::core::contract