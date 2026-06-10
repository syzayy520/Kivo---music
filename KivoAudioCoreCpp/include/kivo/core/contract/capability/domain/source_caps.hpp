// =============================================================================
// KivoAudioCoreCpp — source_caps.hpp
// Contract: SourceCapability — Source domain capability
// =============================================================================

#pragma once

#include <cstdint>

#include "../quality/level.hpp"

namespace kivo::core::contract {

// =============================================================================
// SourceCapability — Source domain capability assessment
// =============================================================================
struct SourceCapability {
    CapabilityLevel source_level = CapabilityLevel::None;
    bool seekable = false;
    bool range_readable = false;
    bool duration_known = false;

    [[nodiscard]] bool operator==(const SourceCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
