#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class DeviceLostDuringSeekRule : uint8_t {
    Unknown = 0,
    EnterRecovering,
    FailSeek,
    PreserveSeekTarget,
    RequireRecoveryThenSeek
};

} // namespace kivo::core::contract