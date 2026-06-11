#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CommandPriority : uint8_t {
    Unknown = 0,
    Critical,
    High,
    Normal,
    Low
};

} // namespace kivo::core::contract