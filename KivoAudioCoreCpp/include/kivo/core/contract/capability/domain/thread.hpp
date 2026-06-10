// =============================================================================
// KivoAudioCoreCpp — thread.hpp
// Contract: ThreadCapability — Thread domain capability
// =============================================================================
//
// Uses CapabilityLevel for capability expression.
// Does NOT use bool supports_* style.
// =============================================================================

#pragma once

#include "../quality/level.hpp"

namespace kivo::core::contract {

// =============================================================================
// ThreadCapability — Thread domain capability assessment
// =============================================================================
struct ThreadCapability {
    CapabilityLevel thread_level = CapabilityLevel::None;
    bool realtime_safe = false;

    [[nodiscard]] bool operator==(const ThreadCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
