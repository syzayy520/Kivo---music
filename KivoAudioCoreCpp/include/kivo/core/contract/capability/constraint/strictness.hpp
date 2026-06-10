// =============================================================================
// KivoAudioCoreCpp — strictness.hpp
// Contract: ConstraintStrictness — How strict a requirement is
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// ConstraintStrictness — Requirement enforcement level
// =============================================================================
enum class ConstraintStrictness : uint8_t {
    Advisory = 0,
    Preferred,
    Required,
    Absolute
};

} // namespace kivo::core::contract
