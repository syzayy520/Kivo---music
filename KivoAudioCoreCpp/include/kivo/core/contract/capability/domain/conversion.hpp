// =============================================================================
// KivoAudioCoreCpp — conversion.hpp
// Contract: ConversionCapability — Conversion domain capability
// =============================================================================

#pragma once

#include "../quality/level.hpp"

namespace kivo::core::contract {

// =============================================================================
// ConversionCapability — Conversion domain capability assessment
// =============================================================================
struct ConversionCapability {
    CapabilityLevel conversion_level = CapabilityLevel::None;
    bool resample_capable = false;
    bool channel_remix_capable = false;

    [[nodiscard]] bool operator==(const ConversionCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
