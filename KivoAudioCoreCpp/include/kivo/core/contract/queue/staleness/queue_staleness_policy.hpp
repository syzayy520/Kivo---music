#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class QueueStalenessPolicy : uint8_t {
    NotSpecified = 0,
    DiscardStale,
    KeepStale,
    MarkStale
};

} // namespace kivo::core::contract