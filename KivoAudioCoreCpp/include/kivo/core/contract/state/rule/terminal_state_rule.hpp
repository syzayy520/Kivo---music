#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class TerminalStateRule : uint8_t {
    Unknown = 0,
    RejectAllMutations,
    AllowCloseOnly,
    AllowInspectOnly,
    AlreadyClosedIsIdempotent
};

} // namespace kivo::core::contract