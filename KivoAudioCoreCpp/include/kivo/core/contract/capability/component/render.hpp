// =============================================================================
// KivoAudioCoreCpp — render.hpp
// Contract: RenderCapability — Render domain capability
// =============================================================================
//
// Expresses abstract capability level, CapabilityPath, and ceiling fields.
// Does NOT contain format lists or platform format types.
// =============================================================================

#pragma once

#include <cstdint>

#include "../quality/level.hpp"
#include "../quality/path.hpp"

namespace kivo::core::contract {

// =============================================================================
// RenderCapability — Render domain capability assessment
// =============================================================================
struct RenderCapability {
    CapabilityLevel render_level = CapabilityLevel::None;
    CapabilityPath path = CapabilityPath::GeneralPurposePath;
    uint32_t max_sample_rate_ceiling = 0;  // 0 = unknown
    uint8_t max_channels_ceiling = 0;      // 0 = unknown

    [[nodiscard]] bool operator==(const RenderCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
