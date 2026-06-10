// =============================================================================
// KivoAudioCoreCpp — source.hpp
// Contract: CapabilitySource — Where a capability originates
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SourceOrigin — Origin of a capability assessment
// =============================================================================
enum class SourceOrigin : uint8_t {
    Device = 0,
    Codec,
    Pipeline,
    System,
    User
};

// =============================================================================
// CapabilitySource — Source of capability information
// =============================================================================
struct CapabilitySource {
    SourceOrigin origin = SourceOrigin::System;

    [[nodiscard]] bool operator==(const CapabilitySource&) const noexcept = default;
};

} // namespace kivo::core::contract
