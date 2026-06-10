// =============================================================================
// KivoAudioCoreCpp — decode.hpp
// Contract: DecodeCapability — Decode domain capability
// =============================================================================
//
// Expresses abstract capability level, CapabilityPath, and ceiling fields.
// Does NOT contain format lists, codec tables, or sample format enums.
// =============================================================================

#pragma once

#include <cstdint>

#include "../quality/level.hpp"
#include "../quality/path.hpp"

namespace kivo::core::contract {

// =============================================================================
// DecodeCapability — Decode domain capability assessment
// =============================================================================
struct DecodeCapability {
    CapabilityLevel decode_level = CapabilityLevel::None;
    CapabilityPath path = CapabilityPath::GeneralPurposePath;
    uint32_t max_sample_rate_ceiling = 0;  // 0 = unknown
    uint8_t max_channels_ceiling = 0;      // 0 = unknown

    [[nodiscard]] bool operator==(const DecodeCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
