// =============================================================================
// KivoAudioCoreCpp — subject.hpp
// Contract: CapabilitySubject — What is being assessed
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SubjectKind — Type of entity being assessed
// =============================================================================
enum class SubjectKind : uint8_t {
    Device = 0,
    Codec,
    Pipeline,
    System
};

// =============================================================================
// CapabilitySubject — Identity of the assessed entity
// =============================================================================
struct CapabilitySubject {
    SubjectKind kind = SubjectKind::System;

    [[nodiscard]] bool operator==(const CapabilitySubject&) const noexcept = default;
};

} // namespace kivo::core::contract
