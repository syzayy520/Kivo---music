#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CommandOrigin : uint8_t {
    Unknown = 0,
    User,
    System,
    Recovery,
    Scheduler,
    External
};

} // namespace kivo::core::contract