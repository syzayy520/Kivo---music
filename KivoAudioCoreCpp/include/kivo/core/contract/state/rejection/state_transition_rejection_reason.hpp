#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class StateTransitionRejectionReason : uint8_t {
    Unknown = 0,
    IllegalTransition,
    TerminalState,
    ConcurrentTransition,
    GenerationMismatch,
    RuleViolation,
    ScenarioConflict
};

} // namespace kivo::core::contract