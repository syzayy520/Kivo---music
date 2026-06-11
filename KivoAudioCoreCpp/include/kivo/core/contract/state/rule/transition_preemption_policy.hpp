#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class TransitionPreemptionPolicy : uint8_t {
    Unknown = 0,
    AllowPreemption,
    RejectPreemption,
    QueueBehind,
    CancelCurrent
};

} // namespace kivo::core::contract