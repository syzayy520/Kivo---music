#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class ClosedMutationRule : uint8_t {
    Unknown = 0,
    RejectMutation,
    ReturnAlreadyClosed,
    InspectOnly
};

} // namespace kivo::core::contract