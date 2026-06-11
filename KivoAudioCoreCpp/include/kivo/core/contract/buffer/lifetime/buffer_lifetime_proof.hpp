#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class BufferLifetimeProof : uint8_t {
    NotSpecified = 0,
    Scoped,
    ReferenceCounted,
    ArenaManaged,
    ManualFree
};

} // namespace kivo::core::contract