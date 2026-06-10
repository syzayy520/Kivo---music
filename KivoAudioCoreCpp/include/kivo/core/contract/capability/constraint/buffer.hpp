// =============================================================================
// KivoAudioCoreCpp — buffer.hpp
// Contract: BufferCapability — Buffer domain capability
// =============================================================================
//
// Uses CapabilityLevel for capability expression.
// Does NOT use bool supports_* style.
// =============================================================================

#pragma once

#include <cstdint>

#include "../quality/level.hpp"

namespace kivo::core::contract {

// =============================================================================
// BufferCapability — Buffer domain capability assessment
// =============================================================================
struct BufferCapability {
    CapabilityLevel buffer_level = CapabilityLevel::None;
    uint32_t max_buffer_frames_ceiling = 0;  // 0 = unknown

    [[nodiscard]] bool operator==(const BufferCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
