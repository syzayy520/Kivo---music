// =============================================================================
// KivoAudioCoreCpp — output.hpp
// Contract: OutputCapability — Output domain capability
// =============================================================================

#pragma once

#include "../quality/level.hpp"
#include "../quality/path.hpp"

namespace kivo::core::contract {

// =============================================================================
// OutputCapability — Output domain capability assessment
// =============================================================================
struct OutputCapability {
    CapabilityLevel output_level = CapabilityLevel::None;
    CapabilityPath path = CapabilityPath::GeneralPurposePath;

    [[nodiscard]] bool operator==(const OutputCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
