#pragma once

#include <mutex>

#include "kivo/core/playback/session/controller/playback_session_controller.hpp"

namespace kivo::core::playback {

class PlaybackSessionController::Impl {
public:
    [[nodiscard]] PlaybackCommandResult submit(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] bool report_rendered_position(
        contract::SamplePosition position,
        uint64_t session_generation) noexcept;
    [[nodiscard]] bool report_failure(
        uint64_t session_generation,
        contract::ErrorDomain domain) noexcept;
    [[nodiscard]] bool complete_seek(
        uint64_t session_generation,
        bool succeeded) noexcept;
    [[nodiscard]] PlaybackDrainTicket begin_drain(
        uint64_t session_generation) noexcept;
    [[nodiscard]] bool complete_drain(
        PlaybackDrainTicket ticket,
        PlaybackDrainCompletion completion) noexcept;
    [[nodiscard]] PlaybackRecoveryDecision begin_recovery(
        uint64_t session_generation,
        contract::ErrorDomain domain) noexcept;
    [[nodiscard]] bool complete_recovery(
        uint64_t session_generation,
        PlaybackRecoveryCompletion completion) noexcept;
    [[nodiscard]] PlaybackSessionSnapshot read_snapshot() const noexcept;

private:
    static void saturating_increment(uint64_t& value) noexcept;
    [[nodiscard]] PlaybackCommandResult accept(
        contract::CommandId command_id,
        contract::CoreState previous) noexcept;
    [[nodiscard]] PlaybackCommandResult reject(
        contract::CommandId command_id,
        PlaybackCommandFailure failure,
        contract::CoreState previous) noexcept;

    mutable std::mutex mutex_{};
    PlaybackSessionSnapshot snapshot_{};
    contract::CoreState state_before_seek_{contract::CoreState::Ready};
    contract::CoreState state_before_recovery_{contract::CoreState::Ready};
};

} // namespace kivo::core::playback
