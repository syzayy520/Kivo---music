// =============================================================================
// KivoAudioCoreCpp — realtime_path_context.hpp
// Contract: Realtime path context (side of the boundary)
// =============================================================================
//
// Identifies which side of a realtime boundary the current execution is on.
// Contract-side identity, not thread types.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// RealtimePathContext — Which side of the realtime boundary
// =============================================================================
enum class RealtimePathContext : uint8_t {
    Unknown = 0,
    RenderSide,
    DecodeSide,
    ControlSide,
    BackgroundSide,
    EventSide
};

} // namespace kivo::core::contract