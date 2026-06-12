#pragma once

#include "kivo/core/contract/command/decision/command_decision.hpp"
#include "kivo/core/contract/command/identity/command_id.hpp"
#include "kivo/core/contract/state/phase/core_state.hpp"
#include "kivo/core/playback/session/command/playback_command_failure.hpp"

namespace kivo::core::playback {

struct PlaybackCommandResult {
    contract::CommandId command_id{};
    contract::CommandDecision decision{contract::CommandDecision::Unknown};
    PlaybackCommandFailure failure{PlaybackCommandFailure::InvalidCommand};
    contract::CoreState previous_state{contract::CoreState::Unknown};
    contract::CoreState current_state{contract::CoreState::Unknown};

    [[nodiscard]] constexpr bool accepted() const noexcept {
        return decision == contract::CommandDecision::Accept
            || decision == contract::CommandDecision::Replace
            || decision == contract::CommandDecision::Coalesce;
    }

    [[nodiscard]] constexpr bool operator==(
        const PlaybackCommandResult&) const noexcept = default;
};

} // namespace kivo::core::playback
