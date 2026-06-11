#pragma once

#include <memory>

#include "kivo/core/playback/session/playback_command.hpp"
#include "kivo/core/playback/session/playback_command_result.hpp"
#include "kivo/core/playback/session/playback_session_snapshot.hpp"
#include "kivo/core/playback/recovery/playback_recovery_completion.hpp"
#include "kivo/core/playback/recovery/playback_recovery_policy.hpp"

namespace kivo::core::playback {

class PlaybackSessionController final {
public:
    PlaybackSessionController();
    ~PlaybackSessionController();

    PlaybackSessionController(const PlaybackSessionController&) = delete;
    PlaybackSessionController& operator=(const PlaybackSessionController&) = delete;
    PlaybackSessionController(PlaybackSessionController&&) = delete;
    PlaybackSessionController& operator=(PlaybackSessionController&&) = delete;

    [[nodiscard]] PlaybackCommandResult submit(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] bool report_rendered_position(
        contract::SamplePosition position,
        uint64_t session_generation) noexcept;
    [[nodiscard]] bool report_failure(
        uint64_t session_generation) noexcept;
    [[nodiscard]] bool complete_seek(
        uint64_t session_generation,
        bool succeeded) noexcept;
    [[nodiscard]] PlaybackRecoveryDecision begin_recovery(
        uint64_t session_generation,
        contract::ErrorDomain domain) noexcept;
    [[nodiscard]] bool complete_recovery(
        uint64_t session_generation,
        bool succeeded) noexcept;
    [[nodiscard]] bool complete_recovery(
        uint64_t session_generation,
        PlaybackRecoveryCompletion completion) noexcept;
    [[nodiscard]] PlaybackSessionSnapshot snapshot() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace kivo::core::playback
