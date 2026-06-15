#pragma once

#include <cstdint>

#include "kivo/core/contract/error/recovery/recovery_action.hpp"
#include "kivo/core/playback/recovery/completion/playback_recovery_completion.hpp"

namespace kivo::core::playback {

class PlaybackRecoveryOperations {
public:
    virtual ~PlaybackRecoveryOperations() = default;

    [[nodiscard]] virtual PlaybackRecoveryCompletion execute(
        contract::RecoveryAction action,
        uint64_t session_generation) noexcept = 0;
};

} // namespace kivo::core::playback
