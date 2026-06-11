#pragma once

#include "kivo/core/contract/error/recovery/recovery_action.hpp"
#include "kivo/core/playback/recovery/playback_recovery_completion.hpp"

namespace kivo::core::playback {

struct PlaybackRecoveryExecutionResult {
    bool accepted{false};
    bool succeeded{false};
    bool fallback_attempted{false};
    contract::RecoveryAction primary{contract::RecoveryAction::None};
    contract::RecoveryAction fallback{contract::RecoveryAction::None};
    PlaybackRecoveryCompletion completion{
        PlaybackRecoveryCompletion::Failed};

    [[nodiscard]] constexpr bool operator==(
        const PlaybackRecoveryExecutionResult&) const noexcept = default;
};

} // namespace kivo::core::playback
