#pragma once

#include <cstdint>

#include "kivo/core/contract/error/domain/error_domain.hpp"
#include "kivo/core/playback/recovery/playback_recovery_execution_result.hpp"
#include "kivo/core/playback/recovery/playback_recovery_operations.hpp"

namespace kivo::core::playback {

class PlaybackSessionController;

class PlaybackRecoveryExecutor final {
public:
    PlaybackRecoveryExecutor(
        PlaybackSessionController& session,
        PlaybackRecoveryOperations& operations) noexcept;

    [[nodiscard]] PlaybackRecoveryExecutionResult recover(
        uint64_t session_generation,
        contract::ErrorDomain domain) noexcept;

private:
    PlaybackSessionController& session_;
    PlaybackRecoveryOperations& operations_;
};

} // namespace kivo::core::playback
