#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class RapidSeekRule : uint8_t {
    Unknown = 0,
    KeepLatest,
    KeepFirst,
    MergeAll,
    RejectIntermediate,
    QueueAll
};

} // namespace kivo::core::contract