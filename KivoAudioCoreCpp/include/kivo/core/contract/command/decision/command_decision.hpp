#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CommandDecision : uint8_t {
    Unknown = 0,
    Accept,
    Reject,
    Replace,
    Supersede,
    Coalesce,
    Queue,
    Defer
};

} // namespace kivo::core::contract