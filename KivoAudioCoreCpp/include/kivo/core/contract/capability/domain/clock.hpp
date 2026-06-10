// =============================================================================
// KivoAudioCoreCpp — clock.hpp
// Contract: ClockCapability — Clock domain capability
// =============================================================================
//
// ClockCapability::monotonic is an invariant/truth property.
// It is NOT a capability support field.
// =============================================================================

#pragma once

#include "../quality/level.hpp"

namespace kivo::core::contract {

// =============================================================================
// ClockCapability — Clock domain capability assessment
// =============================================================================
struct ClockCapability {
    CapabilityLevel clock_level = CapabilityLevel::None;
    bool monotonic = false;  // Invariant/truth property, NOT capability support

    [[nodiscard]] bool operator==(const ClockCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
