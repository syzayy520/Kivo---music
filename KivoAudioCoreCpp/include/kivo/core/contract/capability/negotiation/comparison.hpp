// =============================================================================
// KivoAudioCoreCpp — comparison.hpp
// Contract: NegotiationComparison — Comparison between negotiation candidates
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// ComparisonOutcome — Result of comparing two candidates
// =============================================================================
enum class ComparisonOutcome : uint8_t {
    FirstBetter = 0,
    SecondBetter,
    Equivalent,
    Incomparable
};

// =============================================================================
// NegotiationComparison — Comparison result wrapper
// =============================================================================
struct NegotiationComparison {
    ComparisonOutcome outcome = ComparisonOutcome::Incomparable;

    [[nodiscard]] bool operator==(const NegotiationComparison&) const noexcept = default;
};

} // namespace kivo::core::contract
