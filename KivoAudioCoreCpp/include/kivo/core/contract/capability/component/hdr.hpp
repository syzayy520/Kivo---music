// =============================================================================
// KivoAudioCoreCpp — hdr.hpp
// Contract: HdrCapability — HDR processing capability
// =============================================================================
//
// Uses CapabilityLevel, NOT bool supports_hdr.
// =============================================================================

#pragma once

#include "../quality/level.hpp"
#include "../quality/path.hpp"

namespace kivo::core::contract {

// =============================================================================
// HdrCapability — HDR processing capability assessment
// =============================================================================
struct HdrCapability {
    CapabilityLevel hdr_level = CapabilityLevel::None;
    CapabilityPath path = CapabilityPath::GeneralPurposePath;

    [[nodiscard]] bool operator==(const HdrCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
