// =============================================================================
// KivoAudioCoreCpp — memory.hpp
// Contract: MemoryCapability — Memory domain capability
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
// MemoryCapability — Memory domain capability assessment
// =============================================================================
struct MemoryCapability {
    CapabilityLevel memory_level = CapabilityLevel::None;
    uint64_t pool_budget_hint_bytes = 0;  // 0 = unknown

    [[nodiscard]] bool operator==(const MemoryCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
