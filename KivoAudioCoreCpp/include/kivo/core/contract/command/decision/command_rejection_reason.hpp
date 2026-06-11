#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CommandRejectionReason : uint8_t {
    Unknown = 0,
    StaleGeneration,
    DuplicateKind,
    PriorityConflict,
    PolicyViolation,
    ResourceExhausted,
    InvalidTransition,
    DeviceLost,
    SourceUnavailable,
    ShutdownInProgress
};

} // namespace kivo::core::contract