#pragma once

#include "source_outcome_reason.hpp"
#include "source_recovery_policy.hpp"

namespace kivo::core::contract {

/// @brief Source outcome — result of a source operation.
struct SourceOutcome {
    SourceOutcomeReason reason{SourceOutcomeReason::NotSpecified};
    SourceRecoveryPolicy recovery{SourceRecoveryPolicy::NotSpecified};

    [[nodiscard]] constexpr bool operator==(const SourceOutcome&) const noexcept = default;
};

} // namespace kivo::core::contract
