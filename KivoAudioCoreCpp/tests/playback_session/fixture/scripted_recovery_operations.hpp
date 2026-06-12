#pragma once

#include <array>

#include "kivo/core/playback/recovery/operations/playback_recovery_operations.hpp"

namespace playback_session_test {

class ScriptedRecoveryOperations final
    : public kivo::core::playback::PlaybackRecoveryOperations {
public:
    kivo::core::contract::RecoveryAction primary_action{
        kivo::core::contract::RecoveryAction::None};
    kivo::core::playback::PlaybackRecoveryCompletion primary_completion{
        kivo::core::playback::PlaybackRecoveryCompletion::Failed};
    kivo::core::contract::RecoveryAction fallback_action{
        kivo::core::contract::RecoveryAction::None};
    kivo::core::playback::PlaybackRecoveryCompletion fallback_completion{
        kivo::core::playback::PlaybackRecoveryCompletion::Failed};
    std::array<kivo::core::contract::RecoveryAction, 2> observed_actions{};
    size_t execution_count{0};

    [[nodiscard]] kivo::core::playback::PlaybackRecoveryCompletion execute(
        kivo::core::contract::RecoveryAction action,
        uint64_t session_generation) noexcept override {
        if (session_generation == 0
            || execution_count >= observed_actions.size()) {
            return kivo::core::playback::PlaybackRecoveryCompletion::Failed;
        }
        observed_actions[execution_count++] = action;
        if (action == primary_action) {
            return primary_completion;
        }
        if (action == fallback_action) {
            return fallback_completion;
        }
        return kivo::core::playback::PlaybackRecoveryCompletion::Failed;
    }
};

} // namespace playback_session_test
