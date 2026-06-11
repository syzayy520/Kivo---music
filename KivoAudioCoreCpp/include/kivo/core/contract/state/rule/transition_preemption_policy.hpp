#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class TransitionPreemptionPolicy : uint8_t {
    Unknown = 0,
    NeverPreempt,
    AllowSafePreempt,
    QueueUntilCurrentCompletes,
    RejectUntilCurrentCompletes,
    ForceCloseOnly
};

} // namespace kivo::core::contract